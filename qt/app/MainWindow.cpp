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

    serial_ = new MySerial(this);

    connect(serial_, SIGNAL(recvChannelData(uint16_t *)), this, SLOT(recvChannelData(uint16_t *)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::recvChannelData(uint16_t *chl)
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

