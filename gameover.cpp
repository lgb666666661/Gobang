#include "gameover.h"

#include "ui_gameover.h"
#include "utils.h"

GameOver::GameOver(QWidget* parent) : QDialog(parent), ui(new Ui::GameOver) {
    ui->setupUi(this);

    QString strQss =
        getQssString(QString(":/resources"
                             "/dialog_style.css"));
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setStyleSheet(strQss);
}

GameOver::~GameOver() { delete ui; }

void GameOver::on_pushButton_clicked() { this->hide(); }

void GameOver::setLabel(const QString& s) {
    ui->label->setText(s);
    this->update();
}
