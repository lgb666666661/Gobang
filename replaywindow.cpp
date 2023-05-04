//
// Created by user-kk on 2023/5/4.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ReplayWindow.h" resolved

#include <QFile>
#include "replaywindow.h"
#include "ui_ReplayWindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringListModel>


ReplayWindow::ReplayWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::ReplayWindow) {
    ui->setupUi(this);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    load();
    connect(ui->no,&QPushButton::clicked,[this](){
        this->close();
    });
    connect(ui->listView,&QListView::clicked,[this](const QModelIndex &index){
        select(index.row());
    });
}

ReplayWindow::~ReplayWindow() {
    delete ui;
}

void ReplayWindow::load() {
    QFile file{"./data/data.json"};
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QString jsonString{file.readAll()};
    file.close();
    if (!jsonString.isEmpty()) {
        QJsonDocument document;
        QJsonParseError err;
        document = QJsonDocument::fromJson(jsonString.toUtf8(), &err);
        if (err.error != QJsonParseError::NoError) {
            qDebug() << "Parse json " << jsonString.toUtf8() << " error: " << err.error;
            return;
        }
        array = document.array();
    }
    QStringList list;
    for (auto i: array) {
        list.append(i.toObject().value("time").toString());
    }
    auto *listModel = new QStringListModel{list, this};
    this->ui->listView->setModel(listModel);
}

void ReplayWindow::select(int index) {
    this->close();
    delete fupan;
    fupan=new chessboard_fupan(nullptr,0);
    fupan->load_data(array[index].toObject());
    fupan->show();
}
