#ifndef UTILS_H
#define UTILS_H

#endif // UTILS_H
// 颜色宏
#define BLACK 1
#define WHITE -1
#define EMPTY 0
// 对局类型宏
#define LOCAL_PVP 0
#define LOCAL_PVE 1
#define REMOTE_PVP 2

struct Point {
    int x, y;
};

struct Chess {
    int x, y, color;
};

