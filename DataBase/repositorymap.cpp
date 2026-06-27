#include "repositorymap.h"

RepositoryMap::RepositoryMap(QSqlDatabase db)
    : BaseRepository(db)
{
}

//---Layer---
bool RepositoryMap::insertLayer(const MapLayerStruct &item, int *outId)
{
    if (!isDbOpen())
        return false;
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO tblMapLayer
        (name, sortOrder, isVisible, isDeleted, created, userID)
        VALUES
        (:name, :sortOrder, :isVisible, :isDeleted, :created, :userID)
        RETURNING eID
    )");
    query.bindValue(":name", item.name);
    query.bindValue(":sortOrder", item.sortOrder);
    query.bindValue(":isVisible", item.isVisible);
    query.bindValue(":isDeleted", item.isDeleted);
    query.bindValue(":created", item.createTime.isValid()
                                    ? item.createTime
                                    : QDateTime::currentDateTimeUtc());
    query.bindValue(":userID", item.userID);
    if (!execQuery(query))
        return false;
    if (query.next() && outId)
        *outId = query.value(0).toInt();
    return true;
}

QVector<MapLayerStruct> RepositoryMap::selectLayers() const
{
    QVector<MapLayerStruct> result;
    if (!isDbOpen())
        return result;
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT *
        FROM tblMapLayer
        WHERE isDeleted = FALSE
        ORDER BY sortOrder ASC, eID ASC
    )");
    if (!execQuery(query))
        return result;
    while (query.next())
        result.push_back(mapQueryToLayer(query));
    return result;
}

QVector<MapLayerStruct> RepositoryMap::selectAllWithItems() const
{
    QVector<MapLayerStruct> result = selectLayers();
    for (MapLayerStruct &layer : result) {
        layer.items = selectItemsByLayer(layer.eID);
    }
    return result;
}

bool RepositoryMap::updateLayer(int id, const QMap<QString, QVariant> &fields)
{
    return updateFields("tblMapLayer", id, fields);
}

bool RepositoryMap::updateLayerSortOrders(const QVector<int> &layerIds)
{
    if (!isDbOpen())
        return false;

    if (!db.transaction())
        return false;

    QSqlQuery query(db);

    query.prepare(R"(
        UPDATE tblMapLayer
        SET sortOrder = :sortOrder
        WHERE eID = :id
          AND isDeleted = FALSE
    )");

    for (int i = 0; i < layerIds.size(); ++i) {
        query.bindValue(":sortOrder", i + 1);
        query.bindValue(":id", layerIds[i]);

        if (!execQuery(query)) {
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        db.rollback();
        return false;
    }

    return true;
}

bool RepositoryMap::removeLayer(int id)
{
    if (!isDbOpen())
        return false;
    if (!db.transaction())
        return false;
    QSqlQuery itemQuery(db);
    itemQuery.prepare(R"(
        UPDATE tblMapItem
        SET isDeleted = TRUE
        WHERE layerID = :layerID
          AND isDeleted = FALSE
    )");
    itemQuery.bindValue(":layerID", id);
    if (!execQuery(itemQuery)) {
        db.rollback();
        return false;
    }
    QSqlQuery layerQuery(db);
    layerQuery.prepare(R"(
        UPDATE tblMapLayer
        SET isDeleted = TRUE
        WHERE eID = :id
          AND isDeleted = FALSE
    )");
    layerQuery.bindValue(":id", id);
    if (!execQuery(layerQuery)) {
        db.rollback();
        return false;
    }
    return db.commit();
}

bool RepositoryMap::removeAllLayers()
{
    if (!isDbOpen())
        return false;
    if (!db.transaction())
        return false;
    QSqlQuery q1(db);
    q1.prepare("UPDATE tblMapItem SET isDeleted = TRUE WHERE isDeleted = FALSE");
    if (!execQuery(q1)) {
        db.rollback();
        return false;
    }
    QSqlQuery q2(db);
    q2.prepare("UPDATE tblMapLayer SET isDeleted = TRUE WHERE isDeleted = FALSE");
    if (!execQuery(q2)) {
        db.rollback();
        return false;
    }
    return db.commit();
}

//---Item---
bool RepositoryMap::insertItem(const MapItemStruct &item, int *outId)
{
    if (!isDbOpen())
        return false;
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO tblMapItem
        (name, type, note,
         strokeColor, fillColor, textColor,
         strokeWidth, pointSize, fontSize,
         opacity, rotation, iconPath,
         isVisible, isDeleted, created, layerID, userID)
        VALUES
        (:name, :type, :note,
         :strokeColor, :fillColor, :textColor,
         :strokeWidth, :pointSize, :fontSize,
         :opacity, :rotation, :iconPath,
         :isVisible, :isDeleted, :created, :layerID, :userID)
        RETURNING eID
    )");
    query.bindValue(":name", item.name);
    query.bindValue(":type", static_cast<int>(item.type));
    query.bindValue(":note", item.note);
    bindItemStyle(query, item);
    query.bindValue(":isVisible", item.isVisible);
    query.bindValue(":isDeleted", item.isDeleted);
    query.bindValue(":created", item.createTime.isValid()
                                    ? item.createTime
                                    : QDateTime::currentDateTimeUtc());
    query.bindValue(":layerID", item.layerID);
    query.bindValue(":userID", item.userID);
    if (!execQuery(query))
        return false;
    if (query.next() && outId)
        *outId = query.value(0).toInt();
    return true;
}

QVector<MapItemStruct> RepositoryMap::selectItemsByLayer(int layerID) const
{
    QVector<MapItemStruct> result;
    if (!isDbOpen())
        return result;
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT *
        FROM tblMapItem
        WHERE layerID = :layerID
          AND isDeleted = FALSE
        ORDER BY eID ASC
    )");
    query.bindValue(":layerID", layerID);
    if (!execQuery(query))
        return result;
    while (query.next()) {
        MapItemStruct item = mapQueryToItem(query);
        item.points = selectPointsByItem(item.eID);
        result.push_back(item);
    }
    return result;
}

bool RepositoryMap::updateItem(int id, const QMap<QString, QVariant> &fields)
{
    return updateFields("tblMapItem", id, fields);
}

bool RepositoryMap::removeItem(int id)
{
    return softDelete("tblMapItem", id);
}

bool RepositoryMap::removeItemsByLayer(int layerID)
{
    if (!isDbOpen())
        return false;
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE tblMapItem
        SET isDeleted = TRUE
        WHERE layerID = :layerID
          AND isDeleted = FALSE
    )");
    query.bindValue(":layerID", layerID);
    return execQuery(query);
}

//---Point---
bool RepositoryMap::insertPoint(int mapItemID, const GeoPointStruct &point, int userID)
{
    if (!isDbOpen())
        return false;
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO tblMapPoint
        (lat, lon, pointIndex, mapItemID, userID)
        VALUES
        (:lat, :lon, :pointIndex, :mapItemID, :userID)
    )");
    query.bindValue(":lat", point.lat);
    query.bindValue(":lon", point.lon);
    query.bindValue(":pointIndex", point.pointIndex);
    query.bindValue(":mapItemID", mapItemID);
    query.bindValue(":userID", userID);
    return execQuery(query);
}

QVector<GeoPointStruct> RepositoryMap::selectPointsByItem(int mapItemID) const
{
    QVector<GeoPointStruct> result;
    if (!isDbOpen())
        return result;
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT *
        FROM tblMapPoint
        WHERE mapItemID = :mapItemID
        ORDER BY pointIndex ASC, eID ASC
    )");
    query.bindValue(":mapItemID", mapItemID);
    if (!execQuery(query))
        return result;
    while (query.next())
        result.push_back(mapQueryToPoint(query));
    return result;
}

bool RepositoryMap::removePointsByItem(int mapItemID)
{
    if (!isDbOpen())
        return false;
    QSqlQuery query(db);
    query.prepare("DELETE FROM tblMapPoint WHERE mapItemID = :mapItemID");
    query.bindValue(":mapItemID", mapItemID);
    return execQuery(query);
}

bool RepositoryMap::replaceItemPoints(int mapItemID,
                                      const QVector<GeoPointStruct> &points,
                                      int userID)
{
    if (!isDbOpen())
        return false;
    if (!db.transaction())
        return false;
    if (!replaceItemPointsNoTransaction(mapItemID, points, userID)) {
        db.rollback();
        return false;
    }
    return db.commit();
}

//---FullSave---
bool RepositoryMap::saveItemFull(MapItemStruct &item)
{
    if (!isDbOpen())
        return false;
    if (!db.transaction())
        return false;
    if (!saveItemFullNoTransaction(item)) {
        db.rollback();
        return false;
    }
    return db.commit();
}

bool RepositoryMap::saveLayerFull(MapLayerStruct &layer)
{
    if (!isDbOpen())
        return false;
    if (!db.transaction())
        return false;
    int layerID = layer.eID;
    if (layerID <= 0) {
        if (!insertLayer(layer, &layerID)) {
            db.rollback();
            return false;
        }
        layer.eID = layerID;
    } else {
        QMap<QString, QVariant> fields;
        fields["name"] = layer.name;
        fields["sortOrder"] = layer.sortOrder;
        fields["isVisible"] = layer.isVisible;
        fields["isDeleted"] = layer.isDeleted;
        fields["userID"] = layer.userID;
        if (!updateLayer(layerID, fields)) {
            db.rollback();
            return false;
        }
    }
    for (MapItemStruct &item : layer.items) {
        item.layerID = layerID;
        if (!saveItemFullNoTransaction(item)) {
            db.rollback();
            return false;
        }
    }
    return db.commit();
}

bool RepositoryMap::saveItemFullNoTransaction(MapItemStruct &item)
{
    int itemID = item.eID;
    if (itemID <= 0) {
        if (!insertItem(item, &itemID))
            return false;
        item.eID = itemID;
    } else {
        QMap<QString, QVariant> fields;
        fields["name"] = item.name;
        fields["type"] = static_cast<int>(item.type);
        fields["note"] = item.note;
        fields["strokeColor"] = item.style.strokeColor;
        fields["fillColor"] = item.style.fillColor;
        fields["textColor"] = item.style.textColor;
        fields["strokeWidth"] = item.style.strokeWidth;
        fields["pointSize"] = item.style.pointSize;
        fields["fontSize"] = item.style.fontSize;
        fields["opacity"] = item.style.opacity;
        fields["rotation"] = item.style.rotation;
        fields["iconPath"] = item.style.iconPath;
        fields["isVisible"] = item.isVisible;
        fields["isDeleted"] = item.isDeleted;
        fields["layerID"] = item.layerID;
        fields["userID"] = item.userID;
        if (!updateItem(itemID, fields))
            return false;
    }
    return replaceItemPointsNoTransaction(item.eID, item.points, item.userID);
}

bool RepositoryMap::replaceItemPointsNoTransaction(int mapItemID,
                                                   const QVector<GeoPointStruct> &points,
                                                   int userID)
{
    QSqlQuery deleteQuery(db);
    deleteQuery.prepare("DELETE FROM tblMapPoint WHERE mapItemID = :mapItemID");
    deleteQuery.bindValue(":mapItemID", mapItemID);
    if (!execQuery(deleteQuery))
        return false;
    for (int i = 0; i < points.size(); ++i) {
        GeoPointStruct point = points[i];
        point.pointIndex = i;
        QSqlQuery insertQuery(db);
        insertQuery.prepare(R"(
            INSERT INTO tblMapPoint
            (lat, lon, pointIndex, mapItemID, userID)
            VALUES
            (:lat, :lon, :pointIndex, :mapItemID, :userID)
        )");
        insertQuery.bindValue(":lat", point.lat);
        insertQuery.bindValue(":lon", point.lon);
        insertQuery.bindValue(":pointIndex", point.pointIndex);
        insertQuery.bindValue(":mapItemID", mapItemID);
        insertQuery.bindValue(":userID", userID);
        if (!execQuery(insertQuery))
            return false;
    }
    return true;
}

//---Clear---
bool RepositoryMap::clearAllAndRestartID()
{
    if (!isDbOpen())
        return false;
    QSqlQuery query(db);
    query.prepare(R"(
        TRUNCATE TABLE tblMapPoint, tblMapItem, tblMapLayer RESTART IDENTITY CASCADE
    )");
    return execQuery(query);
}

//---Mapping---
MapLayerStruct RepositoryMap::mapQueryToLayer(const QSqlQuery &query) const
{
    MapLayerStruct item;
    item.eID = query.value("eID").toInt();
    item.name = query.value("name").toString();
    item.sortOrder = query.value("sortOrder").toInt();
    item.isVisible = query.value("isVisible").toBool();
    item.isDeleted = query.value("isDeleted").toBool();
    item.createTime = query.value("created").toDateTime();
    item.userID = query.value("userID").toInt();
    return item;
}

MapItemStruct RepositoryMap::mapQueryToItem(const QSqlQuery &query) const
{
    MapItemStruct item;
    item.eID = query.value("eID").toInt();
    item.name = query.value("name").toString();
    item.type = static_cast<EnumMapShapeType>(query.value("type").toInt());
    item.note = query.value("note").toString();
    item.style.strokeColor = query.value("strokeColor").toString();
    item.style.fillColor = query.value("fillColor").toString();
    item.style.textColor = query.value("textColor").toString();
    item.style.strokeWidth = query.value("strokeWidth").toInt();
    item.style.pointSize = query.value("pointSize").toInt();
    item.style.fontSize = query.value("fontSize").toInt();
    item.style.opacity = query.value("opacity").toDouble();
    item.style.rotation = query.value("rotation").toDouble();
    item.style.iconPath = query.value("iconPath").toString();
    item.isVisible = query.value("isVisible").toBool();
    item.isDeleted = query.value("isDeleted").toBool();
    item.createTime = query.value("created").toDateTime();
    item.layerID = query.value("layerID").toInt();
    item.userID = query.value("userID").toInt();
    return item;
}

GeoPointStruct RepositoryMap::mapQueryToPoint(const QSqlQuery &query) const
{
    GeoPointStruct item;
    item.lat = query.value("lat").toDouble();
    item.lon = query.value("lon").toDouble();
    item.pointIndex = query.value("pointIndex").toInt();
    return item;
}

void RepositoryMap::bindItemStyle(QSqlQuery &query, const MapItemStruct &item) const
{
    query.bindValue(":strokeColor", item.style.strokeColor);
    query.bindValue(":fillColor", item.style.fillColor);
    query.bindValue(":textColor", item.style.textColor);
    query.bindValue(":strokeWidth", item.style.strokeWidth);
    query.bindValue(":pointSize", item.style.pointSize);
    query.bindValue(":fontSize", item.style.fontSize);
    query.bindValue(":opacity", item.style.opacity);
    query.bindValue(":rotation", item.style.rotation);
    query.bindValue(":iconPath", item.style.iconPath);
}