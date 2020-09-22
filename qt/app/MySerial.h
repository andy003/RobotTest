#ifndef MYSERIAL_H
#define MYSERIAL_H

#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#define SLIP_END 0300     /* indicates end of packet */
#define SLIP_ESC 0333     /* indicates byte stuffing */
#define SLIP_ESC_END 0334 /* ESC ESC_END means END data byte */
#define SLIP_ESC_ESC 0335 /* ESC ESC_ESC means ESC data byte */

class MySerialException : public std::logic_error
{
public:
    MySerialException(QString what) : std::logic_error(what.toStdString())
    {

    }
};

class MySerial : public QObject
{
    Q_OBJECT

    enum RecvState
    {
        NORMAL = 0, /**< 正常接收 */
        ESCAPE,     /**< 需要转义 */
    };

private:
    QSerialPort serial_;
    uint32_t recv_index_;
    RecvState recv_state_;
    uint8_t frame_buffer_[100];

public:
    /**
     * @brief MySerial
     * @param parrent
     */
    MySerial(QObject* parrent);

    /**
     * @brief 判断串口是否打开
     * @return true 打开，false 关闭
     */
    bool isOpen() const;

    /**
     * @brief 打开串口
     * @param 端口
     * @param 波特率
     * @param 数据位
     * @param 停止位
     * @param 奇偶校验位
     */
    void open(QString port, uint baudrates, QSerialPort::DataBits data_bits,
              QSerialPort::StopBits stop_bits, QSerialPort::Parity parity);

    /**
     * @brief 关闭串口
     */
    void close();

private slots:
    void recvData();

Q_SIGNALS:
    void recvChannelData(uint16_t* chl);

private:
    void parseChar(uint8_t dat);
    void recvFrame(uint8_t* buf, uint32_t len);
    uint32_t getPacket(void* buf, uint32_t len);
};

#endif // MYSERIAL_H
