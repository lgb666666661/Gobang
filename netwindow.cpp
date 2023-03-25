#include "netwindow.h"
#include "ui_netwindow.h"

NetWindow::NetWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetWindow)
{
    ui->setupUi(this);
}

NetWindow::~NetWindow()
{
    delete ui;
}
