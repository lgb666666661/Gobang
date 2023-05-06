//
// Created by user-kk on 2023/3/26.
//

#ifndef GOBANG_TCPSERVER_H
#define GOBANG_TCPSERVER_H

#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "TcpAbstract.h"
///@class TcpServer
///@brief Tcp服务器类
class TcpServer : public TcpAbstract {
   private:
    QTcpServer server;  ///< 服务器
    int port = 10086;   ///< 默认的端口号，端口被占用时会被改变
    QTcpSocket *sendSocket = nullptr;  ///< 用于通信的socket
   protected:
    ///@brief 断开上一份连接
    void handleDisconnect() override;

   public:
    TcpServer();
    ///@brief 向对方发送终止游戏的信息，关闭tcp连接
    ///@note 这是主动断开连接不会触发重连
    void stop() override;

    ///@brief 得到服务器启动的端口号
    int getServerPort() override;
    ///@brief 向对方发送信息
    ///@param s JSON格式信息
    bool send(const QString &s) override;
};

#endif  // GOBANG_TCPSERVER_H
