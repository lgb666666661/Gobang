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
namespace Ui { class Chessboard_Remote_PVP; }
QT_END_NAMESPACE

class Chessboard_Remote_PVP : public ChessBoard {
Q_OBJECT

public:
    enum Mode {
        SERVER,
        CLIENT
    };

    explicit Chessboard_Remote_PVP(QWidget *parent = nullptr) = delete;

    explicit Chessboard_Remote_PVP(Chess_color color, Mode mode, QWidget *parent = nullptr, int new_game_mode = 0,
                                   const QString &hostAddress = "", int port = 0);

    void mousePressEvent(QMouseEvent *event) override;

    void set_restrict_level(int level) override;

    int getPort();

    ~Chessboard_Remote_PVP() override;

signals:
    void refuseLink();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    enum CanChess {
        CAN = 0,
        CANNOT = 1,
        STOP = 2
    };
    enum State {
        PAUSE,
        RUNNING,
        TERMINATE
    };

    struct Time {
        int time = 0;
        QString prefix;
        QLabel *timeLabel = nullptr;
        int restrictTime = -1;
        std::function<void()> f; ///<超时函数
        std::function<void()> f2; ///<快要超时调用的函数
        Time &operator++() {
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

        void clear() {
            time = 0;
            if (timeLabel != nullptr)
                timeLabel->setText(prefix + "0分" + "0秒");
        }
    };

    Ui::Chessboard_Remote_PVP *ui{};
    Mode chessMode;
    Chess_color myChessColor;
    TcpServer *server = nullptr;
    TcpClient *client = nullptr;
    State state = PAUSE;
    QTimer repentTimer;
    QTimer timer;
    bool isReconnected=false;///< 指示是否为重连的棋局


    Time *myLocalTime = nullptr;
    Time *peerLocalTime = nullptr;
    Time *myStepTime = nullptr;
    Time *peerStepTime = nullptr;
    Time *time = nullptr;


    void systemMessage(const QString &s);

    void myMessage(const QString &s);

    void peerMessage(const QString &s);

    void start();

    void pause();

    void systemDo(const QJsonObject &order);

    void send(const QString &s);

    void win(const QString &info);

    void sendMessage();

    void setState(State newState);

    void timeUp();

    void gameOver();

    void setActiveExit();
};


#endif //GOBANG_CHESSBOARD_REMOTE_PVP_H
