#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

#include "chessboard.h"
#include "chessboard_fupan.h"
#include "chessboard_local_pvp.h"
#include "netwindow.h"
#include "open_local_pvp_dialog.h"
#include "openpvehouse.h"
#include "replaywindow.h"
#include "ui_open_local_pvp_dialog.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

///@brief 主界面类
class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private slots:
    void on_netButton_clicked();
    void backSlot();

    void on_pvpButton_clicked();
    void slot_back_from_localpvp();

    void on_pveButton_clicked();

    void on_pvpButton_2_clicked();

    void slot_local_pvp_set_mode(int game_mode);

   private:
    NetWindow *netwindow = nullptr;
    Chessboard_Local_PVP *localpvp_window = nullptr;
    openpvehouse *open_pve_house = nullptr;
    ReplayWindow *replayWindow = nullptr;
    Ui::MainWindow *ui;
    QImage img2;

    QSize availableSize;
    int local_pvp_game_mode;

   protected:
    void paintEvent(QPaintEvent *);
};
#endif  // MAINWINDOW_H
