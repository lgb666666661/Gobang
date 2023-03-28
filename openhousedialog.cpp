#include "openhousedialog.h"
#include "ui_openhousedialog.h"

OpenHouseDialog::OpenHouseDialog(QWidget *parent,QString name) :
    QDialog(parent),
    ui(new Ui::OpenHouseDialog)
{
    this->name=name;
    ui->setupUi(this);
    time=new QTimer();
    connect(time,&QTimer::timeout,this,&OpenHouseDialog::upsenddata);
}

OpenHouseDialog::~OpenHouseDialog()
{
    delete ui;
}

void OpenHouseDialog::closeEvent(QCloseEvent *e){
   emit cancel();
}
void OpenHouseDialog::sendBroadcast(){
    QList<QString> ips = getIpListOfComputer();
       for (auto &ip: ips) {
           auto *s = new QUdpSocket(this);
           s->bind(QHostAddress(ip));
           sendSockets.push_back(s);
       }
       for (auto i: sendSockets) {
           QJsonObject jsonobject;
           jsonobject.insert("name",this->name);
           jsonobject.insert("gamemodel",ui->forbiddenCombolBox->currentText());
           QString s=ui->colorCombolBox->currentText();
           QString b;
           if(s=="白棋"){
               b="0";
           }else if(s=="黑棋"){
               b="0";
           }else{
               b="1";
                int rand =QRandomGenerator::global()->bounded(1.0);	//生成一个0和1.0之间的浮点数
                //产生十以内的随机数(0-9)
                if(rand>=0.5){
                    s="白棋";
                }else{
                    s="黑棋";
                }
           }
           jsonobject.insert("chesscolor",s);
           jsonobject.insert("israndom",b);
           jsonobject.insert("port","10129");
           QJsonDocument jsondocument;
           jsondocument.setObject(jsonobject);
           QByteArray dataarray=jsondocument.toJson();
           i->writeDatagram(dataarray,QHostAddress::Broadcast,10123);
       }
}

QList<QString> OpenHouseDialog::getIpListOfComputer() {
    QList<QString> ret_list;
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interfaceItem: interfaceList) {
        if (interfaceItem.flags().testFlag(QNetworkInterface::IsUp)
            && interfaceItem.flags().testFlag(QNetworkInterface::IsRunning)
            && interfaceItem.flags().testFlag(QNetworkInterface::CanBroadcast)
            && interfaceItem.flags().testFlag(QNetworkInterface::CanMulticast)
            && !interfaceItem.flags().testFlag(QNetworkInterface::IsLoopBack)
                ) {
            QList<QNetworkAddressEntry> addressEntryList = interfaceItem.addressEntries();
            for (QNetworkAddressEntry &addressEntryItem: addressEntryList) {
                if (addressEntryItem.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    ret_list.append(addressEntryItem.ip().toString());
                }
            }
        }
    }
    return ret_list;
}

void OpenHouseDialog::on_okButton_clicked()
{
    //此处缺少新建tcpSocket
    sendBroadcast();
    time->start(1000);
}

void OpenHouseDialog::upsenddata(){
    sendSockets.clear();
    sendBroadcast();
    time->setInterval(1000);
    time->start();
}


void OpenHouseDialog::on_cancelButton_clicked()
{
    emit back();
}

