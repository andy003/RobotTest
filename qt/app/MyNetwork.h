#ifndef MYNETWORK_H
#define MYNETWORK_H

#include <QObject>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QTcpSocket>
#include <QTcpServer>

class MyNetworkException : public std::logic_error
{
public:
    MyNetworkException(QString what) : std::logic_error(what.toStdString())
    {

    }
};

class MyNetwork : public QObject
{
    Q_OBJECT

public:
    enum Mode
    {
        UDP_CLIENT = 0,
        UDP_SERVER,
        TCP_CLIENT,
        TCP_SERVER
    };

private:
    Mode mode_;
    bool is_open_;
    uint16_t remote_port_;
    QHostAddress remote_ip_;
    QUdpSocket* udp_client_;
    QUdpSocket* udp_server_;
    QTcpSocket* tcp_client_;
    QTcpServer* tcp_server_;
    QTcpSocket* tcp_connect_;

public:
    /**
     * @brief MyNetwork
     * @param pattent
     */
    MyNetwork(QObject* pattent);

    /**
     * @brief open
     * @param index
     * @param local_port
     * @param remote_ip
     * @param remote_port
     */
    void open(int index, uint16_t local_port, QString remote_ip, uint16_t remote_port);

    /**
     * @brief isOpen
     * @return
     */
    bool isOpen() const;

    /**
     * @brief close
     */
    void close();

    /**
     * @brief send
     * @param dat
     */
    void send(QString dat);

    /**
     * @brief tcpConnect
     */
    void tcpConnect();

    /**
     * @brief isTcpConnected
     * @return
     */
    bool isTcpConnected() const;

    /**
     * @brief tcpDisconnect
     */
    void tcpDisconnect();

public slots:
    void tcpNewConnection();

signals:
    void recvData(QByteArray dat);

private:
    void udpRecvReady();
    void tcpClientRecvReady();
    void tcpServerRecvReady();
};

#endif // MYNETWORK_H
