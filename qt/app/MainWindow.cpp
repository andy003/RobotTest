#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->serial_btn_, &QPushButton::clicked, this, &MainWindow::serialOpenBtnClick);

    for(QSerialPortInfo port : QSerialPortInfo::availablePorts())
    {
        ui->serial_port_cmb_->addItem(port.portName());
    }

    QPalette palette(ui->serial_status_->palette());
    palette.setColor(QPalette::Background, Qt::red);
    ui->serial_status_->setAutoFillBackground(true);
    ui->serial_status_->setPalette(palette);

    QPalette pa(ui->nettest_status_->palette());
    pa.setColor(QPalette::Background, Qt::red);
    ui->nettest_status_->setAutoFillBackground(true);
    ui->nettest_status_->setPalette(pa);

    setNetworkMode(ui->nettest_mode_cbox_->currentIndex());

    serial_ = new MySerial(this);
    network_ = new MyNetwork(this);

    ui->serial_send_text_->setEnabled(false);
    ui->serial_send_btn_->setEnabled(false);

    connect(serial_, &MySerial::recvChannelData, this, &MainWindow::serialRecvChannelData);
    connect(ui->nettest_open_btn_, &QPushButton::clicked, this, &MainWindow::nettestClicked);
    connect(ui->nettest_send_btn_, &QPushButton::clicked, this, &MainWindow::networkSend);
    connect(ui->nettest_mode_cbox_, SIGNAL(currentIndexChanged(int)), this, SLOT(setNetworkMode(int)));
    connect(network_, &MyNetwork::recvData, this, &MainWindow::networkRecvData);
    connect(ui->nettest_clear_btn_, &QPushButton::clicked, this, &MainWindow::networkClearRecvData);
    connect(ui->nettest_connect_btn_, &QPushButton::clicked, this, &MainWindow::tcpConnect);
    connect(ui->serial_send_btn_, &QPushButton::clicked, this, &MainWindow::serialSend);
    connect(serial_, &MySerial::recvEchoData, this, &MainWindow::serialEchoRecv);
    connect(ui->serial_clear_btn_, &QPushButton::clicked, this, &MainWindow::serialClear);
    setFixedSize(this->width(), this->height());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::tcpConnect()
{
    if (network_->isTcpConnected())
    {
        network_->tcpDisconnect();
        ui->nettest_connect_btn_->setText("连接");
    }
    else
    {
        network_->tcpConnect();
        ui->nettest_connect_btn_->setText("断开");
    }
}

void MainWindow::serialRecvChannelData(uint16_t *chl)
{
    Q_UNUSED(chl);

    ui->ch1_num_->display(chl[0]);
    ui->ch2_num_->display(chl[1]);
    ui->ch3_num_->display(chl[2]);
    ui->ch4_num_->display(chl[3]);
    ui->ch5_num_->display(chl[4]);
    ui->ch6_num_->display(chl[5]);
    ui->ch7_num_->display(chl[6]);
    ui->ch8_num_->display(chl[7]);
}

void MainWindow::nettestClicked()
{
    try
    {
        if (network_->isOpen())
        {
            network_->close();
            ui->nettest_open_btn_->setText("打开");
            ui->nettest_mode_cbox_->setEnabled(true);
            ui->nettest_remote_ip_text_->setEnabled(true);
            ui->nettest_local_port_text_->setEnabled(true);
            ui->nettest_remote_port_text_->setEnabled(true);

            QPalette palette(ui->nettest_status_->palette());
            palette.setColor(QPalette::Background, Qt::red);
            ui->nettest_status_->setAutoFillBackground(true);
            ui->nettest_status_->setPalette(palette);
        }
        else
        {
            bool ok;
            uint16_t local_port = 0, remote_port = 0;

            switch ((MyNetwork::Mode)ui->nettest_mode_cbox_->currentIndex())
            {
                case MyNetwork::UDP_CLIENT:
                case MyNetwork::UDP_SERVER:
                    local_port = ui->nettest_local_port_text_->text().toUInt(&ok);
                    if (!ok)
                        throw std::exception("本地端口填写错误");
                    remote_port = ui->nettest_remote_port_text_->text().toUInt(&ok);
                    if (!ok)
                        throw std::exception("远程端口填写错误");
                    break;

                case MyNetwork::TCP_CLIENT:
                    remote_port = ui->nettest_remote_port_text_->text().toUInt(&ok);
                    if (!ok)
                        throw std::exception("远程端口填写错误");
                    break;

                case MyNetwork::TCP_SERVER:
                    local_port = ui->nettest_local_port_text_->text().toUInt(&ok);
                    if (!ok)
                        throw std::exception("本地端口填写错误");
                    break;
            }
            network_->open(ui->nettest_mode_cbox_->currentIndex(), local_port, ui->nettest_remote_ip_text_->text(), remote_port);
            ui->nettest_mode_cbox_->setEnabled(false);
            ui->nettest_remote_ip_text_->setEnabled(false);
            ui->nettest_local_port_text_->setEnabled(false);
            ui->nettest_remote_port_text_->setEnabled(false);

            QPalette palette(ui->nettest_status_->palette());
            palette.setColor(QPalette::Background, Qt::blue);
            ui->nettest_status_->setAutoFillBackground(true);
            ui->nettest_status_->setPalette(palette);
            ui->nettest_open_btn_->setText("关闭");
        }

 }
    catch (const std::exception& err)
    {
        QMessageBox msg_box(this);
        msg_box.setText(err.what());
        msg_box.exec();
    }
}

void MainWindow::networkSend()
{
    QString send_buf = ui->nettest_send_dat_text_->toPlainText();

    if (send_buf.length() > 0)
        network_->send(ui->nettest_send_dat_text_->toPlainText());
    ui->nettest_send_dat_text_->clear();
}

void MainWindow::setNetworkMode(int mode)
{
    switch ((MyNetwork::Mode)mode)
    {
        case MyNetwork::UDP_CLIENT:
            ui->nettest_local_port_text_->setVisible(false);
            ui->nettest_local_port_label_->setVisible(false);
            ui->nettest_remote_ip_text_->setVisible(true);
            ui->nettest_remote_ip_label_->setVisible(true);
            ui->nettest_remote_port_text_->setVisible(true);
            ui->nettest_remote_port_label_->setVisible(true);
            ui->nettest_connect_btn_->setVisible(false);
            break;

        case MyNetwork::UDP_SERVER:
            ui->nettest_local_port_text_->setVisible(true);
            ui->nettest_local_port_label_->setVisible(true);
            ui->nettest_remote_ip_text_->setVisible(true);
            ui->nettest_remote_ip_label_->setVisible(true);
            ui->nettest_remote_port_text_->setVisible(true);
            ui->nettest_remote_port_label_->setVisible(true);
            ui->nettest_connect_btn_->setVisible(false);
            break;

        case MyNetwork::TCP_CLIENT:
            ui->nettest_local_port_text_->setVisible(false);
            ui->nettest_local_port_label_->setVisible(false);
            ui->nettest_remote_ip_text_->setVisible(true);
            ui->nettest_remote_ip_label_->setVisible(true);
            ui->nettest_remote_port_text_->setVisible(true);
            ui->nettest_remote_port_label_->setVisible(true);
            ui->nettest_connect_btn_->setVisible(true);
            break;

        case MyNetwork::TCP_SERVER:
            ui->nettest_local_port_text_->setVisible(true);
            ui->nettest_local_port_label_->setVisible(true);
            ui->nettest_remote_ip_text_->setVisible(false);
            ui->nettest_remote_ip_label_->setVisible(false);
            ui->nettest_remote_port_text_->setVisible(false);
            ui->nettest_remote_port_label_->setVisible(false);
            ui->nettest_connect_btn_->setVisible(false);
            break;

        default:
            break;
    }
}

void MainWindow::networkRecvData(QByteArray dat)
{
    ui->nettest_text_->append(dat);
}

void MainWindow::networkClearRecvData()
{
    ui->nettest_text_->clear();
}

void MainWindow::serialSend()
{
    QString str = ui->serial_send_text_->toPlainText();
    if (str.length() != 0)
        serial_->send(str);
}

void MainWindow::serialEchoRecv(QString str)
{
    ui->serial_recv_data_text_->append(str);
}

void MainWindow::serialClear()
{
    ui->serial_recv_data_text_->clear();
}

void MainWindow::serialOpenBtnClick(bool click)
{
    Q_UNUSED(click);

    try {
        if (serial_->isOpen())
        {
            serial_->close();
            ui->serial_btn_->setText("打开");

            QPalette palette(ui->serial_status_->palette());
            palette.setColor(QPalette::Background, Qt::red);
            ui->serial_status_->setAutoFillBackground(true);
            ui->serial_status_->setPalette(palette);
            ui->serial_send_text_->setEnabled(false);
            ui->serial_send_btn_->setEnabled(false);
        }
        else
        {
            serial_->open(ui->serial_port_cmb_->currentText(), ui->baudrates_cmb_->currentText().toUInt(),
                         getSerialDataBits(), getSerialStopBits(), getSerialParity());
            ui->serial_btn_->setText("关闭");
            QPalette palette(ui->serial_status_->palette());
            palette.setColor(QPalette::Background, Qt::blue);
            ui->serial_status_->setAutoFillBackground(true);
            ui->serial_status_->setPalette(palette);
            ui->serial_send_text_->setEnabled(true);
            ui->serial_send_btn_->setEnabled(true);
        }
    }
    catch (MySerialException err)
    {
        QMessageBox::warning(this, "串口异常", err.what());
    }
}

QSerialPort::Parity MainWindow::getSerialParity()
{
    QSerialPort::Parity parity = QSerialPort::NoParity;
    switch (ui->parity_cmb_->currentIndex())
    {
        case 0:
            parity = QSerialPort::NoParity;
            break;

        case 1:
            parity = QSerialPort::OddParity;
            break;

        case 2:
            parity = QSerialPort::EvenParity;
            break;
        default:
            break;
    }
    return parity;
}

QSerialPort::DataBits MainWindow::getSerialDataBits()
{
    QSerialPort::DataBits data_bits = QSerialPort::Data8;
    switch (ui->data_bits_cmb_->currentIndex())
    {
        case 0:
            data_bits = QSerialPort::Data5;
            break;

        case 1:
            data_bits = QSerialPort::Data6;
            break;

        case 2:
            data_bits = QSerialPort::Data7;
            break;
        case 3:
            data_bits = QSerialPort::Data8;
            break;
        default:
            break;
    }
    return data_bits;
}

QSerialPort::StopBits MainWindow::getSerialStopBits()
{
    QSerialPort::StopBits stop_bits = QSerialPort::OneStop;
    switch (ui->data_bits_cmb_->currentIndex())
    {
        case 0:
            stop_bits = QSerialPort::OneStop;
            break;

        case 1:
            stop_bits = QSerialPort::OneAndHalfStop;
            break;

        case 2:
            stop_bits = QSerialPort::TwoStop;
            break;

        default:
            break;
    }
    return stop_bits;
}

