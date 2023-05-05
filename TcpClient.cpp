//
// Created by user-kk on 2023/3/26.
//

#include "TcpClient.h"

#include <iostream>

TcpClient::TcpClient(const QHostAddress &address, int port) : socketPort(port) {
    connect(&socket, &QTcpSocket::connected, [this]() {
        // 设置连接成功
        setConnected(true);
        peerAddress = new QHostAddress(socket.peerAddress());
        heartbeatSendTimer.start(500);
        heartbeatCheckTimer.start(1000);
    });
    connect(&socket, &QTcpSocket::readyRead, [this]() {
        QByteArray buf = socket.readAll();
        handleMessage(buf);
    });
    connect(&socket, &QTcpSocket::errorOccurred,
            [this]() { setConnected(false); });
    connect(&heartbeatSendTimer, &QTimer::timeout, this,
            [this]() { sendHeartbeat(); });
    connect(&heartbeatCheckTimer, &QTimer::timeout,
            [this]() { checkHeartbeat(); });
    socket.connectToHost(address, port);
}

bool TcpClient::send(const QString &s) {
    if (isConnected) {
        socket.write(s.toUtf8());
        return true;
    }
    return false;
}

void TcpClient::stop() {
    QJsonObject object;
    object.insert("type", "system");
    object.insert("do", "terminate");
    send(QString(QJsonDocument(object).toJson(QJsonDocument::Compact)));
    socket.close();
}

void TcpClient::handleDisconnect() {
    socket.close();
    socket.connectToHost(*peerAddress, socketPort);
}
