#include "mainwindow.h"
#include <QApplication>
#include "chessboard.h"
#include "chessboard_remote_pvp.h"
#include "chessboard_local_pvp.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow mainWindow;
//    mainWindow.show();
//    Chessboard_Local_PVP w(0, 1); // 第二个参数：0表示无禁手 1表示禁手
//    // ChessBoard w;
//     w.show();
    Chessboard_Remote_PVP rPVP(BLACK, Chessboard_Remote_PVP::SERVER,nullptr, 0);
    rPVP.show();
    Chessboard_Remote_PVP rPVP2(WHITE, Chessboard_Remote_PVP::CLIENT,nullptr, 0,"127.0.0.1",10086);
    rPVP2.show();
    return a.exec();
}

