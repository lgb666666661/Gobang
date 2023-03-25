/**
 * @file 文件名
 * @brief 简介
 * @details 细节
 * @author 作者
 * @version 版本号
 * @date 年-月-日
 * @copyright 版权
 */
#ifndef UTILS_H
#define UTILS_H

#endif // UTILS_H
// 颜色宏
//#define BLACK 1
//#define WHITE -1
//#define EMPTY 0
enum  Chess_color {BLACK=1, WHITE=-1, EMPTY=0};
// 对局类型宏
//#define LOCAL_PVP 0
//#define LOCAL_PVE 1
//#define REMOTE_PVP 2
enum  Game_type {LOCAL_PVP=0, LOCAL_PVE=1, REMOTE_PVP=2};
// 对局状态宏
//#define BLACK_WINS 1
//#define WHITE_WINS -1
//#define NOBODY_WINS 0

enum  Game_status {BLACK_WINS=1, WHITE_WINS=-1, NOBODY_WINS=0};


//typedef enum
//{
//    BLACK_WINS=1, /*!< 黑棋赢*/
//    WHITE_WINS=-1,
//    NOBODY_WINS=0
//} game_status;

struct Point {
    int x, y;
};

struct Chess {
    int x, y, color;
};
