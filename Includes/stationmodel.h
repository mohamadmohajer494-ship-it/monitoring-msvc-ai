#ifndef STATIONMODEL_H
#define STATIONMODEL_H

#include <QObject>
#include "Includes/GlobalEnum.h"
#include "cdfsocket.h"

class StationModel : public QObject
{
    Q_OBJECT
public:
    explicit StationModel(QObject *parent = nullptr);

private:
    int iID = 0;
    EnumSystemMode sysMode = Role_Monitoring;
    QString sysName = "New Station";
    QString sysIcon = "";
    QString sysIp = "192.168.0.10";
    QString sysMac = "A1:B2:C3:D4:E5";
    EnumConnectionType sysConnectionType = TCP_TCP;
    int portTcp = 8085;
    int portOcx = 8085;
    int portUdpSend = 8088;
    int portUdpReceive = 1024;
    QString portSerCom = "COM3";
    int portSerBaudRate = 115200;
    bool isScanSync = true;
    QString scanIp = "192.168.0.11";
    QString scanMac = "A1:B2:C3:D4:E5";
    EnumConnectionType scanConnectionType = TCP_TCP;
    int scanTcp = 8085;
    int scanOcx = 8085;
    int scanUdpSend = 8088;
    int scanUdpReceive = 1024;
    QString scanSerCom = "COM3";
    int scanSerBaudRate = 115200;
    bool isGps = false;
    EnumPositionType posType = DMS;
    QString posLat = "35.6892";
    QString posLong = "51.3890";
    QString sysNote = "";
    // SocketManager *socketManager = nullptr;
    // QThread *socketThread = nullptr;
    // int socketIndex = -1;
    bool isConnected = false;
    bool isConnecting = false;

public:
    int getID() { return iID; }
    void setID(int id) { iID = id; }
    EnumSystemMode getSysMode() { return sysMode; }
    void setSysMode(EnumSystemMode mode) { sysMode = mode; }
    QString getSysName() { return sysName; }
    void setSysName(QString name) { sysName = name; }
    QString getSysIcon() { return sysIcon; }
    void setSysIcon(QString icon) { sysIcon = icon; }
    QString getSysIp() { return sysIp; }
    void setSysIp(QString ip) { sysIp = ip; }
    QString getSysMac() { return sysMac; }
    void setSysMac(QString mac) { sysMac = mac; }
    EnumConnectionType getSysConnectionType() { return sysConnectionType; }
    void setSysConnectionType(EnumConnectionType type) { sysConnectionType = type; }
    int getPortTcp() { return portTcp; }
    void setPortTcp(int tcp) { portTcp = tcp; }
    int getPortOcx() { return portOcx; }
    void setPortOcx(int ocx) { portOcx = ocx; }
    int getPortUdpSend() { return portUdpSend; }
    void setPortUdpSend(int send) { portUdpSend = send; }
    int getPortUdpReceive() { return portUdpReceive; }
    void setPortUdpReceive(int rece) { portUdpReceive = rece; }
    QString getPortSerCom() { return portSerCom; }
    void setPortSerCom(QString com) { portSerCom = com; }
    int getPortSerBaudRate() { return portSerBaudRate; }
    void setPortSerBaudRate(int baud) { portSerBaudRate = baud; }
    bool getScanSync() { return isScanSync; }
    void setScanSync(bool sync) { isScanSync = sync; }
    QString getScanIp() { return scanIp; }
    void setScanIp(QString ip) { scanIp = ip; }
    QString getScanMac() { return scanMac; }
    void setScanMac(QString mac) { scanMac = mac; }
    EnumConnectionType getScanConnectionType() { return scanConnectionType; }
    void setScanConnectionType(EnumConnectionType type) { scanConnectionType = type; }
    int getScanTcp() { return scanTcp; }
    void setScanTcp(int tcp) { scanTcp = tcp; }
    int getScanOcx() { return scanOcx; }
    void setScanOcx(int ocx) { scanOcx = ocx; }
    int getScanUdpSend() { return scanUdpSend; }
    void setScanUdpSend(int send) { scanUdpSend = send; }
    int getScanUdpReceive() { return scanUdpReceive; }
    void setScanUdpReceive(int rece) { scanUdpReceive = rece; }
    QString getScanSerCom() { return scanSerCom; }
    void setScanSerCom(QString com) { scanSerCom = com; }
    int getScanSerBaudRate() { return scanSerBaudRate; }
    void setScanSerBaudRate(int baud) { scanSerBaudRate = baud; }
    bool getGps() { return isGps; }
    void setGPS(bool gps) { isGps = gps; }
    EnumPositionType getPosType() { return posType; }
    void setPosType(EnumPositionType type) { posType = type; }
    QString getPosLat() { return posLat; }
    void setPosLat(QString pos) { posLat = pos; }
    QString getPosLong() { return posLong; }
    void setPosLong(QString pos) { posLong = pos; }
    QString getSysNote() { return sysNote; }
    void setSysNote(QString text) { sysNote = text; }
    bool getConnected() { return isConnected; }
    void setConnected(bool connect) { isConnected = connect; }

public:
    void connectToSystem();
    void disconnectFromSystem();

signals:
};

#endif // STATIONMODEL_H
