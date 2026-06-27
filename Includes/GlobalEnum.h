#ifndef GLOBALENUM_H
#define GLOBALENUM_H

enum EnumSystemMode {
    Role_NoMode = 0,
    Role_Monitoring = 1,
    Role_Scan = 2,
    Role_DF = 3,
    Role_HandOff = 4
};

enum EnumConnectionType {
    OCX_OCX = 0,
    TCP_TCP = 1,
    UDP_UDP = 2,
    SER_UDP = 3,
    SER_SER = 4
};

enum EnumPositionType {
    DMS = 0,
    UTM = 1,
    LatLong = 2,
    MGRS = 3
};

enum EnumFreqUnit {
    Hz = 0,
    KHz = 1,
    MHz = 2,
    GHz = 3
};

enum EnumPCHTheme {
    Current,
    Viridis,
    Inferno,
    Turbo,
    IceFire,
    Gray
};

enum EnumSocketState
{
    Socket_Free,
    Socket_Connecting,
    Socket_Connected,
    Socket_Disconnecting
};

enum EnumStatinStatus
{
    Statin_Unavailable,
    Statin_Available,
    Statin_Reachable,
    Statin_Connected
};

enum EnumMapShapeType
{
    Marker = 0,
    Text = 1,
    Line = 10,
    Polyline = 11,
    Rectangle = 20,
    Circle = 21,
    Ellipse = 22,
    Polygon = 23,
    Freehand = 24,
    Sector = 30,
    Arc = 31,
    ImageOverlay = 40,
    Measure = 50,
    Unknown = 99
};

enum EnumMapToolMode
{
    MapToolNone = 0,
    MapToolPan = 1,
    MapToolZoomBox = 2,

    MapToolMarker = 10,
    MapToolText = 11,

    MapToolLine = 20,
    MapToolPolyline = 21,

    MapToolRectangle = 30,
    MapToolCircle = 31,
    MapToolEllipse = 32,
    MapToolPolygon = 33,
    MapToolFreehand = 34,

    MapToolDistance = 40,
    MapToolSelect = 50
};


#endif // GLOBALENUM_H
