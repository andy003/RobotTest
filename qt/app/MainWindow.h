#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include "MySerial.h"

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
    MySerial* serial_;

private slots:
    void recvChannelData(uint16_t* chl);

private:
    void serialOpenBtnClick(bool click);

    QSerialPort::Parity getSerialParity();
    QSerialPort::DataBits getSerialDataBits();
    QSerialPort::StopBits getSerialStopBits();
};

#endif // MAINWINDOW_H
