#ifndef NETWINDOW_H
#define NETWINDOW_H

#include <QWidget>

namespace Ui {
class NetWindow;
}

class NetWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NetWindow(QWidget *parent = nullptr);
    ~NetWindow();

private:
    Ui::NetWindow *ui;
};

#endif // NETWINDOW_H
