#include "open_local_pvp_dialog.h"

#include "ui_open_local_pvp_dialog.h"
#include "utils.h"

open_local_pvp_dialog::open_local_pvp_dialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::open_local_pvp_dialog) {
    ui->setupUi(this);

    QString strQss =
        getQssString(QString(":/resources"
                             "/dialog_style.css"));
    this->setStyleSheet(strQss);
}

open_local_pvp_dialog::~open_local_pvp_dialog() { delete ui; }

void open_local_pvp_dialog::on_pushButton_clicked() {
    this->hide();
    emit mode_chosen(this->ui->comboBox->currentIndex());
}

void open_local_pvp_dialog::on_pushButton_2_clicked()
{
    this->hide();
    emit return_to_mainwindow();
}

