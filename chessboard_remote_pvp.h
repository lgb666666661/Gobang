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


///@class Chessboard_Remote_PVP_Abstract
///@brief 远程对局的棋盘抽象类
class Chessboard_Remote_PVP_Abstract : public ChessBoard {
Q_OBJECT

public:

    explicit Chessboard_Remote_PVP_Abstract(QWidget *parent = nullptr) = delete;

    ///@brief 构造函数
    ///@param color 本方的棋子颜色 见@ref Chess_color
    ///@param parent 要绑定的父对象的指针
    ///@param new_game_mode 有无禁手 见@ref game_mode
    explicit Chessboard_Remote_PVP_Abstract(Chess_color color, QWidget *parent = nullptr, int new_game_mode = 0);

    ///@brief 鼠标事件,根据鼠标的预选位置(棋盘上的红点)下棋,调用@ref send 发送落子的信息
    void mousePressEvent(QMouseEvent *event) override;

    ///@brief 设置棋盘是否能落子,如果在能落或不能落的状态之间切换成功,会更新步时和局时的计数器以及悔棋按钮的可触发性,以及更新本轮落子的Label
    ///@param level @see
    void set_restrict_level(int level) override;

    ~Chessboard_Remote_PVP_Abstract() override;

signals:
    ///@brief 当客户端想要连接服务器,但是服务器已经和另一个客户端建立连接了,会触发这个信号,表示拒绝连接
    void refuseLink();

    ///@brief 对局开始会触发这个信号,一个对局只能触发一次
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
