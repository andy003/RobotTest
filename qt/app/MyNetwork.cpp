#include <QtDebug>
#include <QMessageBox>
#include "MyNetwork.h"

MyNetwork::MyNetwork(QObject* pattent) : QObject(pattent), is_open_(false)
{
    udp_client_ = new QUdpSocket(this);
    udp_server_ = new QUdpSocket(this);
    tcp_client_ = new QTcpSocket(this);
    tcp_server_ = new QTcpServer(this);
    tcp_connect_ = nullptr;

    connect(udp_server_, &QUdpSocket::readyRead, this, &MyNetwork::udpRecvReady);
    connect(tcp_client_, &QTcpSocket::readyRead, this, &MyNetwork::tcpClientRecvReady);
    connect(tcp_server_, &QTcpServer::newConnection, this, &MyNetwork::tcpNewConnection);
}

void MyNetwork::open(int index, uint16_t local_port, QString remote_ip, uint16_t remote_port)
{
    mode_ = (Mode)index;
    remote_port_ = remote_port;
    remote_ip_.setAddress(remote_ip);

    switch ((Mode)index)
    {
        case UDP_CLIENT:
            if (!udp_client_->open(QIODevice::ReadWrite))
                throw MyNetworkException(udp_client_->errorString());
            break;

        case UDP_SERVER:
            if (!udp_server_->open(QIODevice::ReadWrite))
                throw MyNetworkException(udp_server_->errorString());
            if (!udp_server_->bind(QHostAddress::Any, local_port))
                throw MyNetworkException(udp_server_->errorString());
            break;

        case TCP_CLIENT:
            if (!tcp_client_->open(QIODevice::ReadWrite))
                throw MyNetworkException(tcp_client_->errorString());
            break;

        case TCP_SERVER:
            if (!tcp_server_->listen(QHostAddress::Any, local_port))
                throw MyNetworkException(tcp_server_->errorString());
            tcp_server_->setMaxPendingConnections(1);
            break;
    }

    qDebug() << "get here initSocket" << (Mode) index;

    is_open_ = true;
}

bool MyNetwork::isOpen() const
{
    return is_open_;
}

void MyNetwork::close()
{
    switch (mode_)
    {
        case UDP_CLIENT:
            udp_client_->close();
            break;

        case UDP_SERVER:
            udp_server_->close();
            break;

        case TCP_CLIENT:
            tcp_client_->close();
            break;

        case TCP_SERVER:
            tcp_server_->close();
            break;
    }

    is_open_ = false;
}

void MyNetwork::send(QString dat)
{
    qint64 result;

    try
    {
        switch (mode_)
        {
        case UDP_CLIENT:
            result = udp_client_->writeDatagram(dat.toStdString().c_str(), dat.length(), remote_ip_, remote_port_);
            if (result <= 0)
                throw MyNetworkException(udp_client_->errorString());
            break;

        case UDP_SERVER:
            result = udp_server_->writeDatagram(dat.toStdString().c_str(), dat.length(), remote_ip_, remote_port_);
            if (result <= 0)
                throw MyNetworkException(udp_client_->errorString());
            break;

        case TCP_CLIENT:
            result = tcp_client_->write(dat.toStdString().c_str(), dat.length());
            if (result <= 0)
                throw MyNetworkException(udp_client_->errorString());
            break;

        case TCP_SERVER:
            if (tcp_connect_ != nullptr && tcp_connect_->state() == QTcpSocket::ConnectedState)
                tcp_connect_->write(dat.toStdString().c_str(), dat.length());
            break;
        }
    }
    catch (const MyNetworkException& err)
    {
        QMessageBox::about((QWidget*)this->parent(), "异常", err.what());
    }
}

bool MyNetwork::isTcpConnected() const
{
    return tcp_client_->state() == QTcpSocket::ConnectedState;
}

void MyNetwork::tcpDisconnect()
{
    tcp_client_->disconnect();
}

void MyNetwork::tcpNewConnection()
{
    qDebug() << "tcpNewConnection";
    while (tcp_server_->hasPendingConnections())
    {
        tcp_connect_ = tcp_server_->nextPendingConnection();
        connect(tcp_connect_, &QTcpSocket::readyRead, this, &MyNetwork::tcpServerRecvReady);
    }
}

void MyNetwork::tcpConnect()
{
    tcp_client_->connectToHost(QHostAddress(remote_ip_), remote_port_);
}

void MyNetwork::udpRecvReady()
{
    QByteArray dat;
    while (udp_server_->hasPendingDatagrams())
    {
        dat.resize(udp_server_->pendingDatagramSize());
        udp_server_->readDatagram(dat.data(), dat.size());
    }
    emit recvData(dat);
}

void MyNetwork::tcpClientRecvReady()
{
    QByteArray dat;

    dat = tcp_client_->readAll();
    emit recvData(dat);
}

void MyNetwork::tcpServerRecvReady()
{
    QByteArray dat;

    dat = tcp_connect_->readAll();
    emit recvData(dat);
}
