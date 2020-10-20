#include "MyCan.h"
#include "QDebug"

MyCan::MyCan(QObject *parrent) : QObject(parrent), is_open_(false)
{

}

QList<QString> MyCan::getCanDevices()
{
    QString ProductSn[50];
    devices_num_ = VCI_FindUsbDevice2(board_info_);

    QString strtemp,str;
    for(int i=0;i<devices_num_;i++)
    {
        str="";
        for(int j=0;j<20;j++)
        {
            strtemp = board_info_[i].str_Serial_Num[j];
            str+=strtemp;
        }
        ProductSn[i]="USBCAN-"+str;
    }

    QList<QString> devices;
    for (int i = 0; i < devices_num_; i++)
    {
        devices.append(ProductSn[i]);
    }
    return devices;
}

bool MyCan::isOpen() const
{
    return is_open_;
}

void MyCan::open(int device_type, int device_index, int can_index)
{
    VCI_INIT_CONFIG config;

    config.Mode = 0;
    config.Filter = 0;

    if (VCI_OpenDevice(device_type, device_index, can_index) != 1)
        throw CanException("VCI_OpenDevice");
    if (VCI_InitCAN(device_type, device_index, can_index, &config) != 1)
        throw CanException("VCI_InitCAN");
    if (VCI_StartCAN(device_type, device_index, can_index) != 1)
        throw CanException("VCI_StartCAN");
}

void MyCan::send(QString dat)
{
    VCI_CAN_OBJ obj;
    VCI_Transmit(device_type_, device_index_, can_index_, &obj, 1);
    Q_UNUSED(dat);
}
