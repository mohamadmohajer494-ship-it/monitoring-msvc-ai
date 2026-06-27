#ifndef MAPLAYERMANAGER_H
#define MAPLAYERMANAGER_H

#include <QObject>
#include <QVector>
#include <QSqlDatabase>
#include "Includes/globalvariables.h"
#include "DataBase/repositorymap.h"
#include "DataCore/maplayerfilecodecbinary.h"

class MapLayerManager : public QObject
{
    Q_OBJECT

public:
    explicit MapLayerManager(GlobalVariables *gVar, QSqlDatabase db, QObject *parent = nullptr);


private:
    GlobalVariables *gVar;

public:
    bool loadAll();

    const QVector<MapLayerStruct>& layers() const;

    int activeLayerID() const;
    bool setActiveLayer(int layerID);

    MapLayerStruct* activeLayer();
    MapLayerStruct* findLayer(int layerID);
    MapItemStruct* findItem(int itemID);

    bool addLayer(const QString &name, int userID = -1);
    bool updateLayerName(int layerID, const QString &name);
    bool setLayerVisible(int layerID, bool visible);
    bool reorderLayersByIds(const QVector<int> &layerIds);
    bool removeLayer(int layerID);

    bool addItemToActiveLayer(MapItemStruct item);
    bool addItem(int layerID, MapItemStruct item);
    bool updateItem(MapItemStruct item);
    bool setItemVisible(int itemID, bool visible);
    bool removeItem(int itemID);

    bool saveItem(MapItemStruct &item);
    bool saveLayer(MapLayerStruct &layer);

    bool exportLayersBinary(const QString &filePath) const;
    bool importLayersBinary(const QString &filePath,
                            bool replaceExisting = false);

    void clearMemory();

private:
    RepositoryMap repo;
    QVector<MapLayerStruct> m_layers;
    int m_activeLayerID = -1;

    int nextSortOrder() const;

signals:
    void layersChanged();
    void mapChanged();
    void activeLayerChanged(int layerID);
};

#endif // MAPLAYERMANAGER_H