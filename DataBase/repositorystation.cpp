#include "repositorystation.h"

RepositoryStation::RepositoryStation(QSqlDatabase db)
    : BaseRepository(db)
{
}

bool RepositoryStation::insert(const StationStruct &item, int *outId)
{
    if (!isDbOpen())
        return false;
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO tblStation
        (mode, name, icon, ip, mac, connection, tcp, ocx, udpSend, udpReseive,
         serialCom, serialBaudrate, ipScan, macScan, connectionScan, tcpScan,
         ocxScan, udpSendScan, udpReseiveScan, serialComScan, serialBaudrateScan,
         gps, position, lat, long, note, isAuto, lastSeen, created, userID)
        VALUES
        (:mode, :name, :icon, :ip, :mac, :connection, :tcp, :ocx, :udpSend, :udpReseive,
         :serialCom, :serialBaudrate, :ipScan, :macScan, :connectionScan, :tcpScan,
         :ocxScan, :udpSendScan, :udpReseiveScan, :serialComScan, :serialBaudrateScan,
         :gps, :position, :lat, :long, :note, :isAuto, :lastSeen, :created, :userID)
        RETURNING eID
    )");
    query.bindValue(":mode", item.mode);
    query.bindValue(":name", item.name);
    query.bindValue(":icon", item.icon);
    query.bindValue(":ip", item.ip);
    query.bindValue(":mac", item.mac);
    query.bindValue(":connection", item.connection);
    query.bindValue(":tcp", item.tcp);
    query.bindValue(":ocx", item.ocx);
    query.bindValue(":udpSend", item.udpSend);
    query.bindValue(":udpReseive", item.udpReseive);
    query.bindValue(":serialCom", item.serialCom);
    query.bindValue(":serialBaudrate", item.serialBaudRate);
    query.bindValue(":ipScan", item.ip);
    query.bindValue(":macScan", item.mac);
    query.bindValue(":syncScan", item.syncScan);
    query.bindValue(":connectionScan", item.connectionScan);
    query.bindValue(":tcpScan", item.tcpScan);
    query.bindValue(":ocxScan", item.ocxScan);
    query.bindValue(":udpSendScan", item.udpSendScan);
    query.bindValue(":udpReseiveScan", item.udpReseiveScan);
    query.bindValue(":serialComScan", item.serialComScan);
    query.bindValue(":serialBaudrateScan", item.serialBaudRateScan);
    query.bindValue(":gps", item.gps);
    query.bindValue(":position", item.position);
    query.bindValue(":lat", item.pLat);
    query.bindValue(":long", item.pLong);
    query.bindValue(":note", item.note);
    query.bindValue(":isAuto", item.isAuto);
    query.bindValue(":lastSeen", item.lastSeen);
    query.bindValue(":created", item.createTime);
    query.bindValue(":userID", item.userID);

    if (!execQuery(query))
        return false;

    if (query.next() && outId)
        *outId = query.value(0).toInt();

    return true;
}

QVector<StationStruct> RepositoryStation::selectAll() const
{
    QVector<StationStruct> result;
    if (!isDbOpen())
        return result;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM tblStation WHERE isDeleted = FALSE ORDER BY eID ASC");
    if (!execQuery(query))
        return result;
    while (query.next())
        result.push_back(mapQueryToItem(query));
    return result;
}

QVector<StationStruct> RepositoryStation::selectByFilter(const QVariantMap &filters) const
{
    QVector<StationStruct> result;
    if (!isDbOpen())
        return result;
    QStringList conditions;
    for (auto it = filters.begin(); it != filters.end(); ++it)
        conditions << QString("%1 = :%1").arg(it.key());
    QString sql = "SELECT * FROM tblStation WHERE isDeleted = FALSE";
    if (!conditions.isEmpty())
        sql += " AND " + conditions.join(" AND ");
    sql += " ORDER BY eID ASC";
    QSqlQuery query(db);
    query.prepare(sql);
    for (auto it = filters.begin(); it != filters.end(); ++it)
        query.bindValue(":" + it.key(), it.value());
    if (!execQuery(query))
        return result;
    while (query.next())
        result.push_back(mapQueryToItem(query));
    return result;
}

QVector<StationStruct> RepositoryStation::selectByAnyFilter(const QVariantMap &filters) const
{
    QVector<StationStruct> result;
    if (!isDbOpen())
        return result;
    QStringList conditions;
    QSqlQuery query(db);
    QString sql = "SELECT * FROM tblStation WHERE isDeleted = FALSE";
    if (!filters.isEmpty()) {
        for (auto it = filters.begin(); it != filters.end(); ++it)
            conditions << QString("%1 = :%1").arg(it.key());
        sql += " AND (" + conditions.join(" OR ") + ")";
    }
    query.prepare(sql);
    for (auto it = filters.begin(); it != filters.end(); ++it)
        query.bindValue(":" + it.key(), it.value());
    if (!execQuery(query))
        return result;
    while (query.next())
        result.push_back(mapQueryToItem(query));
    return result;
}

bool RepositoryStation::update(int id, const QMap<QString, QVariant> &fields)
{
    return updateFields("tblStation", id, fields);
}

bool RepositoryStation::remove(int id)
{
    return softDelete("tblStation", id);
}

bool RepositoryStation::removeAll()
{
    return softDeleteAll("tblStation");
}

bool RepositoryStation::clearAllAndRestartID()
{
    return truncateTable("tblStation");
}

StationStruct RepositoryStation::mapQueryToItem(const QSqlQuery &query) const
{
    StationStruct item;
    item.eID = query.value("eID").toInt();
    item.mode = query.value("mode").toInt();
    item.name = query.value("name").toString();
    item.icon = query.value("icon").toString();
    item.ip = query.value("ip").toString();
    item.mac = query.value("mac").toString();
    item.connection = query.value("connection").toInt();
    item.tcp = query.value("tcp").toInt();
    item.ocx = query.value("ocx").toInt();
    item.udpSend = query.value("udpSend").toInt();
    item.udpReseive = query.value("udpReseive").toInt();
    item.serialCom = query.value("serialCom").toString();
    item.serialBaudRate = query.value("serialBaudrate").toInt();
    item.ipScan = query.value("ipScan").toString();
    item.macScan = query.value("macScan").toString();
    item.syncScan = query.value("syncScan").toBool();
    item.connectionScan = query.value("connectionScan").toInt();
    item.tcpScan = query.value("tcpScan").toInt();
    item.ocxScan = query.value("ocxScan").toInt();
    item.udpSendScan = query.value("udpSendScan").toInt();
    item.udpReseiveScan = query.value("udpReseiveScan").toInt();
    item.serialComScan = query.value("serialComScan").toString();
    item.serialBaudRateScan = query.value("serialBaudrateScan").toInt();
    item.gps = query.value("gps").toBool();
    item.position = query.value("position").toInt();
    item.pLat = query.value("lat").toString();
    item.pLong = query.value("long").toString();
    item.note = query.value("note").toString();
    item.isAuto = query.value("isAuto").toBool();
    item.isDeleted = query.value("isDeleted").toBool();
    item.lastSeen = query.value("lastSeen").toDateTime();
    item.createTime = query.value("created").toDateTime();
    item.userID = query.value("userID").toInt();
    return item;
}

