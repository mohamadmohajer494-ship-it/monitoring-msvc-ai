#include "maplayerfilecodecbinary.h"

MapLayerFileCodecBinary::Result MapLayerFileCodecBinary::writeBinary(
    const QString &filePath,
    const QVector<MapLayerStruct> &layers)
{
    Result result;
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        result.error = file.errorString();
        return result;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);

    // تعداد لایه‌ها
    out << quint32(layers.size());

    for (const MapLayerStruct &layer : layers) {
        writeLayer(out, layer);
    }

    result.ok = true;
    return result;
}

void MapLayerFileCodecBinary::writeLayer(QDataStream &out, const MapLayerStruct &layer)
{
    out << layer.eID;
    out << layer.name;
    out << layer.sortOrder;
    out << layer.isVisible;
    out << layer.userID;
    out << layer.createTime.toMSecsSinceEpoch();

    // تعداد آیتم‌ها
    out << quint32(layer.items.size());
    for (const MapItemStruct &item : layer.items) {
        writeItem(out, item);
    }
}

void MapLayerFileCodecBinary::writeItem(QDataStream &out, const MapItemStruct &item)
{
    out << item.eID;
    out << item.layerID;
    out << item.name;
    out << static_cast<quint32>(item.type);
    out << item.note;
    out << item.isVisible;
    out << item.userID;
    out << item.createTime.toMSecsSinceEpoch();

    // Style
    out << item.style.strokeColor;
    out << item.style.fillColor;
    out << item.style.textColor;
    out << item.style.strokeWidth;
    out << item.style.pointSize;
    out << item.style.fontSize;
    out << item.style.opacity;
    out << item.style.rotation;
    out << item.style.iconPath;

    // نقاط
    out << quint32(item.points.size());
    for (const GeoPointStruct &p : item.points) {
        writePoint(out, p);
    }
}

void MapLayerFileCodecBinary::writePoint(QDataStream &out, const GeoPointStruct &p)
{
    out << p.lat;
    out << p.lon;
    out << p.pointIndex;
}

// ----------------- Read -----------------

MapLayerFileCodecBinary::Result MapLayerFileCodecBinary::readBinary(
    const QString &filePath,
    QVector<MapLayerStruct> &outLayers)
{
    Result result;
    outLayers.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        result.error = file.errorString();
        return result;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_15);

    quint32 layerCount;
    in >> layerCount;

    for (quint32 i = 0; i < layerCount; ++i) {
        MapLayerStruct layer = readLayer(in);
        outLayers.push_back(layer);
    }

    result.ok = true;
    return result;
}

MapLayerStruct MapLayerFileCodecBinary::readLayer(QDataStream &in)
{
    MapLayerStruct layer;
    in >> layer.eID;
    in >> layer.name;
    in >> layer.sortOrder;
    in >> layer.isVisible;
    in >> layer.userID;

    qint64 msecs;
    in >> msecs;
    layer.createTime = QDateTime::fromMSecsSinceEpoch(msecs, Qt::UTC);

    quint32 itemCount;
    in >> itemCount;
    for (quint32 i = 0; i < itemCount; ++i) {
        layer.items.push_back(readItem(in));
    }

    return layer;
}

MapItemStruct MapLayerFileCodecBinary::readItem(QDataStream &in)
{
    MapItemStruct item;
    in >> item.eID;
    in >> item.layerID;
    in >> item.name;
    quint32 type;
    in >> type;
    item.type = static_cast<EnumMapShapeType>(type);
    in >> item.note;
    in >> item.isVisible;
    in >> item.userID;

    qint64 msecs;
    in >> msecs;
    item.createTime = QDateTime::fromMSecsSinceEpoch(msecs, Qt::UTC);

    // Style
    in >> item.style.strokeColor;
    in >> item.style.fillColor;
    in >> item.style.textColor;
    in >> item.style.strokeWidth;
    in >> item.style.pointSize;
    in >> item.style.fontSize;
    in >> item.style.opacity;
    in >> item.style.rotation;
    in >> item.style.iconPath;

    // نقاط
    quint32 pointCount;
    in >> pointCount;
    for (quint32 i = 0; i < pointCount; ++i) {
        item.points.push_back(readPoint(in));
    }

    return item;
}

GeoPointStruct MapLayerFileCodecBinary::readPoint(QDataStream &in)
{
    GeoPointStruct p;
    in >> p.lat;
    in >> p.lon;
    in >> p.pointIndex;
    return p;
}