#ifndef chessboard_fupan_H
#define chessboard_fupan_H
#include "chessboard.h"

#include <QMainWindow>

namespace Ui {
class chessboard_fupan;
}

/**
 * @brief 本地双人对战的棋盘类
 */
class chessboard_fupan : public ChessBoard
{
    Q_OBJECT

public:
    /**
     * @brief 无参构造函数。
     * 一般不使用，应该使用带new_game_mode的构造函数指定对局类型。
     */
    explicit chessboard_fupan(QWidget *parent = nullptr);

    /**
     * @brief 可以指定有无禁手的构造函数。
     * @param parent
     * @param new_game_mode int\n
     * 0: 无禁手\n
     * 1: 有禁手\n
     * 见 @ref game_mode
     * @return 构造一个 @ref game_mode = new_game_mode的新对象
     */
    explicit chessboard_fupan(QWidget *parent = nullptr,
                                  int new_game_mode = 0);
    ~chessboard_fupan() override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override; ///< 缩放事件

    Ui::chessboard_fupan *ui;
    static void save_data(const vector<Chess>& chess_data,const QString &win_message);//把对局数据保存到文件中
    void load_data(const QJsonObject & object);//把对局复盘展示
    int count=0;
private:
    vector<Chess> chess_data;
    void check();
private slots:
    void next();
    void last();
};

#endif // chessboard_fupan_H

