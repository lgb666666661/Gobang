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
#include<QCloseEvent>
#include<QMessageBox>
namespace Ui { class NetWindow;
}

class NetWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NetWindow(QWidget *parent = nullptr);
    void receiveBroadcast();
    void showIpAddress();
    void filterAddress();
    void closeEvent(QCloseEvent* e);
    ~NetWindow();
signals:
    void backToMain();
private slots:
    void filterSlot();
    void on_openHouseButton_clicked();
    void updataIp();
    void backSlot();
    void cancelSlot();
private:
    Ui::NetWindow *ui;
    QTimer* time;
    quint16 port;
    QList<UdpData> udpdatalist;
    QList<QNetworkDatagram> datagramlist;
    QUdpSocket listenSocket;
    OpenHouseDialog* openhousedialog;

};

#endif // NETWINDOW_H
