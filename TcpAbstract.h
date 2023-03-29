//
// Created by user-kk on 2023/3/26.
//

#ifndef GOBANG_TCPABSTRACT_H
#define GOBANG_TCPABSTRACT_H

#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QTimer>
#include <iostream>
/**
 * @class TcpAbstract
 * @brief 提供了一些TCP服务器和客户端的公共功能的抽象基类
 */
class TcpAbstract : public QObject {
    Q_OBJECT
   protected:
    bool isConnected = false;  ///< 目前的状态是否是连接状态
    QHostAddress *peerAddress = nullptr;  ///< 对方的ip地址
    bool haveHeartbeat = false;///< 对方的心跳
    QTimer heartbeatSendTimer;///< 心跳发送计时器，每隔半秒发送一次心跳包
    QTimer heartbeatCheckTimer;///< 检查心跳的计时器，每隔一秒检查一次对方的心跳
    bool isActiveExit=false;///< 指示用户是否主动终止了游戏,用户终止游戏时不会再尝试重连


     /// @brief 设置isConnected是否连接,连接发送connected 信号，不连接发送disconnected 信号
     /// @param b 是否连接 b为false时，停止发送心跳包，在isActiveExit为false时调用handleDisconnect尝试重连
    void setConnected(bool b) {
        if (b) {
            isConnected = true;
            emit connected();
        } else {
            isConnected = false;
            emit disconnected();
            //停止心跳包
            heartbeatCheckTimer.stop();
            heartbeatSendTimer.stop();

            if(!isActiveExit){ //不是用户主动结束的游戏,尝试重连
                handleDisconnect();
            }
        }
    };
    ///< @brief处理重连情况的纯虚函数
    virtual void handleDisconnect()=0;

    /// @brief 处理接收到的消息
    /// 如果为心跳包，设置haveHeartbeat为true，其他消息则转发为systemDo或userDo信号
    /// @param s JOSN格式的字符串，
    /// @warning s中必须有一个"type":"XXX"类型的字段,否则会触发断言
    /// @warning s中不能含有换行符，否则会导致无法处理
    void handleMessage(const QString &s) {
        //!!!处理粘包问题
        QRegularExpression re(R"({.*?})");
        auto i = re.globalMatch(s);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            auto token = match.captured(0);
            QJsonParseError err;
            QJsonDocument doc = QJsonDocument::fromJson(token.toUtf8(), &err);
            QJsonObject jsonObject = doc.object();
            assert(jsonObject.contains("type"));
            auto type = jsonObject.value("type").toString();
            if (type == "heart") {
                haveHeartbeat = true;
            } else if (type == "system") {
                emit systemDo(jsonObject);
            } else if (type == "user") {
                emit userDo(jsonObject);
            }
        }
    }
    ///@brief 发送一份内容为{"type":"heart"}的心跳包字符串给对方
    void sendHeartbeat() {
        QJsonObject object;
        object.insert("type", "heart");
        send(QString(QJsonDocument(object).toJson(QJsonDocument::Compact)));
    }
    ///@brief 检查心跳，如果对方没有心跳 调用setConnected(false) @ref setConnected
    void checkHeartbeat() {
        if (haveHeartbeat) {
            haveHeartbeat = false;
        } else {
            setConnected(false);
        }
    }

   public:

    ///@brief 发送字符串给对方
    ///@param s 一定为JOSN格式并且具有"type"字段 否则对方不会处理,@see handleMessage
    virtual bool send(const QString &s) = 0;

    ///@brief 主动停止服务器/客户端,会调用setActiveExit,不会触发重连
    virtual void stop() = 0;

    /// @brief 设置是自己主动结束的,使重连不被触发
    void setActiveExit(){
        isActiveExit=true;
    }

    ~TcpAbstract() override { delete peerAddress; };
   signals:

    /// @brief 连接成功信号
    void connected();

    /// @brief 断开连接信号
    void disconnected();

    /// @brief 发送系统命令，格式见文档
    void systemDo(QJsonObject object);

    /// @brief 发送用户聊天记录
    void userDo(QJsonObject object);
};

#endif  // GOBANG_TCPABSTRACT_H
