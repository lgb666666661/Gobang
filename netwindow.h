#ifndef NETWINDOW_H
#define NETWINDOW_H

#include <QWidget>
#include<QNetworkInterface>
#include<QUdpSocket>
#include<QNetworkDatagram>
#include<QJsonDocument>
#include<QJsonParseError>
#include<QJsonObject>
#include<QTimer>
#include"openhousedialog.h"
#include"utils.h"
#include<iostream>
namespace Ui { class NetWindow;
}

class NetWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NetWindow(QWidget *parent = nullptr);
    void receiveBroadcast();
    void showIpAddress();
    ~NetWindow();

private slots:
    void on_openHouseButton_clicked();
    void updataIp();

private:
    Ui::NetWindow *ui;
    QTimer* time;
    quint16 port;
    QList<UdpData> udpdatalist;
    QList<QNetworkDatagram> datagramlist;
    QUdpSocket listenSocket;

};

#endif // NETWINDOW_H
