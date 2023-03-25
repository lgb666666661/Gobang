#ifndef OPENHOUSEDIALOG_H
#define OPENHOUSEDIALOG_H

#include <QDialog>

namespace Ui {
class OpenHouseDialog;
}

class OpenHouseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenHouseDialog(QWidget *parent = nullptr);
    ~OpenHouseDialog();

private:
    Ui::OpenHouseDialog *ui;
};

#endif // OPENHOUSEDIALOG_H
