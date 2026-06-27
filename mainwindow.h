#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QAbstractAnimation>
#include <QMessageBox>
#include <QShortcut>
#include "Includes/globalvariables.h"
#include "DataBase/repositorystation.h"
#include "Layouts/mainlayout.h"
#include "Layouts/menulayout.h"
#include "Layouts/leftlayout.h"
#include "Layouts/rightlayout.h"
#include "Windows/connectionwin.h"
#include "DataCore/maplayermanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    GlobalVariables *gVar;
    QString style;
    // --------- Layouts ---------
    QVBoxLayout *vLayout = nullptr;
    QSplitter *hSplitter = nullptr;
    QList<int> hSizes;
    MainLayout *mainLyout = nullptr;
    MenuLayout *menuLyout = nullptr;
    LeftLayout *leftLyout = nullptr;
    RightLayout *rightLyout = nullptr;
    // --------- Windows ---------
    ConnectionWin *connectionWin = nullptr;
    MapLayerManager *mapLayerManager = nullptr;
    // --------- Socket pool ---------
    QVector<SocketStruct> socketList;
    static constexpr int kIdxMonMain  = 0;
    static constexpr int kIdxMonScan  = 1;
    static constexpr int kIdxHandoff  = 2;
    static constexpr int kIdxDfFirst  = 3;
    static constexpr int kDfPoolSize  = 9;
    // --------- Left-menu state ---------
    bool isLeftMenuOpen = false;

private:
    // --- Socket helpers ---
    void   initSockets();
    void   wireSocketSignals(SocketManager *sock);
    int    findFreeDF() const;
    int    findByStationId(int id) const;
    void   connectEntry(int idx, StationModel *st, bool useScanParams = false);
    void   disconnectEntry(int idx);
    template<typename Func>
    void forEachSynced(Func fn)
    {
        for (const SocketStruct &e : socketList) {
            if (e.state == Socket_Connected &&
                (e.role == Role_Monitoring || e.isSynced))
                fn(e.socket);
        }
    }
    SocketStruct* entryOf(QObject *sender);
    // --- Init helpers ---
    void createDatabase();
    void createLyouts();
    void createMenuConnections();
    void createMenuViewConnections();
    void createMapLayerManager();
    void createLeftMenuConnections();
    void createDatabaseConnections();
    void createShortcuts();
    // --- Layout helpers ---
    void leftLayoutAnimate(int size);
    void leftLyoutEnter();
    void leftLyoutLeave();
    // --- Menu handlers ---
    void menuHomeConnectionLeftClick();
    void menuHomeFrequencyChange(double freq);
    void menuHomeBandwidthChange(QString bw);
    void menuHomeResolutionChange(QString res);
    void menuHomeModulationChange(QString mod);
    void menuHomeIFFilterChange(QString iff);
    void menuHomeRFHeadChange(QString rfh);
    void menuViewSetActiveView(QString name);
    void menuViewSetInView(QString name);
    void menuViewSetOutView(QString name);
    // --- Left-menu handlers ---
    void leftMenuRightMenuOpen(bool open);
    void leftMenuConnectStation(int stationId);
    void leftMenuCloseStation(int stationId);
    void leftMenuEditStation(int stationId);
    void leftMenuDeleteStation(int stationId);
    void leftMenuDisconnectConnected(int stationId);

public slots:
    void onConnectedState(bool connected);
    void onXferPSD(const double *data,short len);
    void onXferDefault(SystemDefualtsStruct def);
    void onSystemParam(SystemParametersStruct param);
    void onBandwidthChanged(SocketStruct socket, BANDWIDTH num);
    void onResolutionChanged(SocketStruct socket, SystemParameters_FFTPOINT num);
    void onModulationChanged(SocketStruct socket, SystemParameters_DEMOD num);
    void onIFFilterChanged(SocketStruct socket, SystemParameters_FILTERS num);
    void onRFHeadChanged(SocketStruct socket, SystemParameters_RFHEAD num);
    void onFrequencyChanged(double num);
    void onThresholdChanged(double num);
    void slotThemeChange();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

};
#endif // MAINWINDOW_H
