#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include "chessboard.h"
#include "chessboard_remote_pvp.h"
#include "chessboard_local_pvp.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



    MainWindow mainWindow;

    QFile f(":/resources/stylesheet.css");
    f.open(QIODevice::ReadOnly);
    QString strQss = f.readAll();
    mainWindow.setStyleSheet(strQss);
    f.close();

    mainWindow.showMaximized();
    int w = 1024, h = 768;
    QScreen *deskScreen = QApplication::primaryScreen();
    if(deskScreen)
        {
            QSize availableSize = deskScreen->availableVirtualSize();
            h = availableSize.width();
            w = availableSize.height();
        }
    mainWindow.setFixedSize({h, w});
//    Chessboard_Local_PVP w(0, 1); // 第二个参数：0表示无禁手 1表示禁手
//    // ChessBoard w;
//     w.show();
//    Chessboard_Remote_PVP_Server rPVP(BLACK,nullptr, 0);
//    rPVP.show();
//    Chessboard_Remote_PVP rPVP2(WHITE, Chessboard_Remote_PVP::CLIENT,nullptr, 0,"192.168.43.9",10086);
//    rPVP2.show();
    return a.exec();
}

