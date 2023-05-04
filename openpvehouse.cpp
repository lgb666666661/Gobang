#include "openpvehouse.h"
#include "ui_openpvehouse.h"

openpvehouse::openpvehouse(QWidget *parent) :
                                              QWidget(parent),
                                              ui(new Ui::openpvehouse)
{
    ui->setupUi(this);
}

openpvehouse::~openpvehouse()
{
    delete ui;
}

void openpvehouse::on_okpushButton_clicked()
{
    this->close();
    int forbid=this->ui->forbiddencomboBox->currentIndex();
    chessboard_pve* a=new chessboard_pve(nullptr,forbid);
    //this->close();
    a->show();
    //a->chessboard_pve_ai(-1,-1);
}


void openpvehouse::on_cancelpushButton_clicked()
{
    this->close();
}

