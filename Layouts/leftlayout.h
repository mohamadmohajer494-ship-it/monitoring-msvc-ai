#ifndef LEFTLAYOUT_H
#define LEFTLAYOUT_H

#include <QWidget>
#include <QFrame>
#include <QScrollArea>
#include <QProcess>
#include <QTimer>
#include "Includes/globalvariables.h"
#include "Widgets/itmstation.h"

class LeftLayout : public QFrame
{
    Q_OBJECT

public:
    explicit LeftLayout(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~LeftLayout() override;

private:
    GlobalVariables *gVar;
    QString style;
    QVBoxLayout *vLayout;
    QScrollArea *scrollArea;
    QWidget *containerWidget;
    QList<ItmStation*> itemList;
    QTimer *pingTimer;
    int pingTimerInterval = 1000;
    QProcess *currentPingProcess = nullptr;
    QQueue<ItmStation*> pingQueue;
    bool isPingInProgress = false;
    bool isShuttingDown = false;

private:
    void startNextPing();

public slots:
    void loadAllStation();
    void loadNewStation(StationModel *station);
    void updateStation(StationModel *station);
    void clearStations();
    void setOpen(bool act);
    ItmStation* findItemById(int id);
    void slotThemeChange();

private slots:
    void tickPingTimer();
    void onPingFinished(int exitCode, QProcess::ExitStatus exitStatus);

signals:
    void openRightMenu(bool open);
    void clickRightMenuConnect(int stationId);
    void clickRightMenuClose(int stationId);
    void clickRightMenuEdit(int stationId);
    void clickRightMenuDelete(int stationId);
    void disconnectConnectedStation(int stationId);
};

#endif // LEFTLAYOUT_H
