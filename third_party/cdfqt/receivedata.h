#ifndef RECEIVEDATA_H
#define RECEIVEDATA_H

#include <QObject>
#include <QTcpSocket>
#include <Connection.pb.h>
#define HEADERSIZE 2
#define FOOTERSIZE 2
#define HEADER1 0xAA
#define HEADER2 0xFF
#define FOOTER1 0x55
#define FOOTER2 0x55
class ReceiveData : public QObject
{
    Q_OBJECT
public:
    explicit ReceiveData(QTcpSocket *TcpSocket,QObject *parent = nullptr);
private:
    QTcpSocket *TcpSocket;
    QByteArray data;
    bool ResumeFlag = false;
public slots:
    void started();
signals:

};

#endif // RECEIVEDATA_H
