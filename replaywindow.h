//
// Created by user-kk on 2023/5/4.
//

#ifndef GOBANG_REPLAYWINDOW_H
#define GOBANG_REPLAYWINDOW_H

#include <QMainWindow>
#include <QJsonArray>
#include "chessboard_fupan.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ReplayWindow; }
QT_END_NAMESPACE

class ReplayWindow : public QMainWindow {
Q_OBJECT

public:
    explicit ReplayWindow(QWidget *parent = nullptr);

    ~ReplayWindow() override;

private:
    Ui::ReplayWindow *ui;
    QJsonArray array;
    chessboard_fupan* fupan= nullptr;
    void load();
    void select(int index);

};


#endif //GOBANG_REPLAYWINDOW_H
