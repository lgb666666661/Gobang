#include "chessboard_local_pvp.h"
#include "ui_gameover.h"
#include "ui_chessboard_local_pvp.h"

Chessboard_Local_PVP::Chessboard_Local_PVP(QWidget *parent) :
    ChessBoard(parent, 0),
    ui(new Ui::Chessboard_Local_PVP)
{
    ui->setupUi(this);
    this->ui->pushButton->move({STARTX + 20*GRIDSIZE, STARTY+7*GRIDSIZE});
    this->ui->pushButton_2->move({STARTX + 20*GRIDSIZE, STARTY+14*GRIDSIZE});
    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
}

Chessboard_Local_PVP::Chessboard_Local_PVP(QWidget *parent, int new_game_mode) :
    ChessBoard(parent, new_game_mode),
    ui(new Ui::Chessboard_Local_PVP)
{
    ui->setupUi(this);
    this->ui->pushButton->move({STARTX + 20*GRIDSIZE, STARTY+6*GRIDSIZE});
    this->ui->pushButton_2->move({STARTX + 20*GRIDSIZE, STARTY+8*GRIDSIZE});
    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
}

Chessboard_Local_PVP::~Chessboard_Local_PVP()
{
    delete ui;
}

// 落子
void Chessboard_Local_PVP::mousePressEvent(QMouseEvent *event) {
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
            dialog->ui->label->setText(s);
            dialog->exec();
        }
    }
}

void Chessboard_Local_PVP::on_pushButton_clicked() // 悔棋
{
    qDebug() << "悔棋";
    cancel();
    if(game_status == NOBODY_WINS) {
        restrict_level = 0;
    }
}


void Chessboard_Local_PVP::on_pushButton_2_clicked() // 清空
{
    qDebug() << "清空";
    clear();
    if(game_status == NOBODY_WINS) {
        restrict_level = 0;
    }
}

