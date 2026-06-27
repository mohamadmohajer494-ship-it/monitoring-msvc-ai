#include "migrationrunner.h"

QVector<Migration> MigrationRunner::migrations()
{
    QVector<Migration> list;
    {
        Migration m;
        m.version = 1;
        m.sqlUp = {
            "CREATE TABLE IF NOT EXISTS tblStation ("
            "  eID BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,"
            "  mode INTEGER,"
            "  name TEXT,"
            "  icon TEXT,"
            "  ip TEXT,"
            "  mac TEXT,"
            "  connection INTEGER,"
            "  tcp INTEGER,"
            "  ocx INTEGER,"
            "  udpSend INTEGER,"
            "  udpReseive INTEGER,"
            "  serialCom TEXT,"
            "  serialBaudrate INTEGER,"
            "  ipScan TEXT,"
            "  macScan TEXT,"
            "  syncScan BOOLEAN DEFAULT FALSE,"
            "  connectionScan INTEGER,"
            "  tcpScan INTEGER,"
            "  ocxScan INTEGER,"
            "  udpSendScan INTEGER,"
            "  udpReseiveScan INTEGER,"
            "  serialComScan TEXT,"
            "  serialBaudrateScan INTEGER,"
            "  gps BOOLEAN DEFAULT FALSE,"
            "  position INTEGER,"
            "  lat TEXT,"
            "  long TEXT,"
            "  note TEXT,"
            "  isAuto BOOLEAN DEFAULT FALSE,"
            "  isDeleted BOOLEAN DEFAULT FALSE,"
            "  lastSeen TIMESTAMPTZ,"
            "  created TIMESTAMPTZ DEFAULT now(),"
            "  userID INTEGER"
            ");",
            "CREATE TABLE IF NOT EXISTS tblMapLayer ("
            "  eID BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,"
            "  name TEXT,"
            "  sortOrder INTEGER DEFAULT 0,"
            "  isVisible BOOLEAN DEFAULT TRUE,"
            "  isDeleted BOOLEAN DEFAULT FALSE,"
            "  created TIMESTAMPTZ DEFAULT now(),"
            "  userID BIGINT"
            ");",
            "CREATE TABLE IF NOT EXISTS tblMapItem ("
            "  eID BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,"
            "  name TEXT,"
            "  type INTEGER DEFAULT 99,"
            "  note TEXT,"
            "  strokeColor TEXT DEFAULT '#00FF00',"
            "  fillColor TEXT DEFAULT '#3300FF00',"
            "  textColor TEXT DEFAULT '#FFFFFF',"
            "  strokeWidth INTEGER DEFAULT 2,"
            "  pointSize INTEGER DEFAULT 8,"
            "  fontSize INTEGER DEFAULT 12,"
            "  opacity DOUBLE PRECISION DEFAULT 1.0,"
            "  rotation DOUBLE PRECISION DEFAULT 0.0,"
            "  iconPath TEXT DEFAULT '',"
            "  isVisible BOOLEAN DEFAULT TRUE,"
            "  isDeleted BOOLEAN DEFAULT FALSE,"
            "  created TIMESTAMPTZ DEFAULT now(),"
            "  layerID BIGINT,"
            "  userID BIGINT"
            ");",
            "CREATE TABLE IF NOT EXISTS tblMapPoint ("
            "  eID BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,"
            "  lat DOUBLE PRECISION,"
            "  lon DOUBLE PRECISION,"
            "  pointIndex INTEGER DEFAULT 0,"
            "  mapItemID BIGINT,"
            "  userID BIGINT"
            ");",
            "CREATE INDEX IF NOT EXISTS idx_tblMapLayer_deleted_sort "
            "ON tblMapLayer(isDeleted, sortOrder, eID);",
            "CREATE INDEX IF NOT EXISTS idx_tblMapItem_layer_deleted "
            "ON tblMapItem(layerID, isDeleted, eID);",
            "CREATE INDEX IF NOT EXISTS idx_tblMapPoint_item_index "
            "ON tblMapPoint(mapItemID, pointIndex, eID);",
        };
        list.push_back(m);
    }
    return list;
}

bool MigrationRunner::runPendingMigrations(QSqlDatabase db)
{
    if (!db.isOpen())
        return false;
    QSqlQuery q(db);
    if (!q.exec("SELECT version FROM schema_migrations ORDER BY version ASC;")) {
        qDebug() << "Read schema_migrations failed:" << q.lastError().text();
        return false;
    }
    QSet<int> applied;
    while (q.next()) {
        applied.insert(q.value(0).toInt());
    }
    auto all = migrations();
    for (const auto& m : all) {
        if (applied.contains(m.version))
            continue;
        if (!db.transaction())
            return false;
        for (const auto& stmt : m.sqlUp) {
            if (stmt.trimmed().isEmpty())
                continue;
            QSqlQuery up(db);
            if (!up.exec(stmt)) {
                db.rollback();
                return false;
            }
        }
        QSqlQuery ins(db);
        ins.prepare("INSERT INTO schema_migrations(version) VALUES (?)");
        ins.addBindValue(m.version);
        if (!ins.exec()) {
            qDebug() << "Write schema_migrations failed:" << ins.lastError().text();
            db.rollback();
            return false;
        }
        if (!db.commit())
            return false;
    }
    return true;
}
