#ifndef MYCAN_H
#define MYCAN_H

#include <QObject>
#include "ControlCAN.h"

class CanException : public std::logic_error
{
public:
    CanException(QString what) : std::logic_error(what.toStdString())
    {

    }
};

class MyCan : public QObject
{
    Q_OBJECT

private:
    bool is_open_;
    int devices_num_;
    int can_index_;
    int device_type_;
    int device_index_;
    VCI_BOARD_INFO board_info_[50];

public:
    MyCan(QObject* parrent);

    QList<QString> getCanDevices();

    bool isOpen() const;

    void open(int device_type, int device_index, int can_index);

    void send(QString dat);
};

#endif // MYCAN_H
