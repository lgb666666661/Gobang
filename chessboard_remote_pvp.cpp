//
// Created by user-kk on 2023/3/26.
//

// You may need to build the project (run Qt uic code generator) to get
// "ui_Chessboard_Remote_PVP.h" resolved

#include "chessboard_remote_pvp.h"

#include <QJsonDocument>
#include <QMessageBox>

#include "ui_Chessboard_Remote_PVP.h"
#include "chessboard_fupan.h"

Chessboard_Remote_PVP_Abstract::~Chessboard_Remote_PVP_Abstract() {
    delete ui;
    delete myLocalTime;
    delete peerLocalTime;
    delete myStepTime;
    delete peerStepTime;
    delete time;
}

Chessboard_Remote_PVP_Abstract::Chessboard_Remote_PVP_Abstract(Chess_color color,
                                                               QWidget *parent, int new_game_mode)
        : ChessBoard(parent, new_game_mode),
          ui(new Ui::Chessboard_Remote_PVP_Abstract),
          myChessColor(color) {
    ui->setupUi(this);

    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
    setWindowState(Qt::WindowMaximized);

    //初始化计时label
    myLocalTime = new Time{
            0, "你的总局时", ui->myLocalTimeLabel, 600, [this]() {
                QJsonObject object;
                object.insert("type", "system");
                object.insert("do", "userTimeout");
                object.insert("win", (myChessColor == WHITE) ? "black" : "white");
                QString info;
                if (myChessColor == WHITE) {
                    info.append("黑棋胜:");
                } else {
                    info.append("白棋胜: ");
                }
                info.append("局时超时");
                systemMessage(info);
                object.insert("winInfo", info);
                send(QString(QJsonDocument(object).toJson(QJsonDocument::Compact)));
                gameOver();
                win(info);
            }};
    peerLocalTime = new Time{0, "对方的总局时", ui->peerLocalTimeLabel};
    myStepTime = new Time{
            0, "你的本轮步时", ui->myStepTimeLabel, 120, [this]() {
                QJsonObject object;
                object.insert("type", "system");
                object.insert("do", "userTimeout");
                object.insert("win", (myChessColor == WHITE) ? "black" : "white");
                QString info;
                if (myChessColor == WHITE) {
                    info.append("黑棋胜:");
                } else {
                    info.append("白棋胜: ");
                }
                info.append("步时超时");
                systemMessage(info);
                object.insert("winInfo", info);
                send(QString(QJsonDocument(object).toJson(QJsonDocument::Compact)));
                gameOver();
                win(info);
            }, [this]() {
                systemMessage("你的步时快要达到限制了,请快落子");
            }};
    peerStepTime = new Time{0, "对方的本轮步时", ui->peerStepTimeLabel};
    time = new Time{0, "总时长:", ui->timeLabel};

    //设置第一步下棋,第一步不能悔棋
    restrict_level = myChessColor == BLACK ? CAN : CANNOT;
    ui->myChessColorGroupBox->setTitle(myChessColor == BLACK ? "黑棋方"
                                                             : "白棋方");
    ui->repentButton->setDisabled(true);

    //设置悔棋计时器
    connect(&repentTimer, &QTimer::timeout, this, [this]() {
        systemMessage("对方没有回应你的悔棋....");
        this->repentTimer.stop();
    });

    //设置总计时器
    connect(&timer, &QTimer::timeout, this, [this]() { timeUp(); });

    //设置ui中显示当前状态
    ui->linkLabel->setText("连接状态:连接中断");

    //设置ui显示当前规则
    ui->forbidLabel->setText(QString("开局规则:") +
                             (new_game_mode == 0 ? "无禁手" : "有禁手"));

    //设置ui中显示本轮落子
    ui->currentColorLabel->setText(QString("本轮落子:黑棋"));

    //连接ui
    connect(ui->sendButton, &QPushButton::clicked, this,
            [this]() { sendMessage(); });
    connect(ui->repentButton, &QPushButton::clicked, this, [this]() {
        ui->repentButton->setDisabled(true);
        QJsonObject object;
        object.insert("type", "system");
        object.insert("do", "requestRepent");
        send(QString(QJsonDocument(object).toJson(QJsonDocument::Compact)));
        repentTimer.start(15000);
    });
}

void Chessboard_Remote_PVP_Abstract::mousePressEvent(QMouseEvent *event) {
    if (linkState == TERMINATE || linkState == PAUSE) {
        return;
    }
    if (nearx >= 0 && neary >= 0) {
        if (restrict_level == CANNOT) {
            systemMessage("请等待对方下棋");
            return;
        }
        //创建发送的json
        QJsonObject object;
        object.insert("type", "system");
        object.insert("do", "chess");
        object.insert("x", nearx);
        object.insert("y", neary);
        object.insert("color", myChessColor == BLACK ? "black" : "white");
        chess({nearx, neary}, myChessColor);
        set_restrict_level(CANNOT);
        if (game_status != NOBODY_WINS) {
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
            object.insert("isWin", "yes");
            object.insert("winInfo", info);
            send(QString(QJsonDocument(object).toJson(QJsonDocument::Compact)));
            gameOver();
            win(info);
        } else {
            object.insert("isWin", "no");
            send(QString(QJsonDocument(object).toJson(QJsonDocument::Compact)));
        }
    }
}

void Chessboard_Remote_PVP_Abstract::systemMessage(const QString &s) {
    ui->logEdit->append("系统:" + s);
}

void Chessboard_Remote_PVP_Abstract::start() {

    //如果不是重连的，发送游戏信号，使UDP停止广播
    if (!isReconnected) {
        emit gameStart();
    }
    //打印开始信息
    startMessage();
    isReconnected = true;//棋局开始后,后面的连接都是重连的

    setState(RUNNING);
    //发送棋盘同步信息
    QJsonObject object;
    object.insert("type", "system");
    object.insert("do", "reconnect");
    object.insert("record", int(record.size()));
    send(QString(QJsonDocument(object).toJson(QJsonDocument::Compact)));
    timer.start(1000);
}

void Chessboard_Remote_PVP_Abstract::pause() {//连接失败等待重连
    systemMessage("网络波动,对方断开连接,游戏暂停");
    setState(PAUSE);
    timer.stop();
}

void Chessboard_Remote_PVP_Abstract::systemDo(const QJsonObject &order) {

    QString willDo = order.value("do").toString();

    //判断命令，进行相应动作
    if (willDo == "chess") {
        chess({order.value("x").toInt(), order.value("y").toInt()},
              (order.value("color").toString() == "black") ? BLACK : WHITE);
        set_restrict_level(CAN);  //可以下
        if (order.value("isWin").toString() == "yes") {
            gameOver();
            win(order.value("winInfo").toString());
            systemMessage(order.value("winInfo").toString());
        }
    } else if (willDo == "userTimeout") {
        gameOver();
        win(order.value("winInfo").toString());
        systemMessage(order.value("winInfo").toString());
    } else if (willDo == "terminate") {
        systemMessage("对方断开连接,游戏终止");
        setActiveExit();
        gameOver();
    } else if (willDo == "requestRepent") {
        auto questionDialog = new QMessageBox(
                QMessageBox::Question, "悔棋", "对方向你发起一个悔棋操作,是否同意?",
                QMessageBox::NoButton, this);
        auto agree = questionDialog->addButton("同意", QMessageBox::AcceptRole);
        auto disagree =
                questionDialog->addButton("拒绝", QMessageBox::RejectRole);
        questionDialog->show();
        connect(questionDialog, &QMessageBox::buttonClicked, this,
                [this, agree, disagree](QAbstractButton *button) {
                    if (button == (QAbstractButton *) agree) {
                        QJsonObject object;
                        object.insert("type", "system");
                        object.insert("do", "agreeRepent");
                        send(QString(QJsonDocument(object).toJson(
                                QJsonDocument::Compact)));
                        cancel();
                        set_restrict_level(CANNOT);
                        systemMessage("你同意了对方的悔棋请求");
                    } else {
                        QJsonObject object;
                        object.insert("type", "system");
                        object.insert("do", "disagreeRepent");
                        send(QString(QJsonDocument(object).toJson(
                                QJsonDocument::Compact)));
                        systemMessage("你拒绝了对方的悔棋请求");
                    }
                });

    } else if (willDo == "agreeRepent") {
        cancel();
        set_restrict_level(CAN);
        systemMessage("对方同意了你的悔棋请求");
        repentTimer.stop();
    } else if (willDo == "disagreeRepent") {
        systemMessage("对方拒绝了你的悔棋请求");
        repentTimer.stop();
    } else if (willDo == "refuse") {
        emit refuseLink();
    } else if (willDo == "reconnect") {
        int peerRecordSize = order.value("record").toInt();
        if (int(record.size()) < peerRecordSize) {//如果我比对方的小,我不用动,等待对方撤回一个子
            return;
        } else if (int(record.size()) == peerRecordSize) {//如果我和对方一样,为了防止悔棋的同意信号没有被发出,重新设置先后手
            //重新设置先后手
            if (myChessColor == BLACK) {
                set_restrict_level(CAN);
            } else {
                set_restrict_level(CANNOT);
            }
        } else { //如果我比对方大
            cancel();
            set_restrict_level(CAN);
        }
    }
}

void Chessboard_Remote_PVP_Abstract::win(const QString &info) {

    chessboard_fupan::save_data(this->record,info);
    if (info.contains("黑棋")) {
        ui->resultLabel->setText(QString("对局结果:") + "黑棋胜");
    } else {
        ui->resultLabel->setText(QString("对局结果:") + "白棋胜");
    }

    auto questionDialog = new QMessageBox(
            QMessageBox::Information, "游戏结束", info + "           ",
            QMessageBox::NoButton, this);

    questionDialog->show();
}

void Chessboard_Remote_PVP_Abstract::closeEvent(QCloseEvent *event) {
    setActiveExit();
    if (linkState == TERMINATE || linkState == PAUSE) {
        emit cancelToMain();
        return;
    }
    exit();
    emit cancelToMain();
}

void Chessboard_Remote_PVP_Abstract::sendMessage() {
    if (linkState == PAUSE || linkState == TERMINATE) {
        systemMessage("已断开连接,无法发送");
        return;
    }
    QJsonObject object;
    object.insert("type", "user");
    object.insert("message", ui->textEdit->toPlainText());
    myMessage(ui->textEdit->toPlainText());
    send(QString(QJsonDocument(object).toJson(QJsonDocument::Compact)));
}

void Chessboard_Remote_PVP_Abstract::myMessage(const QString &s) {
    ui->logEdit->append("我:" + s);
}

void Chessboard_Remote_PVP_Abstract::peerMessage(const QString &s) {
    ui->logEdit->append("对方:" + s);
}

void Chessboard_Remote_PVP_Abstract::setState(LinkState newState) {
    if (newState == TERMINATE || newState == PAUSE) {
        set_restrict_level(STOP);
    }

    ui->linkLabel->setText(
            QString("连接状态:") +
            ((newState == TERMINATE)
             ? "连接结束"
             : (newState == PAUSE ? "连接中断" : "连接正常")));
    linkState = newState;
}

void Chessboard_Remote_PVP_Abstract::set_restrict_level(int level) {
    if (restrict_level == CAN && level == CANNOT) {
        myStepTime->clear();
    }
    if (restrict_level == CANNOT && level == CAN) {
        peerStepTime->clear();
    }

    ChessBoard::set_restrict_level(level);
    if (record.empty()) {//当前记录为空时不能悔棋
        ui->repentButton->setDisabled(true);
    } else {
        if (level == CAN || level == STOP) {
            ui->repentButton->setDisabled(true);
        } else {
            ui->repentButton->setDisabled(false);
        }
    }

    if (level == STOP) {
        ui->currentColorLabel->setText("本轮落子:");
        return;
    }
    ui->currentColorLabel->setText(QString("本轮落子:") +
                                   (myChessColor == BLACK
                                    ? (level == CAN ? "黑棋" : "白棋")
                                    : (level == CAN ? "白棋" : "黑棋")));
}

void Chessboard_Remote_PVP_Abstract::timeUp() {
    if (linkState == PAUSE || linkState == TERMINATE) {
        return;
    }

    if (restrict_level == CAN) {
        ++(*myLocalTime);
        ++(*myStepTime);
        ++(*time);
    }
    if (restrict_level == CANNOT) {
        ++(*peerLocalTime);
        ++(*peerStepTime);
        ++(*time);
    }
}

void Chessboard_Remote_PVP_Abstract::gameOver() {
    setState(TERMINATE);
    timer.stop();
}


Chessboard_Remote_PVP_Abstract::Time &Chessboard_Remote_PVP_Abstract::Time::operator++() {
    time++;
    if (restrictTime != -1 && time > restrictTime) {
        f();
    }
    if (restrictTime != -1 && time > restrictTime * 0.8) {
        f2();
    }
    if (timeLabel != nullptr)
        timeLabel->setText(prefix + QString::number(time / 60) + "分" + QString::number(time % 60) + "秒");
    return *this;
}

void Chessboard_Remote_PVP_Abstract::Time::clear() {
    time = 0;
    if (timeLabel != nullptr)
        timeLabel->setText(prefix + "0分" + "0秒");
}

////////////////////////以下是两个实体类/////////////////////////////////////////////////////////////////////


void Chessboard_Remote_PVP_Server::initNetWork() {
    server = new TcpServer();
    connect(server, &TcpServer::connected, this, [this]() { start(); });
    connect(server, &TcpServer::disconnected, this, [this]() {
        if (linkState != TERMINATE && linkState != PAUSE) {
            pause();
        }
    });
    connect(server, &TcpAbstract::systemDo, this,
            [this](const QJsonObject &object) { systemDo(object); });
    connect(server, &TcpAbstract::userDo, this,
            [this](const QJsonObject &object) {
                peerMessage(object.value("message").toString());
            });
}

Chessboard_Remote_PVP_Server::~Chessboard_Remote_PVP_Server() {
    delete server;
}

Chessboard_Remote_PVP_Server::Chessboard_Remote_PVP_Server(Chess_color color, QWidget *parent, int new_game_mode)
        : Chessboard_Remote_PVP_Abstract(color, parent, new_game_mode) {
    initNetWork();
}

int Chessboard_Remote_PVP_Server::getPort() {
    return server->getPort();
}

void Chessboard_Remote_PVP_Server::startMessage() {
    if (!isReconnected) {
        systemMessage("XXX已进入房间,游戏开始");
    } else {
        systemMessage("对局重连成功,游戏继续");
    }
}

void Chessboard_Remote_PVP_Server::send(const QString &s) {
    server->send(s);
}

void Chessboard_Remote_PVP_Server::exit() {
    server->stop();
}

void Chessboard_Remote_PVP_Server::setActiveExit() {
    server->setActiveExit();
}

void Chessboard_Remote_PVP_Client::initNetWork() {
    client = new TcpClient(QHostAddress(hostAddress), port);
    connect(client, &TcpClient::connected, this, [this]() { start(); });
    connect(client, &TcpClient::disconnected, this, [this]() {
        if (linkState != TERMINATE && linkState != PAUSE) {
            pause();
        }
    });
    connect(client, &TcpAbstract::systemDo, this,
            [this](const QJsonObject &object) { systemDo(object); });
    connect(client, &TcpAbstract::userDo, this,
            [this](const QJsonObject &object) {
                peerMessage(object.value("message").toString());
            });
}

Chessboard_Remote_PVP_Client::~Chessboard_Remote_PVP_Client() {
    delete client;
}

Chessboard_Remote_PVP_Client::Chessboard_Remote_PVP_Client(Chess_color color, const QHostAddress &serverAddress,
                                                           int serverPort, QWidget *parent, int new_game_mode) :
        Chessboard_Remote_PVP_Abstract(color, parent, new_game_mode), hostAddress(serverAddress), port(serverPort) {
    initNetWork();
}

void Chessboard_Remote_PVP_Client::startMessage() {
    if (!isReconnected) {
        systemMessage("连接成功,游戏开始");
    } else {
        systemMessage("对局重连成功,游戏继续");
    }
}

void Chessboard_Remote_PVP_Client::send(const QString &s) {
    client->send(s);
}

void Chessboard_Remote_PVP_Client::exit() {
    client->stop();
}

void Chessboard_Remote_PVP_Client::setActiveExit() {
    client->setActiveExit();
}

