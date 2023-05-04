#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

