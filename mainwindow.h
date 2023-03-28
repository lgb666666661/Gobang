#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include"netwindow.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_netButton_clicked();
    void backSlot();

private:
    NetWindow* netwindow;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

