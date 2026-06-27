#include "leftlayout.h"

LeftLayout::LeftLayout(GlobalVariables *gVar, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
{
    setFrameShape(QFrame::NoFrame);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    containerWidget = new QWidget();
    vLayout = new QVBoxLayout(containerWidget);
    vLayout->setContentsMargins(5, 5, 5, 5);
    vLayout->setSpacing(7);
    scrollArea->setWidget(containerWidget);
    mainLayout->addWidget(scrollArea);
    currentPingProcess = nullptr;
    isPingInProgress = false;
    pingTimer = new QTimer(this);
    pingTimer->setInterval(pingTimerInterval);
    connect(pingTimer, &QTimer::timeout, this,
            &LeftLayout::tickPingTimer);
    pingTimer->start();
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&LeftLayout::slotThemeChange);
}

LeftLayout::~LeftLayout()
{
    isShuttingDown = true;
    if (pingTimer) {
        pingTimer->stop();
    }
    pingQueue.clear();
    isPingInProgress = false;
    if (currentPingProcess) {
        disconnect(currentPingProcess, nullptr, this, nullptr);
        if (currentPingProcess->state() != QProcess::NotRunning) {
            currentPingProcess->kill();
            currentPingProcess->waitForFinished(3000);
        }
        currentPingProcess->deleteLater();
        currentPingProcess = nullptr;
    }
}

void LeftLayout::startNextPing()
{
    if (isShuttingDown)
        return;
    if (pingQueue.isEmpty()) {
        isPingInProgress = false;
        return;
    }
    isPingInProgress = true;
    ItmStation *item = pingQueue.dequeue();
    if (!item) {
        startNextPing();
        return;
    }
    QString ip = item->getIp();
    if (currentPingProcess) {
        disconnect(currentPingProcess, nullptr, this, nullptr);
        if (currentPingProcess->state() != QProcess::NotRunning) {
            currentPingProcess->kill();
            currentPingProcess->waitForFinished(1000);
        }
        currentPingProcess->deleteLater();
        currentPingProcess = nullptr;
    }
    currentPingProcess = new QProcess(this);
    currentPingProcess->setProperty("itemPtr", QVariant::fromValue(item));
    connect(currentPingProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            &LeftLayout::onPingFinished);
    currentPingProcess->start("ping",
                              QStringList() << "-n" << "1" << "-w" << "2000" << ip);
}

void LeftLayout::loadAllStation()
{
    clearStations();
    const auto &stations = gVar->getStationList();
    for (int i = 0; i < stations.size(); ++i) {
        StationModel *stationModel = stations[i];
        if (!stationModel) continue;
        ItmStation *newStation = new ItmStation(gVar);
        newStation->setStation(stationModel);
        newStation->setId(stationModel->getID());
        switch (stationModel->getSysMode()) {
        case Role_Monitoring:
            newStation->setIcon(":/Res/Images/Monitoring.png");
            break;
        case Role_DF:
            newStation->setIcon(":/Res/Images/DF.png");
            break;
        case Role_HandOff:
            newStation->setIcon(":/Res/Images/HandOff.png");
            break;
        default:
            break;
        }
        connect(newStation, &ItmStation::openRightMenu,
                this, &LeftLayout::openRightMenu);
        connect(newStation, &ItmStation::clickRightMenuConnect,
                this, &LeftLayout::clickRightMenuConnect);
        connect(newStation, &ItmStation::clickRightMenuClose,
                this, &LeftLayout::clickRightMenuClose);
        connect(newStation, &ItmStation::clickRightMenuEdit,
                this, &LeftLayout::clickRightMenuEdit);
        connect(newStation, &ItmStation::clickRightMenuDelete,
                this, &LeftLayout::clickRightMenuDelete);
        newStation->setName(stationModel->getSysName());
        newStation->setIp(stationModel->getSysIp());
        newStation->setDirection(-1);
        newStation->setFixedHeight(50);
        itemList.append(newStation);
        vLayout->addWidget(newStation);
    }
    vLayout->addStretch();
}

void LeftLayout::loadNewStation(StationModel *station)
{
    ItmStation *newStation = new ItmStation(gVar);
    newStation->setStation(station);
    newStation->setId(station->getID());
    switch (station->getSysMode()) {
    case Role_Monitoring:
        newStation->setIcon(":/Res/Images/Monitoring.png");
        break;
    case Role_DF:
        newStation->setIcon(":/Res/Images/DF.png");
        break;
    case Role_HandOff:
        newStation->setIcon(":/Res/Images/HandOff.png");
        break;
    default:
        break;
    }
    newStation->setName(station->getSysName());
    newStation->setIp(station->getSysIp());
    newStation->setDirection(-1);
    newStation->setFixedHeight(50);
    itemList.append(newStation);
    if (vLayout->count() > 0)
        vLayout->insertWidget(vLayout->count() - 1, newStation);
    else {
        vLayout->addWidget(newStation);
        vLayout->addStretch();
    }
}

void LeftLayout::updateStation(StationModel *station)
{
    if (!station) return;
    for (int i = 0; i < itemList.size(); ++i) {
        if (itemList.at(i)->getId() == station->getID()) {
            itemList.at(i)->setStation(station);
            switch (station->getSysMode()) {
            case Role_Monitoring:
                itemList.at(i)->setIcon(":/Res/Images/Monitoring.png");
                break;
            case Role_DF:
                itemList.at(i)->setIcon(":/Res/Images/DF.png");
                break;
            case Role_HandOff:
                itemList.at(i)->setIcon(":/Res/Images/HandOff.png");
                break;
            default:
                break;
            }
            itemList.at(i)->setName(station->getSysName());
            itemList.at(i)->setIp(station->getSysIp());
            itemList.at(i)->update();
            break;
        }
    }
}


void LeftLayout::clearStations()
{
    pingQueue.clear();
    isPingInProgress = false;
    if (currentPingProcess) {
        disconnect(currentPingProcess, nullptr, this, nullptr);
        if (currentPingProcess->state() != QProcess::NotRunning) {
            currentPingProcess->kill();
            currentPingProcess->waitForFinished(1000);
        }
        currentPingProcess->deleteLater();
        currentPingProcess = nullptr;
    }
    QLayoutItem *child;
    while ((child = vLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }
    itemList.clear();
}


void LeftLayout::setOpen(bool act)
{
    for (int var = 0; var < itemList.size(); ++var) {
        itemList[var]->setOpen(act);
    }
    if (act)
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    else
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

ItmStation* LeftLayout::findItemById(int id)
{
    for (ItmStation *item : itemList) {
        if (!item)
            continue;
        if (item->getId() == id) {
            return item;
        }
    }
    return nullptr;
}

void LeftLayout::slotThemeChange()
{
    style = "LeftLayout { ";
    style += "border-right: 2px solid " + gVar->getClrGlow() + ";";
    style += " } ";
    setStyleSheet(style);
    style = "QScrollArea { ";
    style += "background: " + gVar->getClrBackground() + "; ";
    style += "border: none; ";
    style += "} ";
    style += "QScrollBar:vertical { ";
    style += "background: " + gVar->getClrBackground() + "; ";
    style += "width: 10px; ";
    style += "border: none; ";
    style += "border-radius: 5px; ";
    style += "margin: 2px; ";
    style += "} ";
    style += "QScrollBar::handle:vertical { ";
    style += "background: " + gVar->getClrGlow() + "; ";
    style += "min-height: 30px; ";
    style += "border-radius: 5px; ";
    style += "} ";
    style += "QScrollBar::handle:vertical:hover { ";
    style += "background: " + gVar->getClrHighlight() + "; ";
    style += "} ";
    style += "QScrollBar::handle:vertical:pressed { ";
    style += "background: " + gVar->getClrSecondary() + "; ";
    style += "} ";
    style += "QScrollBar::add-line:vertical, ";
    style += "QScrollBar::sub-line:vertical { ";
    style += "height: 0px; ";
    style += "background: none; ";
    style += "} ";
    style += "QScrollBar::add-page:vertical, ";
    style += "QScrollBar::sub-page:vertical { ";
    style += "background: none; ";
    style += "} ";
    scrollArea->setStyleSheet(style);
}

void LeftLayout::tickPingTimer()
{
    if (itemList.isEmpty())
        return;
    if (isPingInProgress)
        return;
    pingQueue.clear();
    for (auto &item : itemList) {
        pingQueue.enqueue(item);
    }
    startNextPing();
}

void LeftLayout::onPingFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (isShuttingDown)
        return;
    QProcess *proc = qobject_cast<QProcess*>(sender());
    if (!proc)
        return;
    ItmStation *item = proc->property("itemPtr").value<ItmStation*>();
    QString output = QString::fromLocal8Bit(proc->readAllStandardOutput());
    output += QString::fromLocal8Bit(proc->readAllStandardError());
    bool hasReply =
        output.contains("TTL=", Qt::CaseInsensitive) ||
        output.contains("bytes=", Qt::CaseInsensitive);
    bool hasFailed =
        output.contains("Request timed out", Qt::CaseInsensitive) ||
        output.contains("Destination host unreachable", Qt::CaseInsensitive) ||
        output.contains("could not find host", Qt::CaseInsensitive) ||
        output.contains("General failure", Qt::CaseInsensitive);
    bool isReachable =
        exitStatus == QProcess::NormalExit &&
        exitCode == 0 &&
        hasReply &&
        !hasFailed;
    if (item) {
        item->setAvailable(isReachable);
        if (!isReachable && item->getSystemStatus() == Statin_Connected) {
            emit disconnectConnectedStation(item->getId());
        }
    }
    if (proc == currentPingProcess) {
        disconnect(currentPingProcess, nullptr, this, nullptr);
        currentPingProcess->deleteLater();
        currentPingProcess = nullptr;
    }
    startNextPing();
}

