#ifndef NETWINDOW_H
#define NETWINDOW_H

#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <QTimer>
#include <QUdpSocket>
#include <QWidget>
#include <iostream>

#include "openhousedialog.h"
#include "utils.h"
namespace Ui {
class NetWindow;
}

/**
 * @class NetWindow
 * @brief 接收信号类，可以选择房间加入
 */

class NetWindow : public QMainWindow {
    Q_OBJECT

   public:
    ///@brief 无参构造函数，创建游戏主界面，并且进行监听广播，收到数据包后显示
    ///@param parent
    explicit NetWindow(QWidget* parent = nullptr);
    void receiveBroadcast();
    void showIpAddress();
    QString convert_to_ipv4_addr(const QHostAddress& addr);
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
    void beginGame();
    void refuseLink();
    void showGame();

   private:
    Ui::NetWindow* ui;
    Chessboard_Remote_PVP_Client* rPVP2 = nullptr;
    QTimer* time;
    quint16 port;
    QList<QString> address;
    QList<UdpData> udpdatalist;
    QList<QNetworkDatagram> datagramlist;
    QList<QNetworkDatagram> currentDatagramlist;
    QUdpSocket listenSocket;
    OpenHouseDialog* openhousedialog = nullptr;
};

#endif  // NETWINDOW_H
