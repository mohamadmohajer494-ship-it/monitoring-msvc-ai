#include "maplayermanager.h"

MapLayerManager::MapLayerManager(GlobalVariables *gVar,
                                 QSqlDatabase db,
                                 QObject *parent)
    : QObject(parent),
    gVar(gVar),
    repo(db)
{
}

bool MapLayerManager::loadAll()
{
    m_layers = repo.selectAllWithItems();
    if (!m_layers.isEmpty()) {
        m_activeLayerID = m_layers.first().eID;
    } else {
        m_activeLayerID = -1;
    }
    emit layersChanged();
    emit mapChanged();
    emit activeLayerChanged(m_activeLayerID);
    return true;
}

const QVector<MapLayerStruct>& MapLayerManager::layers() const
{
    return m_layers;
}

int MapLayerManager::activeLayerID() const
{
    return m_activeLayerID;
}

bool MapLayerManager::setActiveLayer(int layerID)
{
    if (!findLayer(layerID))
        return false;
    if (m_activeLayerID == layerID)
        return true;
    m_activeLayerID = layerID;
    emit activeLayerChanged(m_activeLayerID);
    return true;
}

MapLayerStruct* MapLayerManager::activeLayer()
{
    return findLayer(m_activeLayerID);
}

MapLayerStruct* MapLayerManager::findLayer(int layerID)
{
    for (MapLayerStruct &layer : m_layers) {
        if (layer.eID == layerID)
            return &layer;
    }
    return nullptr;
}

MapItemStruct* MapLayerManager::findItem(int itemID)
{
    for (MapLayerStruct &layer : m_layers) {
        for (MapItemStruct &item : layer.items) {
            if (item.eID == itemID)
                return &item;
        }
    }
    return nullptr;
}

bool MapLayerManager::addLayer(const QString &name, int userID)
{
    MapLayerStruct layer;
    layer.name = name;
    layer.userID = userID;
    layer.sortOrder = nextSortOrder();
    layer.isVisible = true;
    layer.isDeleted = false;
    layer.createTime = QDateTime::currentDateTimeUtc();
    int id = -1;
    if (!repo.insertLayer(layer, &id))
        return false;
    layer.eID = id;
    m_layers.push_back(layer);
    m_activeLayerID = id;
    emit layersChanged();
    emit mapChanged();
    emit activeLayerChanged(m_activeLayerID);
    return true;
}

bool MapLayerManager::updateLayerName(int layerID, const QString &name)
{
    MapLayerStruct *layer = findLayer(layerID);
    if (!layer)
        return false;
    QMap<QString, QVariant> fields;
    fields["name"] = name;
    if (!repo.updateLayer(layerID, fields))
        return false;
    layer->name = name;
    emit layersChanged();
    return true;
}

bool MapLayerManager::setLayerVisible(int layerID, bool visible)
{
    MapLayerStruct *layer = findLayer(layerID);
    if (!layer)
        return false;
    QMap<QString, QVariant> fields;
    fields["isVisible"] = visible;
    if (!repo.updateLayer(layerID, fields))
        return false;
    layer->isVisible = visible;
    emit layersChanged();
    emit mapChanged();
    return true;
}

bool MapLayerManager::reorderLayersByIds(const QVector<int> &layerIds)
{
    if (layerIds.size() != m_layers.size())
        return false;

    QSet<int> seenIds;
    QVector<MapLayerStruct> reorderedLayers;
    reorderedLayers.reserve(m_layers.size());

    for (int i = 0; i < layerIds.size(); ++i) {
        const int layerId = layerIds[i];

        if (seenIds.contains(layerId))
            return false;

        seenIds.insert(layerId);

        MapLayerStruct *layer = findLayer(layerId);

        if (!layer)
            return false;

        MapLayerStruct copiedLayer = *layer;
        copiedLayer.sortOrder = i + 1;

        reorderedLayers.push_back(copiedLayer);
    }

    if (!repo.updateLayerSortOrders(layerIds))
        return false;

    m_layers = reorderedLayers;

    emit layersChanged();
    emit mapChanged();

    return true;
}

bool MapLayerManager::removeLayer(int layerID)
{
    if (!repo.removeLayer(layerID))
        return false;
    for (int i = 0; i < m_layers.size(); ++i) {
        if (m_layers[i].eID == layerID) {
            m_layers.removeAt(i);
            break;
        }
    }
    if (m_activeLayerID == layerID) {
        m_activeLayerID = m_layers.isEmpty() ? -1 : m_layers.first().eID;
        emit activeLayerChanged(m_activeLayerID);
    }
    emit layersChanged();
    emit mapChanged();
    return true;
}

bool MapLayerManager::addItemToActiveLayer(MapItemStruct item)
{
    if (m_activeLayerID <= 0)
        return false;
    return addItem(m_activeLayerID, item);
}

bool MapLayerManager::addItem(int layerID, MapItemStruct item)
{
    MapLayerStruct *layer = findLayer(layerID);
    if (!layer)
        return false;
    item.layerID = layerID;
    item.createTime = QDateTime::currentDateTimeUtc();
    if (!repo.saveItemFull(item))
        return false;
    layer->items.push_back(item);
    emit layersChanged();
    emit mapChanged();
    return true;
}

bool MapLayerManager::updateItem(MapItemStruct item)
{
    MapItemStruct *oldItem = findItem(item.eID);
    if (!oldItem)
        return false;
    if (!repo.saveItemFull(item))
        return false;
    *oldItem = item;
    emit layersChanged();
    emit mapChanged();
    return true;
}

bool MapLayerManager::setItemVisible(int itemID, bool visible)
{
    MapItemStruct *item = findItem(itemID);
    if (!item)
        return false;
    QMap<QString, QVariant> fields;
    fields["isVisible"] = visible;
    if (!repo.updateItem(itemID, fields))
        return false;
    item->isVisible = visible;
    emit layersChanged();
    emit mapChanged();
    return true;
}

bool MapLayerManager::removeItem(int itemID)
{
    if (!repo.removeItem(itemID))
        return false;
    for (MapLayerStruct &layer : m_layers) {
        for (int i = 0; i < layer.items.size(); ++i) {
            if (layer.items[i].eID == itemID) {
                layer.items.removeAt(i);
                emit layersChanged();
                emit mapChanged();
                return true;
            }
        }
    }
    emit layersChanged();
    emit mapChanged();
    return true;
}

bool MapLayerManager::saveItem(MapItemStruct &item)
{
    if (!repo.saveItemFull(item))
        return false;
    return loadAll();
}

bool MapLayerManager::saveLayer(MapLayerStruct &layer)
{
    if (!repo.saveLayerFull(layer))
        return false;
    return loadAll();
}

bool MapLayerManager::exportLayersBinary(const QString &filePath) const
{
    MapLayerFileCodecBinary::Result result =
        MapLayerFileCodecBinary::writeBinary(filePath, m_layers);

    if (!result.ok) {
        qDebug() << "Export map layers failed:" << result.error;
        return false;
    }

    return true;
}

bool MapLayerManager::importLayersBinary(const QString &filePath,
                                         bool replaceExisting)
{
    QVector<MapLayerStruct> importedLayers;

    MapLayerFileCodecBinary::Result result =
        MapLayerFileCodecBinary::readBinary(filePath, importedLayers);

    if (!result.ok) {
        qDebug() << "Import map layers failed:" << result.error;
        return false;
    }

    if (importedLayers.isEmpty()) {
        qDebug() << "Import map layers failed: file has no layers";
        return false;
    }

    if (replaceExisting) {
        if (!repo.clearAllAndRestartID()) {
            qDebug() << "Import map layers failed: cannot clear current map data";
            return false;
        }

        m_layers.clear();
        m_activeLayerID = -1;
    }

    int order = nextSortOrder();

    for (MapLayerStruct &layer : importedLayers) {
        layer.eID = -1;
        layer.sortOrder = order++;
        layer.isDeleted = false;

        if (layer.name.trimmed().isEmpty())
            layer.name = "Imported Layer";

        for (MapItemStruct &item : layer.items) {
            item.eID = -1;
            item.layerID = -1;
            item.isDeleted = false;

            if (item.name.trimmed().isEmpty())
                item.name = "Imported Item";

            for (int i = 0; i < item.points.size(); ++i) {
                item.points[i].pointIndex = i;
            }
        }

        if (!repo.saveLayerFull(layer)) {
            qDebug() << "Import map layers failed while saving layer:" << layer.name;
            return false;
        }
    }

    return loadAll();
}

void MapLayerManager::clearMemory()
{
    m_layers.clear();
    m_activeLayerID = -1;
    emit layersChanged();
    emit mapChanged();
    emit activeLayerChanged(m_activeLayerID);
}

int MapLayerManager::nextSortOrder() const
{
    int maxOrder = 0;
    for (const MapLayerStruct &layer : m_layers) {
        if (layer.sortOrder > maxOrder)
            maxOrder = layer.sortOrder;
    }
    return maxOrder + 1;
}