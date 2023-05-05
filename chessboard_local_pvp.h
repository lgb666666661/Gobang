#ifndef CHESSBOARD_LOCAL_PVP_H
#define CHESSBOARD_LOCAL_PVP_H
#include <QMainWindow>

#include "chessboard.h"
#include "chessboard_fupan.h"

namespace Ui {
class Chessboard_Local_PVP;
}

/**
 * @brief 本地双人对战的棋盘类
 */
class Chessboard_Local_PVP : public ChessBoard {
    Q_OBJECT

   public:
    /**
     * @brief 无参构造函数。
     * 一般不使用，应该使用带new_game_mode的构造函数指定对局类型。
     */
    explicit Chessboard_Local_PVP(QWidget *parent = nullptr);
    /**
     * @brief 可以指定有无禁手的构造函数。
     * @param parent
     * @param new_game_mode int\n
     * 0: 无禁手\n
     * 1: 有禁手\n
     * 见 @ref game_mode
     * @return 构造一个 @ref game_mode = new_game_mode的新对象
     */
    explicit Chessboard_Local_PVP(QWidget *parent = nullptr,
                                  int new_game_mode = 0);
    ~Chessboard_Local_PVP();

    /**
     * @brief 本地PVP对局中的鼠标点击事件。
     */
    void mousePressEvent(QMouseEvent *event);

    void resizeEvent(QResizeEvent *event) override;  ///< 缩放事件

    void closeEvent(QCloseEvent *event) override;  ///< 关闭事件

    Ui::Chessboard_Local_PVP *ui;
   signals:
    void back_from_local_pvp();
   private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
};

#endif  // CHESSBOARD_LOCAL_PVP_H
