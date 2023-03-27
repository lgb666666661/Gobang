#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QMainWindow>
#include <vector>
#include "utils.h"
#include <QPainter>
#include <QMouseEvent>
#include "gameover.h"

using namespace std;



namespace Ui {
class ChessBoard;
}

class ChessBoard : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = nullptr);
    explicit ChessBoard(QWidget *parent = nullptr, int new_game_mode=0);
    // 选择有无禁手的构造函数 0表示无禁手 1表示有禁手
    ~ChessBoard() override;
    // 下棋控制
    void chess(const Point& p, int color); // 在p点位置下黑棋/白棋
    Chess cancel(); // 悔棋。返回最后一步棋的位置和颜色
    void clear(); // 清空
    int get_game_status() {return game_status;} // 得到对局输赢状态
    void change_turn(); // 改变当前黑白状态，返回新的状态
    virtual void set_restrict_level(int level); // level=0可以正常预选并落子
                                        // level=1只能预选位置不能落子
                                        // level=2不能进行任何操作
    // 刷新
    void paintEvent(QPaintEvent *) override;
    // 鼠标移动
    void mouseMoveEvent(QMouseEvent *event) override;
    // 落子
    void mousePressEvent(QMouseEvent *event) override =0;

protected:
    void resizeEvent(QResizeEvent *event) override;

protected:
    Ui::ChessBoard *ui;
    // 超参数
    int STARTX = this->height()/16;
    int STARTY = this->height()/16;
    int GRIDSIZE = this->height()/16;
    int CHESSR = GRIDSIZE / 2 * 0.9; // 棋子半径
    int HINTR = GRIDSIZE / 8; // 落点提示半径
    int HINTR2 = GRIDSIZE / 9; // 新的落子的提示半径
    // 下棋控制
    vector<vector<int>> chessboard; // 当前棋盘状态
    int game_mode; // 0表示无禁手 1表示有禁手
    vector<Chess> record; // 对局记录，记录每一步下的棋
    int game_status = NOBODY_WINS; // 当前对局状态
    int value = 0; // 若非0，记录当前（黑棋）因为哪一种禁手判负
    void _checkStatus(); // 检查当前对局状态
    void __checkStatus1(); // 无禁手的情况下检查当前对局状态
    void __checkStatus2(); // 有禁手的情况下检查对局状态
    bool isLegal(int x, int y) {
        return x >= 0 && x < 15 && y >= 0 && y < 15;
    }
    int __getCnt(int x, int y, const Point& dir, int depth);
    // 检查以(x, y)为中心的dir方向上连成的长度 且必须包含record的最后depth个点
    // 当depth = 0时，单纯检查以(x, y)为中心的dir方向上连成的长度
    bool __is_5(int x, int y, const Point& dir, int depth);
    // 检查以(x, y)为中心的dir方向上是否恰好有五个黑子
    // 且这五个黑子必须包含record的最后depth个点
    bool __is_5_push_3(int x, int y, const Point& dir, int depth);
    // 这个函数为了检查出同一条直线上的两个冲四
    bool __is_live4(int x, int y, const Point& dir, int depth);
    // 检查以(x, y)为中心的dir方向上是否是黑子活四
    int __is_sleep4(int x, int y, const Point& dir, int depth);
    // 检查以(x, y)为中心的dir方向上有几个黑子冲四
    bool __is_live3(int x, int y, const Point& dir);
    // 检查以(x, y)为中心的dir方向上是否是黑子活三

    // 鼠标移动和落子控制
    int nearx = -1, neary = -1;
    int nowx = -1, nowy = -1;

    // 对局控制
    int turn = BLACK; // BLACK:1, WHITE: -1
    int restrict_level = 0;
};

#endif // CHESSBOARD_H

