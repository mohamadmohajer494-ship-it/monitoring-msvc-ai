#ifndef CONNECTIONWIN_H
#define CONNECTIONWIN_H

#include <QWidget>
#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include "Includes/globalvariables.h"
#include "Widgets/btntoggle.h"

class ConnectionWin : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectionWin(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~ConnectionWin() override;

private:
    GlobalVariables *gVar;
    QString style;
    QVBoxLayout *vLayout;
    QWidget *btnSystem;
    QFrame *btnMn;
    QLabel *lblMn;
    QFrame *btnDf;
    QLabel *lblDf;
    QFrame *btnHo;
    QLabel *lblHo;
    EnumSystemMode sytemMode = Role_Monitoring;
    QGroupBox *gbSystem;
    QGroupBox *gbScan;
    QGroupBox *gbLocation;
    QList<QLabel*> lblList;
    QList<QLineEdit*> txtList;
    const EnumSystemMode _defMode = Role_Monitoring;
    const QString _defName = "New Station";
    const QString _defSystemIp = "192.168.0.10";
    const QString _defScanIp = "192.168.0.11";
    const QString _defMac = "1A:2B:3C:4D:5E:6F";
    const EnumConnectionType _defConnection = TCP_TCP;
    const int _defTcpPort = 8085 ;
    const int _defOcxPort = 8085 ;
    const int _defUdpSendPort = 8088 ;
    const int _defUdpReceivePort = 1024 ;
    const QString _defSerPort = "COM3" ;
    const int _defSerBaud = 115200 ;
    const QString _defLat = "35-41-21" ;
    const QString _defLong = "51-23-20" ;
    QComboBox *cmdType;
    QComboBox *cmdTypeScan;
    BtnToggle *btnLocation;
    QComboBox *cmdTypeLoc;
    QComboBox *cmdLatLoc;
    QComboBox *cmdLongLoc;
    QPushButton *btnConnect;

    bool isValid = true;
    int stationId = -1;
    StationModel *station = nullptr;
    bool isEditMode = false;

private:
    void createBtnSystem();
    void createBoxSystem();
    void createBoxScan();
    void createBoxLocation();
    void calcPosition(StationModel *station);
    bool checkValidateFields();

public:
    QPushButton* getBtnConnect() { return btnConnect; }
    void setEditMode(int id);
    void resetDefault();

public slots:
    void slotThemeChange();

private slots:
    void changeSystemCmdTypeIndex(int num);
    void changeScanCmdTypeIndex(int num);
    void changeLocationOnline(bool check);
    void changeLocationTypeIndex(int num);
    void clickBtnConnect();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void clickAddNewStation(StationModel*);
    void clickEditNewStation(StationModel*);
};

#endif // CONNECTIONWIN_H
