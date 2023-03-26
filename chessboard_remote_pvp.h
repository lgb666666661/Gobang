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

QT_BEGIN_NAMESPACE
namespace Ui { class Chessboard_Remote_PVP; }
QT_END_NAMESPACE

class Chessboard_Remote_PVP : public ChessBoard {
Q_OBJECT

public:
    enum Mode{
        SERVER,
        CLIENT
    };
    explicit Chessboard_Remote_PVP(QWidget *parent = nullptr) = delete;

    explicit Chessboard_Remote_PVP(Chess_color color, Mode mode, QWidget *parent = nullptr, int new_game_mode = 0,const QString& hostAddress="",int port=0);

    void mousePressEvent(QMouseEvent *event) override;

    ~Chessboard_Remote_PVP() override;


protected:

private:
    Ui::Chessboard_Remote_PVP *ui{};
    Chess_color myChessColor;
    enum CanChess {
        CAN = 0,
        CANNOT = 1,
        STOP=2
    };
    void systemMessage(const QString &s);
    Mode chessMode;
    TcpServer *server= nullptr;
    TcpClient *client= nullptr;
    int getPort();
    void start();
    void pause();
    void systemDo(const QJsonObject& order);
    void send(const QString &s);
    void win(const QString &info);
};


#endif //GOBANG_CHESSBOARD_REMOTE_PVP_H
