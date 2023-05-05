#include "openhousedialog.h"

#include "ui_openhousedialog.h"
/**
 * @brief OpenHouseDialog::OpenHouseDialog 传参构造函数
 * @details 创建房间界面
 * @param parent
 * @param name 将用户名传入，将用于发送广播数据。
 */
OpenHouseDialog::OpenHouseDialog(QWidget *parent, QString name)
    : QDialog(parent), ui(new Ui::OpenHouseDialog) {
    this->name = name;
    ui->setupUi(this);

    QString strQss =
        getQssString(QString(":/resources"
                             "/dialog_style.css"));
    //    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setStyleSheet(strQss);

    time = new QTimer();
    connect(time, &QTimer::timeout, this, &OpenHouseDialog::upsenddata);
}

OpenHouseDialog::~OpenHouseDialog() { delete ui; }
/**
 * @brief OpenHouseDialog::showGame
 * @details tcp连接成功，显示游戏画面
 */
void OpenHouseDialog::showGame() {
    time->stop();
    this->hide();
    rPVP->show();
}

void OpenHouseDialog::closeEvent(QCloseEvent *e) {
    this->hide();
    emit cancel();
}
void OpenHouseDialog::cancelSlot() {
    delete rPVP;
    this->hide();
    emit cancel();
}
/**
 * @brief OpenHouseDialog::sendBroadcast
 * @details 创建房间，发送广播数据
 * @param rand 0~1之间的随机数，用于随机棋子颜色时使用。
 */
void OpenHouseDialog::sendBroadcast(double rand) {
    QList<QString> ips = getIpListOfComputer();
    for (auto &ip : ips) {
        auto *s = new QUdpSocket(this);
        s->bind(QHostAddress(ip));
        sendSockets.push_back(s);
    }
    for (auto i : sendSockets) {
        QJsonObject jsonobject;
        jsonobject.insert("name", this->name);
        jsonobject.insert("gamemodel", ui->forbiddenCombolBox->currentText());
        QString s = ui->colorCombolBox->currentText();
        QString b;
        if (s == "白棋") {
            b = "0";
            s = "黑棋";
        } else if (s == "黑棋") {
            b = "0";
            s = "白棋";
        } else {
            b = "1";
            // 生成一个0和1.0之间的浮点数
            // 产生十以内的随机数(0-9)
            if (rand >= 0.5) {
                s = "白棋";
            } else {
                s = "黑棋";
            }
        }
        delete rPVP;
        jsonobject.insert("chesscolor", s);
        jsonobject.insert("israndom", b);
        if (s == "黑棋") {
            rPVP = new Chessboard_Remote_PVP_Server(WHITE, nullptr, 0);
        } else {
            rPVP = new Chessboard_Remote_PVP_Server(BLACK, nullptr, 0);
        }
        connect(rPVP, &Chessboard_Remote_PVP_Server::gameStart, this,
                &OpenHouseDialog::showGame);
        connect(rPVP, &Chessboard_Remote_PVP_Server::cancelToMain, this,
                &OpenHouseDialog::cancelSlot);
        QString port = QString::number(rPVP->getPort());
        jsonobject.insert("port", port);
        QJsonDocument jsondocument;
        jsondocument.setObject(jsonobject);
        QByteArray dataarray = jsondocument.toJson();
        i->writeDatagram(dataarray, QHostAddress::Broadcast, 10124);
    }
}
/**
 * @brief OpenHouseDialog::getIpListOfComputer
 * @details
 * 考虑到不同的用户可能拥有多个网卡，这就会导致出现虚拟网卡发送广播数据，而实际的网卡不进行
 *  发送广播数据，此函数可以将对用户计算机进行搜索和检测所有网卡，并返回ip地址
 * @return 返回满足条件的网卡ip地址
 */
QList<QString> OpenHouseDialog::getIpListOfComputer() {
    QList<QString> ret_list;
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interfaceItem : interfaceList) {
        if (interfaceItem.flags().testFlag(QNetworkInterface::IsUp) &&
            interfaceItem.flags().testFlag(QNetworkInterface::IsRunning) &&
            interfaceItem.flags().testFlag(QNetworkInterface::CanBroadcast) &&
            interfaceItem.flags().testFlag(QNetworkInterface::CanMulticast) &&
            !interfaceItem.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            QList<QNetworkAddressEntry> addressEntryList =
                interfaceItem.addressEntries();
            for (QNetworkAddressEntry &addressEntryItem : addressEntryList) {
                if (addressEntryItem.ip().protocol() ==
                    QAbstractSocket::IPv4Protocol) {
                    ret_list.append(addressEntryItem.ip().toString());
                }
            }
        }
    }
    return ret_list;
}
/**
 * @brief OpenHouseDialog::on_okButton_clicked
 * @details 通过广播发送房间信息
 */
void OpenHouseDialog::on_okButton_clicked() {
    if (!sendSockets.isEmpty()) {
        sendSockets.clear();
    }
    rand = QRandomGenerator::global()->bounded(1.0);
    sendBroadcast(rand);
    time->start(1000);
    QMessageBox::information(this, "提示框", "创建房间成功，等待进入房间！",
                             QMessageBox::Ok);
    ui->okButton->setDisabled(true);
}
/**
 * @brief OpenHouseDialog::upsenddata
 *@details  重新发送房间信息
 */
void OpenHouseDialog::upsenddata() {
    sendSockets.clear();
    sendBroadcast(rand);
}

void OpenHouseDialog::on_cancelButton_clicked() { emit back(); }
