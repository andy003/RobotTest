#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>

#include "MyCan.h"
#include "MySerial.h"
#include "MyNetwork.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    MyCan* can_;
    MySerial* serial_;
    MyNetwork* network_;

private slots:
    void tcpConnect();
    void serialRecvChannelData(uint16_t* chl);
    void nettestClicked();
    void networkSend();
    void setNetworkMode(int mode);
    void networkRecvData(QByteArray dat);
    void networkClearRecvData();
    void serialSend();
    void serialEchoRecv(QString str);
    void serialClear();
    void canOpen();

private:
    void serialOpenBtnClick(bool click);

    QSerialPort::Parity getSerialParity();
    QSerialPort::DataBits getSerialDataBits();
    QSerialPort::StopBits getSerialStopBits();
};

#endif // MAINWINDOW_H
