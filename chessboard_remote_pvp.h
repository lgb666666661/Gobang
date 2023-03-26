#ifndef CHESSBOARD_REMOTE_PVP_H
#define CHESSBOARD_REMOTE_PVP_H
#include "chessboard.h"
#include <QWidget>

namespace Ui {
class Chessboard_Remote_PVP;
}

class Chessboard_Remote_PVP : public ChessBoard
{
    Q_OBJECT

public:
    explicit Chessboard_Remote_PVP(QWidget *parent = nullptr);
    explicit Chessboard_Remote_PVP(QWidget *parent = nullptr,
    int new_game_mode = 0);
    ~Chessboard_Remote_PVP() override;

    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::Chessboard_Remote_PVP *ui;
};

#endif // CHESSBOARD_REMOTE_PVP_H
