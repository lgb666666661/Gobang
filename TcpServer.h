//
// Created by user-kk on 2023/3/26.
//

#ifndef GOBANG_TCPSERVER_H
#define GOBANG_TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include "TcpAbstract.h"

class TcpServer : public TcpAbstract {
private:
    QTcpServer server;
    int port = 10086;
    QTcpSocket *sendSocket = nullptr;
protected:
    void handleDisconnect() override;

public:
    TcpServer();

    void stop() override;

    int getPort() const;

    bool send(const QString &s) override;


};


#endif //GOBANG_TCPSERVER_H
