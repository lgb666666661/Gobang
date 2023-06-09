//
// Created by user-kk on 2023/5/4.
//

// You may need to build the project (run Qt uic code generator) to get
// "ui_ReplayWindow.h" resolved

#include "replaywindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>
#include <QStringListModel>

#include "qdialog.h"
#include "ui_ReplayWindow.h"

ReplayWindow::ReplayWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ReplayWindow) {
    ui->setupUi(this);

    QString strQss =
        getQssString(QString(":/resources"
                             "/dialog_style.css"));
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setStyleSheet(strQss);

    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    load();
    connect(ui->listView, &QListView::clicked,
            [this](const QModelIndex &index) { select(index.row()); });
    connect(ui->returnButton, &QPushButton::clicked,
            [this]() { this->close(); });
}

ReplayWindow::~ReplayWindow() { delete ui; }

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
            qDebug() << "Parse json " << jsonString.toUtf8()
                     << " error: " << err.error;
            return;
        }
        array = document.array();
    }
    QStringList list;
    for (auto i : array) {
        list.append(i.toObject().value("time").toString());
    }
    auto *listModel = new QStringListModel{list, this};
    this->ui->listView->setModel(listModel);
}

void ReplayWindow::select(int index) {
    this->close();
    delete fupan;
    fupan = new chessboard_fupan(nullptr, 0);
    fupan->load_data(array[index].toObject());
    QScreen *deskScreen = QApplication::primaryScreen();
    QSize availableSize;
    if (deskScreen) {
        availableSize = deskScreen->availableSize();
    }
    fupan->showMaximized();
    fupan->move({0, 0});
    int h1 = fupan->geometry().y();
    fupan->setFixedSize(availableSize.width(), availableSize.height() - h1);
    fupan->show();
}
