#include "chessboard_remote_pvp.h"
#include "ui_chessboard_remote_pvp.h"

Chessboard_Remote_PVP::Chessboard_Remote_PVP(QWidget *parent) :
    ChessBoard(parent,0),
    ui(new Ui::Chessboard_Remote_PVP)
{
    ui->setupUi(this);
//    this->ui->pushButton->move({STARTX + 20*GRIDSIZE, STARTY+7*GRIDSIZE});
//    this->ui->pushButton_2->move({STARTX + 20*GRIDSIZE, STARTY+14*GRIDSIZE});
//    this->centralWidget()->setMouseTracking(true);
//    this->setMouseTracking(true);
}

Chessboard_Remote_PVP::~Chessboard_Remote_PVP()
{
    delete ui;
}
