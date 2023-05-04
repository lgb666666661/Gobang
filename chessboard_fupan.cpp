#include "chessboard_fupan.h"
#include "chessboard.h"
#include "ui_gameover.h"
#include "ui_chessboard_fupan.h"
#include <fstream>

chessboard_fupan::chessboard_fupan(QWidget *parent) :
                                                              ChessBoard(parent, 0),
                                                              ui(new Ui::chessboard_fupan)
{
    ui->setupUi(this);
    this->ui->pushButton->move({STARTX + 20*GRIDSIZE, STARTY+7*GRIDSIZE});
    this->ui->pushButton_2->move({STARTX + 20*GRIDSIZE, STARTY+14*GRIDSIZE});

    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
}


chessboard_fupan::chessboard_fupan(QWidget *parent, int new_game_mode) :
                                                                                 ChessBoard(parent, new_game_mode),
                                                                                 ui(new Ui::chessboard_fupan)
{
    ui->setupUi(this);
    this->ui->pushButton->move({STARTX + 20*GRIDSIZE, STARTY+6*GRIDSIZE});
    this->ui->pushButton_2->move({STARTX + 20*GRIDSIZE, STARTY+8*GRIDSIZE});
    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
}
void chessboard_fupan::mousePressEvent(QMouseEvent *event)
{

}
chessboard_fupan::~chessboard_fupan()
{
    delete ui;
}

void chessboard_fupan::save_date(vector<Chess> chess_data)//把对局数据保存到文件中
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
vector<Chess> chessboard_fupan::show_fupan()//把对局复盘展示
{
    ifstream ifs;
    ifs.open("fupandata.txt",ios::in);
    if(ifs.is_open()){
        vector<Chess> chess_data;
        while(!ifs.eof())
        {
            Chess temp;
            ifs>>temp.color;
            ifs>>temp.x;
            ifs>>temp.y;
            chess_data.push_back(temp);
        }
        chess_data.pop_back();
        return chess_data;
    }
}
void chessboard_fupan::on_pushButton_clicked() // 下一步
{
    qDebug() << "下一步";
    vector<Chess> chess_data(this->show_fupan());
    int len=chess_data.size();
    if(this->count==len)
    {
        set_restrict_level(2);
        auto dialog = new GameOver();
        auto* temp=new chessboard_fupan(nullptr,1);
        temp->save_date(this->record);
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
        dialog->ui->label->setText(s);
        dialog->exec();//弹出当前无对战数据
        return;
    }
    Point p;
    int i=this->count;
    this->count++;
    p.x=chess_data[i].x;
    p.y=chess_data[i].y;
    this->chess(p,chess_data[i].color);


    if(game_status == NOBODY_WINS) {
        restrict_level = 0;
    }
    if (game_status != NOBODY_WINS) /// 判断对局状态，若产生胜负则禁止继续下棋并显示提示信息
    {
        set_restrict_level(2);
        auto dialog = new GameOver();
        auto* temp=new chessboard_fupan(nullptr,1);
        temp->save_date(this->record);
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
        dialog->ui->label->setText(s);
        dialog->exec();
    }
}


void chessboard_fupan::on_pushButton_2_clicked() // 撤销
{
    qDebug() << "撤销";
    if(this->count>0)
    {
        this->count--;
    }
    cancel();
    if(game_status == NOBODY_WINS) {
        restrict_level = 0;
    }

}

