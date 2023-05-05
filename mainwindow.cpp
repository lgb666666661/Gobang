#include "mainwindow.h"

#include <QFile>

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/resources/dialogIcon.ico"));

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint |
                   Qt::WindowMinimizeButtonHint);

    img2 = QImage(":/resources/welcome.jpg");

    QScreen *deskScreen = QApplication::primaryScreen();
    if (deskScreen) {
        availableSize = deskScreen->availableSize();
    }

    update();
}

MainWindow::~MainWindow() {
    delete ui;
    delete replayWindow;
}
void MainWindow::on_netButton_clicked() {
    netwindow = new NetWindow();
    connect(netwindow, &NetWindow::backToMain, this, &MainWindow::backSlot);
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

void MainWindow::on_pvpButton_clicked()  // 创建本地对局
{
    open_local_pvp_dialog dialog(0);
    dialog.setWindowFlag(Qt::FramelessWindowHint);
    int flag = 0;
    connect(&dialog, &open_local_pvp_dialog::return_to_mainwindow,
            [&]() { flag = 1; });
    connect(&dialog, &open_local_pvp_dialog::mode_chosen, this,
            &MainWindow::slot_local_pvp_set_mode);
    dialog.exec();
    if (flag) return;
    localpvp_window = new Chessboard_Local_PVP(nullptr, local_pvp_game_mode);
    connect(localpvp_window, &Chessboard_Local_PVP::back_from_local_pvp, this,
            &MainWindow::slot_back_from_localpvp);
    this->close();
    // 最大化
    localpvp_window->showMaximized();
    localpvp_window->move({0, 0});
    int h1 = localpvp_window->geometry().y();
    qDebug() << "localpvp 边框 = " << h1;
    localpvp_window->setFixedSize(availableSize.width(),
                                  availableSize.height() - h1);
    localpvp_window->show();
}

void MainWindow::slot_local_pvp_set_mode(int game_mode) {
    local_pvp_game_mode = game_mode;
}

void MainWindow::slot_back_from_localpvp() {  // 从本地对局中返回
    localpvp_window->hide();
    delete localpvp_window;
    localpvp_window = 0;
    this->show();
}

void MainWindow::on_pveButton_clicked() {
    delete open_pve_house;
    open_pve_house = new openpvehouse(this);
    open_pve_house->show();
}

void MainWindow::on_pvpButton_2_clicked() {
    delete replayWindow;
    replayWindow = new ReplayWindow();

    replayWindow->show();
}
