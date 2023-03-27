#include "netwindow.h"
#include "ui_netwindow.h"

NetWindow::NetWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetWindow)
{
    ui->setupUi(this);
    receiveBroadcast();

//    time=new QTimer(this);
//    connect(time,&QTimer::timeout,this,&NetWindow::updataIp);
//    time->start(1000);
}

NetWindow::~NetWindow()
{
    delete ui;
}
void NetWindow::showIpAddress(){
    QJsonParseError jsonerror;
    QJsonDocument jsonDoc;
    for(auto i:datagramlist){
        QJsonParseError* jsonerror=new QJsonParseError();;
        QJsonDocument* jsonDoc=new QJsonDocument();
        *jsonDoc=(QJsonDocument::fromJson(i.data(), jsonerror));
        if(jsonerror->error != QJsonParseError::NoError){
            std::cout << "json error!";
            return;
        }
        QJsonObject rootObj = jsonDoc->object();
        QStringList keys = rootObj.keys();

        UdpData u(rootObj.value(keys.at(0)).toString(),rootObj.value(keys.at(1)).toString(),
                  rootObj.value(keys.at(2)).toString(),rootObj.value(keys.at(3)).toString());
        udpdatalist.push_back(u);
        delete(jsonDoc);
        delete(jsonerror);
    }
    ui->addressListWidget->clear();
    for(auto data:udpdatalist){
        ui->addressListWidget->addItem(data.name+"("+data.gamemodel+data.chesscolor+data.israndom+")");
    }
    udpdatalist.clear();

}
void NetWindow::receiveBroadcast(){
    port=10127;
    while (!listenSocket.bind(QHostAddress::Any,port)) {
//         if (listenSocket.error() == QAbstractSocket::AddressInUseError) {
//失败绑定
//         }
         port++;
     };
    qDebug()<<"绑定成功！"<<port;

     //收到和出现错误时
     connect(&listenSocket, &QUdpSocket::readyRead, [this]() {
         do {
            QNetworkDatagram datagram=listenSocket.receiveDatagram();
            if(datagramlist.empty()||!(datagramlist.contains(datagram)))
             {
                std::cout<<datagram.data().toStdString();
                datagramlist.push_back(datagram);
                showIpAddress();
             }
            std::cout<<datagramlist.size();
//            {
//                "name":"haung",
//                "gamemodel":"bhj",
//                "chesscolor":"yghj",
//                "israndom":"ghj"
//            }
//             datagram.resize(listenSocket.pendingDatagramSize());
//             listenSocket.readDatagram(datagram.data(), datagram.size(),&host,&port);
         } while (listenSocket.hasPendingDatagrams());
     });
//     connect(&listenSocket, &QUdpSocket::errorOccurred, [this]() {
//         this->ui->logTestEdit->append("连接失败");
//         this->ui->logTestEdit->append(listenSocket.errorString());
//     });
}
void NetWindow::on_openHouseButton_clicked()
{
    while(ui->name==NULL){
        qDebug() << "name is empty!";
    }
    listenSocket.close();
    OpenHouseDialog* openhousedialog=new OpenHouseDialog(nullptr,ui->name->text());
    this->close();
    openhousedialog->show();
}

bool operator ==(const QNetworkDatagram &a,const QNetworkDatagram &b){
    return a.data()==b.data() ? true:false;
}

void NetWindow::updataIp(){
    listenSocket.close();
    ui->addressListWidget->clear();
    datagramlist.clear();
    udpdatalist.clear();
    receiveBroadcast();
//    showIpAddress();
    time->setInterval(1000);
    time->start();

}
