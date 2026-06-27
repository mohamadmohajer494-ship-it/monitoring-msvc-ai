#ifndef REPOSITORYMAP_H
#define REPOSITORYMAP_H

#include "baserepository.h"

class RepositoryMap : public BaseRepository
{
public:
    explicit RepositoryMap(QSqlDatabase db);

    //--- Layer ---
    bool insertLayer(const MapLayerStruct &item, int *outId = nullptr);
    QVector<MapLayerStruct> selectLayers() const;
    QVector<MapLayerStruct> selectAllWithItems() const;
    bool updateLayer(int id, const QMap<QString, QVariant> &fields);
    bool updateLayerSortOrders(const QVector<int> &layerIds);
    bool removeLayer(int id);
    bool removeAllLayers();
    //--- Item ---
    bool insertItem(const MapItemStruct &item, int *outId = nullptr);
    QVector<MapItemStruct> selectItemsByLayer(int layerID) const;
    bool updateItem(int id, const QMap<QString, QVariant> &fields);
    bool removeItem(int id);
    bool removeItemsByLayer(int layerID);
    //--- Point ---
    bool insertPoint(int mapItemID, const GeoPointStruct &point, int userID = -1);
    QVector<GeoPointStruct> selectPointsByItem(int mapItemID) const;
    bool removePointsByItem(int mapItemID);
    bool replaceItemPoints(int mapItemID,
                           const QVector<GeoPointStruct> &points,
                           int userID = -1);
    // --- Full Save ---
    bool saveItemFull(MapItemStruct &item);
    bool saveLayerFull(MapLayerStruct &layer);
    bool clearAllAndRestartID();

private:
    MapLayerStruct mapQueryToLayer(const QSqlQuery &query) const;
    MapItemStruct mapQueryToItem(const QSqlQuery &query) const;
    GeoPointStruct mapQueryToPoint(const QSqlQuery &query) const;
    void bindItemStyle(QSqlQuery &query, const MapItemStruct &item) const;
    bool saveItemFullNoTransaction(MapItemStruct &item);
    bool replaceItemPointsNoTransaction(int mapItemID,
                                        const QVector<GeoPointStruct> &points,
                                        int userID = -1);
};

#endif // REPOSITORYMAP_H
