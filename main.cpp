#include "mainwindow.h"

#include <QApplication>
#include "chessboard.h"
#include "chessboard_local_pvp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Chessboard_Local_PVP w(0, 1); // 第二个参数：0表示无禁手 1表示禁手
//    ChessBoard w;
    w.show();
    return a.exec();
}
