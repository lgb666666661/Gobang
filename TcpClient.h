//
// Created by user-kk on 2023/3/26.
//

#ifndef GOBANG_TCPCLIENT_H
#define GOBANG_TCPCLIENT_H

#include <QTcpSocket>
#include "TcpAbstract.h"
#include <QTimer>
class TcpClient : public TcpAbstract {

private:
    QTcpSocket socket;
    int socketPort{};
protected:
    ///@brief 断开上一份连接
    void handleDisconnect() override;

public:
    TcpClient(const QHostAddress &address, int port);

    void stop() override;

    bool send(const QString &s) override;
};


#endif //GOBANG_TCPCLIENT_H
