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
namespace Ui {
class OpenHouseDialog;
}

class OpenHouseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenHouseDialog(QWidget *parent = nullptr,QString name ="");
    QList<QString> getIpListOfComputer();
    void sendBroadcast();
    void closeEvent(QCloseEvent* e);
    ~OpenHouseDialog();
signals:
    void back();
    void cancel();
private slots:
    void on_okButton_clicked();
    void upsenddata();

    void on_cancelButton_clicked();

private:
    QTimer *time;
    QString name;
    Ui::OpenHouseDialog *ui;
    QList<QUdpSocket*> sendSockets;
};

#endif // OPENHOUSEDIALOG_H
