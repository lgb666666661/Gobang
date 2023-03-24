#ifndef CHESSBOARD_LOCAL_PVP_H
#define CHESSBOARD_LOCAL_PVP_H
#include "chessboard.h"

#include <QMainWindow>

namespace Ui {
class Chessboard_Local_PVP;
}

class Chessboard_Local_PVP : public ChessBoard
{
    Q_OBJECT

public:
    explicit Chessboard_Local_PVP(QWidget *parent = nullptr);
    explicit Chessboard_Local_PVP(QWidget *parent = nullptr,
                                  int new_game_mode = 0);
    ~Chessboard_Local_PVP();

    // 落子
    void mousePressEvent(QMouseEvent *event);

    Ui::Chessboard_Local_PVP *ui;
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // CHESSBOARD_LOCAL_PVP_H
