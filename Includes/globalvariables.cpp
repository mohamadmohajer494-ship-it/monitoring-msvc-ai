#include "globalvariables.h"

GlobalVariables::GlobalVariables(QObject *parent)
    : QObject{parent}
{}

void GlobalVariables::getAllData()
{
    repStation = new RepositoryStation(dataBase);
    stationSelect = repStation->selectAll();
    createStationList();
    if (!stationList.isEmpty())
        emit LoadDataBaseStation();
}

void GlobalVariables::addToStationList(StationModel *station)
{
    if (!station)
        return;
    stationList.append(station);
}

void GlobalVariables::removeStation(int stationId)
{
    for (int i = 0; i < stationList.size(); ++i) {
        if (stationList[i]->getID() == stationId) {
            StationModel *ptr = stationList[i];
            stationList.removeAt(i);
            delete ptr;
            break;
        }
    }
}

StationModel* GlobalVariables::findStationById(int id)
{
    for (StationModel* station : stationList) {
        if (station && station->getID() == id) {
            return station;
        }
    }
    return nullptr;
}

void GlobalVariables::addToSocketList(SocketManager *socket)
{
    if (!socket)
        return;
    socketList.append(socket);
}

SocketManager *GlobalVariables::findSocketByStationId(int id)
{
    for (SocketManager* socket : socketList) {
        if (socket && socket->getStationId() == id) {
            return socket;
        }
    }
    return nullptr;
}


void GlobalVariables::createStationList()
{
    for (int var = 0; var < stationSelect.size(); ++var) {
        StationModel *newStatin = new StationModel();
        newStatin->setID(stationSelect[var].eID);
        newStatin->setSysMode(static_cast<EnumSystemMode>(stationSelect[var].mode));
        newStatin->setSysName(stationSelect[var].name);
        // newStatin->setSysIcon(stationSelect[var].icon);
        newStatin->setSysIp(stationSelect[var].ip);
        newStatin->setSysMac(stationSelect[var].mac);
        newStatin->setSysConnectionType(static_cast<EnumConnectionType>(stationSelect[var].connection));
        newStatin->setPortTcp(stationSelect[var].tcp);
        newStatin->setPortOcx(stationSelect[var].ocx);
        newStatin->setPortUdpSend(stationSelect[var].udpSend);
        newStatin->setPortUdpReceive(stationSelect[var].udpReseive);
        newStatin->setPortSerCom(stationSelect[var].serialCom);
        newStatin->setPortSerBaudRate(stationSelect[var].serialBaudRate);
        newStatin->setScanSync(stationSelect[var].syncScan);
        newStatin->setScanIp(stationSelect[var].ipScan);
        newStatin->setScanMac(stationSelect[var].macScan);
        newStatin->setScanConnectionType(static_cast<EnumConnectionType>(stationSelect[var].connectionScan));
        newStatin->setScanTcp(stationSelect[var].tcpScan);
        newStatin->setScanOcx(stationSelect[var].ocxScan);
        newStatin->setScanUdpSend(stationSelect[var].udpSendScan);
        newStatin->setScanUdpReceive(stationSelect[var].udpReseiveScan);
        newStatin->setScanSerCom(stationSelect[var].serialComScan);
        newStatin->setScanSerBaudRate(stationSelect[var].serialBaudRateScan);
        newStatin->setGPS(stationSelect[var].gps);
        newStatin->setPosType(static_cast<EnumPositionType>(stationSelect[var].position));
        newStatin->setPosLat(stationSelect[var].pLat);
        newStatin->setPosLong(stationSelect[var].pLong);
        newStatin->setSysNote(stationSelect[var].note);
        stationList.append(newStatin);
    }
}

QPixmap GlobalVariables::changeColorIcon(const QString &path, const QColor &color, const QSize &size)
{
    QPixmap src(path);
    QPixmap pixmap = src.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            QColor pixelColor = QColor::fromRgba(img.pixel(x, y));
            int alpha = pixelColor.alpha();
            if (alpha > 0) {
                img.setPixelColor(x, y, QColor(color.red(), color.green(), color.blue(), alpha));
            }
        }
    }
    return QPixmap::fromImage(img);
}

QFont GlobalVariables::changeFont(const QString &path, const int &size, const bool &isbold)
{
    int fontID = QFontDatabase::addApplicationFont(path);
    QString family = QFontDatabase::applicationFontFamilies(fontID).at(0);
    QFont font(family);
    font.setPointSize(size);
    font.setBold(isbold);
    return font;
}
