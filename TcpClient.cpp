//
// Created by user-kk on 2023/3/26.
//

#include "TcpClient.h"
#include <iostream>
TcpClient::TcpClient(const QHostAddress &address, int port): socketPort(port) {

    connect(&socket,&QTcpSocket::connected,[this](){
        //设置连接成功
        setConnected(true);
        peerAddress=new QHostAddress(socket.peerAddress());
        connect(&socket,&QTcpSocket::readyRead,[this](){
            QByteArray buf = socket.readAll();
            handleMessage(buf);
        });
    });
    connect(&socket,&QTcpSocket::errorOccurred,[this](){
        std::cout<<"pppp"<<std::endl;
        setConnected(false);
    });
    socket.connectToHost(address,port);

}

bool TcpClient::send(const QString &s) {
    if(isConnected){
        socket.write(s.toUtf8());
        return true;
    }
    return false;
}
