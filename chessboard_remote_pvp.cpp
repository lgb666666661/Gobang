//
// Created by user-kk on 2023/3/26.
//

// You may need to build the project (run Qt uic code generator) to get
// "ui_Chessboard_Remote_PVP.h" resolved

#include "chessboard_remote_pvp.h"

#include <QJsonDocument>
#include <QMessageBox>

#include "ui_Chessboard_Remote_PVP.h"

Chessboard_Remote_PVP::~Chessboard_Remote_PVP() {
    delete ui;
    delete server;
    delete client;
    delete myLocalTime;
    delete peerLocalTime;
    delete myStepTime;
    delete peerStepTime;
    delete time;
}

Chessboard_Remote_PVP::Chessboard_Remote_PVP(Chess_color color, Mode mode,
                                             QWidget *parent, int new_game_mode,
                                             const QString &hostAddress,
                                             int port)
        : ChessBoard(parent, new_game_mode),
          ui(new Ui::Chessboard_Remote_PVP),
          myChessColor(color),
          chessMode(mode) {
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
            0, "你的本轮步时", ui->myStepTimeLabel, 20, [this]() {
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

    //设置网络连接
    if (chessMode == SERVER) {
        server = new TcpServer();
        connect(server, &TcpAbstract::connected, this, [this]() { start(); });
        connect(server, &TcpAbstract::disconnected, this, [this]() {
            if (state != TERMINATE && state != PAUSE) {
                pause();
            }
        });
        connect(server, &TcpAbstract::systemDo, this,
                [this](const QJsonObject &object) { systemDo(object); });
        connect(server, &TcpAbstract::userDo, this,
                [this](const QJsonObject &object) {
                    peerMessage(object.value("message").toString());
                });
    } else {
        client = new TcpClient(QHostAddress(hostAddress), port);
        connect(client, &TcpServer::connected, this, [this]() { start(); });
        connect(client, &TcpAbstract::disconnected, this, [this]() {
            if (state != TERMINATE && state != PAUSE) {
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

void Chessboard_Remote_PVP::mousePressEvent(QMouseEvent *event) {
    if (state == TERMINATE || state == PAUSE) {
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

void Chessboard_Remote_PVP::systemMessage(const QString &s) {
    ui->logEdit->append("系统:" + s);
}

int Chessboard_Remote_PVP::getPort() {
    assert(chessMode == SERVER);
    return server->getPort();
}

void Chessboard_Remote_PVP::start() {
    //开启计时器,开始对局
    systemMessage(chessMode == SERVER ? "XXX已进入房间,游戏开始"
                                      : "连接成功,游戏开始");
    setState(RUNNING);
    timer.start(1000);
}

void Chessboard_Remote_PVP::pause() {
    // todo:连接失败等待重连
    systemMessage("网络波动,对方断开连接,游戏暂停");
    setState(PAUSE);
    timer.stop();
}

void Chessboard_Remote_PVP::systemDo(const QJsonObject &order) {
    if (order.value("do").toString() == "chess") {
        chess({order.value("x").toInt(), order.value("y").toInt()},
              (order.value("color").toString() == "black") ? BLACK : WHITE);
        set_restrict_level(CAN);  //可以下
        if (order.value("isWin").toString() == "yes") {
            gameOver();
            win(order.value("winInfo").toString());
            systemMessage(order.value("winInfo").toString());
        }
    } else if (order.value("do").toString() == "userTimeout") {
        gameOver();
        win(order.value("winInfo").toString());
        systemMessage(order.value("winInfo").toString());
    } else if (order.value("do").toString() == "terminate") {
        systemMessage("对方断开连接,游戏终止");
        gameOver();
    } else if (order.value("do").toString() == "requestRepent") {
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

    } else if (order.value("do").toString() == "agreeRepent") {
        cancel();
        set_restrict_level(CAN);
        systemMessage("对方同意了你的悔棋请求");
        repentTimer.stop();
    } else if (order.value("do").toString() == "disagreeRepent") {
        systemMessage("对方拒绝了你的悔棋请求");
        repentTimer.stop();
    } else if (order.value("do").toString() == "refuse") {
        emit refuseLink();
    }
}

void Chessboard_Remote_PVP::send(const QString &s) {
    if (chessMode == SERVER) {
        server->send(s);
    } else {
        client->send(s);
    }
}

void Chessboard_Remote_PVP::win(const QString &info) {
    if (info.contains("黑棋")) {
        ui->resultLabel->setText(QString("对局结果:") + "黑棋胜");
    } else {
        ui->resultLabel->setText(QString("对局结果:") + "白棋胜");
    }
    GameOver dialog;
    dialog.setLabel(info);
    dialog.exec();
}

void Chessboard_Remote_PVP::closeEvent(QCloseEvent *event) {
    if (chessMode == SERVER) {
        server->stop();
    } else {
        client->stop();
    }
}

void Chessboard_Remote_PVP::sendMessage() {
    if (state == PAUSE || state == TERMINATE) {
        systemMessage("已断开连接,无法发送");
        return;
    }
    QJsonObject object;
    object.insert("type", "user");
    object.insert("message", ui->textEdit->toPlainText());
    myMessage(ui->textEdit->toPlainText());
    send(QString(QJsonDocument(object).toJson(QJsonDocument::Compact)));
}

void Chessboard_Remote_PVP::myMessage(const QString &s) {
    ui->logEdit->append("我:" + s);
}

void Chessboard_Remote_PVP::peerMessage(const QString &s) {
    ui->logEdit->append("对方:" + s);
}

void Chessboard_Remote_PVP::setState(State newState) {
    if (newState == TERMINATE || newState == PAUSE) {
        set_restrict_level(STOP);
    }

    ui->linkLabel->setText(
            QString("连接状态:") +
            ((newState == TERMINATE)
             ? "连接结束"
             : (newState == PAUSE ? "连接中断" : "连接正常")));
    state = newState;
}

void Chessboard_Remote_PVP::set_restrict_level(int level) {
    if (restrict_level == CAN && level == CANNOT) {
        myStepTime->clear();
    }
    if (restrict_level == CANNOT && level == CAN) {
        peerStepTime->clear();
    }

    ChessBoard::set_restrict_level(level);
    if (level == CAN || level == STOP) {
        ui->repentButton->setDisabled(true);
    } else {
        ui->repentButton->setDisabled(false);
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

void Chessboard_Remote_PVP::timeUp() {
    if (state == PAUSE || state == TERMINATE) {
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

void Chessboard_Remote_PVP::gameOver() {
    setState(TERMINATE);
    timer.stop();
}
