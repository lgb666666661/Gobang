/**
 * @file  main.c
 * @brief       项目主函数文件
 * @details  主要包含协议应用栈程序框架，main函数入口
 * @author      wanghuan  any question please send mail to 371463817@qq.com
 * @date        2018-8-17
 * @version     V1.0
 * @copyright    Copyright (c) 2018-2020  江苏亨通光网科技有限公司
 */
#include "chessboard.h"
#include "ui_chessboard.h"
#include "ui_gameover.h"
#include <fstream>

/**
 * @brief 无参构造函数。
 * @details 一般不使用，应该使用带new_game_mode的构造函数指定对局类型
 * @param parent
 */
ChessBoard::ChessBoard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChessBoard)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint|
                   Qt::WindowCloseButtonHint|
                   Qt::WindowMinimizeButtonHint);
    this->showMaximized();
    int w = this->geometry().width();
    int h = this->geometry().height();
    this->setFixedSize(w, h);

    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
    qDebug() << this->centralWidget()->hasMouseTracking();
    chessboard = vector<vector<int>>(15, vector<int>(15, EMPTY));
    record = vector<Chess>();
    this->game_mode = 0;
    this->game_status = NOBODY_WINS;
    this->restrict_level = 0;
    // 画棋盘
    update();
}

/**
 * @brief 有参构造函数 用new_game_mode指定对局类型
 * @param parent
 * @param new_game_mode: 对局的类型
 */
ChessBoard::ChessBoard(QWidget *parent, int new_game_mode) :
    QMainWindow(parent),
    ui(new Ui::ChessBoard)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint|
                   Qt::WindowCloseButtonHint|
                   Qt::WindowMinimizeButtonHint);
    this->showMaximized();
    int w = this->geometry().width();
    int h = this->geometry().height();
    this->setFixedSize(w, h);

    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
    qDebug() << this->centralWidget()->hasMouseTracking();
    chessboard = vector<vector<int>>(15, vector<int>(15, EMPTY));
    record = vector<Chess>();
    this->game_mode = new_game_mode;
    this->game_status = NOBODY_WINS;
    this->restrict_level = 0;
    // 画棋盘
    update();
}

ChessBoard::~ChessBoard()
{
    delete ui;
}

/**
 * @brief ChessBoard::chess 落子并更新棋盘。落子后可以通过game_status
 * @param p const Point& 描述
 * @param color
 * @return 改变game_status
 */
void ChessBoard::chess(const Point &p, int color) { // 在p点位置下黑棋/白棋
    chessboard[p.x][p.y] = color;
    record.push_back({p.x, p.y, color});
    _checkStatus();
    nearx = -1, neary = -1;
    nowx = p.x, nowy = p.y;
    update();
}

Chess ChessBoard::cancel() { // 悔棋
    nowx = -1, nowy = -1;
    if (record.empty()) return {-1, -1, 0};
    auto laststep = record.back();
    record.pop_back();
    chessboard[laststep.x][laststep.y] = EMPTY;
    _checkStatus();
    qDebug() << "value = " << value;
    update();
    change_turn();
    return laststep;
}

void ChessBoard::clear() { // 清空
    nowx = -1, nowy = -1;
    record.clear();
    for(int i = 0; i < 15; i++) {
        for(int j = 0; j < 15; j++) {
            chessboard[i][j] = EMPTY;
        }
    }
    _checkStatus();
    update();
    turn = BLACK;
}

void ChessBoard::save_data(vector<Chess> chess_data)//把对局数据保存到文件中
{
    ofstream ofs;
    ofs.open("fupandata.txt",ios::out|ios::trunc);//文件写，如果存在则清空再写；
    int len=chess_data.size();
    for(int i=0;i<len;i++)
    {
        ofs<<chess_data[i].color<<" "<<chess_data[i].x<<" "<<chess_data[i].y<<endl;
    }
    ofs.close();
}

void ChessBoard::_checkStatus() { // 检查当前对局状态
    if(game_mode == 0) __checkStatus1();
    else __checkStatus2();
}

// 无禁手的情况下检查对局状态
void ChessBoard::__checkStatus1() {
    value = 0;
    game_status = NOBODY_WINS;
    vector<Point> dirs = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    for(int i = 0; i < 15; i++) {
        for(int j = 0; j < 15; j++) {
            for(auto dir : dirs) {
                int cnt = 0, curx = i, cury = j;
                for(int step = 0; step < 5 && curx >= 0 && curx < 15
                    && cury >= 0 &&cury < 15; step++) {
                    cnt += chessboard[curx][cury];
                    curx+= dir.x, cury+= dir.y;
                }
                if(cnt == 5 * BLACK) {
                    game_status = BLACK_WINS; return;
                }
                else if (cnt == 5 * WHITE) {
                    game_status = WHITE_WINS; return;
                }
            }
        }
    }
}

// 有禁手的情况下检查对局状态
void ChessBoard::__checkStatus2() {
    game_status = NOBODY_WINS;
    value = 0;
    if(record.empty()) {value = 0; return;}

    vector<Point> dirs = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    for(auto dir : dirs) {
        int x = record.back().x;
        int y = record.back().y;
        int cnt = __getCnt(x, y, dir, 0);
        if(cnt == 5 * BLACK) {
            game_status = BLACK_WINS; value = 0; return;
        }
        else if(cnt <= 5 * WHITE) {
            game_status = WHITE_WINS; value = 0; return;
        }
    }

    // 没有胜出的情况下检查黑棋禁手
    // 长连禁手
    for(auto dir : dirs) {
        int x = record.back().x;
        int y = record.back().y;
        int cnt = __getCnt(x, y, dir, 0);
        if(cnt > 5 * BLACK) {
            game_status = WHITE_WINS; value = 1; return; // 长连禁手
        }
    }
    // 四四禁手
    int live4_cnt = 0;
    for(auto dir : dirs) {
        if(__is_live4(record.back().x, record.back().y, dir, 0)) {
            live4_cnt++;
        }
    }
    int sleep4_cnt = 0;
    for(auto dir : dirs) {
        sleep4_cnt += __is_sleep4(record.back().x, record.back().y, dir, 0);
    }
//    for(auto dir : dirs) {
//        dir = {-dir.x, -dir.y};
//        if(__is_sleep4(record.back().x, record.back().y, dir, 0)) {
//            sleep4_cnt++;
//        }
//    }

    qDebug() << "live4_cnt = " << live4_cnt;
    qDebug() << "sleep4_cnt = " << sleep4_cnt;
    if(live4_cnt + sleep4_cnt >= 2) {
        game_status = WHITE_WINS; value = 2; return; // 四四禁手
    }
    // 三三禁手
    int live3_cnt = 0;
    for(auto dir : dirs) {
        if(__is_live3(record.back().x, record.back().y, dir)) {
            live3_cnt++;
        }
    }
    qDebug() << "live3_cnt = " << live3_cnt;
    qDebug() << " ";
    if(live3_cnt >= 2) {
        game_status = WHITE_WINS; value = 3; return; // 三三禁手
    }
}

bool ChessBoard::__is_5(int x, int y, const Point& dir, int depth)
// 检查以(x, y)为中心的dir双向方向上是否恰好有五个黑子 且这五个黑子必须包含tx,ty
{
    if(chessboard[x][y] != BLACK) return false;

    return __getCnt(x, y, dir, depth) == 5 * BLACK;
}

bool ChessBoard::__is_5_push_3(int x, int y, const Point& dir, int depth) {
// 检查是否有以x,y为中心包含depth的连五，
// 若有，将连五上除了x,y和record最后一个点的其余三个点加入进record
    if(chessboard[x][y] != BLACK) return false;

    vector<int> flags(depth, 0);
    int flag = 1;
    vector<Chess> tmp(record.end() - depth, record.end());
    for(int i = 0; i < depth; i++) {
        flags[i] = (x == tmp[i].x && y == tmp[i].y);
    }

    int cnt = 1;
    int curx = x + dir.x, cury = y + dir.y;
    vector<Chess> memo;
    while(isLegal(curx, cury)) {
        if(chessboard[curx][cury] == BLACK) cnt += BLACK;
        else break;
        for(int i = 0; i < depth; i++) {
            if(curx == tmp[i].x && cury == tmp[i].y) flags[i] = 1;
        }
        if(!(curx == record.back().x && cury == record.back().y))
            memo.push_back({curx, cury, BLACK});
        curx += dir.x, cury += dir.y;
    }

    curx = x - dir.x, cury = y - dir.y;
    while(isLegal(curx, cury)) {
        if(chessboard[curx][cury] == BLACK) cnt += BLACK;
        else break;
        for(int i = 0; i < depth; i++) {
            if(curx == tmp[i].x && cury == tmp[i].y) flags[i] = 1;
        }
        if(!(curx == record.back().x && cury == record.back().y))
            memo.push_back({curx, cury, BLACK});
        curx -= dir.x, cury -= dir.y;
    }

    for(int i = 0; i < depth; i++) {
        if(flags[i] == 0) {
            flag = 0; break;
        }
    }

    if(cnt == 5 * BLACK && flag) {
        for(auto v : memo) {
            record.push_back(v);
        }
        return true;
    }
    return false;
}

bool ChessBoard::__is_live4(int x, int y, const Point& dir, int depth)
// 检查以(x, y)为中心的dir方向上是否是黑子活四
{
//    qDebug() << x << " " << y << " " << dir.x << " " << dir.y;
    if(chessboard[x][y] != BLACK) return false;

    // 在(x, y)点的黑子的dir方向上，距离<=4的点落黑子，判断能不能连五
    // 若有且只有两个点，落黑子后能形成以(x, y)为中心的连五，则是活四

    int cnt = 0;
    for(int step = 1; step <= 4; step++) {
        int curx = x + step * dir.x, cury = y + step * dir.y;
        if(!isLegal(curx, cury)) continue;
        if(chessboard[curx][cury] != EMPTY) continue;
        chessboard[curx][cury] = BLACK;
        record.push_back({curx, cury, BLACK});
        cnt += __is_5(x, y, dir, depth + 1);
//        qDebug() << "setting black:" << curx << cury << dir.x << dir.y;
//        qDebug() << "checking 5:" << x << y << dir.x << dir.y;
//        if(__is_5(x, y, dir, curx, cury)) {
//            qDebug() << "is 5";
//        }
        chessboard[curx][cury] = EMPTY;
        record.pop_back();
    }

    for(int step = 1; step <= 4; step++) {
        int curx = x - step * dir.x, cury = y - step * dir.y;
        if(!isLegal(curx, cury)) continue;
        if(chessboard[curx][cury] != EMPTY) continue;
        chessboard[curx][cury] = BLACK;
        record.push_back({curx, cury, BLACK});
        cnt += __is_5(x, y, dir, depth + 1);
//        qDebug() << "setting black:" << curx << cury << dir.x << dir.y;
//        qDebug() << "checking 5:" << x << y << dir.x << dir.y;
//        if(__is_5(x, y, dir, curx, cury)) {
//            qDebug() << "is 5";
//        }
        chessboard[curx][cury] = EMPTY;
        record.pop_back();
    }
    return cnt == 2;
}

int ChessBoard::__is_sleep4(int x, int y, const Point& dir, int depth)
// 检查以(x, y)为中心的dir方向上有几个黑子冲四
// 个数可能为0/1/2
{
    if(chessboard[x][y] != BLACK) return false;

    // 在(x, y)点的黑子的dir方向上，距离<=4的点落黑子，判断能不能连五
    // 若有且只有一个点，落黑子后能形成以(x, y)为中心的连五，则是冲四
    int flag = 0;
    int cnt = 0;
    for(int step = 1; step <= 4; step++) {
        int curx = x + step * dir.x, cury = y + step * dir.y;
        if(!isLegal(curx, cury)) continue;
        if(chessboard[curx][cury] != EMPTY) continue;
        if(cnt == 0) {
            chessboard[curx][cury] = BLACK;
            record.push_back({curx, cury, BLACK});
            bool is_5 = __is_5_push_3(x, y, dir, depth + 1);
            cnt += is_5;
            chessboard[curx][cury] = EMPTY;
            if(is_5) {
                record.erase(record.end() - 4);
            }
            else {
                record.pop_back();
            }

        }
        else {
            chessboard[curx][cury] = BLACK;
            record.push_back({curx, cury, BLACK});
            bool is_5_same_3 = __is_5(x, y, dir, depth + 4);
            bool is_5 = __is_5(x, y, dir, depth + 1);

            if(is_5 && !is_5_same_3) flag = 1;
            cnt += is_5_same_3;
            chessboard[curx][cury] = EMPTY;
            record.pop_back();
        }
    }

    for(int step = 1; step <= 4; step++) {
        int curx = x - step * dir.x, cury = y - step * dir.y;
        if(!isLegal(curx, cury)) continue;
        if(chessboard[curx][cury] != EMPTY) continue;
        if(cnt == 0) {
            chessboard[curx][cury] = BLACK;
            record.push_back({curx, cury, BLACK});
            bool is_5 = __is_5_push_3(x, y, dir, depth + 1);
            cnt += is_5;
            chessboard[curx][cury] = EMPTY;
            if(is_5) {
                record.erase(record.end() - 4);
            }
            else {
                record.pop_back();
            }
        }
        else {
            chessboard[curx][cury] = BLACK;
            record.push_back({curx, cury, BLACK});
            bool is_5_same_3 = __is_5(x, y, dir, depth + 4);
            bool is_5 = __is_5(x, y, dir, depth + 1);

            if(is_5 && !is_5_same_3) flag = 1;
            cnt += is_5_same_3;
            chessboard[curx][cury] = EMPTY;
            record.pop_back();
        }
    }

    if(cnt >= 1) {
        for(int i = 0; i < 3; i++) record.pop_back();
    }
//    qDebug() << "cnt = " <<  cnt << "flag = " << flag;
    return (cnt==1) + flag;
}

bool ChessBoard::__is_live3(int x, int y, const Point& dir)
// 检查以(x, y)为中心的dir方向上是否是黑子活三
{
    if(chessboard[x][y] != BLACK) return false;

    // 在(x, y)点的黑子的dir方向上，距离<=3的点落黑子，判断能不能成活四
    // 如果至少存在一个点，落黑子后能形成以(x, y)为中心的活四，则是活三

    int cnt = 0;
    for(int step = 1; step <= 3; step++) {
        int curx = x + step * dir.x, cury = y + step * dir.y;
        if(!isLegal(curx, cury)) continue;
        if(chessboard[curx][cury] != EMPTY) continue;
        chessboard[curx][cury] = BLACK;
        record.push_back({curx, cury, BLACK});

        cnt += __is_live4(x, y, dir, 1);
        chessboard[curx][cury] = EMPTY;
        record.pop_back();
    }

    for(int step = 1; step <= 3; step++) {
        int curx = x - step * dir.x, cury = y - step * dir.y;
        if(!isLegal(curx, cury)) continue;
        if(chessboard[curx][cury] != EMPTY) continue;
        chessboard[curx][cury] = BLACK;
        record.push_back({curx, cury, BLACK});

        cnt += __is_live4(x, y, dir, 1);
        chessboard[curx][cury] = EMPTY;
        record.pop_back();
    }
//    qDebug() << "cnt = " << cnt;
    return cnt >= 1;
}

int ChessBoard::__getCnt(int x, int y, const Point& dir, int depth){
// 检查以(x, y)为中心的dir方向（双向）上连成的长度 且必须包含record的最后depth个点
// 当depth = 0时，单纯检查以(x, y)为中心的dir方向上连成的长度
    int color = chessboard[x][y];
    int cnt = color;

    vector<int> flags(depth, 0);
    int flag = 1;
    vector<Chess> tmp(record.end() - depth, record.end());
    for(int i = 0; i < depth; i++) {
        flags[i] = (x == tmp[i].x && y == tmp[i].y);
    }

    int curx = x + dir.x, cury = y + dir.y;
    while(isLegal(curx, cury)) {
        if(chessboard[curx][cury] == color) cnt += color;
        else break;
        for(int i = 0; i < depth; i++) {
            if(curx == tmp[i].x && cury == tmp[i].y) flags[i] = 1;
        }
        curx += dir.x, cury += dir.y;
    }

    curx = x - dir.x, cury = y - dir.y;
    while(isLegal(curx, cury)) {
        if(chessboard[curx][cury] == color) cnt += color;
        else break;
        for(int i = 0; i < depth; i++) {
            if(curx == tmp[i].x && cury == tmp[i].y) flags[i] = 1;
        }
        curx -= dir.x, cury -= dir.y;
    }

    for(int i = 0; i < depth; i++) {
        if(flags[i] == 0) {
            flag = 0; break;
        }
    }
//    qDebug() << "getcnt ";
//    qDebug() << x << y;
//    qDebug() << dir.x << dir.y;
//    qDebug() << "cnt=" << cnt;
    return cnt * flag;
};

// 刷新
void ChessBoard::paintEvent(QPaintEvent *) {

    int window_w = this->geometry().width();
    int window_h = this->geometry().height();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 画图片
    // 背景
    QImage img2(":/resources/welcome.jpg");
    QRectF boarder2(0, 0, window_w, window_h);
    painter.drawImage(boarder2, img2);
    // 棋盘外层
    QImage img3(":/resources/chessboard.png");
    int tmp = 0.5 * GRIDSIZE;
    QRectF boarder3(STARTX - tmp,
                    STARTY - tmp,
                    14 * GRIDSIZE + 2 * tmp,
                    14 * GRIDSIZE + 2 * tmp);
    painter.drawImage(boarder3, img3);
    // 棋盘内层
    QImage img1(":/resources/chessboard_inner.png");
    QRectF boarder1(STARTX, STARTY, 14 * GRIDSIZE, 14 * GRIDSIZE);
    painter.drawImage(boarder1, img1);
//    for(int i = 0; i < 15; i++) {
//        painter.drawLine(STARTX + i * GRIDSIZE, STARTY,
//                         STARTX + i * GRIDSIZE, STARTY + 14 * GRIDSIZE);
//        painter.drawLine(STARTX, STARTY + i * GRIDSIZE,
//                         STARTX + 14 * GRIDSIZE, STARTY + i * GRIDSIZE);
//    }

    for(int i = 0; i < 15; i++) {
        for(int j = 0; j < 15; j++) {
            if(chessboard[i][j] == BLACK) {
                painter.setBrush(Qt::black);
                painter.drawEllipse({STARTX + i * GRIDSIZE, STARTY + j * GRIDSIZE},
                                    CHESSR, CHESSR);
            }
            if(chessboard[i][j] == WHITE) {
                painter.setBrush(Qt::white);
                painter.drawEllipse({STARTX + i * GRIDSIZE, STARTY + j * GRIDSIZE},
                                    CHESSR, CHESSR);
            }
        }
    }
    if(nearx >= 0 && neary >= 0) {
        painter.setBrush(Qt::red);
        painter.drawEllipse({STARTX + nearx * GRIDSIZE, STARTY + neary * GRIDSIZE},
                            HINTR, HINTR);
    }

    if(nowx >= 0 && nowy >= 0) {
        auto pen = QPen(Qt::NoPen);
        painter.setPen(pen);
        painter.setBrush(Qt::red);
        painter.drawEllipse({STARTX + nowx * GRIDSIZE, STARTY + nowy * GRIDSIZE - int(GRIDSIZE* 0.3)},
                            HINTR2, HINTR2);
    }

    if(value > 0) {
        int x = record.back().x, y = record.back().y;
        auto pen = QPen(Qt::red);
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawLine(STARTX + x * GRIDSIZE - GRIDSIZE / 6,
                         STARTY + y * GRIDSIZE - GRIDSIZE / 6,
                         STARTX + x * GRIDSIZE + GRIDSIZE / 6,
                         STARTY + y * GRIDSIZE + GRIDSIZE / 6);
        painter.setPen(pen);
        painter.drawLine(STARTX + x * GRIDSIZE + GRIDSIZE / 6,
                         STARTY + y * GRIDSIZE - GRIDSIZE / 6,
                         STARTX + x * GRIDSIZE - GRIDSIZE / 6,
                         STARTY + y * GRIDSIZE + GRIDSIZE / 6);
    }
}

// 鼠标移动
void ChessBoard::mouseMoveEvent(QMouseEvent *event) {
    if(restrict_level == 2) return;

    qDebug() << event->pos();
    int curx = event->pos().x();
    int cury = event->pos().y();

    nearx = -1, neary = -1;
    int THRES = GRIDSIZE / 2;
    if(curx >= STARTX - THRES && curx <= STARTX + 14 * GRIDSIZE + THRES &&
            cury >= STARTY - THRES && cury <= STARTY + 14 * GRIDSIZE + THRES) {
        nearx = (curx - STARTX) / GRIDSIZE;
        if((curx - STARTX) % GRIDSIZE > THRES)
            nearx += 1;
        neary = (cury - STARTY) / GRIDSIZE;
        if((cury - STARTY) % GRIDSIZE > THRES)
            neary += 1;

        if(chessboard[nearx][neary] != EMPTY)
            nearx = -1, neary = -1;
    }

    update();
}

void ChessBoard::change_turn() { // 改变当前黑白状态，返回新的状态
    turn = -turn;
}



void ChessBoard::set_restrict_level(int level) {
    restrict_level = level;
}

void ChessBoard::rescale() {
    STARTX = 1.2 * this->height() / 16;
    STARTY = 1.2 * this->height() / 16;
    GRIDSIZE = this->height() / 16;
    CHESSR = GRIDSIZE / 2 * 0.9;
    HINTR = GRIDSIZE / 8;
    HINTR2 = GRIDSIZE / 9;
}

void ChessBoard::resizeEvent(QResizeEvent *event) {
    rescale();
    this->update();
}
