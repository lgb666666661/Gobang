//
// Created by user-kk on 2023/3/26.
//

#include "TcpServer.h"
#include <iostream>
TcpServer::TcpServer() {
    //启动服务器
    while (!server.listen(QHostAddress::Any, port)) {
        port++;
    };

    //有新连接时
    connect(&server, &QTcpServer::newConnection, this, [this]() {
        QTcpSocket *socket = server.nextPendingConnection();
        if (socket != nullptr) {
            //如果有新的连接,但却不是对手的ip地址,拒绝他
            if(peerAddress!= nullptr&&(*peerAddress)!=socket->peerAddress()){
                //todo:拒绝新连接
                return ;
            }

            //设置连接成功
            setConnected(true);
            sendSocket=socket;
            peerAddress=new QHostAddress(socket->peerAddress());


            connect(socket, &QTcpSocket::readyRead, [this, socket]() {
                QString s= socket->readAll();
                handleMessage(s);
            });

            connect(socket,&QTcpSocket::errorOccurred,[this,socket](){
                //todo 写入日志
                setConnected(false);
            });
        }
    });
}


bool TcpServer::send(const QString& s) {
    if(sendSocket&&isConnected){
        sendSocket->write(s.toUtf8());
        return true;
    }
    return false;
}

int TcpServer::getPort() const {
    return port;
}

void TcpServer::stop() {
    sendSocket->close();
}
