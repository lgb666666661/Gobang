#include "netwindow.h"
#include "ui_netwindow.h"

NetWindow::NetWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetWindow)
{
    ui->setupUi(this);
    receiveBroadcast();
    connect(&listenSocket, &QUdpSocket::readyRead, [this]() {
        do {
           QNetworkDatagram datagram=listenSocket.receiveDatagram();
           if(datagramlist.empty()||!(datagramlist.contains(datagram)))
            {
               qDebug()<<QString(datagram.data());
               datagramlist.push_back(datagram);
               showIpAddress();
            }
           std::cout<<datagramlist.size();
        } while (listenSocket.hasPendingDatagrams());
    });
    connect(ui->forbiddenCombolBox,&QComboBox::currentTextChanged,this,&NetWindow::filterSlot);
    connect(ui->colorCombolBox,&QComboBox::currentTextChanged,this,&NetWindow::filterSlot);
    time=new QTimer(this);
    connect(time,&QTimer::timeout,this,&NetWindow::updataIp);
    connect(ui->addressListWidget,&QListWidget::doubleClicked,this,&NetWindow::beginGame);
    time->start(10000);
}
NetWindow::~NetWindow()
{
    delete ui;
}
void NetWindow::showGame(){
    qDebug()<<"hjkl";
    time->stop();
    this->hide();
    rPVP2->show();
}
void NetWindow::refuseLink(){
    delete rPVP2;
    QMessageBox::information(this,"提示框","连接已过期！",QMessageBox::Ok);
    updataIp();
}
void NetWindow::showIpAddress(){
    udpdatalist.clear();
    for(auto i:datagramlist){
        QJsonParseError* jsonerror=new QJsonParseError();;
        QJsonDocument* jsonDoc=new QJsonDocument();
        *jsonDoc=(QJsonDocument::fromJson(i.data(), jsonerror));
        if(jsonerror->error != QJsonParseError::NoError){
            qDebug()<<jsonerror->errorString();
            QMessageBox::critical(this,"消息提示框","包解析错误！");
            return;
        }
        QJsonObject rootObj = jsonDoc->object();
        QStringList keys = rootObj.keys();
        UdpData u(rootObj.value(keys.at(3)).toString(),rootObj.value(keys.at(1)).toString(),
                  rootObj.value(keys.at(0)).toString(),rootObj.value(keys.at(2)).toString(),rootObj.value(keys.at(4)).toString());
        udpdatalist.push_back(u);
        delete jsonDoc;
        delete jsonerror;
    }
    filterAddress();
}
void NetWindow::filterSlot(){
    filterAddress();
}
void NetWindow::filterAddress(){
    ui->addressListWidget->clear();
    qDebug()<<udpdatalist.size();
    for(auto data:udpdatalist){
        qDebug()<<data.gamemodel;
        qDebug()<<ui->forbiddenCombolBox->currentText();
        if(data.gamemodel==ui->forbiddenCombolBox->currentText())
            if(data.chesscolor==ui->colorCombolBox->currentText()||(data.israndom=="1"&&ui->colorCombolBox->currentText()=="随机"))
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
    delete openhousedialog;
    this->show();
}
void NetWindow::cancelSlot(){
    delete rPVP2;
    delete openhousedialog;
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
void NetWindow::beginGame(){
   int index=ui->addressListWidget->currentRow();
   qDebug()<<index;
   qDebug()<<datagramlist.size();
   qDebug()<<udpdatalist.size();
   QNetworkDatagram d=datagramlist.at(index);
   QHostAddress h=d.senderAddress();
   UdpData d1=udpdatalist.at(index);
   int p=d1.port.toInt();
   int model=0;
   if(d1.gamemodel=="有禁手"){
       model=1;
   }
   delete rPVP2;
   if(d1.chesscolor=="白棋"){
       rPVP2=new Chessboard_Remote_PVP_Client(WHITE, h,p,nullptr,
                                   model);
   }else{
       rPVP2=new Chessboard_Remote_PVP_Client(BLACK, h,p,nullptr,
                                 model);
   }
   connect(rPVP2,&Chessboard_Remote_PVP_Client::refuseLink,this,&NetWindow::refuseLink);
   connect(rPVP2,&Chessboard_Remote_PVP_Client::gameStart,this,&NetWindow::showGame);
   connect(rPVP2,&Chessboard_Remote_PVP_Client::cancel,this,&NetWindow::cancelSlot);
   qDebug()<<h<<p;
}
