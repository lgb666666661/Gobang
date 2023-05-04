#include "chessboard_fupan.h"
#include "chessboard.h"
#include "ui_gameover.h"
#include "ui_chessboard_fupan.h"

#include <fstream>
#include <filesystem>

chessboard_fupan::chessboard_fupan(QWidget *parent) :
        ChessBoard(parent, 0),
        ui(new Ui::chessboard_fupan) {
    ui->setupUi(this);
    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
    load_data();
}


chessboard_fupan::chessboard_fupan(QWidget *parent, int new_game_mode) :
        ChessBoard(parent, new_game_mode),
        ui(new Ui::chessboard_fupan) {
    ui->setupUi(this);
    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
    this->ui->next->show();
    this->ui->last->show();

    connect(this->ui->next, &QPushButton::clicked, this, &chessboard_fupan::next);
    connect(this->ui->last, &QPushButton::clicked, this, &chessboard_fupan::last);
    restrict_level = 2;
    load_data();
    check();

}

void chessboard_fupan::mousePressEvent(QMouseEvent *event) {

}

chessboard_fupan::~chessboard_fupan() {
    delete ui;
}

void chessboard_fupan::save_data(const vector<Chess> &chess_data)//把对局数据保存到文件中
{

    if (!filesystem::exists("./data")) {
        filesystem::create_directory("./data");
    }
    ofstream ofs{"./data/fupandata.txt", ios::out};
    if (ofs.is_open()) {
        for (auto &i: chess_data) {
            ofs << i.color << " " << i.x << " " << i.y << endl;
        }
    }

    ofs.close();
}

void chessboard_fupan::load_data()//把对局复盘展示
{
    ifstream ifs;
    ifs.open("./data/fupandata.txt", ios::in);
    if (ifs.good()) {
        if (ifs.is_open()) {
            while (!ifs.eof()) {
                Chess temp{};
                ifs >> temp.color;
                ifs >> temp.x;
                ifs >> temp.y;
                chess_data.push_back(temp);
            }
            chess_data.pop_back();
        }
    }

}

void chessboard_fupan::next() // 下一步
{
    qDebug() << "下一步";
    Point p{};
    int i = this->count;
    this->count++;
    p.x = chess_data[i].x;
    p.y = chess_data[i].y;
    this->chess(p, chess_data[i].color);
    check();
}


void chessboard_fupan::last() // 撤销
{
    qDebug() << "撤销";
    if (this->count > 0) {
        this->count--;
        cancel();
    }
    check();

}

void chessboard_fupan::check() {
    this->ui->last->setDisabled(this->count == 0);
    this->ui->next->setDisabled(this->count == chess_data.size());
}

