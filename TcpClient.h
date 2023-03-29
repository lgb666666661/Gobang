//
// Created by user-kk on 2023/3/26.
//

#ifndef GOBANG_TCPCLIENT_H
#define GOBANG_TCPCLIENT_H

#include <QTcpSocket>
#include "TcpAbstract.h"
#include <QTimer>
///@class TcpClient
///@brief Tcp客户端类
class TcpClient : public TcpAbstract {

private:
    QTcpSocket socket;///< 用于通信的socket
    int socketPort{};///< 保存对方的端口号
protected:
    ///@brief 断开上一份连接，尝试重连
    void handleDisconnect() override;

public:
    TcpClient(const QHostAddress &address, int port);

    ///@brief 向对方发送终止游戏的信息，关闭tcp连接
    ///@note 这是主动断开连接不会触发重连
    void stop() override;

    ///@brief 向对方发送信息
    ///@param s JSON格式信息
    bool send(const QString &s) override;
};


#endif //GOBANG_TCPCLIENT_H
