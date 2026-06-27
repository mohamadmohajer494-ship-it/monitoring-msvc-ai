#ifndef POSITION_H
#define POSITION_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QPoint>
#include <QPointF>
#include <QString>
#include <QVector>
#include <QtMath>
#include <QLineF>
#include <QTransform>
#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QGraphicsOpacityEffect>
#include <QEvent>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QDateTime>
#include <QPolygonF>
#include <QTimer>
#include <QElapsedTimer>
#include <QFrame>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QFile>
#include <QToolButton>
#include <QWidgetAction>

#include <gdal_priv.h>
#include <cpl_conv.h>

#include "Includes/globalvariables.h"
#include "Includes/GlobalEnum.h"
#include "Includes/GlobalStruct.h"
#include "Widgets/mapbtnmenu.h"
#include "DataCore/maplayermanager.h"

class Position : public QWidget
{
    Q_OBJECT

public:
    explicit Position(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~Position() override;

private:
    //--- Core ---
    GlobalVariables *gVar = nullptr;
    MapLayerManager *mapLayerManager = nullptr;

    QString style;
    QString currentMapPath;
    QString defaultMapPath;

    QImage mapImage;
    double geoTransform[6] = {0, 1, 0, 0, 0, -1};
    bool isGeoTransform = false;
    bool isMapLoaded = false;
    bool isDefaultMapLoading = false;

    GDALDataset *mapDataset = nullptr;
    bool isGdalStreamingMap = false;

    int mapRasterWidth = 0;
    int mapRasterHeight = 0;
    int mapRasterBandCount = 0;

    mutable QImage gdalVisibleImage;
    mutable QRectF gdalVisibleTargetRect;
    mutable QRect gdalVisibleSourceRect;

    QString mapProjectionWkt;
    bool isMapWebMercator = false;
    bool isMapGeographic = false;

    //--- View State ---
    QPointF offset = QPointF(0, 0);
    QPoint lastMousePos;
    QPointF lastContextMenuPos;

    double defaultLat = 32.4279;
    double defaultLon = 53.6880;
    double currentLat = 32.4279;
    double currentLon = 53.6880;
    double mouseLat = 32.4279;
    double mouseLon = 53.6880;

    double defaultZoom = 1.55;
    double currentZoom = 1.55;
    double minimumZoom = 0.01;
    double maximumZoom = 80.0;
    double zoomStepFactor = 1.15;

    bool isPanning = false;
    bool isMiddlePanning = false;
    bool isLeftPanEnabled = false;

    //--- Tool State ---
    EnumMapToolMode currentTool = MapToolPan;
    EnumMapShapeType currentShape = Marker;

    QString selectedMarkerIconPath = ":/Res/Images/Map/marker.png";
    QString selectedMarkerName = "Marker";

    bool isDrawing = false;
    bool isTemporaryPreviewVisible = false;

    QVector<GeoPointStruct> temporaryPoints;
    QPointF temporaryCurrentImagePos;
    QPointF temporaryStartImagePos;
    QPointF temporaryEndImagePos;

    //--- Selection State ---
    int selectedLayerID = -1;
    int selectedItemID = -1;
    bool isItemSelectionEnabled = false;

    //--- Zoom Box ---
    bool isZoomBoxMode = false;
    bool isZoomBoxDrawing = false;
    QPointF zoomBoxStartWidgetPos;
    QPointF zoomBoxEndWidgetPos;

    //--- Distance Measure ---
    bool isDistanceMode = false;
    bool isDistanceChainMode = true;
    bool isDistanceDrawing = false;
    QVector<GeoPointStruct> distancePoints;
    QPointF distanceCurrentImagePos;

    //--- Grid ---
    bool isGridVisible = false;
    bool isGridLabelVisible = true;
    double gridMinimumPixelSpacing = 80.0;

    QColor gridLineColor = QColor(255, 255, 255, 170);
    QColor gridTextColor = QColor(255, 255, 255, 230);
    int gridLineWidth = 1;

    //--- Map Helpers ---
    bool isScaleBarVisible = true;
    bool isNorthArrowVisible = true;
    bool isMouseInfoVisible = true;
    bool isLabelsVisible = true;
    //--- DF ---
    QVector<MapDfStationStruct> dfStations;
    int nextDfStationID = 1;

    QString dfStationIconPath = ":/Res/Images/Map/DfStation.png";
    QString dfStationLineColor = "#c9882d";
    int dfStationIconSize = 32;
    int dfStationLineLength = 500;
    //--- PF ---
    bool isPfMode = false;
    QTimer *pfTimer = nullptr;
    QElapsedTimer pfElapsed;

    //--- Right Click Menu ---
    QMenu *contextMenu = nullptr;

    QAction *actCenterHere = nullptr;
    QAction *actFitMap = nullptr;
    QAction *actResetView = nullptr;
    QAction *actCopyCoordinates = nullptr;

    QAction *actAddMarkerHere = nullptr;
    QAction *actAddTextHere = nullptr;
    QAction *actStartMeasureHere = nullptr;

    QAction *actShowGrid = nullptr;
    QAction *actShowInfoBox = nullptr;
    QAction *actShowLabels = nullptr;

    //--- Bottom Menu ---
    QFrame *botMenu = nullptr;
    QHBoxLayout *botLayout = nullptr;
    QGraphicsOpacityEffect *botMenuOpacity = nullptr;

    MapBtnMenu *btnMap = nullptr;
    MapBtnMenu *btnImport = nullptr;
    MapBtnMenu *btnPan = nullptr;
    MapBtnMenu *btnZoomBox = nullptr;
    MapBtnMenu *btnMarker = nullptr;
    MapBtnMenu *btnText = nullptr;
    MapBtnMenu *btnLine = nullptr;
    MapBtnMenu *btnArea = nullptr;
    MapBtnMenu *btnDistance = nullptr;
    MapBtnMenu *btnGrid = nullptr;
    MapBtnMenu *btnInfo = nullptr;
    MapBtnMenu *btnFit = nullptr;
    MapBtnMenu *btnPf = nullptr;

    QMenu *mapMenu = nullptr;
    QMenu *importMenu = nullptr;
    QMenu *markerMenu = nullptr;
    QMenu *textMenu = nullptr;
    QMenu *lineMenu = nullptr;
    QMenu *areaMenu = nullptr;
    QMenu *distanceMenu = nullptr;
    QMenu *gridMenu = nullptr;

    //--- Right Info ---
    QFrame *infoBox = nullptr;
    QGridLayout *infoLayout = nullptr;

    QLabel *lblTargetTitle = nullptr;
    QLabel *lblTargetTimeTitle = nullptr;
    QLabel *lblTargetTime = nullptr;
    QLabel *lblTargetLatTitle = nullptr;
    QLabel *lblTargetLat = nullptr;
    QLabel *lblTargetLonTitle = nullptr;
    QLabel *lblTargetLon = nullptr;

    QLabel *lblDistanceTotalTitle = nullptr;
    QLabel *lblDistanceTotal = nullptr;

    QLabel *lblMouseTitle = nullptr;
    QLabel *lblMouseLatTitle = nullptr;
    QLabel *lblMouseLat = nullptr;
    QLabel *lblMouseLonTitle = nullptr;
    QLabel *lblMouseLon = nullptr;
    QLabel *lblMousePixelTitle = nullptr;
    QLabel *lblMousePixel = nullptr;
    QLabel *lblZoomTitle = nullptr;
    QLabel *lblZoom = nullptr;

public:
    //--- Map Manager ---
    void setMapLayerManager(MapLayerManager *manager);
    MapLayerManager* getMapLayerManager() const;

    //--- Map Loading ---
    bool loadDefaultMap();
    bool loadMap(const QString &filePath);
    QString getCurrentMapPath() const;
    void setDefaultMapPath(const QString &filePath);

    //--- View Control ---
    void setZoom(double value);
    void zoomAtWidgetPoint(const QPointF &widgetPos, double zoomFactor);
    void pan(int dx, int dy);

    void fitMap();
    void resetView();
    void centerOnLatLon(double lat, double lon);
    void centerOnImagePoint(const QPointF &imagePos);
    void centerOnWidgetPoint(const QPointF &widgetPos);

    //--- Coordinate Conversion ---
    QPointF latLonToImage(double lat, double lon) const;
    QPointF imageToLatLon(double x, double y) const;
    QPointF imageToWidget(const QPointF &imagePos) const;
    QPointF widgetToImage(const QPointF &widgetPos) const;
    QPointF imageTopLeft() const;

    QPointF lonLatToMapCoordinate(double lat, double lon) const;
    QPointF mapCoordinateToLonLat(double x, double y) const;
    QPointF lonLatToWebMercator(double lat, double lon) const;
    QPointF webMercatorToLonLat(double x, double y) const;

    bool isPointOnMap(const QPointF &widgetPos) const;
    bool isImagePointInsideMap(const QPointF &imagePos) const;

    bool isWidgetPointDrawable(const QPointF &point, double margin = 2000.0) const;
    bool isWidgetRectDrawable(const QRectF &rect, double margin = 2000.0) const;

    //--- Tool Control ---
    void setToolMode(EnumMapToolMode toolMode);
    EnumMapToolMode getToolMode() const;

    void setCurrentShape(EnumMapShapeType shapeType);
    EnumMapShapeType getCurrentShape() const;

    void cancelCurrentTool();
    void finishCurrentDrawing();

    //--- Layer Item Actions ---
    bool addMarkerAt(double lat, double lon, const QString &name = "Marker");
    bool addTextAt(double lat, double lon, const QString &text = "Text");
    bool addItemToActiveLayer(MapItemStruct item);

    //--- Df Station ---
    int addDfStation(double lat, double lon, const QString &name = "DfStation", double angle = -1.0);
    bool setDfStationAngle(int dfStationID, double angle);
    bool updateDfStation(int dfStationID, double lat, double lon, const QString &name = "DfStation", double angle = -1.0);
    void clearDfStations();

    //--- Options ---
    void setGridVisible(bool visible);
    bool getGridVisible() const;

    void setMouseInfoVisible(bool visible);
    bool getMouseInfoVisible() const;

    void setLabelsVisible(bool visible);
    bool getLabelsVisible() const;

    void setDistanceMode(bool active);
    bool getDistanceMode() const;

    void setLeftPanEnabled(bool enabled);
    bool getLeftPanEnabled() const;

    //--- Utility ---
    double calculateDistance(double startLat, double startLon, double endLat, double endLon) const;
    QString formatDistance(double meters) const;
    QString formatCoordinate(double value) const;
    GeoPointStruct destinationPointByBearing(double lat,
                                             double lon,
                                             double bearingDegree,
                                             double distanceKm) const;

private:
    //--- UI Create ---
    void createContextMenu();
    void createBotMenu();
    void createBotMapMenu();
    void createBotImportMenu();
    void createBotMarkerMenu();
    void createBotTextMenu();
    void createBotLineMenu();
    void createBotAreaMenu();
    void createBotDistanceMenu();
    void createBotGridMenu();

    void createInfoBox();

    //--- UI Update ---
    void updateBotMenuGeometry();
    void updateInfoBoxGeometry();
    void updateInfoBoxValues();
    void updateActionStates();
    void updateButtonStates();

    //--- Map Load Helpers ---
    bool loadRasterWithGdal(const QString &filePath);
    bool loadRasterAsImage(const QString &filePath);
    void clearMap();
    void initializeDefaultView();

    void closeGdalMap();
    bool loadRasterWithGdalStreaming(const QString &filePath);
    void drawGdalStreamingMap(QPainter &painter);

    bool hasMapData() const;
    int mapPixelWidth() const;
    int mapPixelHeight() const;

    //--- Tool Helpers ---
    void startPan(const QPoint &mousePos, bool middleButton);
    void updatePan(const QPoint &mousePos);
    void stopPan();

    void startZoomBox(const QPointF &widgetPos);
    void updateZoomBox(const QPointF &widgetPos);
    void finishZoomBox();

    void startDrawingAt(const QPointF &widgetPos);
    void updateDrawingAt(const QPointF &widgetPos);
    void addDrawingPointAt(const QPointF &widgetPos);
    void finishDrawingByTool();
    void cancelDrawing();

    void startDistanceAt(const QPointF &widgetPos);
    void updateDistanceAt(const QPointF &widgetPos);
    void addDistancePointAt(const QPointF &widgetPos);
    void resetDistance();

    //--- Build Item Helpers ---
    MapItemStruct createItemFromTemporaryPoints() const;
    MapItemStruct createMarkerItem(double lat, double lon, const QString &name) const;
    MapItemStruct createTextItem(double lat, double lon, const QString &text) const;

    //--- Drawing Main ---
    void drawMap(QPainter &painter);
    void drawMapLayers(QPainter &painter);
    void drawMapLayer(QPainter &painter, const MapLayerStruct &layer);
    void drawMapItem(QPainter &painter, const MapItemStruct &item);
    void drawMapDfStations(QPainter &painter);
    void drawMapDfStation(QPainter &painter, const MapDfStationStruct &station);

    //--- Drawing Items ---
    void drawMarkerItem(QPainter &painter, const MapItemStruct &item);
    void drawTextItem(QPainter &painter, const MapItemStruct &item);
    void drawLineItem(QPainter &painter, const MapItemStruct &item);
    void drawPolylineItem(QPainter &painter, const MapItemStruct &item);
    void drawRectangleItem(QPainter &painter, const MapItemStruct &item);
    void drawCircleItem(QPainter &painter, const MapItemStruct &item);
    void drawEllipseItem(QPainter &painter, const MapItemStruct &item);
    void drawPolygonItem(QPainter &painter, const MapItemStruct &item);
    void drawFreehandItem(QPainter &painter, const MapItemStruct &item);
    void drawSectorItem(QPainter &painter, const MapItemStruct &item);
    void drawArcItem(QPainter &painter, const MapItemStruct &item);
    void drawImageOverlayItem(QPainter &painter, const MapItemStruct &item);
    void drawMeasureItem(QPainter &painter, const MapItemStruct &item);
    void drawMeasurePin(QPainter &painter, const QPointF &bottomCenter) const;
    void drawTextWithWhiteBackground(QPainter &painter, const QPointF &pos, const QString &text) const;

    //--- Drawing Overlays ---
    void drawTemporaryShape(QPainter &painter);
    void drawZoomBox(QPainter &painter);
    void drawDistanceOverlay(QPainter &painter);
    void drawGridOverlay(QPainter &painter);
    void drawScaleBar(QPainter &painter);
    void drawNorthArrow(QPainter &painter);
    void drawNoMapMessage(QPainter &painter);

    //--- Drawing Utility ---
    void applyItemStyle(QPainter &painter, const MapItemStruct &item, bool useFill = false, bool isTemporary = true) const;
    void drawPointMarker(QPainter &painter, const QPointF &pos, const MapItemStruct &item) const;
    void drawTextLabel(QPainter &painter, const QPointF &pos, const QString &text, const MapItemStruct &item) const;
    void drawOutlinedText(QPainter &painter, const QPointF &pos, const QString &text) const;
    QString resolveMarkerIconPath(const MapItemStruct &item) const;

    QPolygonF pointsToWidgetPolygon(const QVector<GeoPointStruct> &points) const;
    QVector<QPointF> pointsToWidgetPoints(const QVector<GeoPointStruct> &points) const;

    double distanceBetweenGeoPoints(const GeoPointStruct &a, const GeoPointStruct &b) const;
    double distanceBetweenImagePoints(const QPointF &a, const QPointF &b) const;

    //--- Import Helpers ---
    bool importLayerFromFile(const QString &filePath);
    bool importGeoJsonLayer(const QString &filePath);
    bool importCsvPointLayer(const QString &filePath);

public slots:
    //--- File Slots ---
    bool loadMapFromFileBrowser();
    bool importLayerFromFileBrowser();

    //--- Theme ---
    void slotThemeChange();

private slots:
    //--- PF ---
    void tickPfTimer();
    void clickBtnPf();

    //--- Bottom Menu Clicks ---
    void clickBtnMap();
    void clickBtnImport();
    void clickBtnPan();
    void clickBtnZoomBox();

    void clickBtnMarker();
    void clickBtnText();
    void clickBtnLine();
    void clickBtnArea();
    void clickBtnDistance();
    void clickBtnGrid();
    void clickBtnInfo();
    void clickBtnFit();

    //--- Map Menu Actions ---
    void actionLoadMap();
    void actionLoadDefaultMap();

    //--- Import Menu Actions ---
    void actionImportGeoJson();
    void actionImportCsvPoints();

    //--- Draw Menu Actions ---
    void actionDrawMarker();
    void actionDrawText();
    void actionDrawLine();
    void actionDrawPolyline();
    void actionDrawRectangle();
    void actionDrawCircle();
    void actionDrawEllipse();
    void actionDrawPolygon();
    void actionDrawFreehand();

    //--- Distance Menu Actions ---
    void actionDistanceSingle();
    void actionDistanceChain();

    //--- Grid Menu Actions ---
    void actionToggleGrid();
    void actionGridSettings();

    //--- Context Menu Actions ---
    void actionCenterHere();
    void actionFitMap();
    void actionResetView();
    void actionCopyCoordinates();
    void actionAddMarkerHere();
    void actionAddTextHere();
    void actionStartMeasureHere();
    void actionToggleInfoBox();
    void actionToggleLabels();

    //--- Manager Events ---
    void onMapLayersChanged();
    void onActiveLayerChanged(int layerID);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void mapLoaded(const QString &filePath);
    void mapLoadFailed(const QString &filePath);
    void mouseGeoPositionChanged(double lat, double lon);
    void toolModeChanged(EnumMapToolMode toolMode);
    void itemCreated(int itemID);
    void mapViewChanged(double zoom, QPointF offset);
};

#endif // POSITION_H