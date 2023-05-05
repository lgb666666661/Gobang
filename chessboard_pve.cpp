#include "chessboard_pve.h"
#include"chessboard.h"
#include "ui_gameover.h"
#include "ui_chessboard_pve.h"
#define MY_INT_MIN (-100000)
#define MY_INT_MAX 100000
chessboard_pve::chessboard_pve(QWidget *parent) :
                                                              ChessBoard(parent, 0),
                                                              ui(new Ui::chessboard_pve)
{
    ui->setupUi(this);
    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
}


chessboard_pve::chessboard_pve(QWidget *parent, int new_game_mode) :
                                                                                 ChessBoard(parent, new_game_mode),
                                                                                 ui(new Ui::chessboard_pve)
{
    ui->setupUi(this);
    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
}
chessboard_pve::~chessboard_pve()
{
    delete ui;
}
void chessboard_pve::alpha_beta() {
    int best_score = MY_INT_MIN;
    pair<int, int> best_move;
    int alpha = MY_INT_MIN;
    int beta = MY_INT_MAX;

    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (chessboard[i][j] == 0) {
                chessboard[i][j] = 1;
                int score = min_value(alpha, beta, 4);
                chessboard[i][j] = 0;

                if (score >= best_score) {
                    best_score = score;
                    best_move = { i, j };
                }
                alpha = max(alpha, best_score);

                if (beta <= alpha) {
                    break;
                }
            }
        }
    }
    int i=0;
    //chess({best_move.first,best_move.second}, 1);
}

int chessboard_pve::max_value(int alpha, int beta, int depth) {
    if (depth == 0) {
        return heuristic(1);
    }

    int score = MY_INT_MIN;

    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (chessboard[i][j] == 0) {
                chessboard[i][j] = 1;
                score = max(score, min_value(alpha, beta, depth - 1));
                chessboard[i][j] = 0;

                alpha = max(alpha, score);
                if (beta <= alpha) {
                    return score;
                }
            }
        }
    }

    return score;
}

int chessboard_pve::min_value(int alpha, int beta, int depth) {
    if (depth == 0) {
        return heuristic( -1);
    }

    int score = MY_INT_MAX;

    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (chessboard[i][j] == 0) {
                chessboard[i][j] = -1;
                score = min(score, max_value( alpha, beta, depth - 1));
                chessboard[i][j] = 0;

                beta = min(beta, score);
                if (beta <= alpha) {
                    return score;
                }
            }
        }
    }

    return score;
}
int chessboard_pve::heuristic( int player) {
    int score = 0;
    // 统计每个位置的连续子数量
    vector<vector<int>> consecutive(15, vector<int>(15));
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (chessboard[i][j] == player) {
                // 横向
                int cnt = 1;
                for (int k = j + 1; k < 15 && chessboard[i][k] == player; ++k) {
                    ++cnt;
                }
                for (int k = j - 1; k >= 0 && chessboard[i][k] == player; --k) {
                    ++cnt;
                }
                consecutive[i][j] += cnt;

                        // 竖向
                cnt = 1;
                for (int k = i + 1; k < 15 && chessboard[k][j] == player; ++k) {
                    ++cnt;
                }
                for (int k = i - 1; k >= 0 && chessboard[k][j] == player; --k) {
                    ++cnt;
                }
                consecutive[i][j] += cnt;

                        // 正斜线
                cnt = 1;
                for (int k = i + 1, l = j + 1; k < 15 && l < 15 && chessboard[k][l] == player; ++k, ++l) {
                    ++cnt;
                }
                for (int k = i - 1, l = j - 1; k >= 0 && l >= 0 && chessboard[k][l] == player; --k, --l) {
                    ++cnt;
                }
                consecutive[i][j] += cnt;

                        // 反斜线
                cnt = 1;
                for (int k = i + 1, l = j - 1; k < 15 && l >= 0 && chessboard[k][l] == player; ++k, --l) {
                    ++cnt;
                }
                for (int k = i - 1, l = j + 1; k >= 0 && l < 15 && chessboard[k][l] == player; --k, ++l) {
                    ++cnt;
                }
                consecutive[i][j] += cnt;
            }
        }
    }

            // 对连续子数量进行统计得分
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (consecutive[i][j] >= 5) {
                return MY_INT_MAX;
            } else if (consecutive[i][j] == 4) {
                score += 100000;
            } else if (consecutive[i][j] == 3) {
                score += 1000;
            } else if (consecutive[i][j] == 2) {
                score += 100;
            } else if (consecutive[i][j] == 1) {
                score += 10;
            }
        }
    }

            // 统计棋局状态的空闲位置数
    int empty_cnt = 0;
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (chessboard[i][j] == 0) {
                ++empty_cnt;
            }
        }
    }

    score += empty_cnt;

    return player * score;
}
void chessboard_pve::easyai(int last_x,int last_y)
{
    int dx[8] = {-1, -1, 0, 1, 1, 1, 0, -1};  // 方向数组，表示八个方向
    int dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};

    int best_x = last_x+1, best_y = last_y+1;  // 初始化最佳位置
    int max_score = -10;

    for (int i = 0; i < 8; i++) {  // 遍历八个方向
        int x = last_x + dx[i];
        int y = last_y + dy[i];
        if (x >= 0 && x < 15 && y >= 0 && y < 15 && chessboard[x][y] == 0) {  // 如果该位置为空位
            int score = 0;
            for (int j = -4; j < 4; j++) {  // 沿着该方向判断
                if(j==0||j==-1)continue;
                int nx = x + j*dx[i];
                int ny = y + j*dy[i];

                if (nx >= 0 && nx < 15 && ny >= 0 && ny < 15 && chessboard[nx][ny] == 1) {
                    score++;  // 如果某个方向上有白棋，则增加该位置的得分
                }
            }

            if (score > max_score) {  // 如果该位置得分更高，则更新最佳位置和得分
                best_x = x;
                best_y = y;
                max_score = score;
            }
        }
    }
     chess({best_x, best_y}, -1);


}
void chessboard_pve::mousePressEvent(QMouseEvent *event) {
     if(nearx >= 0 && neary >= 0) {  /// 若预选落点没有在棋盘内，退出
        int x=nearx,y=neary;
        chess({x,y}, turn); /// 落子
        change_turn();
        easyai(x,y);
        //        qDebug() << game_status;
        change_turn();          /// 交换棋权
        if (game_status != NOBODY_WINS) /// 判断对局状态，若产生胜负则禁止继续下棋并显示提示信息
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
            chessboard_fupan::save_data(this->record,s);
            dialog->ui->label->setText(s);
            dialog->exec();
        }
     }
}

void chessboard_pve::on_pushButton_clicked() // 悔棋
{
    qDebug() << "悔棋";
    cancel();
    cancel();
    if(game_status == NOBODY_WINS) {
        restrict_level = 0;
    }
}


void chessboard_pve::on_pushButton_2_clicked() // 清空
{
    qDebug() << "清空";
    clear();
    if(game_status == NOBODY_WINS) {
        restrict_level = 0;
    }
}

