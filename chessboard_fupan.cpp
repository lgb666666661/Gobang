#include "chessboard_fupan.h"

#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <filesystem>
#include <fstream>

#include "chessboard.h"
#include "ui_chessboard_fupan.h"
#include "ui_gameover.h"

chessboard_fupan::chessboard_fupan(QWidget *parent)
    : ChessBoard(parent, 0), ui(new Ui::chessboard_fupan) {
    ui->setupUi(this);
    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);
}

chessboard_fupan::chessboard_fupan(QWidget *parent, int new_game_mode)
    : ChessBoard(parent, new_game_mode), ui(new Ui::chessboard_fupan) {
    ui->setupUi(this);

    QString strQss =
        getQssString(QString(":/resources"
                             "/stylesheet.css"));
    this->setStyleSheet(strQss);

    this->setWindowIcon(QIcon(":/resources/dialogIcon.ico"));

    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);

    connect(this->ui->next, &QPushButton::clicked, this,
            &chessboard_fupan::next);
    connect(this->ui->last, &QPushButton::clicked, this,
            &chessboard_fupan::last);

    restrict_level = 2;
    check();
}

void chessboard_fupan::mousePressEvent(QMouseEvent *event) {}

void chessboard_fupan::resizeEvent(QResizeEvent *event) {
    rescale();
    this->ui->winInfo->move({STARTX + 20 * GRIDSIZE, STARTY + 3 * GRIDSIZE});
    this->ui->next->move({STARTX + 20 * GRIDSIZE, STARTY + 5 * GRIDSIZE});
    this->ui->last->move({STARTX + 20 * GRIDSIZE, STARTY + 7 * GRIDSIZE});
    this->ui->label->move({STARTX + 20 * GRIDSIZE, STARTY + 9 * GRIDSIZE});
    this->ui->progressBar->move(
        {STARTX + 20 * GRIDSIZE, STARTY + int(9.5 * GRIDSIZE)});
    update();
}

chessboard_fupan::~chessboard_fupan() { delete ui; }

void chessboard_fupan::save_data(
    const vector<Chess> &chess_data,
    const QString &win_message)  // 把对局数据保存到文件中
{
    if (!filesystem::exists("./data")) {
        filesystem::create_directory("./data");
    }
    QFile file{"./data/data.json"};
    if (!file.open(QIODevice::ReadWrite)) {
        return;
    }
    QJsonArray objectArray;
    QString jsonString{file.readAll()};
    file.close();
    if (!jsonString.isEmpty()) {
        QJsonDocument document;
        QJsonParseError err;
        document = QJsonDocument::fromJson(jsonString.toUtf8(), &err);
        if (err.error != QJsonParseError::NoError) {
            qDebug() << "Parse json " << jsonString.toUtf8()
                     << " error: " << err.error;
            return;
        }
        objectArray = document.array();
    }
    if (!file.open(QIODevice::WriteOnly | QFile::Truncate)) {
        return;
    }

    QJsonObject object;
    object.insert("win_message", win_message);
    QJsonArray chessArray;
    for (auto &i : chess_data) {
        QJsonObject chess;
        chess.insert("x", i.x);
        chess.insert("y", i.y);
        chess.insert("color", i.color);
        chessArray.push_back(chess);
    }
    object.insert("chess", chessArray);
    object.insert("time",
                  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    objectArray.push_back(object);

    file.write(
        QString(QJsonDocument(objectArray).toJson(QJsonDocument::Compact))
            .toUtf8());
    file.close();
}

void chessboard_fupan::load_data(const QJsonObject &object)  // 把对局复盘展示
{
    QJsonArray jsonArray = object.value("chess").toArray();
    for (auto i : jsonArray) {
        QJsonObject o = i.toObject();
        Chess tmp{o.value("x").toInt(), o.value("y").toInt(),
                  o.value("color").toInt()};
        chess_data.emplace_back(tmp);
    }
    ui->progressBar->setRange(0, (int)chess_data.size());
    ui->winInfo->setText(object.value("win_message").toString());
    check();
    if (chess_data.empty()) {
        ui->progressBar->setRange(0, 1);
        ui->progressBar->setValue(1);
    }
}

void chessboard_fupan::next()  // 下一步
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

void chessboard_fupan::last()  // 撤销
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
    this->ui->progressBar->setValue(this->count);
}
