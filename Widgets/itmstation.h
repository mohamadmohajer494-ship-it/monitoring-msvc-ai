#ifndef ITMSTATION_H
#define ITMSTATION_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QCheckBox>
#include <QEnterEvent>
#include <QEvent>
#include <QPainter>
#include <QWidgetAction>
#include "Includes/globalvariables.h"

class ItmStation : public QFrame
{
    Q_OBJECT
public:
    explicit ItmStation(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~ItmStation() override;

private:
    GlobalVariables *gVar;
    QString style;
    QHBoxLayout *hLayout;
    QVBoxLayout *vLayout;
    QGridLayout *gLayout;
    QString icon = ":/Res/Images/Monitoring.png";
    QString name = "Name";
    QString ip = "192.168.0.10";
    QString dir = ":/Res/Images/DF.png";
    QPixmap dfPixmap;
    int dfAngle = 0;
    QLabel *lblIcon;
    QLabel *lblName;
    QLabel *lblIp;
    QLabel *lblDf;
    QCheckBox *chkConnect;
    QWidget *statusCircle;
    bool isShowDir = false;
    bool isAvailable = false;
    StationModel *station;
    int iID = 0;
    bool isConnected = false;
    EnumStatinStatus systemStatus = Statin_Unavailable;

public:
    StationModel* getStation() { return station; }
    void setStation(StationModel *item) { station = item; }
    int getId() { return iID; }
    void setId(int id) { iID = id; }
    QString getIp() { return ip; }
    void setOpen(bool act);
    void setIcon(QString txt);
    void setName(QString txt);
    void setIp(QString txt);
    void setDirection(int angle = -1);
    void setAvailable(bool check);
    int getConnected() { return isConnected; }
    void setConnected(bool check);
    EnumStatinStatus getSystemStatus() { return systemStatus; }
    void setSystemStatus(EnumStatinStatus status);

private:
    void createStatusCircle();
    void updateStatusCircle();

public slots:
    void slotThemeChange();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void openRightMenu(bool open);
    void clickRightMenuConnect(int stationId);
    void clickRightMenuClose(int stationId);
    void clickRightMenuEdit(int stationId);
    void clickRightMenuDelete(int stationId);
};

#endif // ITMSTATION_H
