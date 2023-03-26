//
// Created by user-kk on 2023/3/26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Chessboard_Remote_PVP.h" resolved

#include "chessboard_remote_pvp.h"
#include "ui_Chessboard_Remote_PVP.h"
#include <QMessageBox>
#include <QJsonDocument>


Chessboard_Remote_PVP::~Chessboard_Remote_PVP() {
    delete ui;
    delete server;
    delete client;

}

Chessboard_Remote_PVP::Chessboard_Remote_PVP(Chess_color color, Mode mode, QWidget *parent, int new_game_mode,const QString& hostAddress,int port)
        : ChessBoard(parent, new_game_mode),
          ui(new Ui::Chessboard_Remote_PVP),
          myChessColor(color),
          chessMode(mode) {
    ui->setupUi(this);
    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
    setWindowState(Qt::WindowMaximized);
    //设置第一步下棋
    if (myChessColor == BLACK) {
        restrict_level = CAN;//可以下
    } else {
        restrict_level = CANNOT;//等对方下
    }
    if (chessMode == SERVER) {
        server = new TcpServer();
        connect(server,&TcpAbstract::connected,this,[this](){
            systemMessage("XXX已进入房间,游戏开始");
            start();
        });
        connect(server,&TcpAbstract::disconnected,this,[this](){
            systemMessage("网络波动,对方断开连接,游戏暂停");
            pause();
        });
        connect(server,&TcpAbstract::systemDo,this,[this](const QJsonObject& object){
            systemDo(object);
        });
        connect(server,&TcpAbstract::userDo,this,[this](const QJsonObject& object){
            peerMessage(object.value("message").toString());
        });
    }else{
        client=new TcpClient(QHostAddress(hostAddress),port);
        connect(client,&TcpServer::connected,this,[this](){
            systemMessage("连接成功,游戏开始");
            start();
        });
        connect(client,&TcpAbstract::disconnected,this,[this](){
            systemMessage("网络波动,对方断开连接,游戏暂停");
            pause();
        });
        connect(client,&TcpAbstract::systemDo,this,[this](const QJsonObject& object){
            systemDo(object);
        });
        connect(client,&TcpAbstract::userDo,this,[this](const QJsonObject& object){
            peerMessage(object.value("message").toString());
        });
    }
    connect(ui->sendButton,&QPushButton::clicked,this,[this](){
        sendMessage();
    });

}

void Chessboard_Remote_PVP::mousePressEvent(QMouseEvent *event) {
    if (restrict_level == CANNOT) {
        systemMessage("请等待对方下棋");
        return;
    } else if (restrict_level == STOP) {
        return;
    }

    if (nearx >= 0 && neary >= 0) {
        //创建发送的json
        QJsonObject object;
        object.insert("type","system");
        object.insert("do","chess");
        object.insert("x",nearx);
        object.insert("y",neary);
        object.insert("color",myChessColor==BLACK?"black":"white");
        chess({nearx, neary}, myChessColor);
        restrict_level=CANNOT;
        if (game_status != NOBODY_WINS) {
            set_restrict_level(STOP);
            QString info;
            if (game_status == BLACK_WINS) {
                info.append("黑棋胜");
            } else {
                info.append("白棋胜: ");
                if (value == 1) info.append("长连禁手");
                if (value == 2) info.append("四四禁手");
                if (value == 3) info.append("三三禁手");
            }
            systemMessage(info);
            object.insert("isWin","yes");
            object.insert("winInfo",info);
            send(QString(QJsonDocument(object).toJson()));
            win(info);
        }else{
            object.insert("isWin","no");
            send(QString(QJsonDocument(object).toJson()));
        }
    }
}

void Chessboard_Remote_PVP::systemMessage(const QString &s) {
    ui->logEdit->append("系统:" + s);
}

int Chessboard_Remote_PVP::getPort() {
    assert(chessMode==SERVER);
    return server->getPort();
}

void Chessboard_Remote_PVP::start() {
    //todo:开启计时器,开始对局
    isDisconnected= false;
}

void Chessboard_Remote_PVP::pause() {
    //todo:连接失败等待重连
    restrict_level=STOP;
    isDisconnected=true;
}

void Chessboard_Remote_PVP::systemDo(const QJsonObject& order) {
    if(order.value("do").toString()=="chess"){
        chess({order.value("x").toInt(),order.value("y").toInt()},(order.value("color").toString()=="black")?BLACK:WHITE);
        restrict_level = CAN;//可以下
        if(order.value("win").toString()=="yes"){
            win(order.value("winInfo").toString());
            systemMessage(order.value("winInfo").toString());
            restrict_level=STOP;
        }
    }
}

void Chessboard_Remote_PVP::send(const QString &s) {
    if(chessMode==SERVER){
        server->send(s);
    }else{
        client->send(s);
    }
}

void Chessboard_Remote_PVP::win(const QString &info) {
    GameOver dialog;
    dialog.setLabel(info);
    dialog.exec();
}

void Chessboard_Remote_PVP::closeEvent(QCloseEvent *event) {
    if(chessMode==SERVER){
        server->stop();
    }else{
        client->stop();
    }
}

void Chessboard_Remote_PVP::sendMessage() {
    if(isDisconnected){
        systemMessage("已断开连接,无法发送");
        return;
    }
    QJsonObject object;
    object.insert("type","user");
    object.insert("message",ui->textEdit->toPlainText());
    myMessage(ui->textEdit->toPlainText());
    send(QString(QJsonDocument(object).toJson()));
}

void Chessboard_Remote_PVP::myMessage(const QString &s) {
    ui->logEdit->append("我:"+s);
}

void Chessboard_Remote_PVP::peerMessage(const QString &s) {
    ui->logEdit->append("对方:"+s);
}

