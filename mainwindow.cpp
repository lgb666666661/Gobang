#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    QImage img2(":/resources/welcome.jpg");
    QRectF boarder2(0, 0, window_w, window_h);
    painter.drawImage(boarder2, img2);
}

void MainWindow::on_pvpButton_clicked()
{
    Chessboard_Local_PVP* a=new Chessboard_Local_PVP(nullptr,1);
    //this->close();
    a->show();
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
    a->show();
}

