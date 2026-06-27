#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(GlobalVariables *gVar, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , gVar(gVar)
{
    ui->setupUi(this);
    statusBar()->hide();
    installEventFilter(this);
    // --- Data ---
    createDatabase();
    // --- Layouts ---
    createLyouts();
    // --- Connections that need layouts ---
    createDatabaseConnections();
    createMenuViewConnections();
    createLeftMenuConnections();
    // --- Load data into UI ---
    if (!gVar->getStationList().isEmpty())
        leftLyout->loadAllStation();
    // --- Map Layer Manager ---
    createMapLayerManager();
    // --- Windows ---
    connectionWin = new ConnectionWin(gVar);
    connect(connectionWin, &ConnectionWin::clickAddNewStation,
            leftLyout, &LeftLayout::loadNewStation);
    connect(connectionWin, &ConnectionWin::clickEditNewStation,
            leftLyout, &LeftLayout::updateStation);
    // --- Shortcut ---
    createShortcuts();
    // --- Socket ---
    initSockets();
    // --- Theme ---
    slotThemeChange();
    connect(gVar, &GlobalVariables::ThemeChange,
            this, &MainWindow::slotThemeChange);
}

MainWindow::~MainWindow()
{
    for (int var = 0; var < socketList.size(); ++var)
        disconnectEntry(var);
    delete ui;
}

void MainWindow::initSockets()
{
    const struct { EnumSystemMode role; bool isSynced; } defs[] = {
                 { Role_Monitoring, false },
                 { Role_Scan, false },
                 { Role_HandOff, true  },
                 { Role_DF, true  },
                 { Role_DF, true  },
                 { Role_DF, true  },
                 { Role_DF, true  },
                 { Role_DF, true  },
                 { Role_DF, true  },
                 { Role_DF, true  },
                 { Role_DF, true  },
                 { Role_DF, true  },
                 };
    constexpr int total = sizeof(defs) / sizeof(defs[0]);
    socketList.resize(total);
    for (int i = 0; i < total; ++i) {
        auto &e = socketList[i];
        e.socket = new SocketManager(this);
        e.role = defs[i].role;
        e.isSynced = defs[i].isSynced;
        e.state = Socket_Free;
        e.stationId = -1;
        e.station = nullptr;
        wireSocketSignals(e.socket);
        gVar->addToSocketList(e.socket);
    }
}

void MainWindow::wireSocketSignals(SocketManager *sock)
{
    connect(sock, &SocketManager::ConnectedState,
            this, &MainWindow::onConnectedState);

    connect(sock, &SocketManager::XferPSD,
            this, &MainWindow::onXferPSD);
    connect(sock, &SocketManager::XferDefault,
            this, &MainWindow::onXferDefault);
    connect(sock, &SocketManager::SystemParamEnvet,
            this, &MainWindow::onSystemParam);
    connect(sock, &SocketManager::FrequencyChanged,
            this, &MainWindow::onFrequencyChanged);
    connect(sock, &SocketManager::ThresholdChanged,
            this, &MainWindow::onThresholdChanged);
}

int MainWindow::findFreeDF() const
{
    for (int i = kIdxDfFirst; i < socketList.size(); ++i) {
        if (socketList[i].state == Socket_Free)
            return i;
    }
    return -1;
}

int MainWindow::findByStationId(int id) const
{
    for (int i = 0; i < socketList.size(); ++i) {
        if (socketList[i].stationId == id)
            return i;
    }
    return -1;
}

void MainWindow::connectEntry(int idx, StationModel *st, bool useScanParams)
{
    SocketStruct &e = socketList[idx];
    e.pages.clear();
    switch (e.role) {
    case Role_Monitoring:
        e.pages = { mainLyout->getOverview(),
                   mainLyout->getPanorama(),
                   mainLyout->getAnalysis() };
        break;
    case Role_Scan:
        e.pages = { mainLyout->getPanorama() };
        break;
    case Role_HandOff:
        e.pages = { mainLyout->getHandoff() };
        break;
    case Role_DF:
        e.pages = { mainLyout->getDirectionPage(st->getID()) };
        break;
    default: break;
    }
    const EnumConnectionType ct = useScanParams
                                      ? st->getScanConnectionType()
                                      : st->getSysConnectionType();
    e.socket->setHandler(ct == OCX_OCX
                             ? SocketManager::OCX_Mode
                             : SocketManager::LIB_Mode);
    const ConnectionMode cm = static_cast<ConnectionMode>(
        (useScanParams ? st->getScanConnectionType()
                       : st->getSysConnectionType()) - 1);

    e.socket->ConnectToSystem(
        cm,
        BroadCastType,
        useScanParams ? st->getScanIp()          : st->getSysIp(),
        useScanParams ? st->getScanTcp()          : st->getPortTcp(),
        useScanParams ? st->getScanUdpSend()      : st->getPortUdpSend(),
        useScanParams ? st->getScanUdpReceive()   : st->getPortUdpReceive(),
        1000,
        useScanParams ? st->getScanSerCom()       : st->getPortSerCom(),
        useScanParams ? st->getScanSerBaudRate()  : st->getPortSerBaudRate());
    e.state     = Socket_Connecting;
    e.stationId = st->getID();
    e.station   = st;
}

void MainWindow::disconnectEntry(int idx)
{
    if (idx < 0 || idx >= socketList.size())
        return;
    SocketStruct &e = socketList[idx];
    if (e.state == Socket_Free)
        return;
    e.socket->disconnectFromSystem();
    e.state     = Socket_Free;
    e.stationId = -1;
    e.station   = nullptr;
}

SocketStruct *MainWindow::entryOf(QObject *sender)
{
    auto *sock = qobject_cast<SocketManager*>(sender);
    if (!sock) return nullptr;
    for (auto &e : socketList)
        if (e.socket == sock) return &e;
    return nullptr;
}

void MainWindow::createDatabase()
{
    DbConfig cfg;
    auto& inst = DbManager::instance();
    if (!inst.init(cfg,true)) {
        QMessageBox::critical(this, "DB Error", inst.lastError());
        return;
    }
    qDebug() << "DB connected. Driver:" << inst.db().driverName();
    gVar->setDbManager(&inst);
    gVar->setDatabase(inst.db());
    if (!gVar->getDatabase().isOpen()) {
        QMessageBox::critical(this, "DB Error", "Database is not open!");
        return;
    }
    gVar->getAllData();
}

void MainWindow::createLyouts()
{
    vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    ui->centralwidget->setLayout(vLayout);
    menuLyout = new MenuLayout(gVar, this);
    vLayout->addWidget(menuLyout);
    leftLyout = new LeftLayout(gVar, this);
    leftLyout->setFixedWidth(gVar->getLeftLyoutCloseWidth());
    leftLyout->installEventFilter(this);
    mainLyout = new MainLayout(gVar, this);
    rightLyout = new RightLayout(gVar, this);
    hSplitter = new QSplitter(Qt::Horizontal, this);
    hSplitter->addWidget(leftLyout);
    hSplitter->addWidget(mainLyout);
    hSplitter->addWidget(rightLyout);
    rightLyout->setMinimumWidth(0);
    mainLyout->setMinimumWidth(300);

    rightLyout->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    mainLyout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    int leftWidth = gVar->getLeftLyoutCloseWidth();
    int remaining = qMax(0, width() - leftWidth);

    int mainWidth = remaining * gVar->getMainLyoutWidth();
    int rightWidth = remaining * gVar->getRightLyoutWidth();

    hSizes.clear();
    hSizes << leftWidth << mainWidth << rightWidth;

    hSplitter->setSizes(hSizes);

    hSplitter->setStretchFactor(0, 0);
    hSplitter->setStretchFactor(1, 99);
    hSplitter->setStretchFactor(2, 1);
    hSplitter->handle(1)->setEnabled(false);
    hSplitter->setCollapsible(0, false);
    hSplitter->setCollapsible(1, false);
    hSplitter->setCollapsible(2, false);
    vLayout->addWidget(hSplitter);

    createMenuConnections();
}

void MainWindow::createMenuConnections()
{
    connect(menuLyout->getHomeTab()->getBtnConnection(),&BtnConnection::leftClick,
            this,&MainWindow::menuHomeConnectionLeftClick);
    connect(menuLyout->getHomeTab()->getFreqBox(),&FrequencyBox::changeFrequency,
            this,&MainWindow::menuHomeFrequencyChange);
    connect(menuLyout->getHomeTab()->getBtnBandwidth(),&BtnTextFlip::changeSelected,
            this,&MainWindow::menuHomeBandwidthChange);
    connect(menuLyout->getHomeTab()->getBtnResolution(),&BtnTextFlip::changeSelected,
            this,&MainWindow::menuHomeResolutionChange);
    connect(menuLyout->getHomeTab()->getBtnModulation(),&BtnTextFlip::changeSelected,
            this,&MainWindow::menuHomeModulationChange);
    connect(menuLyout->getHomeTab()->getBtnIFFilter(),&BtnTextFlip::changeSelected,
            this,&MainWindow::menuHomeIFFilterChange);
    connect(menuLyout->getHomeTab()->getBtnRFHead(),&BtnTextFlip::changeSelected,
            this,&MainWindow::menuHomeRFHeadChange);
}

void MainWindow::createMenuViewConnections()
{
    connect(mainLyout,&MainLayout::setActiveView,
            this,&MainWindow::menuViewSetActiveView);
    connect(mainLyout,&MainLayout::setOutView,
            this,&MainWindow::menuViewSetOutView);
    connect(mainLyout,&MainLayout::setInView,
            this,&MainWindow::menuViewSetInView);
    for (auto btn : menuLyout->getViewTab()->getAllViewButtons()) {
        connect(btn, &ImgButton::leftClick,
                mainLyout, &MainLayout::showWidget);
        connect(btn, &ImgButton::rightClick,
                mainLyout, &MainLayout::detachWidget);
    }

}

void MainWindow::createMapLayerManager()
{
    mapLayerManager = new MapLayerManager(gVar,
                                          gVar->getDatabase(),
                                          this);
    mapLayerManager->loadAll();
    if (mapLayerManager->layers().isEmpty()) {
        mapLayerManager->addLayer("Default Layer", 0);
    }
    mainLyout->getPosition()->setMapLayerManager(mapLayerManager);
    rightLyout->getMapLayerTab()->setMapLayerManager(mapLayerManager);
}

void MainWindow::createLeftMenuConnections()
{
    connect(leftLyout,&LeftLayout::openRightMenu,
            this,&MainWindow::leftMenuRightMenuOpen);
    connect(leftLyout,&LeftLayout::clickRightMenuConnect,
            this,&MainWindow::leftMenuConnectStation);
    connect(leftLyout,&LeftLayout::clickRightMenuClose,
            this,&MainWindow::leftMenuCloseStation);
    connect(leftLyout,&LeftLayout::clickRightMenuEdit,
            this,&MainWindow::leftMenuEditStation);
    connect(leftLyout,&LeftLayout::clickRightMenuDelete,
            this,&MainWindow::leftMenuDeleteStation);
    connect(leftLyout,&LeftLayout::disconnectConnectedStation,
            this,&MainWindow::leftMenuDisconnectConnected);
}

void MainWindow::createDatabaseConnections()
{
    connect(gVar,&GlobalVariables::LoadDataBaseStation,
            leftLyout,&LeftLayout::loadAllStation);
}

void MainWindow::createShortcuts()
{
    QShortcut *shcShowDeveloper = new QShortcut(QKeySequence("Ctrl+Shift+D"), this);
    connect(shcShowDeveloper, &QShortcut::activated, this, [this](){

    });
    QShortcut *shcDockDeveloper = new QShortcut(QKeySequence("Ctrl+Alt+D"), this);
    connect(shcDockDeveloper, &QShortcut::activated, this, [this](){

    });
}

void MainWindow::leftLayoutAnimate(int size)
{
    QPropertyAnimation *animation = new QPropertyAnimation(leftLyout, "minimumWidth");
    animation->setDuration(250);
    animation->setStartValue(leftLyout->width());
    animation->setEndValue(size);
    animation->setEasingCurve(QEasingCurve::InOutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::leftLyoutEnter()
{
    leftLyout->setFixedWidth(gVar->getLeftLyoutOpenWidth());
    leftLyout->setOpen(true);
}

void MainWindow::leftLyoutLeave()
{
    leftLyout->setFixedWidth(gVar->getLeftLyoutCloseWidth());
    leftLyout->setOpen(false);
}

void MainWindow::menuHomeConnectionLeftClick()
{
    connectionWin->resetDefault();
    connectionWin->exec();
}

void MainWindow::menuHomeFrequencyChange(double freq)
{
    // socketDf01->SendFrequency(freq);
    // forEachSyncedSocket([&](auto socket) {
    //     socket->SendFrequency(freq);
    // });
}

void MainWindow::menuHomeBandwidthChange(QString bw)
{
    static const QMap<QString, BANDWIDTH> map = {
        {"1KHz", _DFBW_1_KHz},
        {"15KHz", _DFBW_15_KHz},
        {"25KHz", _DFBW_25_KHz},
        {"50KHz", _DFBW_50_KHz},
        {"100KHz", _DFBW_100_KHz},
        {"150KHz", _DFBW_150_KHz},
        {"200KHz", _DFBW_200_KHz},
        {"250KHz", _DFBW_250_KHz},
        {"500KHz", _DFBW_500_KHz},
        {"1MHz", _DFBW_1_MHz},
        {"1.5MHz", _DFBW_1M5_MHz},
        {"2MHz", _DFBW_2_MHz},
        {"4MHz", _DFBW_4_MHz},
        {"10MHz", _DFBW_10_MHz},
        {"20MHz", _DFBW_20_MHz},
        {"33MHz", _DFBW_33_MHz},
        {"40MHz", _DFBW_40_MHz},
        {"80MHz", _DFBW_80_MHz}
    };
    if (map.contains(bw)) {
        auto val = map[bw];
        // forEachSyncedSocket([&](auto socket) { socket->SendBandwidth(val); });
    } else {
        qWarning() << "Invalid Bandwidth value:" << bw;
    }
}

void MainWindow::menuHomeResolutionChange(QString res)
{
    static const QMap<QString, SystemParameters_FFTPOINT> map = {
        {"512", SystemParameters_FFTPOINT_FFTSample_512},
        {"1024", SystemParameters_FFTPOINT_FFTSample_1024},
        {"2048", SystemParameters_FFTPOINT_FFTSample_2048}
    };
    if (map.contains(res)) {
        auto val = map[res];
        // forEachSyncedSocket([&](auto socket) { socket->SendFFTPoint(val); });
    } else {
        qWarning() << "Invalid FFTPoint value:" << res;
    }
}

void MainWindow::menuHomeModulationChange(QString mod)
{
    static const QMap<QString, SystemParameters_DEMOD> map = {
        {"AM", SystemParameters_DEMOD_AM},
        {"FM", SystemParameters_DEMOD_FM},
        {"PM", SystemParameters_DEMOD_PM}
    };
    if (map.contains(mod)) {
        auto val = map[mod];
        // forEachSyncedSocket([&](auto socket) { socket->SendDemod(val); });
    } else {
        qWarning() << "Invalid Demod value:" << mod;
    }
}

void MainWindow::menuHomeIFFilterChange(QString iff)
{
    static const QMap<QString, SystemParameters_FILTERS> map = {
        {"1KHz", SystemParameters_FILTERS__1KHz},
        {"2KHz", SystemParameters_FILTERS__2KHz},
        {"3KHz", SystemParameters_FILTERS__3KHz},
        {"7.5KHz", SystemParameters_FILTERS__7_5KHz},
        {"15KHz", SystemParameters_FILTERS__15KHz},
        {"125KHz", SystemParameters_FILTERS__125KHz},
        {"250KHz", SystemParameters_FILTERS__250KHz}
    };
    if (map.contains(iff)) {
        auto val = map[iff];
        // forEachSyncedSocket([&](auto socket) { socket->SendFilter(val); });
    } else {
        qWarning() << "Invalid Filter value:" << iff;
    }
}

void MainWindow::menuHomeRFHeadChange(QString rfh)
{
    static const QMap<QString, SystemParameters_RFHEAD> map = {
                                                               {"HL", SystemParameters_RFHEAD_HL},
                                                               {"HS", SystemParameters_RFHEAD_HS},
                                                               {"INT", SystemParameters_RFHEAD_Int},
                                                               {"ATT", SystemParameters_RFHEAD_Attenuator},
                                                               };
    if (map.contains(rfh)) {
        auto val = map[rfh];
        // forEachSyncedSocket([&](auto socket) { socket->SendRFHead(val); });
    } else {
        qWarning() << "Invalid RFHead value:" << rfh;
    }
}

void MainWindow::menuViewSetActiveView(QString name)
{
    for (auto btn : menuLyout->getViewTab()->getAllViewButtons()) {
        btn->setStyle(false);
    }
    if (auto btn = menuLyout->getViewTab()->getViewButton(name)) {
        btn->setStyle(true);
    }
}

void MainWindow::menuViewSetInView(QString name)
{
    if (auto btn = menuLyout->getViewTab()->getViewButton(name)) {
        btn->setLockStyle(false);
        btn->setStyle(false);
    }
}

void MainWindow::menuViewSetOutView(QString name)
{
    if (auto btn = menuLyout->getViewTab()->getViewButton(name)) {
        btn->setStyle(true);
        btn->setLockStyle(true);
    }
}

void MainWindow::leftMenuRightMenuOpen(bool open)
{
    isLeftMenuOpen = open;
    if (!open)
        leftLyoutLeave();
}

void MainWindow::leftMenuConnectStation(int stationId)
{
    StationModel *st = gVar->findStationById(stationId);
    if (!st) return;

    switch (st->getSysMode()) {
    case Role_Monitoring:
        connectEntry(kIdxMonMain,  st, false);
        connectEntry(kIdxMonScan,  st, true);
        break;

    case Role_HandOff:
        connectEntry(kIdxHandoff, st, false);
        break;

    case Role_DF: {
        const int freeIdx = findFreeDF();
        if (freeIdx < 0) {
            QMessageBox::warning(this, "Connection","All DF Socket's Full");
            return;
        }
        connectEntry(freeIdx, st, false);
        break;
    }

    default:
        break;
    }
}

void MainWindow::leftMenuCloseStation(int stationId)
{
    const int idx = findByStationId(stationId);
    if (idx >= 0)
        disconnectEntry(idx);
    ItmStation *item = leftLyout->findItemById(stationId);
    if (item)
        item->setConnected(false);
}

void MainWindow::leftMenuEditStation(int stationId)
{
    connectionWin->setEditMode(stationId);
    connectionWin->exec();
}

void MainWindow::leftMenuDeleteStation(int stationId)
{
    gVar->getRepStation()->remove(stationId);
    gVar->removeStation(stationId);
    leftLyout->clearStations();
    leftLyout->loadAllStation();
}

void MainWindow::leftMenuDisconnectConnected(int stationId)
{
    leftMenuCloseStation(stationId);
}

void MainWindow::onConnectedState(bool connected)
{
    auto *sock = qobject_cast<SocketManager *>(sender());
    if (!sock)
        return;
    int idx = -1;
    for (int var = 0; var < socketList.size(); ++var) {
        if (socketList[var].socket == sock) {
            idx = var;
            break;
        }
    }
    if (idx < 0)
        return;
    SocketStruct &e = socketList[idx];
    if (connected) {
        e.state = Socket_Connected;
        if (idx == kIdxMonMain) {
            gVar->setMainSystem(Role_Monitoring);
            for (SocketStruct sck : socketList)
                sck.isMainSystem = false;
            e.isMainSystem = true;
        }
        else if (idx == kIdxHandoff) {
            gVar->setMainSystem(Role_HandOff);
            for (SocketStruct sck : socketList)
                sck.isMainSystem = false;
            e.isMainSystem = true;
        }
        else if (e.role == Role_DF) {
            gVar->setMainSystem(Role_DF);
            for (SocketStruct sck : socketList)
                sck.isMainSystem = false;
            e.isMainSystem = true;
        }
        ItmStation *item = leftLyout->findItemById(e.stationId);
        if (item)
            item->setConnected(true);
    }
    else {
        e.state     = Socket_Free;
        e.stationId = -1;
        e.station   = nullptr;
        ItmStation *item = leftLyout->findItemById(e.stationId);
        if (item)
            item->setConnected(false);
    }
}

void MainWindow::onXferPSD(const double *data, short len)
{
    qDebug() << "PSD";
}

void MainWindow::onXferDefault(SystemDefualtsStruct def)
{
    auto *e = entryOf(sender());
    if (!e) return;
    e->sysDefualt = def;
    for (auto *page : e->pages)
        page->onXferDefault(def);
}

void MainWindow::onSystemParam(SystemParametersStruct param)
{
    auto *e = entryOf(sender());
    if (!e) return;
    onBandwidthChanged(*e, param.bandwidth);
    onResolutionChanged(*e, param.Fft);
    onModulationChanged(*e, param.demod);
    onIFFilterChanged(*e, param.filter);
    onRFHeadChanged(*e, param.rfhead);
    for (auto *page : e->pages)
        page->onSystemParam(param);
}

void MainWindow::onBandwidthChanged(SocketStruct socket, BANDWIDTH num)
{
    QString strBandwidth;
    double numBandwidth;
    switch (num) {
    case _DFBW_1_KHz: strBandwidth = "1KHz"; numBandwidth = 0.001; break;
    case _DFBW_15_KHz: strBandwidth = "15KHz"; numBandwidth = 0.015; break;
    case _DFBW_25_KHz: strBandwidth = "25KHz"; numBandwidth = 0.025; break;
    case _DFBW_50_KHz: strBandwidth = "50KHz"; numBandwidth = 0.050; break;
    case _DFBW_100_KHz: strBandwidth = "100KHz"; numBandwidth = 0.1; break;
    case _DFBW_150_KHz: strBandwidth = "150KHz"; numBandwidth = 0.15; break;
    case _DFBW_200_KHz: strBandwidth = "200KHz"; numBandwidth = 0.2; break;
    case _DFBW_250_KHz: strBandwidth = "250KHz"; numBandwidth = 0.25; break;
    case _DFBW_500_KHz: strBandwidth = "500KHz"; numBandwidth = 0.5; break;
    case _DFBW_1_MHz: strBandwidth = "1MHz"; numBandwidth = 1; break;
    case _DFBW_1M5_MHz: strBandwidth = "1.5MHz"; numBandwidth = 1.5; break;
    case _DFBW_2_MHz: strBandwidth = "2MHz"; numBandwidth = 2; break;
    case _DFBW_4_MHz: strBandwidth = "4MHz"; numBandwidth = 4; break;
    case _DFBW_10_MHz: strBandwidth = "10MHz"; numBandwidth = 10; break;
    case _DFBW_20_MHz: strBandwidth = "20MHz"; numBandwidth = 20; break;
    case _DFBW_33_MHz: strBandwidth = "33MHz"; numBandwidth = 33; break;
    case _DFBW_40_MHz: strBandwidth = "40MHz"; numBandwidth = 40; break;
    case _DFBW_80_MHz: strBandwidth = "80MHz"; numBandwidth = 80; break;
    case _DFBW_WB:
    default:
        if (socket.sysDefualt.SystemType == Defaults_SYSTEMTYPE_miI) {
            strBandwidth = "40MHz";
            numBandwidth = 40;
        }
        if (socket.sysDefualt.SystemType == Defaults_SYSTEMTYPE_miII) {
            strBandwidth = "80MHz";
            numBandwidth = 80;
        }
        if (socket.sysDefualt.SystemType == Defaults_SYSTEMTYPE_mob) {
            strBandwidth = "80MHz";
            numBandwidth = 80;
        }
        break;
    }
    if (socket.isMainSystem) {
        menuLyout->getHomeTab()->getBtnBandwidth()->setSelectedItem(strBandwidth);
    }
    for (auto *page : socket.pages)
        page->onBandwidthChanged(strBandwidth, numBandwidth);
}

void MainWindow::onResolutionChanged(SocketStruct socket, SystemParameters_FFTPOINT num)
{
    QString strResolution;
    double numResolution;
    switch (num) {
    case SystemParameters_FFTPOINT_FFTSample_512: strResolution = "512"; numResolution = 512; break;
    case SystemParameters_FFTPOINT_FFTSample_1024: strResolution = "1024"; numResolution = 1024; break;
    case SystemParameters_FFTPOINT_FFTSample_2048: strResolution = "2048"; numResolution = 2048; break;
    default: strResolution = ""; numResolution = 0;  break;
    }
    if (socket.isMainSystem) {
        menuLyout->getHomeTab()->getBtnResolution()->setSelectedItem(strResolution);
    }
    for (auto *page : socket.pages)
        page->onResolutionChanged(strResolution, numResolution);
}

void MainWindow::onModulationChanged(SocketStruct socket, SystemParameters_DEMOD num)
{
    QString strModulation;
    switch (num) {
    case SystemParameters_DEMOD_AM: strModulation = "AM"; break;
    case SystemParameters_DEMOD_FM: strModulation = "FM"; break;
    case SystemParameters_DEMOD_PM: strModulation = "PM"; break;
    default: strModulation = "";  break;
    }
    if (socket.isMainSystem) {
        menuLyout->getHomeTab()->getBtnModulation()->setSelectedItem(strModulation);
    }
    for (auto *page : socket.pages)
        page->onModulationChanged(strModulation);
}

void MainWindow::onIFFilterChanged(SocketStruct socket, SystemParameters_FILTERS num)
{
    QString strIFFilter;
    switch (num) {
    case SystemParameters_FILTERS__15KHz: strIFFilter = "15KHz"; break;
    case SystemParameters_FILTERS__3KHz: strIFFilter = "3KHz"; break;
    case SystemParameters_FILTERS__7_5KHz: strIFFilter = "7.5KHz"; break;
    case SystemParameters_FILTERS__2KHz: strIFFilter = "2KHz"; break;
    case SystemParameters_FILTERS__1KHz: strIFFilter = "1KHz"; break;
    case SystemParameters_FILTERS__125KHz: strIFFilter = "125KHz"; break;
    case SystemParameters_FILTERS__250KHz: strIFFilter = "250KHz"; break;
    default:
        strIFFilter = "250KHz";
        break;
    }
    if (socket.isMainSystem) {
        menuLyout->getHomeTab()->getBtnIFFilter()->setSelectedItem(strIFFilter);
    }
    for (auto *page : socket.pages)
        page->onIFFilterChanged(strIFFilter);
}

void MainWindow::onRFHeadChanged(SocketStruct socket, SystemParameters_RFHEAD num)
{
    QString strRFHead;
    QString shortRFHead;
    switch (num) {
    case SystemParameters_RFHEAD_HL: shortRFHead="HL"; strRFHead = "High Linearity"; break;
    case SystemParameters_RFHEAD_HS: shortRFHead="HS"; strRFHead = "High Sensitivity"; break;
    case SystemParameters_RFHEAD_Int: shortRFHead="INT"; strRFHead = "Intermediate"; break;
    case SystemParameters_RFHEAD_Attenuator: shortRFHead="ATT"; strRFHead = "Attenator"; break;
    default: strRFHead = "";  break;
    }
    if (socket.isMainSystem) {
        menuLyout->getHomeTab()->getBtnRFHead()->setSelectedItem(shortRFHead);
    }
    for (auto *page : socket.pages)
        page->onRFHeadChanged(strRFHead);
}

void MainWindow::onFrequencyChanged(double freq)
{
    auto *e = entryOf(sender());
    if (!e) return;
    if (e->isMainSystem) {
        menuLyout->getHomeTab()->getFreqBox()->setFrequency(freq);
    }
    for (auto *page : e->pages)
        page->onFrequencyChanged(freq);

}

void MainWindow::onThresholdChanged(double thr)
{
    auto *e = entryOf(sender());
    if (!e) return;
    for (auto *page : e->pages)
        page->onThresholdChanged(thr);
}

void MainWindow::slotThemeChange()
{
    style = "padding : 0 ; ";
    style += "margin : 0 ; ";
    style += "color : " + gVar->getClrPrimary() + " ; ";
    style += "background : " + gVar->getClrBackground() + " ; " ;
    setStyleSheet(style);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    gVar->setScreenWidth(width());
    gVar->setScreenHeight(height());
    if (menuLyout) {
        menuLyout->setFixedHeight(gVar->getMenuLyoutHeight());
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == leftLyout) {
        if (event->type() == QEvent::Enter) {
            leftLyoutEnter();
            return true;
        }
        if (event->type() == QEvent::Leave && !isLeftMenuOpen) {
            leftLyoutLeave();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
