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

    ~Chessboard_Remote_PVP() override;

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
    struct Time{
        int time=0;
        QString prefix;
        QLabel timeLabel;
        Time& operator++(){
            time++;
            timeLabel.setText(prefix+QString::number(time/60)+"分"+QString::number(time%60)+"秒");
            return *this;
        }
    };

    Ui::Chessboard_Remote_PVP *ui{};
    Mode chessMode;
    Chess_color myChessColor;
    TcpServer *server = nullptr;
    TcpClient *client = nullptr;
    bool isDisconnected = true;
    State state = PAUSE;
    QTimer repentTimer;
    QTimer timer;

    QLabel stepTime;
    QLabel localTime;
    QLabel time;


    void systemMessage(const QString &s);

    void myMessage(const QString &s);

    void peerMessage(const QString &s);

    int getPort();

    void start();

    void pause();

    void systemDo(const QJsonObject &order);

    void send(const QString &s);

    void win(const QString &info);

    void sendMessage();

    void setState(State newState);


};


#endif //GOBANG_CHESSBOARD_REMOTE_PVP_H