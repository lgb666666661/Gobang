#ifndef chessboard_pve_H
#define chessboard_pve_H
#include "chessboard.h"
#include <QMainWindow>
#include "chessboard_fupan.h"
namespace Ui {
class chessboard_pve;
}

/**
 * @brief 本地双人对战的棋盘类
 */
class chessboard_pve : public ChessBoard
{
    Q_OBJECT

   public:
    void alpha_beta();
    int max_value(int alpha, int beta, int depth);
    int min_value(int alpha, int beta, int depth);
    int heuristic(int player);
    void easyai(int last_x,int last_y);
    /**
     * @brief 无参构造函数。
     * 一般不使用，应该使用带new_game_mode的构造函数指定对局类型。
     */
    explicit chessboard_pve(QWidget *parent = nullptr);
    /**
     * @brief 可以指定有无禁手的构造函数。
     * @param parent
     * @param new_game_mode int\n
     * 0: 无禁手\n
     * 1: 有禁手\n
     * 见 @ref game_mode
     * @return 构造一个 @ref game_mode = new_game_mode的新对象
     */
    explicit chessboard_pve(QWidget *parent = nullptr,
                                  int new_game_mode = 0);
    ~chessboard_pve();

    /**
     * @brief 本地PVP对局中的鼠标点击事件。
     */
    void mousePressEvent(QMouseEvent *event);

    Ui::chessboard_pve *ui;
    //int last_x,last_y;
   private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // chessboard_pve_H

