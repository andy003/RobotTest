#include <QDebug>
#include "MySerial.h"

MySerial::MySerial(QObject* parrent) : QObject(parrent), recv_index_(0), recv_state_(NORMAL)
{
    connect(&serial_, &QSerialPort::readyRead, this, &MySerial::recvData);
}

bool MySerial::isOpen() const
{
    return serial_.isOpen();
}

void MySerial::open(QString port, uint baudrates, QSerialPort::DataBits data_bits, QSerialPort::StopBits stop_bits, QSerialPort::Parity parity)
{
    serial_.setPortName(port);
    if (!serial_.setBaudRate(baudrates))
        throw MySerialException("setBaudRate : " + serial_.errorString());
    if (!serial_.setDataBits(data_bits))
        throw MySerialException("setDataBits : " + serial_.errorString());
    if (!serial_.setStopBits(stop_bits))
        throw MySerialException("setStopBits : " + serial_.errorString());
    if (!serial_.setParity(parity))
        throw MySerialException("setParity : " + serial_.errorString());
    if (!serial_.setFlowControl(QSerialPort::NoFlowControl))
        throw MySerialException("setFlowControl : " + serial_.errorString());
    if (!serial_.open(QSerialPort::ReadWrite))
        throw MySerialException("open : " + serial_.errorString());
}

void MySerial::close()
{
    serial_.close();
}

void MySerial::send(QString str)
{
    Pdu pdu;
    pdu.code = 1;
    pdu.rf_state_ = 0;
    memcpy(pdu.dat, str.toStdString().c_str(), str.length() + 1);
    slipSend((uint8_t *)&pdu, str.length() + 3);
}

void MySerial::recvData()
{
    QByteArray dat = serial_.readAll();
    const char*tmp = dat.data();
    for (int i = 0; i < dat.size(); i++)
        parseChar(tmp[i]);
}

void MySerial::slipSend(uint8_t* dat, uint32_t len)
{
    serial_.putChar(SLIP_END);
    while (len--)
    {
        switch (*dat)
        {
            case SLIP_END:
                serial_.putChar(SLIP_ESC);
                serial_.putChar(SLIP_ESC_END);
                break;

            case SLIP_ESC:
                serial_.putChar(SLIP_ESC);
                serial_.putChar(SLIP_ESC_ESC);
                break;

            default:
                serial_.putChar(*dat);
                break;
        }
        dat++;
    }
    serial_.putChar(SLIP_END);
}

void MySerial::parseChar(uint8_t dat)
{
    switch (recv_state_)
    {
    case NORMAL:
        if (dat == SLIP_END)
        {
            if (recv_index_ != 0)
            {
                recvFrame(frame_buffer_, recv_index_);
            }
            recv_index_ = 0;
        }
        else if (dat == SLIP_ESC)
        {
            recv_state_ = ESCAPE;
            break;
        }
        else
        {
            if (recv_index_ < sizeof(frame_buffer_))
            {
                frame_buffer_[recv_index_++] = dat;
            }
        }
        break;

    case ESCAPE:
        switch (dat)
        {
        case SLIP_ESC_END:
            dat = SLIP_END;
            break;

        case SLIP_ESC_ESC:
            dat = SLIP_ESC;
            break;
        }
        if (recv_index_ < sizeof(frame_buffer_))
        {
            frame_buffer_[recv_index_++] = dat;
        }
        recv_state_ = NORMAL;
        break;
    }
}

void MySerial::recvFrame(uint8_t *buf, uint32_t len)
{
    Q_UNUSED(len);
    Pdu* pdu = (Pdu*) buf;

    switch (pdu->code)
    {
        case 0:
            emit recvChannelData((uint16_t *)pdu->dat);
            break;
        case 1:
            emit recvEchoData((char *)pdu->dat);
            break;
    }

}
