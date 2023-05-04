#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include"netwindow.h"
#include"openpvehouse.h"
#include"chessboard_local_pvp.h"
#include"chessboard_fupan.h"
#include "chessboard.h"
#include <QMainWindow>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

///@brief 主界面类
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_netButton_clicked();
    void backSlot();

    void on_pvpButton_clicked();

    void on_pveButton_clicked();

    void on_pvpButton_2_clicked();

   private:
    NetWindow* netwindow;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

