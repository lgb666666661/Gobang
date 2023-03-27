//
// Created by user-kk on 2023/3/26.
//

#ifndef GOBANG_TCPABSTRACT_H
#define GOBANG_TCPABSTRACT_H

#include <QTimer>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <iostream>

class TcpAbstract : public QObject {
Q_OBJECT
protected:
    bool isConnected = false;  ///< 目前的状态是否是连接状态
    QHostAddress *peerAddress = nullptr;  ///< 对方的ip地址
    bool haveHeartbeat = false;
    QTimer heartbeatSendTimer;
    QTimer heartbeatCheckTimer;


    void setConnected(bool b) {
        if (b) {
            isConnected = true;
            emit connected();
        } else {
            isConnected = false;
            emit disconnected();
            heartbeatCheckTimer.stop();
            heartbeatSendTimer.stop();
        }
    };

    void handleMessage(const QString &s) {
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(s.toUtf8(), &err);
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

    void sendHeartbeat() {
        QJsonObject object;
        object.insert("type", "heart");
        send(QString(QJsonDocument(object).toJson()));
    }

    void checkHeartbeat() {
        static int i=0;
        std::cout<<"check"<<haveHeartbeat<<" "<<i++<<std::endl;
        if (haveHeartbeat) {
            haveHeartbeat=false;
        }else{
            setConnected(false);
        }
    }


public:
    virtual bool send(const QString &s) = 0;

    virtual void stop() = 0;

    ~TcpAbstract() override { delete peerAddress; };
signals:

    void connected();

    void disconnected();

    void systemDo(QJsonObject object);

    void userDo(QJsonObject object);
};

#endif  // GOBANG_TCPABSTRACT_H
