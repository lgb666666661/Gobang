//
// Created by user-kk on 2023/3/26.
//

#ifndef GOBANG_TCPABSTRACT_H
#define GOBANG_TCPABSTRACT_H

#include <QObject>
#include <QHostAddress>
#include <QJsonObject>
#include <QString>
#include <iostream>
#include <QJsonDocument>

class TcpAbstract : public QObject {
Q_OBJECT
protected:
    bool isConnected = false;///< 目前的状态是否是连接状态
    void setConnected(bool b) {
        if (b) {
            isConnected = true;
            emit connected();
        } else {
            isConnected = false;
            emit disconnected();
        }
    };
    QHostAddress *peerAddress = nullptr;///< 对方的ip地址
    void handleMessage(const QString &s) {
        std::cout<<s.toStdString()<<std::endl;
        QJsonParseError err;

        QJsonDocument doc = QJsonDocument::fromJson(s.toUtf8(),&err);
        QJsonObject jsonObject=doc.object();
        assert(jsonObject.contains("type"));
        auto type = jsonObject.value("type").toString();
        std::cout<<type.toStdString()<<std::endl;
        if (type == "system") {
            emit systemDo(jsonObject);
        } else if (type == "user") {
            emit userDo(jsonObject);
        }
    }

public:
    virtual bool send(const QString &s) = 0;

    ~TcpAbstract() override {
        delete peerAddress;
    };
signals:

    void connected();

    void disconnected();

    void systemDo(QJsonObject object);

    void userDo(QJsonObject object);
};


#endif //GOBANG_TCPABSTRACT_H
