QT += core gui widgets sql
QT += opengl openglwidgets
QT += network serialport
QT += axcontainer
QT += location positioning multimedia
QT += xml

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Connection/connectioninterface.cpp \
    Connection/libhandler.cpp \
    Connection/ocxhandler.cpp \
    Connection/socketmanager.cpp \
    DataBase/baserepository.cpp \
    DataBase/dbmanager.cpp \
    DataBase/migrationrunner.cpp \
    DataBase/repositorymap.cpp \
    DataBase/repositorystation.cpp \
    DataCore/maplayerfilecodecbinary.cpp \
    DataCore/maplayermanager.cpp \
    Includes/stationmodel.cpp \
    Includes/globalvariables.cpp \
    Navigation/hometab.cpp \
    Navigation/toolstab.cpp \
    Navigation/viewtab.cpp \
    PlotSource/imgui/backends/imgui_impl_opengl3.cpp \
    PlotSource/imgui/imgui.cpp \
    PlotSource/imgui/imgui_demo.cpp \
    PlotSource/imgui/imgui_draw.cpp \
    PlotSource/imgui/imgui_tables.cpp \
    PlotSource/imgui/imgui_widgets.cpp \
    PlotSource/implot/implot.cpp \
    PlotSource/implot/implot_demo.cpp \
    PlotSource/implot/implot_items.cpp \
    Layouts/leftlayout.cpp \
    Layouts/mainlayout.cpp \
    Layouts/menulayout.cpp \
    Layouts/rightlayout.cpp \
    Plots/baseplots.cpp \
    Plots/dfgauge.cpp \
    Plots/pchlib.cpp \
    Plots/psdlib.cpp \
    Plots/wflib.cpp \
    RightMenu/caretabletab.cpp \
    RightMenu/maplayertab.cpp \
    RightMenu/quickaccesstab.cpp \
    Widgets/btnconnection.cpp \
    Widgets/btntextflip.cpp \
    Widgets/btntoggle.cpp \
    Widgets/caretableitem.cpp \
    Widgets/frequencybox.cpp \
    Widgets/imgbutton.cpp \
    Widgets/imgtoggle.cpp \
    Widgets/itmstation.cpp \
    Widgets/layertreewidget.cpp \
    Widgets/mapbtnmenu.cpp \
    Widgets/txtfreq.cpp \
    Windows/analysis.cpp \
    Windows/basewindows.cpp \
    Windows/connectionwin.cpp \
    Windows/developer.cpp \
    Windows/direction.cpp \
    Windows/handoff.cpp \
    Windows/overview.cpp \
    Windows/panorama.cpp \
    Windows/position.cpp \
    floatwindow.cpp \
    glviewport.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Connection/connectioninterface.h \
    Connection/libhandler.h \
    Connection/ocxhandler.h \
    Connection/socketmanager.h \
    DataBase/baserepository.h \
    DataBase/dbmanager.h \
    DataBase/migrationrunner.h \
    DataBase/repositorymap.h \
    DataBase/repositorystation.h \
    DataCore/maplayerfilecodecbinary.h \
    DataCore/maplayermanager.h \
    Includes/GlobalEnum.h \
    Includes/GlobalStruct.h \
    Includes/stationmodel.h \
    Includes/globalvariables.h \
    Navigation/hometab.h \
    Navigation/toolstab.h \
    Navigation/viewtab.h \
    PlotSource/imgui/backends/imgui_impl_opengl3.h \
    PlotSource/imgui/imconfig.h \
    PlotSource/imgui/imgui.h \
    PlotSource/imgui/imgui_internal.h \
    PlotSource/implot/implot.h \
    PlotSource/implot/implot_internal.h \
    Layouts/leftlayout.h \
    Layouts/mainlayout.h \
    Layouts/menulayout.h \
    Layouts/rightlayout.h \
    Plots/baseplots.h \
    Plots/dfgauge.h \
    Plots/pchlib.h \
    Plots/psdlib.h \
    Plots/wflib.h \
    RightMenu/caretabletab.h \
    RightMenu/maplayertab.h \
    RightMenu/quickaccesstab.h \
    Widgets/btnconnection.h \
    Widgets/btntextflip.h \
    Widgets/btntoggle.h \
    Widgets/caretableitem.h \
    Widgets/frequencybox.h \
    Widgets/imgbutton.h \
    Widgets/imgtoggle.h \
    Widgets/itmstation.h \
    Widgets/layertreewidget.h \
    Widgets/mapbtnmenu.h \
    Widgets/txtfreq.h \
    Windows/analysis.h \
    Windows/basewindows.h \
    Windows/connectionwin.h \
    Windows/developer.h \
    Windows/direction.h \
    Windows/handoff.h \
    Windows/overview.h \
    Windows/panorama.h \
    Windows/position.h \
    floatwindow.h \
    glviewport.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TRANSLATIONS += \
    Monitoring_ar_LB.ts \
    Monitoring_en_US.ts \
    Monitoring_fa_IR.ts \
    Monitoring_ru_RU.ts \
    Monitoring_zh_CN.ts

RESOURCES += \
    Resources.qrc

# --------- THIRD PARTY ---------
THIRD_PARTY_ROOT = $$PWD/third_party

# Protobuf
INCLUDEPATH += $$THIRD_PARTY_ROOT/protobuf-msvc/include
DEPENDPATH  += $$THIRD_PARTY_ROOT/protobuf-msvc/include
LIBS += $$THIRD_PARTY_ROOT/protobuf-msvc/lib/libprotobuf.lib

# Opus
INCLUDEPATH += $$THIRD_PARTY_ROOT/opus-msvc/include
DEPENDPATH  += $$THIRD_PARTY_ROOT/opus-msvc/include
LIBS += $$THIRD_PARTY_ROOT/opus-msvc/lib/opus.lib

# SpeexDSP
INCLUDEPATH += $$THIRD_PARTY_ROOT/speexdsp-msvc/include
DEPENDPATH  += $$THIRD_PARTY_ROOT/speexdsp-msvc/include
LIBS += $$THIRD_PARTY_ROOT/speexdsp-msvc/lib/speexdsp.lib

# Zlib
INCLUDEPATH += $$THIRD_PARTY_ROOT/zlib-msvc/include
DEPENDPATH  += $$THIRD_PARTY_ROOT/zlib-msvc/include
LIBS += $$THIRD_PARTY_ROOT/zlib-msvc/lib/z.lib
# --------- CDF embedded ---------
DEFINES += CDFSOCKET_LIBRARY APP_VERSION=\\\"6.12.0\\\"
INCLUDEPATH += $$PWD/third_party/cdfqt
DEPENDPATH  += $$PWD/third_party/cdfqt

SOURCES += \
    third_party/cdfqt/Connection.pb.cc \
    third_party/cdfqt/cdfsocket.cpp \
    third_party/cdfqt/receivedata.cpp

HEADERS += \
    third_party/cdfqt/CDFSocket_global.h \
    third_party/cdfqt/Connection.pb.h \
    third_party/cdfqt/OpusDecoderWrapper.h \
    third_party/cdfqt/cdfsocket.h \
    third_party/cdfqt/receivedata.h

# --------- GDAL / vcpkg vendor ---------
VCPKG_VENDOR_ROOT = $$PWD/third_party/vcpkg-msvc
INCLUDEPATH += $$VCPKG_VENDOR_ROOT/include
DEPENDPATH  += $$VCPKG_VENDOR_ROOT/include
LIBS += -L$$VCPKG_VENDOR_ROOT/lib -lgdal
include($$PWD/third_party/vcpkg-msvc/vcpkg-libs.pri)
QMAKE_LFLAGS += /NODEFAULTLIB:libcmt
DEFINES += QT_DEPRECATED_WARNINGS
# --------- Project Includes ---------
INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD

# INCLUDEPATH += $$PWD/Connection
# DEPENDPATH  += $$PWD/Connection

# INCLUDEPATH += $$PWD/Includes
# DEPENDPATH  += $$PWD/Includes

# INCLUDEPATH += $$PWD/Layouts
# DEPENDPATH  += $$PWD/Layouts

# INCLUDEPATH += $$PWD/Navigation
# DEPENDPATH  += $$PWD/Navigation

# INCLUDEPATH += $$PWD/Plots
# DEPENDPATH  += $$PWD/Plots

# INCLUDEPATH += $$PWD/Widgets
# DEPENDPATH  += $$PWD/Widgets

# INCLUDEPATH += $$PWD/Windows
# DEPENDPATH  += $$PWD/Windows
# --------- PlotSource ---------
INCLUDEPATH += $$PWD/PlotSource/implot
DEPENDPATH  += $$PWD/PlotSource/implot
INCLUDEPATH += $$PWD/PlotSource/imgui
DEPENDPATH  += $$PWD/PlotSource/imgui
INCLUDEPATH += $$PWD/PlotSource/imgui/backends
DEPENDPATH  += $$PWD/PlotSource/imgui/backends

# --------- Runtime deployment ---------
win32 {
    QMAKE_POST_LINK += cmd /c $$shell_path($$PWD/tools/deploy_runtime.bat)
}
