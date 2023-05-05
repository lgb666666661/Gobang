#ifndef OPENHOUSEDIALOG_H
#define OPENHOUSEDIALOG_H

#include <QDialog>
#include<QNetworkInterface>
#include<QUdpSocket>
#include<QJsonObject>
#include<QTime>
#include<QRandomGenerator>
#include<QJsonDocument>
#include<QTimer>
#include<QMessageBox>
#include"chessboard_remote_pvp.h"
namespace Ui {
class OpenHouseDialog;
}
/**
 * @class OpenHouseDialog
 * @brief 创建房间类，发送广播信号
 */
class OpenHouseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenHouseDialog(QWidget *parent = nullptr,QString name ="");
    QList<QString> getIpListOfComputer();
    void sendBroadcast(double rand);
    void closeEvent(QCloseEvent* e);
    ~OpenHouseDialog();
signals:
    void back();
    void cancel();
private slots:
    void on_okButton_clicked();
    void upsenddata();
    void showGame();
    void cancelSlot();
    void on_cancelButton_clicked();

private:
    double rand;
    Chessboard_Remote_PVP_Server *rPVP=nullptr;
    QTimer *time;
    QString name;
    Ui::OpenHouseDialog *ui;
    QList<QUdpSocket*> sendSockets;
};

#endif // OPENHOUSEDIALOG_H
