#include "netwindow.h"
#include "ui_netwindow.h"

NetWindow::NetWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetWindow)
{
    ui->setupUi(this);
    receiveBroadcast();
    connect(ui->forbiddenCombolBox,&QComboBox::currentTextChanged,this,&NetWindow::filterSlot);
    connect(ui->colorCombolBox,&QComboBox::currentTextChanged,this,&NetWindow::filterSlot);
    time=new QTimer(this);
    connect(time,&QTimer::timeout,this,&NetWindow::updataIp);
    time->start(10000);
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
            QMessageBox::critical(this,"消息提示框","包解析错误！");
            return;
        }
        QJsonObject rootObj = jsonDoc->object();
        QStringList keys = rootObj.keys();

        UdpData u(rootObj.value(keys.at(0)).toString(),rootObj.value(keys.at(1)).toString(),
                  rootObj.value(keys.at(2)).toString(),rootObj.value(keys.at(3)).toString(),rootObj.value(keys.at(4)).toString());
        udpdatalist.push_back(u);
        delete(jsonDoc);
        delete(jsonerror);
    }
    filterAddress();
    udpdatalist.clear();

}
void NetWindow::filterSlot(){
    filterAddress();
}
void NetWindow::filterAddress(){
    ui->addressListWidget->clear();
    for(auto data:udpdatalist){
        if(data.gamemodel==ui->forbiddenCombolBox->currentData())
            if(data.chesscolor==ui->colorCombolBox->currentData()||(data.israndom=="1"&&ui->colorCombolBox->currentData()=="随机"))
                ui->addressListWidget->addItem(data.name+"("+data.gamemodel+data.chesscolor+data.israndom+")");
    }
}
void NetWindow::receiveBroadcast(){
    port=10124;
    while (!listenSocket.bind(QHostAddress::Any,port)) {
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
         } while (listenSocket.hasPendingDatagrams());
     });
}
void NetWindow::on_openHouseButton_clicked()
{
    if(ui->name->text()!=""){
        listenSocket.close();
        openhousedialog=new OpenHouseDialog(nullptr,ui->name->text());
        connect(openhousedialog,&OpenHouseDialog::back,this,&NetWindow::backSlot);
        connect(openhousedialog,&OpenHouseDialog::cancel,this,&NetWindow::cancelSlot);
        this->hide();
        openhousedialog->show();
    }else{
        QMessageBox::critical(this, "消息提示框", "用户名不能为空！");
    }
}
void NetWindow::backSlot(){
    openhousedialog->hide();
    delete(openhousedialog);
    this->show();
}
void NetWindow::cancelSlot(){
    openhousedialog->hide();
    delete(openhousedialog);
    emit backToMain();
}
void NetWindow::closeEvent(QCloseEvent *e){
    listenSocket.close();
    emit backToMain();
}
bool operator ==(const QNetworkDatagram &a,const QNetworkDatagram &b){
    return a.data()==b.data();
}

void NetWindow::updataIp(){
    listenSocket.close();
    ui->addressListWidget->clear();
    datagramlist.clear();
    udpdatalist.clear();
    receiveBroadcast();
}
