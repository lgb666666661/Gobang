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


