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

class TcpAbstract : public QObject {
    Q_OBJECT
   protected:
    bool isConnected = false;  ///< 目前的状态是否是连接状态
    QHostAddress *peerAddress = nullptr;  ///< 对方的ip地址
    bool haveHeartbeat = false;
    QTimer heartbeatSendTimer;
    QTimer heartbeatCheckTimer;
    bool isActiveExit=false;///< 指示用户是否主动终止了游戏,用户终止游戏时不会再尝试重连

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

    virtual void handleDisconnect()=0;

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

    void sendHeartbeat() {
        QJsonObject object;
        object.insert("type", "heart");
        send(QString(QJsonDocument(object).toJson(QJsonDocument::Compact)));
    }

    void checkHeartbeat() {
        if (haveHeartbeat) {
            haveHeartbeat = false;
        } else {
            setConnected(false);
        }
    }

   public:
    virtual bool send(const QString &s) = 0;

    virtual void stop() = 0;

    void setActiveExit(){//设置是自己主动结束的,不用重连
        isActiveExit=true;
    }

    ~TcpAbstract() override { delete peerAddress; };
   signals:

    void connected();

    void disconnected();

    void systemDo(QJsonObject object);

    void userDo(QJsonObject object);
};

#endif  // GOBANG_TCPABSTRACT_H
