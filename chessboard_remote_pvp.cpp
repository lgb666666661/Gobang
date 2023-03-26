#include "chessboard_remote_pvp.h"
#include "ui_chessboard_remote_pvp.h"
#include <iostream>

Chessboard_Remote_PVP::Chessboard_Remote_PVP(QWidget *parent) :
    ChessBoard(parent,0),
    ui(new Ui::Chessboard_Remote_PVP)
{
    ui->setupUi(this);
//    this->ui->pushButton->move({STARTX + 20*GRIDSIZE, STARTY+7*GRIDSIZE});
//    this->ui->pushButton_2->move({STARTX + 20*GRIDSIZE, STARTY+14*GRIDSIZE});
    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
}

Chessboard_Remote_PVP::~Chessboard_Remote_PVP()
{
    delete ui;
}

void Chessboard_Remote_PVP::mousePressEvent(QMouseEvent *event) {
    if(restrict_level >= 1) return;

    if(nearx >= 0 && neary >= 0) {
        chess({nearx, neary}, turn);

//        qDebug() << game_status;
        change_turn();
        if (game_status != NOBODY_WINS)
        {
            set_restrict_level(2);
            auto dialog = new GameOver();
            QString s = "";
            if(game_status == BLACK_WINS) {
                s.append("黑棋胜");
            }
            else {
                s.append("白棋胜: ");
                if(value == 1) s.append("长连禁手");
                if(value == 2) s.append("四四禁手");
                if(value == 3) s.append("三三禁手");
            }
            //dialog->ui->label->setText(s);
            dialog->exec();
        }
    }
}

Chessboard_Remote_PVP::Chessboard_Remote_PVP(QWidget *parent, int new_game_mode) : ChessBoard(parent, new_game_mode) ,ui(new Ui::Chessboard_Remote_PVP){
    ui->setupUi(this);
    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
    cout<<this->height()<<endl;

}
