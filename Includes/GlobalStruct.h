#ifndef GLOBALSTRUCT_H
#define GLOBALSTRUCT_H

#include <QString>
#include <QDateTime>
#include "Includes/globalenum.h"
#include "cdfsocket.h"
class SocketManager;
class StationModel;
class BaseWindows;
class CDFSocket;

struct StationStruct {
    int eID;
    int mode;
    QString name;
    QString icon;
    QString ip;
    QString mac;
    int connection;
    int tcp;
    int ocx;
    int udpSend;
    int udpReseive;
    QString serialCom;
    int serialBaudRate;
    QString ipScan;
    QString macScan;
    bool syncScan;
    int connectionScan;
    int tcpScan;
    int ocxScan;
    int udpSendScan;
    int udpReseiveScan;
    QString serialComScan;
    int serialBaudRateScan;
    bool gps;
    int position;
    QString pLat;
    QString pLong;
    QString note;
    bool isAuto;
    bool isDeleted;
    QDateTime  lastSeen;
    QDateTime  createTime;
    int userID;
};

struct SocketStruct {
    SocketManager* socket = nullptr;
    EnumSystemMode role = Role_DF;
    EnumSocketState state = Socket_Free;
    bool isSynced = true;
    bool isMainSystem = false;
    int stationId = -1;
    StationModel* station = nullptr;
    QVector<BaseWindows*> pages;
    SystemDefualtsStruct sysDefualt;
};

struct GeoPointStruct
{
    double lat = 0.0;
    double lon = 0.0;
    int pointIndex = 0;
};

struct MapDfStationStruct
{
    int id = -1;
    QString name = "DfStation";
    double lat = 0.0;
    double lon = 0.0;
    double angle = -1.0;
    bool isVisible = true;
};

struct MapItemStyleStruct
{
    QString strokeColor = "#00FF00";
    QString fillColor = "#3300FF00";
    QString textColor = "#FFFFFF";
    int strokeWidth = 2;
    int pointSize = 8;
    int fontSize = 12;
    double opacity = 1.0;
    double rotation = 0.0;
    QString iconPath = "";
};

struct MapItemStruct {
    int eID = -1;
    QString name = "NewItem";
    EnumMapShapeType type = Marker;
    QVector<GeoPointStruct> points;
    QString note = "";
    MapItemStyleStruct style;
    bool isVisible = true;
    bool isDeleted = false;
    QDateTime  createTime;
    int layerID = -1;
    int userID = -1;
};

struct MapLayerStruct {
    int eID = -1;
    QString name = "";
    bool isVisible = true;
    bool isDeleted = false;
    int sortOrder = 0;
    QDateTime  createTime;
    int userID = -1;
    QVector<MapItemStruct> items;
};

struct CareTableStruct {
    int eID = -1;
    QString name = "";
    double freq = 300;
    double bw = 80;
    double threshold = -160;
    double level = -160;
    double azimuth = -1;
    bool isVisible = true;
    bool isDeleted = false;
    QDateTime  lastSeenTime;
    QDateTime  createTime;
    int userID = -1;
};

#endif // GLOBALSTRUCT_H
