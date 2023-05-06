//
// Created by user-kk on 2023/3/26.
//

#ifndef GOBANG_CHESSBOARD_REMOTE_PVP_H
#define GOBANG_CHESSBOARD_REMOTE_PVP_H

#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <functional>

#include "TcpAbstract.h"
#include "TcpClient.h"
#include "TcpServer.h"
#include "chessboard.h"
#include "utils.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Chessboard_Remote_PVP_Abstract;
}
QT_END_NAMESPACE

///@class Chessboard_Remote_PVP_Abstract
///@brief 远程对局的棋盘抽象类
class Chessboard_Remote_PVP_Abstract : public ChessBoard {
    Q_OBJECT

   public:
    explicit Chessboard_Remote_PVP_Abstract(QWidget *parent = nullptr) = delete;

    ///@brief 构造函数
    ///@param color 本方的棋子颜色 见@ref Chess_color
    ///@param parent 要绑定的父对象的指针
    ///@param new_game_mode 有无禁手 见@ref game_mode
    explicit Chessboard_Remote_PVP_Abstract(Chess_color color,
                                            QWidget *parent = nullptr,
                                            int new_game_mode = 0);

    ///@brief 鼠标事件,根据鼠标的预选位置(棋盘上的红点)下棋,调用@ref send
    /// 发送落子的信息
    void mousePressEvent(QMouseEvent *event) override;

    ///@brief
    /// 设置棋盘是否能落子,如果在能落或不能落的状态之间切换成功,会更新步时和局时的计数器以及悔棋按钮的可触发性,以及更新本轮落子的Label
    ///@param level @see restrict_level
    void set_restrict_level(int level) override;

    ~Chessboard_Remote_PVP_Abstract() override;

   signals:

    ///@brief 当用户点击左上角关闭按钮触发这个信号,用于返回主菜单
    void cancelToMain();
    ///@brief
    /// 当客户端想要连接服务器,但是服务器已经和另一个客户端建立连接了,会触发这个信号,表示拒绝连接
    void refuseLink();

    ///@brief 对局开始会触发这个信号,一个对局只能触发一次
    void gameStart();

   protected:
    ///@brief 网络连接状态
    enum LinkState {
        PAUSE,     ///< 暂停态,可被重连
        RUNNING,   ///< 运行态
        TERMINATE  ///< 终止态,不可被重连
    };

    LinkState linkState = PAUSE;  ///< 指示棋盘网络连接状态
    bool isReconnected = false;   ///< 指示是否为重连的棋局

    ///@brief 关闭事件,会调用@ref
    /// setActiveExit来指示当前为主动关闭,不用重连,也会使用 @ref
    /// exit给棋局对方发一个终止信息
    void closeEvent(QCloseEvent *event) override;
    ///@brief 在界面显示对方的聊天信息
    void peerMessage(const QString &s);
    ///@brief 开始棋局的函数,会向对方发送一个重连信息,保证两边的棋盘一致
    void start();
    ///@brief 暂停棋局
    void pause();
    ///@brief 解析对方发来的系统信息,进行相应动作
    void systemDo(const QJsonObject &order);
    ///@brief 向界面打印系统信息
    void systemMessage(const QString &s);
    ///@brief 向界面打印自己的聊天信息
    void myMessage(const QString &s);
    ///@brief 设置连接状态,更新连接状态的label
    void setState(LinkState newState);

   private:
    ///@brief 能否下棋 @see restrict_level
    enum CanChess {
        CAN = 0,     ///< 能
        CANNOT = 1,  ///< 不能,但是能预选
        STOP = 2     ///< 不能预选
    };

    ///@class Time
    ///@brief 绑定一个标签的时间类
    ///@note 时间增加的时候会更新标签,时间要到限制的时候调用回调函数
    struct Time {
        int time = 0;                 ///< 时间秒数
        QString prefix;               ///< 标签字符串的前缀
        QLabel *timeLabel = nullptr;  ///< 绑定的标签
        int restrictTime = -1;        ///< 时间限制,为-1时表示无限制
        std::function<void()> f;      ///< 回调函数,超时调用的函数
        std::function<void()> f2;  ///< 回调函数 快要超时调用的函数
        Time &operator++();
        void clear();  ///< 清空时间
    };

    Ui::Chessboard_Remote_PVP_Abstract *ui{};

    Chess_color myChessColor;  ///< 当前棋局自己这方的颜色,@ref Chess_color

    QTimer repentTimer;  ///< 悔棋计时器,当悔棋无响应超过一定时间,会产生一个提示
    QTimer timer;  ///< 棋局计时器

    Time *myLocalTime = nullptr;    ///< 本方局时
    Time *peerLocalTime = nullptr;  ///< 对方局时
    Time *myStepTime = nullptr;     ///< 本方步时
    Time *peerStepTime = nullptr;   ///< 对方步时
    Time *time = nullptr;           ///< 总时长

    ///@brief 发送信息的函数
    virtual void send(const QString &s) = 0;
    ///@brief 产生一个赢棋的窗口
    void win(const QString &info);
    ///@brief 发送用户聊天记录
    ///@note 连接断开时会提示无法发送
    void sendMessage();
    ///@brief 计时器调用的函数,用来更新对局时间,更新和时间相关的label
    void timeUp();
    ///@brief 设置棋局终止,停止计时器
    void gameOver();
    ///@brief 调用@ref TcpAbstract::setActiveExit()
    virtual void setActiveExit() = 0;
    ///@brief 打印开局信息
    virtual void startMessage() = 0;
    ///@brief 调用@ref TcpAbstract::stop()
    virtual void exit() = 0;

   protected:
    void resizeEvent(QResizeEvent *event) override;  ///< 缩放事件
};

///@class Chessboard_Remote_PVP_Server
/// @brief 远程对局棋盘的服务端类
class Chessboard_Remote_PVP_Server : public Chessboard_Remote_PVP_Abstract {
   protected:
   private:
    TcpAbstract *server = nullptr;  ///< Tcp服务器
    ///@brief 初始化服务器与棋盘类的信号连接
    void initNetWork();
    ///@brief 打印开局信息 @see Chessboard_Remote_PVP_Abstract::startMessage()
    void startMessage() override;
    ///@brief 发送信息的函数 @see Chessboard_Remote_PVP_Abstract::send()
    void send(const QString &s) override;
    ///@brief 调用@ref TcpAbstract::stop() @see
    /// Chessboard_Remote_PVP_Abstract::exit()
    void exit() override;
    ///@brief 调用@ref TcpAbstract::setActiveExit() @see
    /// Chessboard_Remote_PVP_Abstract::setActiveExit()
    void setActiveExit() override;

   public:
    ~Chessboard_Remote_PVP_Server() override;
    ///@brief 构造函数
    ///@param color 本方的棋子颜色 见@ref Chess_color
    ///@param parent 要绑定的父对象的指针
    ///@param new_game_mode 有无禁手 见@ref game_mode
    explicit Chessboard_Remote_PVP_Server(Chess_color color,
                                          QWidget *parent = nullptr,
                                          int new_game_mode = 0);
    ///@brief 得到Tcp服务器建立的端口信息
    int getPort();
};

///@class Chessboard_Remote_PVP_Client
/// @brief 远程对局棋盘的客户端类
class Chessboard_Remote_PVP_Client : public Chessboard_Remote_PVP_Abstract {
   protected:
   private:
    TcpAbstract *client = nullptr;  ///< Tcp客户端
    QHostAddress hostAddress;     ///< Tcp服务器的ip地址
    int port;                     ///< Tcp服务器的端口号
    ///@brief 初始化服务器与棋盘类的信号连接
    void initNetWork();
    ///@brief 打印开局信息 @see Chessboard_Remote_PVP_Abstract::startMessage()
    void startMessage() override;
    ///@brief 发送信息的函数 @see Chessboard_Remote_PVP_Abstract::send()
    void send(const QString &s) override;
    ///@brief 调用@ref TcpAbstract::stop() @see
    /// Chessboard_Remote_PVP_Abstract::exit()
    void exit() override;
    ///@brief 调用@ref TcpAbstract::setActiveExit() @see
    /// Chessboard_Remote_PVP_Abstract::setActiveExit()
    void setActiveExit() override;

   public:
    ///@brief 构造函数
    ///@param color 本方的棋子颜色 见@ref Chess_color
    ///@param serverAddress 服务器的ip地址
    ///@param serverPort 服务器的端口号
    ///@param parent 要绑定的父对象的指针
    ///@param new_game_mode 有无禁手 见@ref game_mode
    Chessboard_Remote_PVP_Client(Chess_color color,
                                 const QHostAddress &serverAddress,
                                 int serverPort, QWidget *parent = nullptr,
                                 int new_game_mode = 0);

    ~Chessboard_Remote_PVP_Client() override;
};

#endif  // GOBANG_CHESSBOARD_REMOTE_PVP_H
