#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint|
                   Qt::WindowCloseButtonHint|
                   Qt::WindowMinimizeButtonHint);


    img2 = QImage(":/resources/welcome.jpg");

    QScreen *deskScreen = QApplication::primaryScreen();
    if(deskScreen)
        {
            availableSize = deskScreen->availableVirtualSize();
            int availableWidth = availableSize.width();
            int availableHeight = availableSize.height();

            qDebug()<<availableWidth<<" "<<availableHeight;
        }

    update();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_netButton_clicked()
{
    netwindow=new NetWindow();
    connect(netwindow,&NetWindow::backToMain,this,&MainWindow::backSlot);
    this->close();
    netwindow->show();
}
void MainWindow::backSlot(){
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

void MainWindow::on_pvpButton_clicked() // 创建本地对局
{
    localpvp_window = new Chessboard_Local_PVP(nullptr,1);
    connect(localpvp_window, &Chessboard_Local_PVP::back_from_local_pvp,
            this, &MainWindow::slot_back_from_localpvp);
    this->close();
    localpvp_window->move({0, 0});
    localpvp_window->setFixedSize(availableSize);

    QFile f(":/resources/stylesheet.css");
    f.open(QIODevice::ReadOnly);
    QString strQss = f.readAll();
    localpvp_window->setStyleSheet(strQss);
    f.close();

    localpvp_window->show();
}

void MainWindow::slot_back_from_localpvp() { // 从本地对局中返回
    localpvp_window->hide();
    delete localpvp_window;
    localpvp_window = 0;
    this->show();
}


void MainWindow::on_pveButton_clicked()
{
    openpvehouse* a=new openpvehouse();
    //this->close();
    a->show();
}


void MainWindow::on_pvpButton_2_clicked()
{
    chessboard_fupan* a=new chessboard_fupan(nullptr,1);
    //this->close();
    a->setFixedSize(availableSize);
    a->show();
}

