#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <QDialog>

namespace Ui {
class GameOver;
}

class GameOver : public QDialog
{
    Q_OBJECT

public:
    explicit GameOver(QWidget *parent = nullptr);
    ~GameOver();
    Ui::GameOver *ui;
    void setLabel(const QString&);

private slots:
    void on_pushButton_clicked();

};

#endif // GAMEOVER_H

