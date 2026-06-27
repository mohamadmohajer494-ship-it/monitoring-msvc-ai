#include "position.h"

#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QFileInfo>
#include <QFontMetrics>
#include <QInputDialog>
#include <QLineEdit>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QTextStream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Position::Position(GlobalVariables *gVar, QWidget *parent)
    : QWidget(parent)
    , gVar(gVar)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    defaultMapPath = "D:/Maps/World_Satellite_Image_Zoom5.tif";
    offset = QPointF(0, 0);

    GDALAllRegister();

    createContextMenu();
    createBotMenu();
    createInfoBox();

    setToolMode(MapToolPan);
    setMouseInfoVisible(true);
    updateButtonStates();
    updateActionStates();

    loadDefaultMap();

    slotThemeChange();

    connect(gVar, &GlobalVariables::ThemeChange,
            this, &Position::slotThemeChange);
}

Position::~Position()
{
    closeGdalMap();
}

//--- Map Manager ---
void Position::setMapLayerManager(MapLayerManager *manager)
{
    if (mapLayerManager == manager)
        return;

    if (mapLayerManager)
        disconnect(mapLayerManager, nullptr, this, nullptr);

    mapLayerManager = manager;

    if (mapLayerManager) {
        connect(mapLayerManager, &MapLayerManager::layersChanged,
                this, &Position::onMapLayersChanged);
        connect(mapLayerManager, &MapLayerManager::mapChanged,
                this, &Position::onMapLayersChanged);
        connect(mapLayerManager, &MapLayerManager::activeLayerChanged,
                this, &Position::onActiveLayerChanged);
    }

    update();
}

MapLayerManager* Position::getMapLayerManager() const
{
    return mapLayerManager;
}

//--- Map Loading ---
bool Position::loadDefaultMap()
{
    if (defaultMapPath.trimmed().isEmpty())
        return false;

    isDefaultMapLoading = true;
    bool ok = loadMap(defaultMapPath);
    isDefaultMapLoading = false;
    return ok;
}

bool Position::loadMap(const QString &filePath)
{
    if (filePath.trimmed().isEmpty())
        return false;

    clearMap();

    const QString suffix = QFileInfo(filePath).suffix().toLower();

    bool ok = false;

    if (suffix == "tif" || suffix == "tiff") {
        ok = loadRasterWithGdalStreaming(filePath);
    } else {
        ok = loadRasterAsImage(filePath);
    }

    if (!ok) {
        emit mapLoadFailed(filePath);
        update();
        return false;
    }

    currentMapPath = filePath;
    isMapLoaded = true;
    initializeDefaultView();
    emit mapLoaded(filePath);
    update();
    return true;
}

QString Position::getCurrentMapPath() const
{
    return currentMapPath;
}

void Position::setDefaultMapPath(const QString &filePath)
{
    defaultMapPath = filePath;
}

bool Position::loadRasterWithGdal(const QString &filePath)
{
    Q_UNUSED(filePath);
    qDebug() << "Position::loadRasterWithGdal is disabled. Use loadRasterWithGdalStreaming instead.";
    return false;
}

bool Position::loadRasterAsImage(const QString &filePath)
{
    QImage image(filePath);

    if (image.isNull())
        return false;

    mapImage = image.convertToFormat(QImage::Format_RGB888);
    isGeoTransform = false;
    geoTransform[0] = 0.0;
    geoTransform[1] = 1.0;
    geoTransform[2] = 0.0;
    geoTransform[3] = 0.0;
    geoTransform[4] = 0.0;
    geoTransform[5] = -1.0;

    return true;
}

void Position::clearMap()
{
    closeGdalMap();

    mapImage = QImage();
    currentMapPath.clear();
    isMapLoaded = false;
    isGeoTransform = false;

    geoTransform[0] = 0.0;
    geoTransform[1] = 1.0;
    geoTransform[2] = 0.0;
    geoTransform[3] = 0.0;
    geoTransform[4] = 0.0;
    geoTransform[5] = -1.0;

    update();
}

void Position::initializeDefaultView()
{
    if (!hasMapData())
        return;

    if (isDefaultMapLoading) {
        currentZoom = qBound(minimumZoom, 1.45, maximumZoom);

        if (isGeoTransform) {
            const QPointF imagePos = latLonToImage(defaultLat, defaultLon);

            if (qIsFinite(imagePos.x()) &&
                qIsFinite(imagePos.y()) &&
                isImagePointInsideMap(imagePos)) {
                centerOnImagePoint(imagePos);
            } else {
                offset = QPointF(0, 0);
            }
        } else {
            offset = QPointF(0, 0);
        }

        updateInfoBoxValues();
        emit mapViewChanged(currentZoom, offset);
        update();
        return;
    }

    currentZoom = qBound(minimumZoom, 0.01, maximumZoom);
    offset = QPointF(0, 0);

    updateInfoBoxValues();
    emit mapViewChanged(currentZoom, offset);
    update();
}

void Position::closeGdalMap()
{
    if (mapDataset) {
        GDALClose(mapDataset);
        mapDataset = nullptr;
    }

    isGdalStreamingMap = false;
    mapRasterWidth = 0;
    mapRasterHeight = 0;
    mapRasterBandCount = 0;
    gdalVisibleImage = QImage();
    gdalVisibleTargetRect = QRectF();
    gdalVisibleSourceRect = QRect();
}

bool Position::loadRasterWithGdalStreaming(const QString &filePath)
{
    closeGdalMap();
    mapImage = QImage();

    qDebug() << "Opening GDAL streaming map:" << filePath;

    GDALDataset *dataset = static_cast<GDALDataset *>(
        GDALOpen(filePath.toUtf8().constData(), GA_ReadOnly));

    if (!dataset) {
        qDebug() << "GDALOpen failed for streaming map.";
        return false;
    }

    const int sourceWidth = dataset->GetRasterXSize();
    const int sourceHeight = dataset->GetRasterYSize();
    const int bandCount = dataset->GetRasterCount();

    qDebug() << "Streaming raster size:" << sourceWidth << "x" << sourceHeight
             << "bands:" << bandCount;

    if (sourceWidth <= 0 || sourceHeight <= 0 || bandCount <= 0) {
        GDALClose(dataset);
        return false;
    }

    if (dataset->GetGeoTransform(geoTransform) == CE_None) {
        isGeoTransform = true;
    } else {
        isGeoTransform = false;
        geoTransform[0] = 0.0;
        geoTransform[1] = 1.0;
        geoTransform[2] = 0.0;
        geoTransform[3] = 0.0;
        geoTransform[4] = 0.0;
        geoTransform[5] = -1.0;
    }
    mapProjectionWkt = QString::fromUtf8(dataset->GetProjectionRef());

    isMapWebMercator = false;
    isMapGeographic = false;

    const QString projectionLower = mapProjectionWkt.toLower();

    if (projectionLower.contains("3857") ||
        projectionLower.contains("pseudo-mercator") ||
        projectionLower.contains("web_mercator") ||
        projectionLower.contains("mercator_auxiliary_sphere")) {
        isMapWebMercator = true;
    }

    if (projectionLower.contains("4326") ||
        projectionLower.contains("wgs 84") ||
        projectionLower.contains("wgs_1984")) {
        isMapGeographic = true;
    }

    qDebug() << "Map projection:"
             << "webMercator =" << isMapWebMercator
             << "geographic =" << isMapGeographic
             << "wkt =" << mapProjectionWkt.left(200);

    qDebug() << "GeoTransform:"
             << geoTransform[0]
             << geoTransform[1]
             << geoTransform[2]
             << geoTransform[3]
             << geoTransform[4]
             << geoTransform[5];

    mapDataset = dataset;
    mapRasterWidth = sourceWidth;
    mapRasterHeight = sourceHeight;
    mapRasterBandCount = bandCount;
    isGdalStreamingMap = true;

    mapImage = QImage();

    return true;
}

void Position::drawGdalStreamingMap(QPainter &painter)
{
    if (!mapDataset || mapRasterWidth <= 0 || mapRasterHeight <= 0) {
        drawNoMapMessage(painter);
        return;
    }

    if (width() <= 0 || height() <= 0 || currentZoom <= 0.0)
        return;

    const QSizeF fullMapWidgetSize(
        static_cast<double>(mapRasterWidth) * currentZoom,
        static_cast<double>(mapRasterHeight) * currentZoom
        );

    const QRectF fullMapTargetRect(imageTopLeft(), fullMapWidgetSize);
    const QRectF viewportRect = rect();

    QRectF visibleTargetRect = fullMapTargetRect.intersected(viewportRect);

    if (visibleTargetRect.isEmpty())
        return;

    if (!qIsFinite(visibleTargetRect.left()) ||
        !qIsFinite(visibleTargetRect.top()) ||
        !qIsFinite(visibleTargetRect.width()) ||
        !qIsFinite(visibleTargetRect.height())) {
        return;
    }

    const QPointF sourceTopLeft = widgetToImage(visibleTargetRect.topLeft());
    const QPointF sourceBottomRight = widgetToImage(visibleTargetRect.bottomRight());

    if (!qIsFinite(sourceTopLeft.x()) ||
        !qIsFinite(sourceTopLeft.y()) ||
        !qIsFinite(sourceBottomRight.x()) ||
        !qIsFinite(sourceBottomRight.y())) {
        return;
    }

    int srcX = qFloor(sourceTopLeft.x());
    int srcY = qFloor(sourceTopLeft.y());
    int srcW = qCeil(sourceBottomRight.x()) - srcX;
    int srcH = qCeil(sourceBottomRight.y()) - srcY;

    if (srcX < 0) {
        srcW += srcX;
        srcX = 0;
    }

    if (srcY < 0) {
        srcH += srcY;
        srcY = 0;
    }

    if (srcX + srcW > mapRasterWidth)
        srcW = mapRasterWidth - srcX;

    if (srcY + srcH > mapRasterHeight)
        srcH = mapRasterHeight - srcY;

    if (srcW <= 0 || srcH <= 0)
        return;

    int outW = qMax(1, qRound(visibleTargetRect.width()));
    int outH = qMax(1, qRound(visibleTargetRect.height()));

    const int maxOutputSide = 4096;

    if (outW > maxOutputSide || outH > maxOutputSide) {
        const double scale = qMin(static_cast<double>(maxOutputSide) / outW,
                                  static_cast<double>(maxOutputSide) / outH);

        outW = qMax(1, qRound(outW * scale));
        outH = qMax(1, qRound(outH * scale));
    }

    const QRect currentSourceRect(srcX, srcY, srcW, srcH);

    if (!gdalVisibleImage.isNull() &&
        gdalVisibleSourceRect == currentSourceRect &&
        qAbs(gdalVisibleTargetRect.left() - visibleTargetRect.left()) < 0.5 &&
        qAbs(gdalVisibleTargetRect.top() - visibleTargetRect.top()) < 0.5 &&
        qAbs(gdalVisibleTargetRect.width() - visibleTargetRect.width()) < 0.5 &&
        qAbs(gdalVisibleTargetRect.height() - visibleTargetRect.height()) < 0.5) {

        painter.drawImage(visibleTargetRect, gdalVisibleImage);
        return;
    }

    QImage image(outW, outH, QImage::Format_RGB888);

    if (image.isNull())
        return;

    CPLErr result = CE_Failure;

    if (mapRasterBandCount >= 3) {
        int bandMap[3] = {1, 2, 3};

        result = mapDataset->RasterIO(
            GF_Read,
            srcX,
            srcY,
            srcW,
            srcH,
            image.bits(),
            outW,
            outH,
            GDT_Byte,
            3,
            bandMap,
            3,
            image.bytesPerLine(),
            1
            );
    } else {
        QVector<unsigned char> gray(outW * outH);

        GDALRasterBand *grayBand = mapDataset->GetRasterBand(1);

        if (!grayBand)
            return;

        result = grayBand->RasterIO(
            GF_Read,
            srcX,
            srcY,
            srcW,
            srcH,
            gray.data(),
            outW,
            outH,
            GDT_Byte,
            0,
            0
            );

        if (result == CE_None) {
            for (int y = 0; y < outH; ++y) {
                uchar *line = image.scanLine(y);

                for (int x = 0; x < outW; ++x) {
                    const uchar v = gray[y * outW + x];
                    line[x * 3 + 0] = v;
                    line[x * 3 + 1] = v;
                    line[x * 3 + 2] = v;
                }
            }
        }
    }

    if (result != CE_None) {
        qDebug() << "GDAL streaming RasterIO failed.";
        return;
    }

    gdalVisibleImage = image;
    gdalVisibleSourceRect = currentSourceRect;
    gdalVisibleTargetRect = visibleTargetRect;

    painter.drawImage(visibleTargetRect, gdalVisibleImage);
}

bool Position::hasMapData() const
{
    if (isGdalStreamingMap && mapDataset && mapRasterWidth > 0 && mapRasterHeight > 0)
        return true;

    return !mapImage.isNull();
}

int Position::mapPixelWidth() const
{
    if (isGdalStreamingMap)
        return mapRasterWidth;

    return mapImage.width();
}

int Position::mapPixelHeight() const
{
    if (isGdalStreamingMap)
        return mapRasterHeight;

    return mapImage.height();
}

//--- View Control ---
void Position::setZoom(double value)
{
    currentZoom = qBound(minimumZoom, value, maximumZoom);
    updateInfoBoxValues();
    emit mapViewChanged(currentZoom, offset);
    update();
}

void Position::zoomAtWidgetPoint(const QPointF &widgetPos, double zoomFactor)
{
    if (!hasMapData())
        return;

    const double oldZoom = currentZoom;
    const double newZoom = qBound(minimumZoom, oldZoom * zoomFactor, maximumZoom);

    if (qFuzzyCompare(oldZoom, newZoom))
        return;

    const QPointF imagePos = widgetToImage(widgetPos);
    currentZoom = newZoom;

    const QSizeF scaledSize(
        static_cast<double>(mapPixelWidth()) * currentZoom,
        static_cast<double>(mapPixelHeight()) * currentZoom
        );

    const QPointF baseTopLeft = rect().center() - QPointF(scaledSize.width() / 2.0,
                                                          scaledSize.height() / 2.0);

    offset = widgetPos - baseTopLeft - imagePos * currentZoom;

    updateInfoBoxValues();
    emit mapViewChanged(currentZoom, offset);
    update();
}

void Position::pan(int dx, int dy)
{
    offset.rx() += dx;
    offset.ry() += dy;
    emit mapViewChanged(currentZoom, offset);
    update();
}

void Position::fitMap()
{
    if (!hasMapData() || width() <= 0 || height() <= 0)
        return;

    const double zoomX = static_cast<double>(width()) / mapPixelWidth();
    const double zoomY = static_cast<double>(height()) / mapPixelHeight();

    currentZoom = qMin(zoomX, zoomY);

    offset = QPointF(0, 0);

    updateInfoBoxValues();
    emit mapViewChanged(currentZoom, offset);
    update();
}

void Position::resetView()
{
    currentZoom = defaultZoom;
    offset = QPointF(0, 0);

    if (isGeoTransform)
        centerOnLatLon(defaultLat, defaultLon);

    updateInfoBoxValues();
    emit mapViewChanged(currentZoom, offset);
    update();
}

void Position::centerOnLatLon(double lat, double lon)
{
    if (!hasMapData() || !isGeoTransform)
        return;

    currentLat = lat;
    currentLon = lon;
    centerOnImagePoint(latLonToImage(lat, lon));
}

void Position::centerOnImagePoint(const QPointF &imagePos)
{
    if (!hasMapData())
        return;

    const QSizeF scaledSize(
        static_cast<double>(mapPixelWidth()) * currentZoom,
        static_cast<double>(mapPixelHeight()) * currentZoom
        );

    const QPointF baseTopLeft = rect().center() - QPointF(scaledSize.width() / 2.0,
                                                          scaledSize.height() / 2.0);
    offset = rect().center() - baseTopLeft - imagePos * currentZoom;

    emit mapViewChanged(currentZoom, offset);
    update();
}

void Position::centerOnWidgetPoint(const QPointF &widgetPos)
{
    if (!isPointOnMap(widgetPos))
        return;

    centerOnImagePoint(widgetToImage(widgetPos));
}

//--- Coordinate Conversion ---
QPointF Position::latLonToImage(double lat, double lon) const
{
    if (!isGeoTransform)
        return QPointF(0, 0);

    const QPointF mapCoord = lonLatToMapCoordinate(lat, lon);

    const double mapX = mapCoord.x();
    const double mapY = mapCoord.y();

    const double determinant = geoTransform[1] * geoTransform[5] - geoTransform[2] * geoTransform[4];
    if (qFuzzyIsNull(determinant))
        return QPointF(0, 0);

    const double dx = mapX - geoTransform[0];
    const double dy = mapY - geoTransform[3];

    const double x = (geoTransform[5] * dx - geoTransform[2] * dy) / determinant;
    const double y = (-geoTransform[4] * dx + geoTransform[1] * dy) / determinant;

    return QPointF(x, y);
}

QPointF Position::imageToLatLon(double x, double y) const
{
    if (!isGeoTransform)
        return QPointF(0, 0);

    const double mapX = geoTransform[0] + x * geoTransform[1] + y * geoTransform[2];
    const double mapY = geoTransform[3] + x * geoTransform[4] + y * geoTransform[5];

    return mapCoordinateToLonLat(mapX, mapY);
}

QPointF Position::imageToWidget(const QPointF &imagePos) const
{
    return imageTopLeft() + imagePos * currentZoom;
}

QPointF Position::widgetToImage(const QPointF &widgetPos) const
{
    const QPointF topLeft = imageTopLeft();
    return QPointF((widgetPos.x() - topLeft.x()) / currentZoom,
                   (widgetPos.y() - topLeft.y()) / currentZoom);
}

QPointF Position::imageTopLeft() const
{
    if (!hasMapData())
        return QPointF(0, 0);

    const QSizeF scaledSize(
        static_cast<double>(mapPixelWidth()) * currentZoom,
        static_cast<double>(mapPixelHeight()) * currentZoom
        );

    QPointF topLeft = rect().center() - QPointF(scaledSize.width() / 2.0,
                                                scaledSize.height() / 2.0);
    topLeft += offset;
    return topLeft;
}

QPointF Position::lonLatToMapCoordinate(double lat, double lon) const
{
    if (isMapWebMercator)
        return lonLatToWebMercator(lat, lon);

    return QPointF(lon, lat);
}

QPointF Position::mapCoordinateToLonLat(double x, double y) const
{
    if (isMapWebMercator)
        return webMercatorToLonLat(x, y);

    return QPointF(x, y);
}

QPointF Position::lonLatToWebMercator(double lat, double lon) const
{
    constexpr double originShift = 20037508.342789244;

    double clampedLat = qBound(-85.05112878, lat, 85.05112878);

    const double x = lon * originShift / 180.0;

    double y = qLn(qTan((90.0 + clampedLat) * M_PI / 360.0)) / (M_PI / 180.0);
    y = y * originShift / 180.0;

    return QPointF(x, y);
}

QPointF Position::webMercatorToLonLat(double x, double y) const
{
    constexpr double originShift = 20037508.342789244;

    const double lon = (x / originShift) * 180.0;

    double lat = (y / originShift) * 180.0;
    lat = 180.0 / M_PI * (2.0 * qAtan(qExp(lat * M_PI / 180.0)) - M_PI / 2.0);

    return QPointF(lon, lat);
}

bool Position::isPointOnMap(const QPointF &widgetPos) const
{
    if (!hasMapData())
        return false;

    return isImagePointInsideMap(widgetToImage(widgetPos));
}

bool Position::isImagePointInsideMap(const QPointF &imagePos) const
{
    if (!hasMapData())
        return false;

    return imagePos.x() >= 0.0 && imagePos.y() >= 0.0 &&
           imagePos.x() < mapPixelWidth() &&
           imagePos.y() < mapPixelHeight();
}

bool Position::isWidgetPointDrawable(const QPointF &point, double margin) const
{
    if (!qIsFinite(point.x()) || !qIsFinite(point.y()))
        return false;

    return point.x() >= -margin &&
           point.y() >= -margin &&
           point.x() <= width() + margin &&
           point.y() <= height() + margin;
}

bool Position::isWidgetRectDrawable(const QRectF &rectValue, double margin) const
{
    if (!qIsFinite(rectValue.left()) ||
        !qIsFinite(rectValue.top()) ||
        !qIsFinite(rectValue.right()) ||
        !qIsFinite(rectValue.bottom()))
        return false;

    QRectF viewport(-margin, -margin,
                    width() + 2.0 * margin,
                    height() + 2.0 * margin);

    return viewport.intersects(rectValue.normalized());
}

//--- Tool Control ---
void Position::setToolMode(EnumMapToolMode toolMode)
{
    currentTool = toolMode;

    isZoomBoxMode = currentTool == MapToolZoomBox;
    isDistanceMode = currentTool == MapToolDistance;
    isLeftPanEnabled = currentTool == MapToolPan;

    if (!isDistanceMode)
        resetDistance();

    if (currentTool == MapToolMarker)
        currentShape = Marker;
    else if (currentTool == MapToolText)
        currentShape = Text;
    else if (currentTool == MapToolLine)
        currentShape = Line;
    else if (currentTool == MapToolPolyline)
        currentShape = Polyline;
    else if (currentTool == MapToolRectangle)
        currentShape = Rectangle;
    else if (currentTool == MapToolCircle)
        currentShape = Circle;
    else if (currentTool == MapToolEllipse)
        currentShape = Ellipse;
    else if (currentTool == MapToolPolygon)
        currentShape = Polygon;
    else if (currentTool == MapToolFreehand)
        currentShape = Freehand;
    else
        currentShape = Unknown;

    cancelDrawing();

    updateButtonStates();
    updateActionStates();

    emit toolModeChanged(currentTool);
    update();
}

EnumMapToolMode Position::getToolMode() const
{
    return currentTool;
}

void Position::setCurrentShape(EnumMapShapeType shapeType)
{
    currentShape = shapeType;
}

EnumMapShapeType Position::getCurrentShape() const
{
    return currentShape;
}

void Position::cancelCurrentTool()
{
    currentTool = MapToolNone;
    isZoomBoxMode = false;
    isDistanceMode = false;
    isLeftPanEnabled = false;
    cancelDrawing();
    resetDistance();
    updateButtonStates();
    updateActionStates();
    emit toolModeChanged(currentTool);
    update();
}

void Position::finishCurrentDrawing()
{
    if (temporaryPoints.isEmpty())
        return;

    MapItemStruct item = createItemFromTemporaryPoints();

    if (item.type == Unknown || item.points.isEmpty()) {
        cancelDrawing();
        return;
    }

    if (!addItemToActiveLayer(item)) {
        qDebug() << "Failed to add map item to active layer";
        cancelDrawing();
        return;
    }

    temporaryPoints.clear();
    isDrawing = false;
    isTemporaryPreviewVisible = false;

    update();
}

//--- Layer Item Actions ---
bool Position::addMarkerAt(double lat, double lon, const QString &name)
{
    return addItemToActiveLayer(createMarkerItem(lat, lon, name));
}

bool Position::addTextAt(double lat, double lon, const QString &text)
{
    return addItemToActiveLayer(createTextItem(lat, lon, text));
}

bool Position::addItemToActiveLayer(MapItemStruct item)
{
    if (!mapLayerManager) {
        qDebug() << "Position::addItemToActiveLayer failed: mapLayerManager is null";
        return false;
    }

    if (item.points.isEmpty()) {
        qDebug() << "Position::addItemToActiveLayer failed: item has no points";
        return false;
    }

    if (mapLayerManager->activeLayerID() <= 0) {
        if (!mapLayerManager->addLayer("Default Layer", item.userID)) {
            qDebug() << "Position::addItemToActiveLayer failed: cannot create default layer";
            return false;
        }
    }

    item.layerID = mapLayerManager->activeLayerID();

    const bool ok = mapLayerManager->addItemToActiveLayer(item);

    if (!ok) {
        qDebug() << "Position::addItemToActiveLayer failed: manager rejected item"
                 << "type =" << static_cast<int>(item.type)
                 << "points =" << item.points.size()
                 << "layerID =" << item.layerID;
        return false;
    }

    update();
    return true;
}

int Position::addDfStation(double lat, double lon, const QString &name, double angle)
{
    MapDfStationStruct station;
    station.id = nextDfStationID++;
    station.name = name.trimmed().isEmpty() ? "DfStation" : name.trimmed();
    station.lat = lat;
    station.lon = lon;
    station.angle = angle;
    station.isVisible = true;

    dfStations.push_back(station);

    update();
    return station.id;
}

bool Position::setDfStationAngle(int dfStationID, double angle)
{
    for (MapDfStationStruct &station : dfStations) {
        if (station.id == dfStationID) {
            station.angle = angle;
            update();
            return true;
        }
    }

    return false;
}

bool Position::updateDfStation(int dfStationID,
                               double lat,
                               double lon,
                               const QString &name,
                               double angle)
{
    for (MapDfStationStruct &station : dfStations) {
        if (station.id == dfStationID) {
            station.lat = lat;
            station.lon = lon;
            station.name = name.trimmed().isEmpty() ? "DfStation" : name.trimmed();
            station.angle = angle;
            station.isVisible = true;

            update();
            return true;
        }
    }

    return false;
}

void Position::clearDfStations()
{
    dfStations.clear();
    update();
}

//--- Options ---
void Position::setGridVisible(bool visible)
{
    isGridVisible = visible;
    updateActionStates();
    updateButtonStates();
    update();
}

bool Position::getGridVisible() const
{
    return isGridVisible;
}

void Position::setMouseInfoVisible(bool visible)
{
    isMouseInfoVisible = visible;

    if (infoBox)
        infoBox->setVisible(isMouseInfoVisible || isPfMode);

    updateActionStates();
    updateInfoBoxGeometry();
}

bool Position::getMouseInfoVisible() const
{
    return isMouseInfoVisible;
}

void Position::setLabelsVisible(bool visible)
{
    isLabelsVisible = visible;
    updateActionStates();
    update();
}

bool Position::getLabelsVisible() const
{
    return isLabelsVisible;
}

void Position::setDistanceMode(bool active)
{
    if (active)
        setToolMode(MapToolDistance);
    else if (currentTool == MapToolDistance)
        cancelCurrentTool();
}

bool Position::getDistanceMode() const
{
    return isDistanceMode;
}

void Position::setLeftPanEnabled(bool enabled)
{
    if (enabled)
        setToolMode(MapToolPan);
    else if (currentTool == MapToolPan)
        cancelCurrentTool();
}

bool Position::getLeftPanEnabled() const
{
    return isLeftPanEnabled;
}

//--- Utility ---
double Position::calculateDistance(double startLat, double startLon, double endLat, double endLon) const
{
    constexpr double earthRadiusMeters = 6371000.0;

    const double lat1 = qDegreesToRadians(startLat);
    const double lon1 = qDegreesToRadians(startLon);
    const double lat2 = qDegreesToRadians(endLat);
    const double lon2 = qDegreesToRadians(endLon);

    const double dLat = lat2 - lat1;
    const double dLon = lon2 - lon1;

    const double a = qSin(dLat / 2.0) * qSin(dLat / 2.0) +
                     qCos(lat1) * qCos(lat2) *
                         qSin(dLon / 2.0) * qSin(dLon / 2.0);
    const double c = 2.0 * qAtan2(qSqrt(a), qSqrt(1.0 - a));

    return earthRadiusMeters * c;
}

QString Position::formatDistance(double meters) const
{
    if (meters < 1000.0)
        return QString("%1 m").arg(QString::number(meters, 'f', 1));

    return QString("%1 km").arg(QString::number(meters / 1000.0, 'f', 3));
}

QString Position::formatCoordinate(double value) const
{
    return QString::number(value, 'f', 6);
}

GeoPointStruct Position::destinationPointByBearing(double lat,
                                                   double lon,
                                                   double bearingDegree,
                                                   double distanceKm) const
{
    constexpr double earthRadiusKm = 6371.0;

    const double bearingRad = qDegreesToRadians(bearingDegree);
    const double lat1 = qDegreesToRadians(lat);
    const double lon1 = qDegreesToRadians(lon);
    const double angularDistance = distanceKm / earthRadiusKm;

    const double lat2 = qAsin(qSin(lat1) * qCos(angularDistance) +
                              qCos(lat1) * qSin(angularDistance) * qCos(bearingRad));

    const double lon2 = lon1 + qAtan2(qSin(bearingRad) * qSin(angularDistance) * qCos(lat1),
                                      qCos(angularDistance) - qSin(lat1) * qSin(lat2));

    GeoPointStruct point;
    point.lat = qRadiansToDegrees(lat2);
    point.lon = qRadiansToDegrees(lon2);
    point.pointIndex = 1;

    return point;
}

//--- UI Create ---
void Position::createContextMenu()
{
    contextMenu = new QMenu(this);

    actCenterHere = contextMenu->addAction("Center Here");

    QMenu *markerMenuContext = contextMenu->addMenu("Add Marker Here");
    markerMenuContext->setIcon(QIcon(":/Res/Images/Map/Marker.png"));

    auto addMarkerIconActionToContext =
        [this, markerMenuContext](const QString &markerName,
                                  const QString &iconPath)
    {
        QToolButton *button = new QToolButton(markerMenuContext);
        button->setFixedSize(44, 44);
        button->setIcon(QIcon(iconPath));
        button->setIconSize(QSize(32, 32));
        button->setToolTip(markerName);
        button->setAutoRaise(true);
        button->setStyleSheet(
            "QToolButton {"
            "  padding: 4px;"
            "  border: 1px solid transparent;"
            "  border-radius: 6px;"
            "  background: transparent;"
            "}"
            "QToolButton:hover {"
            "  border: 1px solid #ffffff;"
            "  background: rgba(255,255,255,35);"
            "}"
            );

        QWidgetAction *widgetAction = new QWidgetAction(markerMenuContext);
        widgetAction->setDefaultWidget(button);
        markerMenuContext->addAction(widgetAction);

        connect(button, &QToolButton::clicked, this, [this, markerMenuContext, markerName, iconPath]() {
            selectedMarkerName = markerName;
            selectedMarkerIconPath = iconPath;

            QPointF imagePos = widgetToImage(lastContextMenuPos);

            if (!isImagePointInsideMap(imagePos)) {
                markerMenuContext->close();
                return;
            }

            QPointF geo = imageToLatLon(imagePos.x(), imagePos.y());

            addMarkerAt(geo.y(), geo.x(), markerName);

            markerMenuContext->close();
            if (contextMenu)
                contextMenu->close();
        });
    };

    addMarkerIconActionToContext("Marker", ":/Res/Images/Map/Marker.png");

    for (int i = 1; i <= 15; ++i) {
        const QString markerName = QString::number(i);
        const QString iconPath = QString(":/Res/Images/Map/%1.png").arg(i);
        addMarkerIconActionToContext(markerName, iconPath);
    }
    actAddTextHere = contextMenu->addAction("Add Text Here");
    actStartMeasureHere = contextMenu->addAction("Start Measure Here");
    contextMenu->addSeparator();
    actCopyCoordinates = contextMenu->addAction("Copy Coordinates");
    contextMenu->addSeparator();
    actFitMap = contextMenu->addAction("Fit Map");
    actResetView = contextMenu->addAction("Reset View");
    contextMenu->addSeparator();
    actShowGrid = contextMenu->addAction("Show Grid");
    actShowGrid->setCheckable(true);
    actShowInfoBox = contextMenu->addAction("Show Info Box");
    actShowInfoBox->setCheckable(true);
    actShowLabels = contextMenu->addAction("Show Labels");
    actShowLabels->setCheckable(true);

    connect(actCenterHere, &QAction::triggered, this, &Position::actionCenterHere);
    // connect(actAddMarkerHere, &QAction::triggered, this, &Position::actionAddMarkerHere);
    connect(actAddTextHere, &QAction::triggered, this, &Position::actionAddTextHere);
    connect(actStartMeasureHere, &QAction::triggered, this, &Position::actionStartMeasureHere);
    connect(actCopyCoordinates, &QAction::triggered, this, &Position::actionCopyCoordinates);
    connect(actFitMap, &QAction::triggered, this, &Position::actionFitMap);
    connect(actResetView, &QAction::triggered, this, &Position::actionResetView);
    connect(actShowGrid, &QAction::triggered, this, &Position::actionToggleGrid);
    connect(actShowInfoBox, &QAction::triggered, this, &Position::actionToggleInfoBox);
    connect(actShowLabels, &QAction::triggered, this, &Position::actionToggleLabels);

    updateActionStates();
}

void Position::createBotMenu()
{
    botMenu = new QFrame(this);
    botMenu->setFixedHeight(60);
    botLayout = new QHBoxLayout(botMenu);
    botLayout->setContentsMargins(6, 6, 6, 6);
    botLayout->setSpacing(6);
    botMenu->setLayout(botLayout);

    botMenuOpacity = new QGraphicsOpacityEffect(botMenu);
    botMenuOpacity->setOpacity(0.35);
    botMenu->setGraphicsEffect(botMenuOpacity);
    botMenu->installEventFilter(this);

    createBotMapMenu();
    createBotImportMenu();
    createBotMarkerMenu();
    createBotTextMenu();
    createBotLineMenu();
    createBotAreaMenu();
    createBotDistanceMenu();
    createBotGridMenu();

    btnMap = new MapBtnMenu(gVar, ":/Res/Images/Map/Import.png", "Map", {}, this);
    btnImport = new MapBtnMenu(gVar, ":/Res/Images/Map/Layer.png", "Import", {}, this);
    btnPan = new MapBtnMenu(gVar, ":/Res/Images/Map/Pan.png", "Pan", {}, this);
    btnZoomBox = new MapBtnMenu(gVar, ":/Res/Images/Map/Zoom.png", "Zoom Box", {}, this);
    btnMarker = new MapBtnMenu(gVar, ":/Res/Images/Map/Marker.png", "Marker", {}, this);
    btnText = new MapBtnMenu(gVar, ":/Res/Images/Map/Text.png", "Text", {}, this);
    btnLine = new MapBtnMenu(gVar, ":/Res/Images/Map/Line.png", "Line", {}, this);
    btnArea = new MapBtnMenu(gVar, ":/Res/Images/Map/Area.png", "Area", {}, this);
    btnDistance = new MapBtnMenu(gVar, ":/Res/Images/Map/Distance.png", "Distance", {}, this);
    btnGrid = new MapBtnMenu(gVar, ":/Res/Images/Map/Grid.png", "Grid", {}, this);
    btnInfo = new MapBtnMenu(gVar, ":/Res/Images/Map/Info.png", "Info", {}, this);
    btnFit = new MapBtnMenu(gVar, ":/Res/Images/Map/Fit.png", "Fit", {}, this);
    btnPf = new MapBtnMenu(gVar, ":/Res/Images/Map/Target.png", "Position Finder", {}, this);

    botLayout->addWidget(btnMap);
    botLayout->addWidget(btnImport);
    botLayout->addWidget(btnPan);
    botLayout->addWidget(btnZoomBox);
    botLayout->addWidget(btnMarker);
    botLayout->addWidget(btnText);
    botLayout->addWidget(btnLine);
    botLayout->addWidget(btnArea);
    botLayout->addWidget(btnDistance);
    botLayout->addWidget(btnGrid);
    botLayout->addWidget(btnInfo);
    botLayout->addWidget(btnFit);
    botLayout->addWidget(btnPf);

    connect(btnMap, &MapBtnMenu::leftClick, this, &Position::clickBtnMap);
    connect(btnImport, &MapBtnMenu::leftClick, this, &Position::clickBtnImport);
    connect(btnPan, &MapBtnMenu::leftClick, this, &Position::clickBtnPan);
    connect(btnZoomBox, &MapBtnMenu::leftClick, this, &Position::clickBtnZoomBox);
    connect(btnMarker, &MapBtnMenu::leftClick, this, &Position::clickBtnMarker);
    connect(btnText, &MapBtnMenu::leftClick, this, &Position::clickBtnText);
    connect(btnLine, &MapBtnMenu::leftClick, this, &Position::clickBtnLine);
    connect(btnArea, &MapBtnMenu::leftClick, this, &Position::clickBtnArea);
    connect(btnDistance, &MapBtnMenu::leftClick, this, &Position::clickBtnDistance);
    connect(btnGrid, &MapBtnMenu::leftClick, this, &Position::clickBtnGrid);
    connect(btnInfo, &MapBtnMenu::leftClick, this, &Position::clickBtnInfo);
    connect(btnFit, &MapBtnMenu::leftClick, this, &Position::clickBtnFit);
    connect(btnPf, &MapBtnMenu::leftClick, this, &Position::clickBtnPf);

    botMenu->raise();
}

void Position::createBotMapMenu()
{
    mapMenu = new QMenu(this);
    mapMenu->addAction("Load Map", this, &Position::actionLoadMap);
    mapMenu->addAction("Load Default Map", this, &Position::actionLoadDefaultMap);
}

void Position::createBotImportMenu()
{
    importMenu = new QMenu(this);
    importMenu->addAction("Import GeoJSON", this, &Position::actionImportGeoJson);
    importMenu->addAction("Import CSV Points", this, &Position::actionImportCsvPoints);
}

void Position::createBotMarkerMenu()
{
    markerMenu = new QMenu(this);

    auto addMarkerIconActionToMenu =
        [this](const QString &markerName,
               const QString &iconPath)
    {
        QToolButton *button = new QToolButton(markerMenu);
        button->setFixedSize(44, 44);
        button->setIcon(QIcon(iconPath));
        button->setIconSize(QSize(32, 32));
        button->setToolTip(markerName);
        button->setAutoRaise(true);
        button->setStyleSheet(
            "QToolButton {"
            "  padding: 4px;"
            "  border: 1px solid transparent;"
            "  border-radius: 6px;"
            "  background: transparent;"
            "}"
            "QToolButton:hover {"
            "  border: 1px solid #ffffff;"
            "  background: rgba(255,255,255,35);"
            "}"
            );

        QWidgetAction *widgetAction = new QWidgetAction(markerMenu);
        widgetAction->setDefaultWidget(button);
        markerMenu->addAction(widgetAction);

        connect(button, &QToolButton::clicked, this, [this, markerName, iconPath]() {
            selectedMarkerName = markerName;
            selectedMarkerIconPath = iconPath;

            setToolMode(MapToolMarker);

            if (markerMenu)
                markerMenu->close();
        });
    };

    for (int i = 1; i <= 15; ++i) {
        const QString markerName = "Marker";
        const QString iconPath = QString(":/Res/Images/Map/%1.png").arg(i);
        addMarkerIconActionToMenu(markerName, iconPath);
    }

    addMarkerIconActionToMenu("Marker", ":/Res/Images/Map/Marker.png");
}

void Position::createBotTextMenu()
{
    textMenu = new QMenu(this);
    textMenu->addAction("Text", this, &Position::actionDrawText);
}

void Position::createBotLineMenu()
{
    lineMenu = new QMenu(this);
    lineMenu->addAction("Single Line", this, &Position::actionDrawLine);
    lineMenu->addAction("Polyline / Road", this, &Position::actionDrawPolyline);
}

void Position::createBotAreaMenu()
{
    areaMenu = new QMenu(this);
    areaMenu->addAction("Rectangle", this, &Position::actionDrawRectangle);
    areaMenu->addAction("Circle", this, &Position::actionDrawCircle);
    areaMenu->addAction("Ellipse", this, &Position::actionDrawEllipse);
    areaMenu->addAction("Polygon", this, &Position::actionDrawPolygon);
    areaMenu->addAction("Freehand", this, &Position::actionDrawFreehand);
}

void Position::createBotDistanceMenu()
{
    distanceMenu = new QMenu(this);
    distanceMenu->addAction("Single Distance", this, &Position::actionDistanceSingle);
    distanceMenu->addAction("Chain Distance", this, &Position::actionDistanceChain);
}

void Position::createBotGridMenu()
{
    gridMenu = new QMenu(this);
    gridMenu->addAction("Show / Hide Grid", this, &Position::actionToggleGrid);
    gridMenu->addAction("Grid Settings", this, &Position::actionGridSettings);
}

void Position::createInfoBox()
{
    infoBox = new QFrame(this);
    infoBox->setFixedWidth(170);
    infoLayout = new QGridLayout(infoBox);
    infoLayout->setContentsMargins(10, 10, 10, 10);
    infoLayout->setHorizontalSpacing(6);
    infoLayout->setVerticalSpacing(4);

    pfTimer = new QTimer(this);
    pfTimer->setInterval(1000);
    connect(pfTimer, &QTimer::timeout, this, &Position::tickPfTimer);

    int row = 0;

    lblTargetTitle = new QLabel("Target");
    infoLayout->addWidget(lblTargetTitle, row++, 0, 1, 2);
    lblTargetTimeTitle = new QLabel("Timer:");
    lblTargetTime = new QLabel("000:00:00");
    infoLayout->addWidget(lblTargetTimeTitle, row, 0);
    infoLayout->addWidget(lblTargetTime, row++, 1);
    lblTargetLatTitle = new QLabel("Lat:");
    lblTargetLat = new QLabel("00.000000");
    infoLayout->addWidget(lblTargetLatTitle, row, 0);
    infoLayout->addWidget(lblTargetLat, row++, 1);
    lblTargetLonTitle = new QLabel("Lon:");
    lblTargetLon = new QLabel("00.000000");
    infoLayout->addWidget(lblTargetLonTitle, row, 0);
    infoLayout->addWidget(lblTargetLon, row++, 1);
    lblTargetTitle->setVisible(false);
    lblTargetTimeTitle->setVisible(false);
    lblTargetTime->setVisible(false);
    lblTargetLatTitle->setVisible(false);
    lblTargetLat->setVisible(false);
    lblTargetLonTitle->setVisible(false);
    lblTargetLon->setVisible(false);

    lblDistanceTotalTitle = new QLabel("Total:");
    lblDistanceTotal = new QLabel("0.000 km");
    infoLayout->addWidget(lblDistanceTotalTitle, row, 0);
    infoLayout->addWidget(lblDistanceTotal, row++, 1);
    lblDistanceTotalTitle->setVisible(false);
    lblDistanceTotal->setVisible(false);

    lblMouseTitle = new QLabel("Mouse");
    infoLayout->addWidget(lblMouseTitle, row++, 0, 1, 2);
    lblMouseLatTitle = new QLabel("Lat:");
    lblMouseLat = new QLabel("00.000000");
    infoLayout->addWidget(lblMouseLatTitle, row, 0);
    infoLayout->addWidget(lblMouseLat, row++, 1);
    lblMouseLonTitle = new QLabel("Lon:");
    lblMouseLon = new QLabel("00.000000");
    infoLayout->addWidget(lblMouseLonTitle, row, 0);
    infoLayout->addWidget(lblMouseLon, row++, 1);
    lblMousePixelTitle = new QLabel("Pixel:");
    lblMousePixel = new QLabel("0, 0");
    infoLayout->addWidget(lblMousePixelTitle, row, 0);
    infoLayout->addWidget(lblMousePixel, row++, 1);
    lblMousePixelTitle->setVisible(false);
    lblMousePixel->setVisible(false);
    lblZoomTitle = new QLabel("Zoom:");
    lblZoom = new QLabel("1.00x");
    infoLayout->addWidget(lblZoomTitle, row, 0);
    infoLayout->addWidget(lblZoom, row++, 1);

    infoBox->raise();
    updateInfoBoxValues();
}

//--- UI Update ---
void Position::updateBotMenuGeometry()
{
    if (!botMenu || !botLayout)
        return;

    const int margin = 20;
    const int frameHeight = 60;
    botMenu->setFixedHeight(frameHeight);
    botLayout->activate();

    int frameWidth = qMax(botMenu->sizeHint().width(), frameHeight);
    frameWidth = qMin(frameWidth, width() - 2 * margin);

    const int x = (width() - frameWidth) / 2;
    const int y = height() - frameHeight - margin;

    botMenu->setGeometry(x, y, frameWidth, frameHeight);
    botMenu->raise();
}

void Position::updateInfoBoxGeometry()
{
    if (!infoBox)
        return;

    infoBox->adjustSize();
    const int marginTop = 20;
    const int marginRight = 20;
    const QSize size = infoBox->sizeHint();
    const int x = width() - infoBox->width() - marginRight;
    const int y = marginTop;

    infoBox->setGeometry(x, y, infoBox->width(), size.height());
    infoBox->raise();
}

void Position::updateInfoBoxValues()
{
    if (!infoBox)
        return;

    double totalDistance = 0.0;
    for (int i = 1; i < distancePoints.size(); ++i) {
        totalDistance += distanceBetweenGeoPoints(distancePoints[i - 1], distancePoints[i]);
    }
    const bool showDistanceTotal =
        currentTool == MapToolDistance &&
        isDistanceChainMode &&
        distancePoints.size() > 1;
    if (lblDistanceTotalTitle)
        lblDistanceTotalTitle->setVisible(showDistanceTotal);
    if (lblDistanceTotal) {
        lblDistanceTotal->setVisible(showDistanceTotal);
        lblDistanceTotal->setText(formatDistance(totalDistance));
    }

    lblMouseLat->setText(formatCoordinate(mouseLat));
    lblMouseLon->setText(formatCoordinate(mouseLon));
    lblZoom->setText(QString("%1x").arg(QString::number(currentZoom, 'f', 2)));

    const QPointF imagePos = widgetToImage(lastMousePos);
    lblMousePixel->setText(QString("%1, %2")
                               .arg(QString::number(imagePos.x(), 'f', 0))
                               .arg(QString::number(imagePos.y(), 'f', 0)));
}

void Position::updateActionStates()
{
    if (actShowGrid)
        actShowGrid->setChecked(isGridVisible);
    if (actShowInfoBox)
        actShowInfoBox->setChecked(isMouseInfoVisible);
    if (actShowLabels)
        actShowLabels->setChecked(isLabelsVisible);
}

void Position::updateButtonStates()
{
    if (btnPan)
        btnPan->setActive(currentTool == MapToolPan);
    if (btnZoomBox)
        btnZoomBox->setActive(currentTool == MapToolZoomBox);
    if (btnMarker)
        btnMarker->setActive(currentTool == MapToolMarker);
    if (btnText)
        btnText->setActive(currentTool == MapToolText);
    if (btnLine)
        btnLine->setActive(currentTool == MapToolLine || currentTool == MapToolPolyline);
    if (btnArea)
        btnArea->setActive(currentTool == MapToolRectangle || currentTool == MapToolCircle ||
                           currentTool == MapToolEllipse || currentTool == MapToolPolygon ||
                           currentTool == MapToolFreehand);
    if (btnDistance)
        btnDistance->setActive(currentTool == MapToolDistance);
    if (btnGrid)
        btnGrid->setActive(isGridVisible);
    if (btnInfo)
        btnInfo->setActive(isMouseInfoVisible);
    if (btnPf)
        btnPf->setActive(isPfMode);
}

//--- Tool Helpers ---
void Position::startPan(const QPoint &mousePos, bool middleButton)
{
    isPanning = true;
    isMiddlePanning = middleButton;
    lastMousePos = mousePos;
    setCursor(Qt::ClosedHandCursor);
}

void Position::updatePan(const QPoint &mousePos)
{
    const QPoint delta = mousePos - lastMousePos;
    pan(delta.x(), delta.y());
    lastMousePos = mousePos;
}

void Position::stopPan()
{
    isPanning = false;
    isMiddlePanning = false;
    setCursor(Qt::ArrowCursor);
}

void Position::startZoomBox(const QPointF &widgetPos)
{
    if (!isPointOnMap(widgetPos))
        return;

    isZoomBoxDrawing = true;
    zoomBoxStartWidgetPos = widgetPos;
    zoomBoxEndWidgetPos = widgetPos;
    update();
}

void Position::updateZoomBox(const QPointF &widgetPos)
{
    if (!isZoomBoxDrawing)
        return;

    zoomBoxEndWidgetPos = widgetPos;
    update();
}

void Position::finishZoomBox()
{
    if (!isZoomBoxDrawing) {
        return;
    }

    isZoomBoxDrawing = false;

    QRectF box(zoomBoxStartWidgetPos, zoomBoxEndWidgetPos);
    box = box.normalized();

    if (box.width() < 10 || box.height() < 10) {
        update();
        return;
    }

    const QPointF imageA = widgetToImage(box.topLeft());
    const QPointF imageB = widgetToImage(box.bottomRight());
    QRectF imageBox(imageA, imageB);
    imageBox = imageBox.normalized();

    if (imageBox.width() <= 0.0 || imageBox.height() <= 0.0)
        return;

    const double zoomX = width() / imageBox.width();
    const double zoomY = height() / imageBox.height();
    currentZoom = qBound(minimumZoom, qMin(zoomX, zoomY) * 0.95, maximumZoom);

    centerOnImagePoint(imageBox.center());
    update();
}

void Position::startDrawingAt(const QPointF &widgetPos)
{
    temporaryPoints.clear();
    isDrawing = true;
    addDrawingPointAt(widgetPos);
}

void Position::updateDrawingAt(const QPointF &widgetPos)
{
    if (!isPointOnMap(widgetPos))
        return;

    temporaryCurrentImagePos = widgetToImage(widgetPos);

    if (currentTool == MapToolFreehand && isDrawing) {
        QPointF geo = imageToLatLon(temporaryCurrentImagePos.x(), temporaryCurrentImagePos.y());
        GeoPointStruct point;
        point.lon = geo.x();
        point.lat = geo.y();
        point.pointIndex = temporaryPoints.size();
        temporaryPoints.push_back(point);
    }

    update();
}

void Position::addDrawingPointAt(const QPointF &widgetPos)
{
    QPointF imagePos = widgetToImage(widgetPos);

    if (!isImagePointInsideMap(imagePos))
        return;

    QPointF geo = imageToLatLon(imagePos.x(), imagePos.y());

    GeoPointStruct point;
    point.lat = geo.y();
    point.lon = geo.x();
    point.pointIndex = temporaryPoints.size();

    temporaryPoints.push_back(point);

    isDrawing = true;
    isTemporaryPreviewVisible = true;
    temporaryCurrentImagePos = imagePos;

    if (currentTool == MapToolMarker || currentTool == MapToolText) {
        finishCurrentDrawing();
        return;
    }

    if (currentTool == MapToolLine ||
        currentTool == MapToolRectangle ||
        currentTool == MapToolCircle ||
        currentTool == MapToolEllipse) {

        if (temporaryPoints.size() >= 2) {
            finishCurrentDrawing();
            return;
        }
    }

    update();
}

void Position::finishDrawingByTool()
{
    if (!isDrawing)
        return;

    bool canSave = false;

    if (currentShape == Line && temporaryPoints.size() >= 2)
        canSave = true;
    else if (currentShape == Polyline && temporaryPoints.size() >= 2)
        canSave = true;
    else if ((currentShape == Rectangle || currentShape == Circle || currentShape == Ellipse) && temporaryPoints.size() >= 2)
        canSave = true;
    else if (currentShape == Polygon && temporaryPoints.size() >= 3)
        canSave = true;
    else if (currentShape == Freehand && temporaryPoints.size() >= 2)
        canSave = true;

    if (!canSave)
        return;

    MapItemStruct item = createItemFromTemporaryPoints();
    addItemToActiveLayer(item);
    cancelDrawing();
}

void Position::cancelDrawing()
{
    isDrawing = false;
    isTemporaryPreviewVisible = false;
    temporaryPoints.clear();
    temporaryCurrentImagePos = QPointF();
    temporaryStartImagePos = QPointF();
    temporaryEndImagePos = QPointF();
    update();
}

void Position::startDistanceAt(const QPointF &widgetPos)
{
    distancePoints.clear();
    isDistanceDrawing = true;
    addDistancePointAt(widgetPos);
}

void Position::updateDistanceAt(const QPointF &widgetPos)
{
    if (!isDistanceMode || !isPointOnMap(widgetPos))
        return;

    distanceCurrentImagePos = widgetToImage(widgetPos);
    update();
}

void Position::addDistancePointAt(const QPointF &widgetPos)
{
    if (!isPointOnMap(widgetPos) || !isGeoTransform)
        return;

    const QPointF imagePos = widgetToImage(widgetPos);
    const QPointF geo = imageToLatLon(imagePos.x(), imagePos.y());

    GeoPointStruct point;
    point.lat = geo.y();
    point.lon = geo.x();
    point.pointIndex = distancePoints.size();

    if (!isDistanceChainMode && distancePoints.size() >= 2) {
        distancePoints.clear();
    }

    distancePoints.push_back(point);
    distanceCurrentImagePos = imagePos;

    if (!isDistanceChainMode && distancePoints.size() >= 2) {
        isDistanceDrawing = false;
    } else {
        isDistanceDrawing = true;
    }

    updateInfoBoxValues();
    update();
}

void Position::resetDistance()
{
    isDistanceDrawing = false;
    distancePoints.clear();
    distanceCurrentImagePos = QPointF();
    update();
}

//--- Build Item Helpers ---
MapItemStruct Position::createItemFromTemporaryPoints() const
{
    MapItemStruct item;

    item.eID = -1;
    item.name = "New Item";
    item.note = "";
    item.isVisible = true;
    item.isDeleted = false;
    item.createTime = QDateTime::currentDateTimeUtc();

    item.style.strokeColor = "#FF0000";
    item.style.fillColor = "#AAFF0000";
    item.style.textColor = "#FFFFFF";
    item.style.strokeWidth = 2;
    item.style.pointSize = 10;
    item.style.fontSize = 12;
    item.style.opacity = 1.0;

    switch (currentTool) {
    case MapToolMarker:
    {
        item.type = Marker;
        item.name = selectedMarkerName.trimmed().isEmpty() ? "Marker" : selectedMarkerName.trimmed();

        QString iconPath = selectedMarkerIconPath.trimmed();

        if (iconPath.isEmpty() || !QFile::exists(iconPath)) {
            bool okNumber = false;
            int number = item.name.toInt(&okNumber);

            if (okNumber && number >= 1 && number <= 15)
                iconPath = QString(":/Res/Images/Map/%1.png").arg(number);
            else
                iconPath = ":/Res/Images/Map/Marker.png";
        }

        item.style.iconPath = iconPath;
        item.style.pointSize = 32;
        item.style.fontSize = 10;
        item.style.textColor = "#FFFFFF";
        break;
    }

    case MapToolText:
        item.type = Text;
        item.name = "Text";
        break;

    case MapToolLine:
        if (temporaryPoints.size() < 2) {
            item.type = Unknown;
            return item;
        }
        item.type = Line;
        item.name = "Line";
        break;

    case MapToolPolyline:
        if (temporaryPoints.size() < 2) {
            item.type = Unknown;
            return item;
        }
        item.type = Polyline;
        item.name = "Polyline";
        break;

    case MapToolRectangle:
        if (temporaryPoints.size() < 2) {
            item.type = Unknown;
            return item;
        }
        item.type = Rectangle;
        item.name = "Rectangle";
        break;

    case MapToolCircle:
        if (temporaryPoints.size() < 2) {
            item.type = Unknown;
            return item;
        }
        item.type = Circle;
        item.name = "Circle";
        break;

    case MapToolEllipse:
        if (temporaryPoints.size() < 2) {
            item.type = Unknown;
            return item;
        }
        item.type = Ellipse;
        item.name = "Ellipse";
        break;

    case MapToolPolygon:
        if (temporaryPoints.size() < 3) {
            item.type = Unknown;
            return item;
        }
        item.type = Polygon;
        item.name = "Polygon";
        break;

    case MapToolFreehand:
        if (temporaryPoints.size() < 2) {
            item.type = Unknown;
            return item;
        }
        item.type = Freehand;
        item.name = "Freehand";
        break;

    default:
        item.type = Unknown;
        return item;
    }

    item.points = temporaryPoints;
    return item;
}

MapItemStruct Position::createMarkerItem(double lat, double lon, const QString &name) const
{
    MapItemStruct item;

    item.eID = -1;
    item.name = name.trimmed().isEmpty() ? selectedMarkerName : name.trimmed();
    item.type = Marker;
    item.note = "";
    item.isVisible = true;
    item.isDeleted = false;
    item.createTime = QDateTime::currentDateTimeUtc();

    QString iconPath = selectedMarkerIconPath.trimmed();

    if (iconPath.isEmpty() || !QFile::exists(iconPath)) {
        bool okNumber = false;
        int number = item.name.toInt(&okNumber);

        if (okNumber && number >= 1 && number <= 15)
            iconPath = QString(":/Res/Images/Map/%1.png").arg(number);
        else
            iconPath = ":/Res/Images/Map/Marker.png";
    }

    item.style.iconPath = iconPath;
    item.style.strokeColor = "#FF0000";
    item.style.fillColor = "#33FF0000";
    item.style.textColor = "#FFFFFF";
    item.style.strokeWidth = 2;
    item.style.pointSize = 32;
    item.style.fontSize = 10;
    item.style.opacity = 1.0;
    item.style.rotation = 0.0;

    GeoPointStruct point;
    point.lat = lat;
    point.lon = lon;
    point.pointIndex = 0;

    item.points.push_back(point);

    qDebug() << "Create marker:"
             << "name =" << item.name
             << "icon =" << item.style.iconPath
             << "exists =" << QFile::exists(item.style.iconPath);

    return item;
}

MapItemStruct Position::createTextItem(double lat, double lon, const QString &text) const
{
    MapItemStruct item;
    item.name = text;
    item.type = Text;
    item.createTime = QDateTime::currentDateTimeUtc();

    GeoPointStruct point;
    point.lat = lat;
    point.lon = lon;
    point.pointIndex = 0;
    item.points.push_back(point);

    return item;
}

//--- Drawing Main ---
void Position::drawMap(QPainter &painter)
{
    if (isGdalStreamingMap) {
        drawGdalStreamingMap(painter);
        return;
    }

    if (mapImage.isNull()) {
        drawNoMapMessage(painter);
        return;
    }

    const QSizeF scaledSize(
        static_cast<double>(mapPixelWidth()) * currentZoom,
        static_cast<double>(mapPixelHeight()) * currentZoom
        );
    const QRectF targetRect(imageTopLeft(), scaledSize);
    painter.drawImage(targetRect, mapImage);
}

void Position::drawMapLayers(QPainter &painter)
{
    if (!mapLayerManager)
        return;

    const QVector<MapLayerStruct> &allLayers = mapLayerManager->layers();

    for (int i = allLayers.size() - 1; i >= 0; --i) {
        drawMapLayer(painter, allLayers[i]);
    }
}

void Position::drawMapLayer(QPainter &painter, const MapLayerStruct &layer)
{
    if (!layer.isVisible || layer.isDeleted)
        return;

    for (const MapItemStruct &item : layer.items)
        drawMapItem(painter, item);
}

void Position::drawMapItem(QPainter &painter, const MapItemStruct &item)
{
    if (!item.isVisible || item.isDeleted)
        return;

    switch (item.type) {
    case Marker: drawMarkerItem(painter, item); break;
    case Text: drawTextItem(painter, item); break;
    case Line: drawLineItem(painter, item); break;
    case Polyline: drawPolylineItem(painter, item); break;
    case Rectangle: drawRectangleItem(painter, item); break;
    case Circle: drawCircleItem(painter, item); break;
    case Ellipse: drawEllipseItem(painter, item); break;
    case Polygon: drawPolygonItem(painter, item); break;
    case Freehand: drawFreehandItem(painter, item); break;
    case Sector: drawSectorItem(painter, item); break;
    case Arc: drawArcItem(painter, item); break;
    case ImageOverlay: drawImageOverlayItem(painter, item); break;
    case Measure: drawMeasureItem(painter, item); break;
    default: break;
    }
}

void Position::drawMapDfStations(QPainter &painter)
{
    for (const MapDfStationStruct &station : dfStations)
        drawMapDfStation(painter, station);
}

void Position::drawMapDfStation(QPainter &painter, const MapDfStationStruct &station)
{
    if (!station.isVisible || !hasMapData() || !isGeoTransform)
        return;

    const QPointF imagePos = latLonToImage(station.lat, station.lon);

    if (!qIsFinite(imagePos.x()) || !qIsFinite(imagePos.y()))
        return;

    if (!isImagePointInsideMap(imagePos))
        return;

    const QPointF center = imageToWidget(imagePos);

    if (!isWidgetPointDrawable(center, 3000.0))
        return;

    painter.save();

    if (!qFuzzyCompare(station.angle, -1.0)) {
        const GeoPointStruct endGeo = destinationPointByBearing(station.lat,
                                                                station.lon,
                                                                station.angle,
                                                                dfStationLineLength);

        const QPointF endImagePos = latLonToImage(endGeo.lat, endGeo.lon);

        if (qIsFinite(endImagePos.x()) &&
            qIsFinite(endImagePos.y()) &&
            isImagePointInsideMap(endImagePos)) {

            const QPointF endPoint = imageToWidget(endImagePos);

            if (isWidgetPointDrawable(endPoint, 3000.0)) {
                QPen linePen(QColor(dfStationLineColor), 2);
                linePen.setCapStyle(Qt::RoundCap);
                painter.setPen(linePen);
                painter.setBrush(Qt::NoBrush);
                painter.drawLine(center, endPoint);
            }
        }
    }

    const int iconSize = qBound(8, dfStationIconSize, 256);

    QRectF iconRect(center.x() - iconSize / 2.0,
                    center.y() - iconSize / 2.0,
                    iconSize,
                    iconSize);

    if (isWidgetRectDrawable(iconRect, 1000.0)) {
        QPixmap icon(dfStationIconPath);

        if (!icon.isNull()) {
            painter.drawPixmap(iconRect.toRect(), icon);
        } else {
            painter.setPen(QPen(QColor(dfStationLineColor), 2));
            painter.setBrush(QColor(201, 136, 45, 180));
            painter.drawEllipse(iconRect);
        }
    }

    if (isLabelsVisible && !station.name.isEmpty()) {
        MapItemStruct labelItem;
        labelItem.style.fontSize = 10;
        labelItem.style.textColor = "#FFFFFF";
        labelItem.style.opacity = 1.0;
        labelItem.style.rotation = 0.0;

        QFont font("Arial", qMax(8, labelItem.style.fontSize), QFont::Bold);
        QFontMetrics metrics(font);

        const int textWidth = metrics.horizontalAdvance(station.name);

        const QPointF textPos(center.x() - textWidth / 2.0,
                              center.y() - iconSize / 2.0 - 8.0);

        QRectF labelRect(textPos.x() - 10.0,
                         textPos.y() - metrics.height() - 10.0,
                         textWidth + 20.0,
                         metrics.height() + 20.0);

        if (isWidgetRectDrawable(labelRect, 1000.0))
            drawTextLabel(painter, textPos, station.name, labelItem);
    }

    painter.restore();
}

//--- Drawing Items ---
void Position::drawMarkerItem(QPainter &painter, const MapItemStruct &item)
{
    if (!hasMapData() || !item.isVisible || item.isDeleted)
        return;

    if (item.points.isEmpty())
        return;

    const GeoPointStruct &pt = item.points.first();

    const QPointF imagePos = latLonToImage(pt.lat, pt.lon);

    if (!qIsFinite(imagePos.x()) || !qIsFinite(imagePos.y()))
        return;

    if (!isImagePointInsideMap(imagePos))
        return;

    const QPointF widgetPos = imageToWidget(imagePos);

    if (!qIsFinite(widgetPos.x()) || !qIsFinite(widgetPos.y()))
        return;

    drawPointMarker(painter, imagePos, item);

    // رسم Label فقط وقتی داخل viewport است
    if (isLabelsVisible && !item.name.isEmpty()) {
        QFont font("Arial", qMax(8, item.style.fontSize), QFont::Bold);
        QFontMetrics metrics(font);

        const int textWidth = metrics.horizontalAdvance(item.name);

        QPointF textPos(widgetPos.x() - textWidth / 2.0,
                        widgetPos.y() - item.style.pointSize / 2.0 - 8.0);

        QRectF labelRect(textPos.x() - 10.0,
                         textPos.y() - metrics.height() - 10.0,
                         textWidth + 20.0,
                         metrics.height() + 20.0);

        if (labelRect.intersects(rect()))
            drawTextLabel(painter, textPos, item.name, item);
    }
}

void Position::drawTextItem(QPainter &painter, const MapItemStruct &item)
{
    if (!hasMapData() || !item.isVisible || item.isDeleted)
        return;

    if (item.points.isEmpty())
        return;

    const GeoPointStruct &geoPoint = item.points.first();

    const QPointF imagePos = latLonToImage(geoPoint.lat, geoPoint.lon);

    if (!qIsFinite(imagePos.x()) || !qIsFinite(imagePos.y()))
        return;

    if (!isImagePointInsideMap(imagePos))
        return;

    const QPointF widgetPos = imageToWidget(imagePos);

    if (!isWidgetPointDrawable(widgetPos, 1000.0))
        return;

    const QString text = item.name.isEmpty() ? item.note : item.name;

    if (text.trimmed().isEmpty())
        return;

    QFont font("Arial", qMax(8, item.style.fontSize), QFont::Bold);
    QFontMetrics metrics(font);

    QRectF textRect(widgetPos.x() - 20.0,
                    widgetPos.y() - metrics.height() - 20.0,
                    metrics.horizontalAdvance(text) + 40.0,
                    metrics.height() + 40.0);

    if (!isWidgetRectDrawable(textRect, 500.0))
        return;

    drawTextLabel(painter, widgetPos, text, item);
}

void Position::drawLineItem(QPainter &painter, const MapItemStruct &item)
{
    if (!hasMapData() || !item.isVisible || item.isDeleted)
        return;

    if (item.points.size() < 2)
        return;

    const QVector<QPointF> points = pointsToWidgetPoints(item.points);

    if (points.size() < 2)
        return;

    if (!qIsFinite(points[0].x()) || !qIsFinite(points[0].y()) ||
        !qIsFinite(points[1].x()) || !qIsFinite(points[1].y())) {
        return;
    }

    QRectF lineRect(points[0], points[1]);
    lineRect = lineRect.normalized();

    if (lineRect.width() < 1.0)
        lineRect.setWidth(1.0);

    if (lineRect.height() < 1.0)
        lineRect.setHeight(1.0);

    if (!isWidgetRectDrawable(lineRect, 3000.0))
        return;

    painter.save();
    applyItemStyle(painter, item, false);
    painter.drawLine(points[0], points[1]);
    painter.restore();
}

void Position::drawPolylineItem(QPainter &painter, const MapItemStruct &item)
{
    if (!hasMapData() || !item.isVisible || item.isDeleted)
        return;

    if (item.points.size() < 2)
        return;

    const QVector<QPointF> points = pointsToWidgetPoints(item.points);

    if (points.size() < 2)
        return;

    QRectF boundingRect;
    bool hasValidPoint = false;

    for (const QPointF &point : points) {
        if (!qIsFinite(point.x()) || !qIsFinite(point.y()))
            return;

        if (!hasValidPoint) {
            boundingRect = QRectF(point, QSizeF(1.0, 1.0));
            hasValidPoint = true;
        } else {
            boundingRect = boundingRect.united(QRectF(point, QSizeF(1.0, 1.0)));
        }
    }

    if (!hasValidPoint || !isWidgetRectDrawable(boundingRect, 3000.0))
        return;

    painter.save();
    applyItemStyle(painter, item, false);

    for (int i = 1; i < points.size(); ++i) {
        QRectF segmentRect(points[i - 1], points[i]);
        segmentRect = segmentRect.normalized();

        if (segmentRect.width() < 1.0)
            segmentRect.setWidth(1.0);

        if (segmentRect.height() < 1.0)
            segmentRect.setHeight(1.0);

        if (isWidgetRectDrawable(segmentRect, 3000.0))
            painter.drawLine(points[i - 1], points[i]);
    }

    painter.restore();
}

void Position::drawRectangleItem(QPainter &painter, const MapItemStruct &item)
{
    if (!hasMapData() || !item.isVisible || item.isDeleted)
        return;

    if (item.points.size() < 2)
        return;

    const QPointF a = imageToWidget(latLonToImage(item.points[0].lat, item.points[0].lon));
    const QPointF b = imageToWidget(latLonToImage(item.points[1].lat, item.points[1].lon));

    if (!qIsFinite(a.x()) || !qIsFinite(a.y()) ||
        !qIsFinite(b.x()) || !qIsFinite(b.y())) {
        return;
    }

    QRectF rectangle(a, b);
    rectangle = rectangle.normalized();

    if (!isWidgetRectDrawable(rectangle, 3000.0))
        return;

    painter.save();
    applyItemStyle(painter, item, true);
    painter.drawRect(rectangle);
    painter.restore();
}

void Position::drawCircleItem(QPainter &painter, const MapItemStruct &item)
{
    if (!hasMapData() || !item.isVisible || item.isDeleted)
        return;

    if (item.points.size() < 2)
        return;

    const QPointF center = imageToWidget(latLonToImage(item.points[0].lat, item.points[0].lon));
    const QPointF edge = imageToWidget(latLonToImage(item.points[1].lat, item.points[1].lon));

    if (!qIsFinite(center.x()) || !qIsFinite(center.y()) ||
        !qIsFinite(edge.x()) || !qIsFinite(edge.y())) {
        return;
    }

    const double radius = QLineF(center, edge).length();

    if (!qIsFinite(radius) || radius <= 0.0)
        return;

    if (radius > 100000.0)
        return;

    QRectF circleRect(center.x() - radius,
                      center.y() - radius,
                      radius * 2.0,
                      radius * 2.0);

    if (!isWidgetRectDrawable(circleRect, 3000.0))
        return;

    painter.save();
    applyItemStyle(painter, item, true);
    painter.drawEllipse(center, radius, radius);
    painter.restore();
}

void Position::drawEllipseItem(QPainter &painter, const MapItemStruct &item)
{
    if (!hasMapData() || !item.isVisible || item.isDeleted)
        return;

    if (item.points.size() < 2)
        return;

    const QPointF a = imageToWidget(latLonToImage(item.points[0].lat, item.points[0].lon));
    const QPointF b = imageToWidget(latLonToImage(item.points[1].lat, item.points[1].lon));

    if (!qIsFinite(a.x()) || !qIsFinite(a.y()) ||
        !qIsFinite(b.x()) || !qIsFinite(b.y())) {
        return;
    }

    QRectF ellipseRect(a, b);
    ellipseRect = ellipseRect.normalized();

    if (!isWidgetRectDrawable(ellipseRect, 3000.0))
        return;

    if (ellipseRect.width() > 200000.0 || ellipseRect.height() > 200000.0)
        return;

    painter.save();
    applyItemStyle(painter, item, true);
    painter.drawEllipse(ellipseRect);
    painter.restore();
}

void Position::drawPolygonItem(QPainter &painter, const MapItemStruct &item)
{
    if (!hasMapData() || !item.isVisible || item.isDeleted)
        return;

    if (item.points.size() < 3)
        return;

    QPolygonF polygon = pointsToWidgetPolygon(item.points);

    if (polygon.size() < 3)
        return;

    QRectF boundingRect;

    for (int i = 0; i < polygon.size(); ++i) {
        const QPointF point = polygon[i];

        if (!qIsFinite(point.x()) || !qIsFinite(point.y()))
            return;

        if (i == 0)
            boundingRect = QRectF(point, QSizeF(1.0, 1.0));
        else
            boundingRect = boundingRect.united(QRectF(point, QSizeF(1.0, 1.0)));
    }

    if (!isWidgetRectDrawable(boundingRect, 3000.0))
        return;

    painter.save();
    applyItemStyle(painter, item, true);
    painter.drawPolygon(polygon);
    painter.restore();
}

void Position::drawFreehandItem(QPainter &painter, const MapItemStruct &item)
{
    drawPolylineItem(painter, item);
}

void Position::drawSectorItem(QPainter &painter, const MapItemStruct &item)
{
    drawCircleItem(painter, item);
}

void Position::drawArcItem(QPainter &painter, const MapItemStruct &item)
{
    drawLineItem(painter, item);
}

void Position::drawImageOverlayItem(QPainter &painter, const MapItemStruct &item)
{
    if (!hasMapData() || !item.isVisible || item.isDeleted)
        return;

    if (item.points.isEmpty() || item.style.iconPath.trimmed().isEmpty())
        return;

    QImage icon(item.style.iconPath);

    if (icon.isNull())
        return;

    const QPointF imagePos = latLonToImage(item.points.first().lat, item.points.first().lon);

    if (!qIsFinite(imagePos.x()) || !qIsFinite(imagePos.y()))
        return;

    if (!isImagePointInsideMap(imagePos))
        return;

    const QPointF point = imageToWidget(imagePos);

    if (!isWidgetPointDrawable(point, 3000.0))
        return;

    const int maxOverlaySide = 2048;

    if (icon.width() > maxOverlaySide || icon.height() > maxOverlaySide) {
        icon = icon.scaled(maxOverlaySide,
                           maxOverlaySide,
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);
    }

    QRectF targetRect(point.x() - icon.width() / 2.0,
                      point.y() - icon.height() / 2.0,
                      icon.width(),
                      icon.height());

    if (!isWidgetRectDrawable(targetRect, 3000.0))
        return;

    painter.save();
    painter.setOpacity(qBound(0.0, item.style.opacity, 1.0));
    painter.drawImage(targetRect.topLeft(), icon);
    painter.restore();
}

void Position::drawMeasureItem(QPainter &painter, const MapItemStruct &item)
{
    drawPolylineItem(painter, item);
}

void Position::drawMeasurePin(QPainter &painter, const QPointF &bottomCenter) const
{
    if (!qIsFinite(bottomCenter.x()) || !qIsFinite(bottomCenter.y()))
        return;

    if (!isWidgetPointDrawable(bottomCenter, 1000.0))
        return;

    QPixmap pin(":/Res/Images/Map/Pin.png");

    const int pinWidth = 32;
    const int pinHeight = 32;

    QRectF targetRect(bottomCenter.x() - pinWidth / 2.0,
                      bottomCenter.y() - pinHeight,
                      pinWidth,
                      pinHeight);

    if (!isWidgetRectDrawable(targetRect, 1000.0))
        return;

    painter.save();

    if (pin.isNull()) {
        painter.setPen(QPen(Qt::red, 2));
        painter.setBrush(Qt::red);
        painter.drawEllipse(bottomCenter, 5, 5);
    } else {
        painter.drawPixmap(targetRect.toRect(), pin);
    }

    painter.restore();
}

void Position::drawTextWithWhiteBackground(QPainter &painter,
                                           const QPointF &pos,
                                           const QString &text) const
{
    if (text.trimmed().isEmpty())
        return;

    if (!qIsFinite(pos.x()) || !qIsFinite(pos.y()))
        return;

    if (!isWidgetPointDrawable(pos, 1000.0))
        return;

    painter.save();

    QFont font("Arial", 10, QFont::Bold);
    painter.setFont(font);

    QFontMetrics metrics(font);
    QRect textRect = metrics.boundingRect(text);

    QRectF backgroundRect(pos.x(),
                          pos.y() - textRect.height(),
                          textRect.width() + 12,
                          textRect.height() + 8);

    if (!isWidgetRectDrawable(backgroundRect, 1000.0)) {
        painter.restore();
        return;
    }

    painter.setPen(QPen(QColor(80, 80, 80), 1));
    painter.setBrush(QColor(255, 255, 255, 230));
    painter.drawRoundedRect(backgroundRect, 4, 4);

    painter.setPen(QColor(0, 0, 0));
    painter.drawText(backgroundRect.adjusted(6, 2, -6, -2),
                     Qt::AlignCenter,
                     text);

    painter.restore();
}

//--- Drawing Overlays ---
void Position::drawTemporaryShape(QPainter &painter)
{
    if (!isDrawing || temporaryPoints.isEmpty())
        return;

    QVector<GeoPointStruct> previewPoints = temporaryPoints;

    const bool needMousePreview =
        currentTool == MapToolLine ||
        currentTool == MapToolPolyline ||
        currentTool == MapToolRectangle ||
        currentTool == MapToolCircle ||
        currentTool == MapToolEllipse ||
        currentTool == MapToolPolygon;

    if (needMousePreview && isImagePointInsideMap(temporaryCurrentImagePos)) {
        const QPointF geo = imageToLatLon(temporaryCurrentImagePos.x(), temporaryCurrentImagePos.y());

        GeoPointStruct point;
        point.lat = geo.y();
        point.lon = geo.x();
        point.pointIndex = previewPoints.size();

        if (currentTool == MapToolLine ||
            currentTool == MapToolRectangle ||
            currentTool == MapToolCircle ||
            currentTool == MapToolEllipse) {

            if (previewPoints.size() == 1)
                previewPoints.push_back(point);

        } else {
            previewPoints.push_back(point);
        }
    }

    MapItemStruct item;
    item.eID = -1;
    item.name = "Preview";
    item.note = "";
    item.isVisible = true;
    item.isDeleted = false;
    item.createTime = QDateTime::currentDateTimeUtc();
    item.points = previewPoints;

    item.style.strokeColor = "#FF0000";
    item.style.fillColor = "#AAFF0000";
    item.style.textColor = "#FFFFFF";
    item.style.strokeWidth = 2;
    item.style.pointSize = 10;
    item.style.fontSize = 12;
    item.style.opacity = 1.0;

    switch (currentTool) {
    case MapToolMarker:
        item.type = Marker;
        break;
    case MapToolText:
        item.type = Text;
        break;
    case MapToolLine:
        item.type = Line;
        break;
    case MapToolPolyline:
        item.type = Polyline;
        break;
    case MapToolRectangle:
        item.type = Rectangle;
        break;
    case MapToolCircle:
        item.type = Circle;
        break;
    case MapToolEllipse:
        item.type = Ellipse;
        break;
    case MapToolPolygon:
        item.type = Polygon;
        break;
    case MapToolFreehand:
        item.type = Freehand;
        break;
    default:
        item.type = Unknown;
        break;
    }

    drawMapItem(painter, item);
}

void Position::drawZoomBox(QPainter &painter)
{
    if (!isZoomBoxDrawing)
        return;

    painter.save();
    QPen pen(QColor(0, 0, 255, 220), 2, Qt::DashLine);
    painter.setPen(pen);
    painter.setBrush(QColor(255, 255, 0, 35));
    painter.drawRect(QRectF(zoomBoxStartWidgetPos, zoomBoxEndWidgetPos).normalized());
    painter.restore();
}

void Position::drawDistanceOverlay(QPainter &painter)
{
    if (!isDistanceMode || distancePoints.isEmpty())
        return;

    painter.save();

    QPen pen(QColor(255, 220, 0, 240), 2);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    QVector<QPointF> widgetPoints;

    for (const GeoPointStruct &point : distancePoints) {
        QPointF imagePoint = latLonToImage(point.lat, point.lon);
        widgetPoints.push_back(imageToWidget(imagePoint));
    }

    double totalDistance = 0.0;

    for (int i = 0; i < widgetPoints.size(); ++i) {
        drawMeasurePin(painter, widgetPoints[i]);
    }

    for (int i = 1; i < widgetPoints.size(); ++i) {
        QPointF a = widgetPoints[i - 1];
        QPointF b = widgetPoints[i];

        painter.setPen(QPen(QColor(255, 220, 0, 240), 2));
        painter.drawLine(a, b);

        double segment = distanceBetweenGeoPoints(distancePoints[i - 1], distancePoints[i]);
        totalDistance += segment;

        QPointF midPoint = (a + b) / 2.0;

        drawTextWithWhiteBackground(painter,
                                    midPoint + QPointF(8, -8),
                                    formatDistance(segment));
    }

    if (isDistanceDrawing &&
        (isDistanceChainMode || distancePoints.size() < 2) &&
        !distancePoints.isEmpty() &&
        isImagePointInsideMap(distanceCurrentImagePos)) {

        QPointF last = widgetPoints.last();
        QPointF current = imageToWidget(distanceCurrentImagePos);

        painter.setPen(QPen(QColor(255, 220, 0, 220), 2, Qt::DashLine));
        painter.drawLine(last, current);

        QPointF geo = imageToLatLon(distanceCurrentImagePos.x(), distanceCurrentImagePos.y());

        GeoPointStruct currentGeo;
        currentGeo.lon = geo.x();
        currentGeo.lat = geo.y();

        double previewDistance = distanceBetweenGeoPoints(distancePoints.last(), currentGeo);

        drawTextWithWhiteBackground(painter,
                                    (last + current) / 2.0 + QPointF(8, -8),
                                    formatDistance(previewDistance));
    }

    // if (distancePoints.size() > 1) {
    //     drawTextWithWhiteBackground(painter,
    //                                 QPointF(20, height() - 90),
    //                                 "Total: " + formatDistance(totalDistance));
    // }

    painter.restore();
}

void Position::drawGridOverlay(QPainter &painter)
{
    if (!isGridVisible || !hasMapData() || !isGeoTransform)
        return;

    if (width() <= 0 || height() <= 0)
        return;

    const QPointF topLeftImage = widgetToImage(QPointF(0, 0));
    const QPointF bottomRightImage = widgetToImage(QPointF(width(), height()));

    if (!qIsFinite(topLeftImage.x()) || !qIsFinite(topLeftImage.y()) ||
        !qIsFinite(bottomRightImage.x()) || !qIsFinite(bottomRightImage.y())) {
        return;
    }

    const QPointF geoTopLeft = imageToLatLon(topLeftImage.x(), topLeftImage.y());
    const QPointF geoBottomRight = imageToLatLon(bottomRightImage.x(), bottomRightImage.y());

    if (!qIsFinite(geoTopLeft.x()) || !qIsFinite(geoTopLeft.y()) ||
        !qIsFinite(geoBottomRight.x()) || !qIsFinite(geoBottomRight.y())) {
        return;
    }

    const double lonMin = qMin(geoTopLeft.x(), geoBottomRight.x());
    const double lonMax = qMax(geoTopLeft.x(), geoBottomRight.x());
    const double latMin = qMin(geoTopLeft.y(), geoBottomRight.y());
    const double latMax = qMax(geoTopLeft.y(), geoBottomRight.y());

    const double visibleLon = qMax(0.000001, lonMax - lonMin);
    const double visibleLat = qMax(0.000001, latMax - latMin);

    const double roughLonStep = visibleLon / qMax(1.0, width() / gridMinimumPixelSpacing);
    const double roughLatStep = visibleLat / qMax(1.0, height() / gridMinimumPixelSpacing);

    auto niceStep = [](double roughStep) -> double {
        if (!qIsFinite(roughStep) || roughStep <= 0.0)
            return 1.0;

        const double pow10 = qPow(10.0, qFloor(qLn(roughStep) / qLn(10.0)));
        const double ratio = roughStep / pow10;

        if (ratio > 5.0)
            return 10.0 * pow10;
        if (ratio > 2.0)
            return 5.0 * pow10;
        if (ratio > 1.0)
            return 2.0 * pow10;

        return pow10;
    };

    const double lonStep = niceStep(roughLonStep);
    const double latStep = niceStep(roughLatStep);

    if (lonStep <= 0.0 || latStep <= 0.0)
        return;

    painter.save();

    QPen pen(gridLineColor, gridLineWidth, Qt::DashLine);
    painter.setPen(pen);
    painter.setFont(QFont("Arial", 8, QFont::Bold));

    const double firstLon = qFloor(lonMin / lonStep) * lonStep;

    for (double lon = firstLon; lon <= lonMax; lon += lonStep) {
        const QPointF a = imageToWidget(latLonToImage(latMin, lon));
        const QPointF b = imageToWidget(latLonToImage(latMax, lon));

        if (!qIsFinite(a.x()) || !qIsFinite(a.y()) ||
            !qIsFinite(b.x()) || !qIsFinite(b.y())) {
            continue;
        }

        QRectF lineRect(a, b);
        lineRect = lineRect.normalized();

        if (lineRect.width() < 1.0)
            lineRect.setWidth(1.0);
        if (lineRect.height() < 1.0)
            lineRect.setHeight(1.0);

        if (isWidgetRectDrawable(lineRect, 1000.0)) {
            painter.setPen(pen);
            painter.drawLine(a, b);

            if (isGridLabelVisible && isWidgetPointDrawable(a, 1000.0)) {
                painter.setPen(gridTextColor);
                painter.drawText(QPointF(a.x() + 4.0, 16.0), QString::number(lon, 'f', 4));
            }
        }
    }

    const double firstLat = qFloor(latMin / latStep) * latStep;

    for (double lat = firstLat; lat <= latMax; lat += latStep) {
        const QPointF a = imageToWidget(latLonToImage(lat, lonMin));
        const QPointF b = imageToWidget(latLonToImage(lat, lonMax));

        if (!qIsFinite(a.x()) || !qIsFinite(a.y()) ||
            !qIsFinite(b.x()) || !qIsFinite(b.y())) {
            continue;
        }

        QRectF lineRect(a, b);
        lineRect = lineRect.normalized();

        if (lineRect.width() < 1.0)
            lineRect.setWidth(1.0);
        if (lineRect.height() < 1.0)
            lineRect.setHeight(1.0);

        if (isWidgetRectDrawable(lineRect, 1000.0)) {
            painter.setPen(pen);
            painter.drawLine(a, b);

            if (isGridLabelVisible && isWidgetPointDrawable(a, 1000.0)) {
                painter.setPen(gridTextColor);
                painter.drawText(QPointF(8.0, a.y() - 4.0), QString::number(lat, 'f', 4));
            }
        }
    }

    painter.restore();
}

void Position::drawScaleBar(QPainter &painter)
{
    if (!isScaleBarVisible || !isGeoTransform || !hasMapData())
        return;

    if (width() <= 0 || height() <= 0)
        return;

    const QPointF start(40.0, height() - 50.0);
    const QPointF end(160.0, height() - 50.0);

    if (!isWidgetPointDrawable(start, 1000.0) ||
        !isWidgetPointDrawable(end, 1000.0)) {
        return;
    }

    const QPointF p1 = widgetToImage(start);
    const QPointF p2 = widgetToImage(end);

    if (!qIsFinite(p1.x()) || !qIsFinite(p1.y()) ||
        !qIsFinite(p2.x()) || !qIsFinite(p2.y())) {
        return;
    }

    const QPointF g1 = imageToLatLon(p1.x(), p1.y());
    const QPointF g2 = imageToLatLon(p2.x(), p2.y());

    if (!qIsFinite(g1.x()) || !qIsFinite(g1.y()) ||
        !qIsFinite(g2.x()) || !qIsFinite(g2.y())) {
        return;
    }

    const double distance = calculateDistance(g1.y(), g1.x(), g2.y(), g2.x());

    if (!qIsFinite(distance) || distance <= 0.0)
        return;

    const QString text = formatDistance(distance);

    painter.save();

    QPen pen(QColor(255, 255, 255), 3);
    pen.setCapStyle(Qt::RoundCap);

    painter.setPen(QPen(QColor(0, 0, 0), 5));
    painter.drawLine(start, end);

    painter.setPen(pen);
    painter.drawLine(start, end);

    painter.setPen(QPen(QColor(255, 255, 255), 2));
    painter.drawLine(QPointF(start.x(), start.y() - 6.0), QPointF(start.x(), start.y() + 6.0));
    painter.drawLine(QPointF(end.x(), end.y() - 6.0), QPointF(end.x(), end.y() + 6.0));

    QFont font("Arial", 9, QFont::Bold);
    painter.setFont(font);

    QFontMetrics metrics(font);
    const int textWidth = metrics.horizontalAdvance(text);

    QPointF textPos((start.x() + end.x()) / 2.0 - textWidth / 2.0,
                    start.y() - 10.0);

    painter.setPen(QColor(0, 0, 0));
    painter.drawText(textPos + QPointF(1.0, 1.0), text);

    painter.setPen(QColor(255, 255, 255));
    painter.drawText(textPos, text);

    painter.restore();
}

void Position::drawNorthArrow(QPainter &painter)
{
    if (!isNorthArrowVisible)
        return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QPointF top(35, 25);
    const QPointF bottom(35, 65);

    QPolygonF arrowHead;
    arrowHead << top
              << QPointF(27, 42)
              << QPointF(33, 39)
              << QPointF(35, 65)
              << QPointF(37, 39)
              << QPointF(43, 42);

    painter.setPen(QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::black);
    painter.drawPolygon(arrowHead);

    painter.setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::white);
    painter.drawPolygon(arrowHead);

    drawOutlinedText(painter, QPointF(30, 82), "N");

    painter.restore();
}

void Position::drawNoMapMessage(QPainter &painter)
{
    painter.save();
    painter.fillRect(rect(), QColor(60, 60, 60));
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, "No map loaded");
    painter.restore();
}

//--- Drawing Utility ---
void Position::applyItemStyle(QPainter &painter,
                              const MapItemStruct &item,
                              bool useFill,
                              bool isTemporary) const
{
    QColor stroke(item.style.strokeColor);
    QColor fill(item.style.fillColor);

    stroke.setAlphaF(qBound(0.0, item.style.opacity, 1.0));

    if (isTemporary)
        fill.setAlphaF(qBound(0.0, item.style.opacity * 0.35, 1.0));
    else
        fill.setAlphaF(qBound(0.0, item.style.opacity, 1.0));

    painter.setPen(QPen(stroke, qMax(1, item.style.strokeWidth)));
    painter.setBrush(useFill ? QBrush(fill) : Qt::NoBrush);
}

void Position::drawPointMarker(QPainter &painter,
                               const QPointF &pos,
                               const MapItemStruct &item) const
{
    if (!hasMapData())
        return;

    if (!qIsFinite(pos.x()) || !qIsFinite(pos.y()))
        return;

    const QPointF widgetPos = imageToWidget(pos);

    if (!qIsFinite(widgetPos.x()) || !qIsFinite(widgetPos.y()))
        return;

    const int iconSize = qBound(8, item.style.pointSize, 256);

    const QRectF extendedViewport(
        -iconSize - 200.0,
        -iconSize - 200.0,
        width() + 2.0 * iconSize + 400.0,
        height() + 2.0 * iconSize + 400.0
        );

    if (!extendedViewport.contains(widgetPos))
        return;

    const double left = widgetPos.x() - iconSize / 2.0;
    const double top = widgetPos.y() - iconSize / 2.0;

    if (left < -100000.0 || top < -100000.0 ||
        left > width() + 100000.0 || top > height() + 100000.0)
        return;

    const QRect drawRect(
        qRound(left),
        qRound(top),
        iconSize,
        iconSize
        );

    QPixmap iconPixmap(resolveMarkerIconPath(item));

    painter.save();

    if (!iconPixmap.isNull()) {
        painter.drawPixmap(drawRect, iconPixmap);
    } else {
        QColor color(item.style.strokeColor);
        painter.setBrush(color);
        painter.setPen(QPen(color, 2));
        painter.drawEllipse(drawRect);
    }

    painter.restore();
}

void Position::drawTextLabel(QPainter &painter,
                             const QPointF &pos,
                             const QString &text,
                             const MapItemStruct &item) const
{
    if (text.isEmpty())
        return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    QFont font("Arial", qMax(8, item.style.fontSize), QFont::Bold);

    QPainterPath path;
    path.addText(pos, font, text);

    QColor outlineColor(Qt::black);
    outlineColor.setAlphaF(qBound(0.0, item.style.opacity, 1.0));

    QColor textColor(item.style.textColor);
    textColor.setAlphaF(qBound(0.0, item.style.opacity, 1.0));

    painter.translate(QPointF(0, 0));
    painter.rotate(item.style.rotation);

    painter.setPen(QPen(outlineColor, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);

    painter.setPen(Qt::NoPen);
    painter.setBrush(textColor);
    painter.drawPath(path);

    painter.restore();
}

void Position::drawOutlinedText(QPainter &painter,
                                const QPointF &pos,
                                const QString &text) const
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    QFont font("Arial", 10, QFont::Bold);

    QPainterPath path;
    path.addText(pos, font, text);

    painter.setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawPath(path);

    painter.restore();
}

QString Position::resolveMarkerIconPath(const MapItemStruct &item) const
{
    QString path = item.style.iconPath.trimmed();

    if (!path.isEmpty() && QFile::exists(path))
        return path;

    bool okNumber = false;
    int markerNumber = item.name.trimmed().toInt(&okNumber);

    if (okNumber && markerNumber >= 1 && markerNumber <= 15) {
        path = QString(":/Res/Images/Map/%1.png").arg(markerNumber);
        if (QFile::exists(path))
            return path;
    }

    path = ":/Res/Images/Map/Marker.png";
    if (QFile::exists(path))
        return path;

    path = ":/Res/Images/Map/Marker.png";
    if (QFile::exists(path))
        return path;

    return QString();
}

QPolygonF Position::pointsToWidgetPolygon(const QVector<GeoPointStruct> &points) const
{
    QPolygonF polygon;

    for (const GeoPointStruct &point : points) {
        const QPointF imagePos = latLonToImage(point.lat, point.lon);

        if (!qIsFinite(imagePos.x()) || !qIsFinite(imagePos.y()))
            continue;

        const QPointF widgetPos = imageToWidget(imagePos);

        if (!qIsFinite(widgetPos.x()) || !qIsFinite(widgetPos.y()))
            continue;

        polygon << widgetPos;
    }

    return polygon;
}

QVector<QPointF> Position::pointsToWidgetPoints(const QVector<GeoPointStruct> &points) const
{
    QVector<QPointF> result;
    result.reserve(points.size());

    for (const GeoPointStruct &point : points) {
        const QPointF imagePos = latLonToImage(point.lat, point.lon);

        if (!qIsFinite(imagePos.x()) || !qIsFinite(imagePos.y()))
            continue;

        const QPointF widgetPos = imageToWidget(imagePos);

        if (!qIsFinite(widgetPos.x()) || !qIsFinite(widgetPos.y()))
            continue;

        result.push_back(widgetPos);
    }

    return result;
}

double Position::distanceBetweenGeoPoints(const GeoPointStruct &a, const GeoPointStruct &b) const
{
    return calculateDistance(a.lat, a.lon, b.lat, b.lon);
}

double Position::distanceBetweenImagePoints(const QPointF &a, const QPointF &b) const
{
    const QPointF geoA = imageToLatLon(a.x(), a.y());
    const QPointF geoB = imageToLatLon(b.x(), b.y());
    return calculateDistance(geoA.y(), geoA.x(), geoB.y(), geoB.x());
}

//--- Import Helpers ---
bool Position::importLayerFromFile(const QString &filePath)
{
    const QString suffix = QFileInfo(filePath).suffix().toLower();

    if (suffix == "geojson" || suffix == "json")
        return importGeoJsonLayer(filePath);

    if (suffix == "csv")
        return importCsvPointLayer(filePath);

    return false;
}

bool Position::importGeoJsonLayer(const QString &filePath)
{
    if (!mapLayerManager)
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (!document.isObject())
        return false;

    const QJsonObject root = document.object();
    const QJsonArray features = root.value("features").toArray();

    if (features.isEmpty())
        return false;

    mapLayerManager->addLayer(QFileInfo(filePath).baseName());

    for (const QJsonValue &featureValue : features) {
        const QJsonObject feature = featureValue.toObject();
        const QJsonObject geometry = feature.value("geometry").toObject();
        const QString type = geometry.value("type").toString();
        const QJsonValue coordinates = geometry.value("coordinates");

        MapItemStruct item;
        item.name = feature.value("properties").toObject().value("name").toString(type);
        item.createTime = QDateTime::currentDateTimeUtc();

        if (type == "Point") {
            const QJsonArray c = coordinates.toArray();
            if (c.size() >= 2) {
                item.type = Marker;
                item.points.push_back({c[1].toDouble(), c[0].toDouble(), 0});
                mapLayerManager->addItemToActiveLayer(item);
            }
        } else if (type == "LineString") {
            item.type = Polyline;
            const QJsonArray arr = coordinates.toArray();
            for (int i = 0; i < arr.size(); ++i) {
                const QJsonArray c = arr[i].toArray();
                if (c.size() >= 2)
                    item.points.push_back({c[1].toDouble(), c[0].toDouble(), i});
            }
            if (item.points.size() >= 2)
                mapLayerManager->addItemToActiveLayer(item);
        } else if (type == "Polygon") {
            item.type = Polygon;
            const QJsonArray rings = coordinates.toArray();
            if (!rings.isEmpty()) {
                const QJsonArray ring = rings.first().toArray();
                for (int i = 0; i < ring.size(); ++i) {
                    const QJsonArray c = ring[i].toArray();
                    if (c.size() >= 2)
                        item.points.push_back({c[1].toDouble(), c[0].toDouble(), i});
                }
            }
            if (item.points.size() >= 3)
                mapLayerManager->addItemToActiveLayer(item);
        }
    }

    return true;
}

bool Position::importCsvPointLayer(const QString &filePath)
{
    if (!mapLayerManager)
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    mapLayerManager->addLayer(QFileInfo(filePath).baseName());

    QTextStream stream(&file);
    int row = 0;

    while (!stream.atEnd()) {
        const QString line = stream.readLine().trimmed();
        if (line.isEmpty())
            continue;

        const QStringList parts = line.split(',', Qt::SkipEmptyParts);
        if (parts.size() < 2)
            continue;

        bool okLat = false;
        bool okLon = false;
        const double lat = parts[0].trimmed().toDouble(&okLat);
        const double lon = parts[1].trimmed().toDouble(&okLon);

        if (!okLat || !okLon)
            continue;

        MapItemStruct item = createMarkerItem(lat, lon,
                                              parts.size() >= 3 ? parts[2].trimmed() : QString("Point %1").arg(row + 1));
        mapLayerManager->addItemToActiveLayer(item);
        ++row;
    }

    return row > 0;
}

//--- Slots ---
bool Position::loadMapFromFileBrowser()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open Map",
        QString(),
        "Raster Files (*.tif *.tiff *.jpg *.jpeg *.png *.bmp);;All Files (*.*)");

    if (filePath.isEmpty())
        return false;

    return loadMap(filePath);
}

bool Position::importLayerFromFileBrowser()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        "Import Layer",
        QString(),
        "Layer Files (*.geojson *.json *.csv);;All Files (*.*)");

    if (filePath.isEmpty())
        return false;

    return importLayerFromFile(filePath);
}

void Position::slotThemeChange()
{
    if (botMenu) {
        style = "QFrame {";
        style += "background-color: " + gVar->getClrBackground() + ";";
        style += "border-radius: 8px;";
        style += "}";
        botMenu->setStyleSheet(style);
    }

    if (infoBox) {
        style = "QFrame {";
        style += "background-color: " + gVar->getClrBackground() + ";";
        style += "border-radius: 8px;";
        style += "}";
        style += "QLabel {";
        style += "color: " + gVar->getClrPrimary() + ";";
        style += "background: transparent;";
        style += "font-size: 12px;";
        style += "font-weight: bold;";
        style += "}";
        infoBox->setStyleSheet(style);
    }

    style = "QMenu { padding: 8px; border: 1px solid #2c3e50; border-radius: 8px; background-color: #1f2d3a; }";
    style += "QMenu::item { padding: 8px; margin: 0px; color: white; background-color: transparent; border-radius: 4px; }";
    style += "QMenu::item:selected { background-color: #2e4053; }";
    style += "QMenu::separator { height: 1px; background: #34495e; margin: 4px 8px; }";

    const QList<QMenu*> menus = {contextMenu, mapMenu, importMenu, markerMenu, textMenu, lineMenu, areaMenu, distanceMenu, gridMenu};
    for (QMenu *menu : menus) {
        if (menu)
            menu->setStyleSheet(style);
    }

    update();
}

void Position::tickPfTimer()
{
    if (!isPfMode)
        return;

    const qint64 sec = pfElapsed.elapsed() / 1000;
    const int hour = sec / 3600;
    const int min = (sec % 3600) / 60;
    const int remSec = sec % 60;

    lblTargetTime->setText(QString("%1:%2:%3")
                               .arg(hour, 3, 10, QChar('0'))
                               .arg(min, 2, 10, QChar('0'))
                               .arg(remSec, 2, 10, QChar('0')));
}

void Position::clickBtnPf()
{
    isPfMode = !isPfMode;

    lblTargetTitle->setVisible(isPfMode);
    lblTargetTimeTitle->setVisible(isPfMode);
    lblTargetTime->setVisible(isPfMode);
    lblTargetLatTitle->setVisible(isPfMode);
    lblTargetLat->setVisible(isPfMode);
    lblTargetLonTitle->setVisible(isPfMode);
    lblTargetLon->setVisible(isPfMode);

    if (isPfMode) {
        pfElapsed.restart();
        lblTargetTime->setText("000:00:00");
        pfTimer->start();
    } else {
        pfTimer->stop();
        lblTargetTime->setText("000:00:00");
    }

    if (infoBox)
        infoBox->setVisible(isMouseInfoVisible || isPfMode);

    updateButtonStates();
    updateInfoBoxGeometry();
}

void Position::clickBtnMap()
{
    btnMap->setStyle(false);
    if (mapMenu)
        mapMenu->exec(btnMap->mapToGlobal(QPoint(0, -mapMenu->sizeHint().height())));
}

void Position::clickBtnImport()
{
    btnImport->setStyle(false);
    if (importMenu)
        importMenu->exec(btnImport->mapToGlobal(QPoint(0, -importMenu->sizeHint().height())));
}

void Position::clickBtnPan()
{
    if (currentTool == MapToolPan) {
        cancelCurrentTool();
        return;
    }

    setToolMode(MapToolPan);
}

void Position::clickBtnZoomBox()
{
    if (currentTool == MapToolZoomBox) {
        cancelCurrentTool();
        return;
    }

    setToolMode(MapToolZoomBox);
}

void Position::clickBtnMarker()
{
    if (currentTool == MapToolMarker) {
        cancelCurrentTool();
        return;
    }

    if (markerMenu) {
        markerMenu->exec(btnMarker->mapToGlobal(QPoint(0, -markerMenu->sizeHint().height())));
    }
}

void Position::clickBtnText()
{
    if (currentTool == MapToolText) {
        cancelCurrentTool();
        return;
    }

    if (textMenu) {
        textMenu->exec(btnText->mapToGlobal(QPoint(0, -textMenu->sizeHint().height())));
    }
}

void Position::clickBtnLine()
{
    if (currentTool == MapToolLine || currentTool == MapToolPolyline) {
        cancelCurrentTool();
        return;
    }
    if (lineMenu) {
        lineMenu->exec(btnLine->mapToGlobal(QPoint(0, -lineMenu->sizeHint().height())));
    }
}

void Position::clickBtnArea()
{
    if (currentTool == MapToolRectangle ||
        currentTool == MapToolCircle ||
        currentTool == MapToolEllipse ||
        currentTool == MapToolPolygon ||
        currentTool == MapToolFreehand) {
        cancelCurrentTool();
        return;
    }
    if (areaMenu) {
        areaMenu->exec(btnArea->mapToGlobal(QPoint(0, -areaMenu->sizeHint().height())));
    }
}

void Position::clickBtnDistance()
{
    if (currentTool == MapToolDistance) {
        cancelCurrentTool();
        return;
    }
    if (distanceMenu) {
        distanceMenu->exec(btnDistance->mapToGlobal(QPoint(0, -distanceMenu->sizeHint().height())));
    }
}

void Position::clickBtnGrid()
{
    setGridVisible(!isGridVisible);
    // if (isGridVisible) {
    //     setGridVisible(false);
    //     return;
    // }
    // if (gridMenu) {
    //     gridMenu->exec(btnGrid->mapToGlobal(QPoint(0, -gridMenu->sizeHint().height())));
    // }
}

void Position::clickBtnInfo()
{
    setMouseInfoVisible(!isMouseInfoVisible);
    updateButtonStates();
}

void Position::clickBtnFit()
{
    fitMap();
}

void Position::actionLoadMap()
{
    loadMapFromFileBrowser();
}

void Position::actionLoadDefaultMap()
{
    loadDefaultMap();
}

void Position::actionImportGeoJson()
{
    const QString filePath = QFileDialog::getOpenFileName(this, "Import GeoJSON", QString(), "GeoJSON (*.geojson *.json)");
    if (!filePath.isEmpty())
        importGeoJsonLayer(filePath);
}

void Position::actionImportCsvPoints()
{
    const QString filePath = QFileDialog::getOpenFileName(this, "Import CSV Points", QString(), "CSV (*.csv)");
    if (!filePath.isEmpty())
        importCsvPointLayer(filePath);
}

void Position::actionDrawMarker()
{
    setToolMode(MapToolMarker);
}

void Position::actionDrawText()
{
    setToolMode(MapToolText);
}

void Position::actionDrawLine()
{
    setToolMode(MapToolLine);
}

void Position::actionDrawPolyline()
{
    setToolMode(MapToolPolyline);
}

void Position::actionDrawRectangle()
{
    setToolMode(MapToolRectangle);
}

void Position::actionDrawCircle()
{
    setToolMode(MapToolCircle);
}

void Position::actionDrawEllipse()
{
    setToolMode(MapToolEllipse);
}

void Position::actionDrawPolygon()
{
    setToolMode(MapToolPolygon);
}

void Position::actionDrawFreehand()
{
    setToolMode(MapToolFreehand);
}

void Position::actionDistanceSingle()
{
    isDistanceChainMode = false;
    resetDistance();
    setToolMode(MapToolDistance);
}

void Position::actionDistanceChain()
{
    isDistanceChainMode = true;
    resetDistance();
    setToolMode(MapToolDistance);
}

void Position::actionToggleGrid()
{
    setGridVisible(!isGridVisible);
}

void Position::actionGridSettings()
{
    QMessageBox::information(this, "Grid Settings", "Grid settings will be added in the next step.");
}

void Position::actionCenterHere()
{
    centerOnWidgetPoint(lastContextMenuPos);
}

void Position::actionFitMap()
{
    fitMap();
}

void Position::actionResetView()
{
    resetView();
}

void Position::actionCopyCoordinates()
{
    if (!isPointOnMap(lastContextMenuPos) || !isGeoTransform)
        return;

    const QPointF imagePos = widgetToImage(lastContextMenuPos);
    const QPointF geo = imageToLatLon(imagePos.x(), imagePos.y());
    QApplication::clipboard()->setText(QString("%1, %2")
                                           .arg(formatCoordinate(geo.y()))
                                           .arg(formatCoordinate(geo.x())));
}

void Position::actionAddMarkerHere()
{
    QPointF imagePos = widgetToImage(lastContextMenuPos);

    if (!isImagePointInsideMap(imagePos))
        return;

    QPointF geo = imageToLatLon(imagePos.x(), imagePos.y());

    addMarkerAt(geo.y(), geo.x(), selectedMarkerName);
}

void Position::actionAddTextHere()
{
    if (!isPointOnMap(lastContextMenuPos) || !isGeoTransform)
        return;

    bool ok = false;
    const QString text = QInputDialog::getText(this, "Text", "Text:", QLineEdit::Normal, "Text", &ok);
    if (!ok || text.trimmed().isEmpty())
        return;

    const QPointF imagePos = widgetToImage(lastContextMenuPos);
    const QPointF geo = imageToLatLon(imagePos.x(), imagePos.y());
    addTextAt(geo.y(), geo.x(), text.trimmed());
}

void Position::actionStartMeasureHere()
{
    setToolMode(MapToolDistance);
    addDistancePointAt(lastContextMenuPos);
}

void Position::actionToggleInfoBox()
{
    setMouseInfoVisible(!isMouseInfoVisible);
    updateButtonStates();
}

void Position::actionToggleLabels()
{
    setLabelsVisible(!isLabelsVisible);
}

void Position::onMapLayersChanged()
{
    update();
}

void Position::onActiveLayerChanged(int layerID)
{
    selectedLayerID = layerID;
    update();
}

//--- Events ---
void Position::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();

    if (event->button() == Qt::MiddleButton) {
        startPan(event->pos(), true);
        event->accept();
        return;
    }

    if (event->button() == Qt::RightButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    if (currentTool == MapToolPan || isLeftPanEnabled) {
        startPan(event->pos(), false);
        event->accept();
        return;
    }

    if (currentTool == MapToolZoomBox) {
        startZoomBox(event->pos());
        event->accept();
        return;
    }

    if (currentTool == MapToolDistance) {
        addDistancePointAt(event->pos());
        event->accept();
        return;
    }

    if (currentTool == MapToolMarker ||
        currentTool == MapToolText ||
        currentTool == MapToolLine ||
        currentTool == MapToolPolyline ||
        currentTool == MapToolRectangle ||
        currentTool == MapToolCircle ||
        currentTool == MapToolEllipse ||
        currentTool == MapToolPolygon ||
        currentTool == MapToolFreehand) {

        addDrawingPointAt(event->pos());
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void Position::mouseMoveEvent(QMouseEvent *event)
{
    if (isPanning) {
        updatePan(event->pos());
        event->accept();
        return;
    }

    lastMousePos = event->pos();

    if (hasMapData() && isGeoTransform && isPointOnMap(event->pos())) {
        const QPointF imagePos = widgetToImage(event->pos());
        const QPointF geo = imageToLatLon(imagePos.x(), imagePos.y());

        mouseLon = geo.x();
        mouseLat = geo.y();

        emit mouseGeoPositionChanged(mouseLat, mouseLon);
    }

    updateInfoBoxValues();

    if (isZoomBoxDrawing) {
        updateZoomBox(event->pos());
        event->accept();
        return;
    }

    if (currentTool == MapToolDistance) {
        updateDistanceAt(event->pos());
    }

    if (isDrawing) {
        updateDrawingAt(event->pos());
    }

    update();
}

void Position::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton || event->button() == Qt::LeftButton) {
        if (isPanning) {
            stopPan();
            event->accept();
            return;
        }
    }

    if (event->button() == Qt::LeftButton && isZoomBoxDrawing) {
        finishZoomBox();
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton &&
        currentTool == MapToolFreehand &&
        isDrawing) {
        finishDrawingByTool();
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

void Position::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (currentTool == MapToolPolyline ||
            currentTool == MapToolPolygon ||
            currentTool == MapToolFreehand) {

            finishCurrentDrawing();
            event->accept();
            return;
        }
    }

    QWidget::mouseDoubleClickEvent(event);
}

void Position::wheelEvent(QWheelEvent *event)
{
    const double factor = event->angleDelta().y() > 0 ? zoomStepFactor : 1.0 / zoomStepFactor;
    zoomAtWidgetPoint(event->position(), factor);
}

void Position::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateBotMenuGeometry();
    updateInfoBoxGeometry();
}

bool Position::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == botMenu && botMenuOpacity) {
        if (event->type() == QEvent::Enter)
            botMenuOpacity->setOpacity(1.0);
        else if (event->type() == QEvent::Leave)
            botMenuOpacity->setOpacity(0.35);
    }

    return QWidget::eventFilter(watched, event);
}

void Position::contextMenuEvent(QContextMenuEvent *event)
{
    lastContextMenuPos = event->pos();

    updateActionStates();

    if (contextMenu)
        contextMenu->exec(event->globalPos());
}

void Position::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.fillRect(rect(), QColor(55, 55, 55));

    drawMap(painter);
    drawGridOverlay(painter);
    drawMapLayers(painter);

    drawMapDfStations(painter);

    drawTemporaryShape(painter);
    drawDistanceOverlay(painter);
    drawZoomBox(painter);
    drawScaleBar(painter);
    drawNorthArrow(painter);
}
