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


void OpenHouseDialog::sendBroadcast(){
    QList<QString> ips = getIpListOfComputer();
       for (auto &ip: ips) {
           auto *s = new QUdpSocket(this);
           s->bind(QHostAddress(ip));
           sendSockets.push_back(s);
           connect(s, &QUdpSocket::errorOccurred, [this]() {

           });
       }
       for (auto i: sendSockets) {
           QJsonObject jsonobject;
           jsonobject.insert("name",this->name);
           jsonobject.insert("gamemodel",ui->forbiddenCombolBox->currentText());
           QString s=ui->colorCombolBox->currentText();
           QString b;
           if(s=="后手"){
               b="0";
           }else if(s=="先手"){
               b="0";
           }else{
               b="1";
                int rand =QRandomGenerator::global()->bounded(1.0);	//生成一个0和1.0之间的浮点数
                //产生十以内的随机数(0-9)
                if(rand>=0.5){
                    s="后手";
                }else{
                    s="先手";
                }
           }
           jsonobject.insert("chesscolor",s);
           jsonobject.insert("israndom",b);
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
                    //这里打印出电脑上的所有网卡，
//                    std::cout << "------------------------------------------------------------";
//                    std::cout << "Adapter Name:" << interfaceItem.name().toStdString() << std::endl;
//                    std::cout << "Adapter Address:" << interfaceItem.hardwareAddress().toStdString() << std::endl;
//                    std::cout << "IP Address:" << addressEntryItem.ip().toString().toStdString() << std::endl;
//                    std::cout << "IP Mask:" << addressEntryItem.netmask().toString().toStdString() << std::endl;
//                    std::cout << "可过滤的网卡名字" << interfaceItem.humanReadableName().toStdString() << std::endl;

                    ret_list.append(addressEntryItem.ip().toString());
                }
            }
        }
    }

    //返回所有可用网卡的Ip地址，注：每个网卡有自己的地址（当然有的网卡其实没有接入局域网，因此虽然有ip地址，但是其实无法完成任何通信）
    return ret_list;
}

void OpenHouseDialog::on_okButton_clicked()
{
    sendBroadcast();
    time->start(1000);
}

void OpenHouseDialog::upsenddata(){
    sendSockets.clear();
    sendBroadcast();
    time->setInterval(1000);
    time->start();
}

