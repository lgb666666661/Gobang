#ifndef OPENPVEHOUSE_H
#define OPENPVEHOUSE_H

#include <QWidget>
#include <QSize>
#include"chessboard_pve.h"
namespace Ui {
class openpvehouse;
}

class openpvehouse : public QWidget
{
    Q_OBJECT

   public:
    explicit openpvehouse(QWidget *parent = nullptr);
    ~openpvehouse();

   private slots:
    void on_okpushButton_clicked();

    void on_cancelpushButton_clicked();

   private:
    Ui::openpvehouse *ui;
    QSize availableSize;
};

#endif // OPENPVEHOUSE_H
