#include "openpvehouse.h"

#include "ui_openpvehouse.h"

openpvehouse::openpvehouse(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::openpvehouse) {
    ui->setupUi(this);

    QString strQss =
        getQssString(QString(":/resources"
                             "/dialog_style.css"));
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setStyleSheet(strQss);

    QScreen *deskScreen = QApplication::primaryScreen();
    if (deskScreen) {
        availableSize = deskScreen->availableSize();
    }
}

openpvehouse::~openpvehouse() { delete ui; }

void openpvehouse::on_okpushButton_clicked() {
    this->close();
    int forbid = this->ui->forbiddencomboBox->currentIndex();
    chessboard_pve *a = new chessboard_pve(nullptr, forbid);
    // 最大化
    a->showMaximized();
    a->move({0, 0});
    int h1 = a->geometry().y();
    qDebug() << "localpvp 边框 = " << h1;
    a->setFixedSize(availableSize.width(), availableSize.height() - h1);

    // this->close();
    a->show();
    // a->chessboard_pve_ai(-1,-1);
}

void openpvehouse::on_cancelpushButton_clicked() { this->close(); }
