#include "netwindow.h"

#include "ui_netwindow.h"

NetWindow::NetWindow(QWidget *parent) : QWidget(parent), ui(new Ui::NetWindow) {
    for (auto sq : QNetworkInterface::allAddresses()) {
        address.push_back(convert_to_ipv4_addr(sq));
    }
    ui->setupUi(this);
    receiveBroadcast();
    connect(&listenSocket, &QUdpSocket::readyRead, [this]() {
        do {
            QNetworkDatagram datagram = listenSocket.receiveDatagram();
            QString qs = convert_to_ipv4_addr(datagram.senderAddress());
            if ((datagramlist.empty() || !(datagramlist.contains(datagram))) &&
                !address.contains(qs)) {
                datagramlist.push_back(datagram);
                showIpAddress();
            }
        } while (listenSocket.hasPendingDatagrams());
    });
    connect(ui->forbiddenCombolBox, &QComboBox::currentTextChanged, this,
            &NetWindow::filterSlot);
    connect(ui->colorCombolBox, &QComboBox::currentTextChanged, this,
            &NetWindow::filterSlot);
    time = new QTimer(this);
    connect(time, &QTimer::timeout, this, &NetWindow::updataIp);
    connect(ui->addressListWidget, &QListWidget::doubleClicked, this,
            &NetWindow::beginGame);
    time->start(10000);
}
NetWindow::~NetWindow() { delete ui; }

/**
 * @brief NetWindow::showGame
 * @details 连接成功后，进入游戏画面
 */
QString NetWindow::convert_to_ipv4_addr(const QHostAddress &addr) {
    quint32 addr_origin = addr.toIPv4Address();
    QHostAddress addr_host = QHostAddress(addr_origin);
    QString addr_str = addr_host.toString();
    return addr_str;
}
void NetWindow::showGame() {
    time->stop();
    this->hide();
    rPVP2->show();
}
/**
 * @brief NetWindow::refuseLink
 * @details 连接超时，收到对方拒绝信息
 */
void NetWindow::refuseLink() {
    delete rPVP2;
    QMessageBox::information(this, "提示框", "连接已过期！", QMessageBox::Ok);
    updataIp();
}
/**
 * @brief NetWindow::showIpAddress
 * @details 解析数据包中的json格式数据
 */
void NetWindow::showIpAddress() {
    udpdatalist.clear();
    for (auto i : datagramlist) {
        QJsonParseError *jsonerror = new QJsonParseError();
        ;
        QJsonDocument *jsonDoc = new QJsonDocument();
        *jsonDoc = (QJsonDocument::fromJson(i.data(), jsonerror));
        if (jsonerror->error != QJsonParseError::NoError) {
            QMessageBox::critical(this, "消息提示框", "包解析错误！");
            return;
        }
        QJsonObject rootObj = jsonDoc->object();
        QStringList keys = rootObj.keys();
        UdpData u(rootObj.value(keys.at(3)).toString(),
                  rootObj.value(keys.at(1)).toString(),
                  rootObj.value(keys.at(0)).toString(),
                  rootObj.value(keys.at(2)).toString(),
                  rootObj.value(keys.at(4)).toString());
        udpdatalist.push_back(u);
        delete jsonDoc;
        delete jsonerror;
    }
    filterAddress();
}
void NetWindow::filterSlot() { filterAddress(); }
/**
 * @brief NetWindow::filterAddress
 * @details 将接收到的数据，根据房间类别进行筛选显示
 */
void NetWindow::filterAddress() {
    ui->addressListWidget->clear();
    for (auto data : udpdatalist) {
        if (data.gamemodel == ui->forbiddenCombolBox->currentText())
            if (data.chesscolor == ui->colorCombolBox->currentText() ||
                (data.israndom == "1" &&
                 ui->colorCombolBox->currentText() == "随机"))
                ui->addressListWidget->addItem(data.name);
    }
}
/**
 * @brief 绑定接收端口，接收广播数据
 */
void NetWindow::receiveBroadcast() {
    port = 10124;
    while (!listenSocket.bind(QHostAddress::Any, port)) {
        port++;
    };
    // 收到和出现错误时
}
/**
 * @brief 点击确定后，创建房间
 */
void NetWindow::on_openHouseButton_clicked() {
    if (ui->name->text() != "") {
        listenSocket.close();
        openhousedialog = new OpenHouseDialog(nullptr, ui->name->text());
        connect(openhousedialog, &OpenHouseDialog::back, this,
                &NetWindow::backSlot);
        connect(openhousedialog, &OpenHouseDialog::cancel, this,
                &NetWindow::cancelSlot);
        this->hide();
        openhousedialog->show();
    } else {
        QMessageBox::critical(this, "消息提示框", "用户名不能为空！");
    }
}

void NetWindow::backSlot() {
    openhousedialog->hide();
    delete openhousedialog;
    this->show();
}

/**
 * @brief  关闭棋盘时触发，回到主窗口
 */
void NetWindow::cancelSlot() {
    delete rPVP2;
    delete openhousedialog;
    emit backToMain();
}
void NetWindow::closeEvent(QCloseEvent *e) {
    listenSocket.close();
    emit backToMain();
}

/**
 * @brief operator == 运算符==
 * @details 重载运算符
 * @param a QNetworkDatagram类型数据
 * @param b QNetworkDatagram类型数据
 * @return 返回比较结果
 */
bool operator==(const QNetworkDatagram &a, const QNetworkDatagram &b) {
    return a.data() == b.data();
}

void NetWindow::updataIp() {
    listenSocket.close();
    ui->addressListWidget->clear();
    datagramlist.clear();
    udpdatalist.clear();
    receiveBroadcast();
}
/**
 * @brief 点击窗口上的房主用户名时，触发，创建tcp对象，进行连接。
 */
void NetWindow::beginGame() {
    int index = ui->addressListWidget->currentRow();
    QNetworkDatagram d = datagramlist.at(index);
    QHostAddress h = d.senderAddress();
    UdpData d1 = udpdatalist.at(index);
    int p = d1.port.toInt();
    int model = 0;
    if (d1.gamemodel == "有禁手") {
        model = 1;
    }
    delete rPVP2;
    if (d1.chesscolor == "白棋") {
        rPVP2 = new Chessboard_Remote_PVP_Client(WHITE, h, p, nullptr, model);
    } else {
        rPVP2 = new Chessboard_Remote_PVP_Client(BLACK, h, p, nullptr, model);
    }
    connect(rPVP2, &Chessboard_Remote_PVP_Client::refuseLink, this,
            &NetWindow::refuseLink);
    connect(rPVP2, &Chessboard_Remote_PVP_Client::gameStart, this,
            &NetWindow::showGame);
    connect(rPVP2, &Chessboard_Remote_PVP_Client::cancelToMain, this,
            &NetWindow::cancelSlot);
}
