//
// Created by user-kk on 2023/3/26.
//

#ifndef GOBANG_CHESSBOARD_REMOTE_PVP_H
#define GOBANG_CHESSBOARD_REMOTE_PVP_H

#include <QMainWindow>
#include "chessboard.h"
#include "utils.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include <QLabel>
#include <QTimer>
#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui { class Chessboard_Remote_PVP_Abstract; }
QT_END_NAMESPACE

class Chessboard_Remote_PVP_Abstract : public ChessBoard {
Q_OBJECT

public:

    explicit Chessboard_Remote_PVP_Abstract(QWidget *parent = nullptr) = delete;

    explicit Chessboard_Remote_PVP_Abstract(Chess_color color, QWidget *parent = nullptr, int new_game_mode = 0);

    void mousePressEvent(QMouseEvent *event) override;

    void set_restrict_level(int level) override;

    int getPort();

    ~Chessboard_Remote_PVP_Abstract() override;

signals:
    void cancelToMain();

    void refuseLink();

    void gameStart();

protected:
    enum State {
        PAUSE,
        RUNNING,
        TERMINATE
    };
    State state = PAUSE;
    bool isReconnected = false;///< 指示是否为重连的棋局

    void closeEvent(QCloseEvent *event) override;

    void peerMessage(const QString &s);

    void start();

    void pause();

    void systemDo(const QJsonObject &order);

    void systemMessage(const QString &s);

    void myMessage(const QString &s);

    void setState(State newState);

private:
    enum CanChess {
        CAN = 0,
        CANNOT = 1,
        STOP = 2
    };


    struct Time {
        int time = 0;
        QString prefix;
        QLabel *timeLabel = nullptr;
        int restrictTime = -1;
        std::function<void()> f; ///<超时函数
        std::function<void()> f2; ///<快要超时调用的函数
        Time &operator++();
        void clear();
    };

    Ui::Chessboard_Remote_PVP_Abstract *ui{};
    Chess_color myChessColor;


    QTimer repentTimer;
    QTimer timer;


    Time *myLocalTime = nullptr;
    Time *peerLocalTime = nullptr;
    Time *myStepTime = nullptr;
    Time *peerStepTime = nullptr;
    Time *time = nullptr;


    virtual void send(const QString &s) = 0;

    void win(const QString &info);

    void sendMessage();

    void timeUp();

    void gameOver();

    virtual void setActiveExit() = 0;

    virtual void startMessage() = 0;

    virtual void exit() = 0;
};

class Chessboard_Remote_PVP_Server : public Chessboard_Remote_PVP_Abstract {
protected:

private:
    TcpServer *server = nullptr;

    void initNetWork();

    void startMessage() override;

    void send(const QString &s) override;

    void exit() override;

    void setActiveExit() override;

public:
    ~Chessboard_Remote_PVP_Server() override;

    explicit Chessboard_Remote_PVP_Server(Chess_color color, QWidget *parent = nullptr, int new_game_mode = 0);

    int getPort();
};

class Chessboard_Remote_PVP_Client : public Chessboard_Remote_PVP_Abstract {
protected:

private:
    TcpClient *client = nullptr;
    QHostAddress hostAddress;
    int port;

    void initNetWork();

    void startMessage() override;

    void send(const QString &s) override;

    void exit() override;

    void setActiveExit() override;

public:
    Chessboard_Remote_PVP_Client(Chess_color color, const QHostAddress &serverAddress, int serverPort,
                                 QWidget *parent = nullptr, int new_game_mode = 0);

    ~Chessboard_Remote_PVP_Client() override;
};


#endif //GOBANG_CHESSBOARD_REMOTE_PVP_H
