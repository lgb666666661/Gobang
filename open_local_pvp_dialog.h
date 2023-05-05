#ifndef OPEN_LOCAL_PVP_DIALOG_H
#define OPEN_LOCAL_PVP_DIALOG_H

#include <QDialog>

namespace Ui {
class open_local_pvp_dialog;
}

class open_local_pvp_dialog : public QDialog {
    Q_OBJECT

   public:
    explicit open_local_pvp_dialog(QWidget *parent = nullptr);
    ~open_local_pvp_dialog();

   public:
    Ui::open_local_pvp_dialog *ui;
   signals:
    void mode_chosen(int game_mode);
    void return_to_mainwindow();
   private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif  // OPEN_LOCAL_PVP_DIALOG_H
