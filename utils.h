/**
 * @file  utils.h
 * @brief       定义了一些类和宏。
 */
#ifndef UTILS_H
#define UTILS_H



/**
 * @brief 颜色宏
 */
enum  Chess_color
{
    BLACK=1, ///< @brief 黑色
    WHITE=-1, ///< @brief 白色
    EMPTY=0 ///< @brief 空
};

/**
 * @brief 对局类型宏
 * @details 主菜单中选择对局类型（本地双人、人机、联机对战）时用
 */
enum  Game_type
{
    LOCAL_PVP=0, ///< @brief 本地PVP
    LOCAL_PVE=1, ///< @brief 本地PVE
    REMOTE_PVP=2 ///< @brief 联机PVP
};

/**
 * @brief 对局状态宏
 */
enum  Game_status
{
    BLACK_WINS=1, ///< @brief 黑棋赢
    WHITE_WINS=-1, ///< @brief 白棋赢
    NOBODY_WINS=0 ///< @brief 未产生胜负
};


/**
 * @brief udp发送的JOSN数据类
 */
struct UdpData
{
    UdpData();
    UdpData(QString name,QString gamemodel,QString chesscolor,QString israndom,QString port){
        this->name=name;
        this->chesscolor=chesscolor;
        this->israndom=israndom;
        this->gamemodel=gamemodel;
        this->port=port;
    }
    QString name;
    QString gamemodel;
    QString  chesscolor;
    QString israndom;
    QString port;
};

/**
 * @brief 点结构体
 */
struct Point {
    int x;
    int y;
};

/**
 * @brief 棋子结构体
 * @details 记录棋子的位置和颜色
 */
struct Chess {
    int x; ///< @brief x值
    int y; ///< @brief y值
    int color; ///< @brief 棋子颜色，见 @ref Chess_color
};

#endif  //UTILS_H
