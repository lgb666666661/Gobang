#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint |
                   Qt::WindowCloseButtonHint |
                   Qt::WindowMinimizeButtonHint);
    setWindowState(Qt::WindowMaximized);
//    int w = this->geometry().width();
//    int h = this->geometry().height();
//    this->setFixedSize(w, h);
    update();
}

MainWindow::~MainWindow() {
    delete ui;
    delete chessboardFupan;
}

void MainWindow::on_netButton_clicked() {
    netwindow = new NetWindow();
    connect(netwindow, &NetWindow::backToMain, this, &MainWindow::backSlot);
    this->close();
    netwindow->show();
}

void MainWindow::backSlot() {
    netwindow->hide();
    delete netwindow;
    this->show();
}

void MainWindow::paintEvent(QPaintEvent *) {
    int window_w = this->geometry().width();
    int window_h = this->geometry().height();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 画图片
    // 背景
    QImage img2(":/resources/welcome.jpg");
    QRectF boarder2(0, 0, window_w, window_h);
    painter.drawImage(boarder2, img2);
    // 移动标签位置
    int tmp = window_h / 8;
    int startx = window_w * 0.7;
    int starty = window_h * 0.3;
    this->ui->pvpButton->move({startx, starty});
    this->ui->netButton->move({startx, starty + 1 * tmp});
    this->ui->pveButton->move({startx, starty + 2 * tmp});
    this->ui->pvpButton_2->move({startx, starty + 3 * tmp});
}

void MainWindow::on_pvpButton_clicked() // 创建本地对局
{
    localpvp_window = new Chessboard_Local_PVP(nullptr, 1);
    connect(localpvp_window, &Chessboard_Local_PVP::back_from_local_pvp,
            this, &MainWindow::slot_back_from_localpvp);
    this->close();
    localpvp_window->show();
}

void MainWindow::slot_back_from_localpvp() { // 从本地对局中返回
    localpvp_window->hide();
    this->show();
}


void MainWindow::on_pveButton_clicked() {

    delete open_pve_house;
    open_pve_house = new openpvehouse(this);
    open_pve_house->show();
}


void MainWindow::on_pvpButton_2_clicked() {
    delete chessboardFupan;
    chessboardFupan = new chessboard_fupan(nullptr, 1);
    chessboardFupan->show();
}

