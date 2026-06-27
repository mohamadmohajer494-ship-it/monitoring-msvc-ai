#ifndef MAPLAYERFILECODECBINARY_H
#define MAPLAYERFILECODECBINARY_H

#include <QVector>
#include <QString>
#include <QFile>
#include <QDataStream>
#include <QDebug>

#include "Includes/GlobalStruct.h"

class MapLayerFileCodecBinary
{
public:
    struct Result
    {
        bool ok = false;
        QString error;
    };

    static Result writeBinary(const QString &filePath, const QVector<MapLayerStruct> &layers);
    static Result readBinary(const QString &filePath, QVector<MapLayerStruct> &outLayers);

private:
    static void writeLayer(QDataStream &out, const MapLayerStruct &layer);
    static void writeItem(QDataStream &out, const MapItemStruct &item);
    static void writePoint(QDataStream &out, const GeoPointStruct &point);

    static MapLayerStruct readLayer(QDataStream &in);
    static MapItemStruct readItem(QDataStream &in);
    static GeoPointStruct readPoint(QDataStream &in);
};

#endif // MAPLAYERFILECODECBINARY_H