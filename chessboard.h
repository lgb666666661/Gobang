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

/**
 * @brief 棋盘基类
 * @details 继承时先正常新建一个QMainWindow类，再手动添加继承关系。\n
 * 没有注释的函数或变量不需要考虑！
 */
class ChessBoard : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 无参构造函数。
     * 一般不使用，应该使用带new_game_mode的构造函数指定对局类型。
     */
    explicit ChessBoard(QWidget *parent = nullptr);

    /**
     * @brief 可以指定有无禁手的构造函数。
     * @param parent
     * @param new_game_mode int\n
     * 0: 无禁手\n
     * 1: 有禁手\n
     * 见 @ref game_mode
     * @return 构造一个 @ref game_mode = new_game_mode的新对象
     */
    explicit ChessBoard(QWidget *parent = nullptr, int new_game_mode=0);
    // 选择有无禁手的构造函数 0表示无禁手 1表示有禁手
    ~ChessBoard();
    // 下棋控制
    /**
     * @brief 落子
     * @param p 在哪个点落子
     * @param color 下什么颜色的棋 见@ref Chess_color
     * @return 更新棋盘界面和下棋的记录，改变@ref game_status
     */
    void chess(const Point& p, int color); // 在p点位置下黑棋/白棋

    /**
     * @brief 悔棋
     * @return 更新棋盘界面和下棋的记录，改变@ref game_status ，返回悔了哪一步棋
     * @retval Chess 悔了哪一步棋
     */
    Chess cancel(); // 悔棋。返回最后一步棋的位置和颜色

    void clear(); ///< 清空棋盘 @return 重置所有信息
    int get_game_status() {return game_status;} ///< 得到对局状态 @return @ref game_status
    void change_turn(); ///< 交换行棋权 @return 改变 @ref turn
    void rescale(); ///< 缩放
    void save_data(vector<Chess> chess_data);

    /**
     * @brief 设置对棋盘的控制权限
     * @param level
     * level=0可以正常预选并落子\n
     * level=1只能预选位置不能落子\n
     * level=2不能进行任何操作\n
     * @return @ref restrict_level := level
     */
    virtual void set_restrict_level(int level); // level=0可以正常预选并落子
                                        // level=1只能预选位置不能落子
                                        // level=2不能进行任何操作
    // 刷新
    void paintEvent(QPaintEvent *);
    // 鼠标移动
    void mouseMoveEvent(QMouseEvent *event);
    // 落子
    void mousePressEvent(QMouseEvent *event) override =0;


protected:
    void resizeEvent(QResizeEvent *event) override;

protected:
    Ui::ChessBoard *ui;
    // 超参数
    int STARTX = 1.2 * this->height()/16;
    int STARTY = 1.2 * this->height()/16;
    int GRIDSIZE = this->height()/16;
    int CHESSR = GRIDSIZE / 2 * 0.9; // 棋子半径
    int HINTR = GRIDSIZE / 8; // 落点提示半径
    int HINTR2 = GRIDSIZE / 9; // 新的落子的提示半径
    // 下棋控制
    /**
     * @brief 当前棋盘状态
     * @details 15 * 15的二维数组，其中的值见 @ref Chess_color
     */
    vector<vector<int>> chessboard; // 当前棋盘状态
    int game_mode; ///< 游戏模式 @details 0 无禁手 \n 1 有禁手
    vector<Chess> record; ///< 对局记录，按顺序记录每一步下的棋 \n 复盘时使用
    int game_status = NOBODY_WINS; ///< 当前对局状态\n @details 见 @ref Game_status
    /**
     * @brief 若非0，记录当前黑棋因为哪一种禁手判负
     * @details 1 长连禁手\n
     * 2 四四禁手\n
     * 3 三三禁手\n
     * 0 没有出现禁手
     */
    int value = 0;
    void _checkStatus(); // 检查当前对局状态
    void __checkStatus1(); // 无禁手的情况下检查当前对局状态
    void __checkStatus2(); // 有禁手的情况下检查对局状态

    /**
     * @brief 判断(x, y)点是否合法
     */
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
    int nearx = -1; ///<当前预选的点的X
    int neary = -1; ///<~
    int nowx = -1; ///<最新落子点的X
    int nowy = -1; ///<~

    // 对局控制
    /**
     * @brief 当前轮到谁下
     * @details 可能取值:{BLACK, WHITE} \n
     * 见 @ref Chess_color
     */
    int turn = BLACK;
    /**
     * @brief 当前的操作权限
     * @details 0 可以正常预选并落子\n
     * 1 只能预选位置不能落子\n
     * 2 不能进行任何操作\n
     */
    int restrict_level = 0;

    // 美化
    QImage img1, img2, img3;
};

#endif // CHESSBOARD_H

