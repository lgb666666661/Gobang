#include "openhousedialog.h"
#include "ui_openhousedialog.h"

OpenHouseDialog::OpenHouseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenHouseDialog)
{
    ui->setupUi(this);
}

OpenHouseDialog::~OpenHouseDialog()
{
    delete ui;
}
