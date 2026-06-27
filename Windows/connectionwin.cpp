#include "connectionwin.h"

ConnectionWin::ConnectionWin(GlobalVariables *gVar, QWidget *parent)
    : QDialog{parent}
    , gVar(gVar)
{
    setWindowTitle("Add or Edit Station");
    setFixedWidth(600);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    vLayout = new QVBoxLayout();
    setLayout(vLayout);
    createBtnSystem();
    gbSystem = new QGroupBox();
    gbSystem->setTitle(tr("System"));
    gbSystem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    vLayout->addWidget(gbSystem);
    createBoxSystem();
    gbScan = new QGroupBox();
    gbScan->setTitle(tr("Scan"));
    gbScan->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    vLayout->addWidget(gbScan);
    createBoxScan();
    gbLocation = new QGroupBox();
    gbLocation->setTitle(tr("Location"));
    gbLocation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    vLayout->addWidget(gbLocation);
    createBoxLocation();
    QHBoxLayout *hBtnLayout = new QHBoxLayout();
    hBtnLayout->addStretch();
    btnConnect = new QPushButton(tr("Connect"));
    btnConnect->setFixedSize(200,35);
    btnConnect->setCursor(Qt::PointingHandCursor);
    connect(btnConnect,&QPushButton::clicked,
            this,&ConnectionWin::clickBtnConnect);
    hBtnLayout->addWidget(btnConnect);
    vLayout->addLayout(hBtnLayout);
    resize(sizeHint());
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&ConnectionWin::slotThemeChange);
}

ConnectionWin::~ConnectionWin()
{

}

void ConnectionWin::createBtnSystem()
{
    btnSystem = new QWidget();
    btnSystem->setFixedSize(150,40);
    vLayout->addWidget(btnSystem);
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    btnSystem->setLayout(hLayout);
    btnMn = new QFrame();
    btnMn->setCursor(Qt::PointingHandCursor);
    btnMn->setObjectName("btnMn");
    btnMn->setToolTip(tr("Monitoring"));
    hLayout->addWidget(btnMn);
    QVBoxLayout *mnLayout = new QVBoxLayout();
    mnLayout->setContentsMargins(0, 0, 0, 0);
    mnLayout->setSpacing(0);
    btnMn->setLayout(mnLayout);
    lblMn = new QLabel();
    mnLayout->addWidget(lblMn, 0, Qt::AlignCenter);

    btnDf = new QFrame();
    btnDf->setCursor(Qt::PointingHandCursor);
    btnDf->setObjectName("btnDf");
    btnDf->setToolTip(tr("DF"));
    hLayout->addWidget(btnDf);
    QVBoxLayout *dfLayout = new QVBoxLayout();
    dfLayout->setContentsMargins(0, 0, 0, 0);
    dfLayout->setSpacing(0);
    btnDf->setLayout(dfLayout);
    lblDf = new QLabel();
    dfLayout->addWidget(lblDf, 0, Qt::AlignCenter);

    btnHo = new QFrame();
    btnHo->setCursor(Qt::PointingHandCursor);
    btnHo->setObjectName("btnHo");
    btnHo->setToolTip(tr("HandOff"));
    hLayout->addWidget(btnHo);
    QVBoxLayout *hoLayout = new QVBoxLayout();
    hoLayout->setContentsMargins(0, 0, 0, 0);
    hoLayout->setSpacing(0);
    btnHo->setLayout(hoLayout);
    lblHo = new QLabel();
    hoLayout->addWidget(lblHo, 0, Qt::AlignCenter);
}

void ConnectionWin::createBoxSystem()
{
    QGridLayout *gLayout = new QGridLayout();
    gbSystem->setLayout(gLayout);
    QLabel *lblName = new QLabel(tr("Station Name : "));
    lblName->setObjectName("lblName");
    lblList.append(lblName);
    gLayout->addWidget(lblName,0,0);
    QLineEdit *txtName = new QLineEdit("New Station");
    txtName->setObjectName("txtName");
    txtList.append(txtName);
    gLayout->addWidget(txtName,0,1);

    QLabel *lblIp = new QLabel(tr("IP Address : "));
    lblIp->setObjectName("lblIp");
    lblList.append(lblIp);
    gLayout->addWidget(lblIp,1,0);
    QLineEdit *txtIp = new QLineEdit("192.168.0.10");
    txtIp->setObjectName("txtIp");
    txtList.append(txtIp);
    gLayout->addWidget(txtIp,1,1);

    QLabel *lblMac = new QLabel(tr("Mac Address : "));
    lblMac->setObjectName("lblMac");
    lblList.append(lblMac);
    gLayout->addWidget(lblMac,1,2);
    QLineEdit *txtMac = new QLineEdit("1A:2B:3C:4D:5E:6F");
    txtMac->setObjectName("txtMac");
    txtList.append(txtMac);
    gLayout->addWidget(txtMac,1,3);

    QLabel *lblType = new QLabel(tr("Connection Type : "));
    lblType->setObjectName("lblType");
    lblList.append(lblType);
    gLayout->addWidget(lblType,2,0);
    cmdType = new QComboBox();
    cmdType->addItem("OCX");
    cmdType->addItem("TCP");
    cmdType->addItem("UDP/UDP");
    cmdType->addItem("SER/UDP");
    cmdType->addItem("SER/SER");
    connect(cmdType,&QComboBox::currentIndexChanged,
            this,&ConnectionWin::changeSystemCmdTypeIndex);
    gLayout->addWidget(cmdType,2,1);

    QLabel *lblPort1 = new QLabel(tr("Port1 : "));
    lblPort1->setObjectName("lblPort1");
    lblList.append(lblPort1);
    gLayout->addWidget(lblPort1,3,0);
    QLineEdit *txtPort1 = new QLineEdit("Port1");
    txtPort1->setObjectName("txtPort1");
    txtList.append(txtPort1);
    gLayout->addWidget(txtPort1,3,1);

    QLabel *lblPort2 = new QLabel(tr("Port2 : "));
    lblPort2->setObjectName("lblPort2");
    lblList.append(lblPort2);
    gLayout->addWidget(lblPort2,3,2);
    QLineEdit *txtPort2 = new QLineEdit("Port2");
    txtPort2->setObjectName("txtPort2");
    txtList.append(txtPort2);
    gLayout->addWidget(txtPort2,3,3);

    QLabel *lblPort3 = new QLabel(tr("Port3 : "));
    lblPort3->setObjectName("lblPort3");
    lblList.append(lblPort3);
    gLayout->addWidget(lblPort3,4,0);
    QLineEdit *txtPort3 = new QLineEdit("Port3");
    txtPort3->setObjectName("txtPort3");
    txtList.append(txtPort3);
    gLayout->addWidget(txtPort3,4,1);

    QLabel *lblPort4 = new QLabel(tr("Port4 : "));
    lblPort4->setObjectName("lblPort4");
    lblList.append(lblPort4);
    gLayout->addWidget(lblPort4,4,2);
    QLineEdit *txtPort4 = new QLineEdit("Port4");
    txtPort4->setObjectName("txtPort4");
    txtList.append(txtPort4);
    gLayout->addWidget(txtPort4,4,3);
    changeSystemCmdTypeIndex(1);

}

void ConnectionWin::createBoxScan()
{
    QGridLayout *gLayout = new QGridLayout();
    gbScan->setLayout(gLayout);
    QLabel *lblIp = new QLabel();
    lblIp->setText(tr("IP Address : "));
    lblIp->setObjectName("lblScanIp");
    lblList.append(lblIp);
    gLayout->addWidget(lblIp,0,0);
    QLineEdit *txtIp = new QLineEdit("192.168.0.11");
    txtIp->setObjectName("txtScanIp");
    txtList.append(txtIp);
    gLayout->addWidget(txtIp,0,1);

    QLabel *lblMac = new QLabel();
    lblMac->setText(tr("Mac Address : "));
    lblMac->setObjectName("lblScanMac");
    lblList.append(lblMac);
    gLayout->addWidget(lblMac,0,2);
    QLineEdit *txtMac = new QLineEdit("1A:2B:3C:4D:5E:6F");
    txtMac->setObjectName("txtScanMac");
    txtList.append(txtMac);
    gLayout->addWidget(txtMac,0,3);

    QLabel *lblType = new QLabel();
    lblType->setText(tr("Connection Type : "));
    lblType->setObjectName("lblScanType");
    lblList.append(lblType);
    gLayout->addWidget(lblType,1,0);
    cmdTypeScan = new QComboBox();
    cmdTypeScan->addItem("OCX");
    cmdTypeScan->addItem("TCP");
    cmdTypeScan->addItem("UDP/UDP");
    cmdTypeScan->addItem("SER/UDP");
    cmdTypeScan->addItem("SER/SER");
    connect(cmdTypeScan,&QComboBox::currentIndexChanged,
            this,&ConnectionWin::changeScanCmdTypeIndex);
    gLayout->addWidget(cmdTypeScan,1,1);

    QLabel *lblPort1 = new QLabel();
    lblPort1->setText(tr("Port1 : "));
    lblPort1->setObjectName("lblScanPort1");
    lblList.append(lblPort1);
    gLayout->addWidget(lblPort1,2,0);
    QLineEdit *txtPort1 = new QLineEdit("Port1");
    txtPort1->setObjectName("txtScanPort1");
    txtList.append(txtPort1);
    gLayout->addWidget(txtPort1,2,1);

    QLabel *lblPort2 = new QLabel();
    lblPort2->setText(tr("Port2 : "));
    lblPort2->setObjectName("lblScanPort2");
    lblList.append(lblPort2);
    gLayout->addWidget(lblPort2,2,2);
    QLineEdit *txtPort2 = new QLineEdit("Port2");
    txtPort2->setObjectName("txtScanPort2");
    txtList.append(txtPort2);
    gLayout->addWidget(txtPort2,2,3);

    QLabel *lblPort3 = new QLabel();
    lblPort3->setText(tr("Port3 : "));
    lblPort3->setObjectName("lblScanPort3");
    lblList.append(lblPort3);
    gLayout->addWidget(lblPort3,3,0);
    QLineEdit *txtPort3 = new QLineEdit("Port3");
    txtPort3->setObjectName("txtScanPort3");
    txtList.append(txtPort3);
    gLayout->addWidget(txtPort3,3,1);

    QLabel *lblPort4 = new QLabel();
    lblPort4->setText(tr("Port4 : "));
    lblPort4->setObjectName("lblScanPort4");
    lblList.append(lblPort4);
    gLayout->addWidget(lblPort4,3,2);
    QLineEdit *txtPort4 = new QLineEdit("Port4");
    txtPort4->setObjectName("txtScanPort4");
    txtList.append(txtPort4);
    gLayout->addWidget(txtPort4,3,3);

    changeScanCmdTypeIndex(1);
}

void ConnectionWin::createBoxLocation()
{
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(5);
    gbLocation->setLayout(vLayout);
    QWidget *locBox = new QWidget();
    // vLayout->addWidget(locBox,0,0,1,2);
    vLayout->addWidget(locBox);
    QHBoxLayout *hLayout = new QHBoxLayout();
    locBox->setLayout(hLayout);
    QLabel *lblOff = new QLabel();
    lblOff->setText(tr("Offline"));
    lblOff->setObjectName("lblLocOff");
    lblList.append(lblOff);
    hLayout->addWidget(lblOff);
    btnLocation = new BtnToggle(gVar);
    connect(btnLocation,&BtnToggle::toggled,
            this,&ConnectionWin::changeLocationOnline);
    hLayout->addWidget(btnLocation);
    QLabel *lblOn = new QLabel();
    lblOn->setText(tr("Online"));
    lblOn->setObjectName("lblLocOn");
    lblList.append(lblOn);
    hLayout->addWidget(lblOn);
    hLayout->addStretch();

    QWidget *typeBox = new QWidget();
    // vLayout->addWidget(locBox,0,0,1,2);
    vLayout->addWidget(typeBox);
    QHBoxLayout *tLayout = new QHBoxLayout();
    tLayout->setContentsMargins(6, 0, 0, 0);
    typeBox->setLayout(tLayout);
    QLabel *lblType = new QLabel();
    lblType->setText(tr("Coordinate Type : "));
    lblType->setObjectName("lblLocType");
    lblList.append(lblType);
    // vLayout->addWidget(lblType,1,0);
    tLayout->addWidget(lblType);
    cmdTypeLoc = new QComboBox();
    cmdTypeLoc->addItem("DMS");
    cmdTypeLoc->addItem("UTM");
    cmdTypeLoc->addItem("LatLong");
    cmdTypeLoc->addItem("MGRS");
    connect(cmdTypeLoc,&QComboBox::currentIndexChanged,
            this,&ConnectionWin::changeLocationTypeIndex);
    // vLayout->addWidget(cmdTypeLoc,1,1);
    tLayout->addWidget(cmdTypeLoc);
    tLayout->addStretch();

    QWidget *cordBox = new QWidget();
    cordBox->setFixedWidth(400);
    vLayout->addWidget(cordBox);
    QGridLayout *gLayout = new QGridLayout();
    cordBox->setLayout(gLayout);
    //---Loc01---
    QLabel *lblLoc11 = new QLabel();
    lblLoc11->setText(tr("lblLoc11"));
    lblLoc11->setObjectName("lblLoc11");
    lblList.append(lblLoc11);
    gLayout->addWidget(lblLoc11,2,0);
    cmdLatLoc = new QComboBox();
    cmdLatLoc->addItem("N");
    cmdLatLoc->addItem("S");
    gLayout->addWidget(cmdLatLoc,2,1);
    QLineEdit *txtLoc11 = new QLineEdit("txtLoc11");
    txtLoc11->setObjectName("txtLoc11");
    txtList.append(txtLoc11);
    gLayout->addWidget(txtLoc11,2,2);

    QLabel *lblLoc12 = new QLabel();
    lblLoc12->setText(tr("lblLoc12"));
    lblLoc12->setObjectName("lblLoc12");
    lblList.append(lblLoc12);
    gLayout->addWidget(lblLoc12,2,3);
    QLineEdit *txtLoc12 = new QLineEdit("txtLoc12");
    txtLoc12->setObjectName("txtLoc12");
    txtList.append(txtLoc12);
    gLayout->addWidget(txtLoc12,2,4);

    QLabel *lblLoc13 = new QLabel();
    lblLoc13->setText(tr("lblLoc13"));
    lblLoc13->setObjectName("lblLoc13");
    lblList.append(lblLoc13);
    gLayout->addWidget(lblLoc13,2,5);
    QLineEdit *txtLoc13 = new QLineEdit("txtLoc13");
    txtLoc13->setObjectName("txtLoc13");
    txtList.append(txtLoc13);
    gLayout->addWidget(txtLoc13,2,6);
    //---Loc02---
    QLabel *lblLoc21 = new QLabel();
    lblLoc21->setText(tr("lblLoc21"));
    lblLoc21->setObjectName("lblLoc21");
    lblList.append(lblLoc21);
    gLayout->addWidget(lblLoc21,3,0);
    cmdLongLoc = new QComboBox();
    cmdLongLoc->addItem("E");
    cmdLongLoc->addItem("W");
    gLayout->addWidget(cmdLongLoc,3,1);
    QLineEdit *txtLoc21 = new QLineEdit("txtLoc21");
    txtLoc21->setObjectName("txtLoc21");
    txtList.append(txtLoc21);
    gLayout->addWidget(txtLoc21,3,2);

    QLabel *lblLoc22 = new QLabel();
    lblLoc22->setText(tr("lblLoc22"));
    lblLoc22->setObjectName("lblLoc22");
    lblList.append(lblLoc22);
    gLayout->addWidget(lblLoc22,3,3);
    QLineEdit *txtLoc22 = new QLineEdit("txtLoc22");
    txtLoc22->setObjectName("txtLoc22");
    txtList.append(txtLoc22);
    gLayout->addWidget(txtLoc22,3,4);

    QLabel *lblLoc23 = new QLabel();
    lblLoc23->setText(tr("lblLoc23"));
    lblLoc23->setObjectName("lblLoc23");
    lblList.append(lblLoc23);
    gLayout->addWidget(lblLoc23,3,5);
    QLineEdit *txtLoc23 = new QLineEdit("txtLoc23");
    txtLoc23->setObjectName("txtLoc23");
    txtList.append(txtLoc23);
    gLayout->addWidget(txtLoc23,3,6);

    changeLocationTypeIndex(0);
}

void ConnectionWin::calcPosition(StationModel *station)
{
    station->setPosType(static_cast<EnumPositionType>(cmdTypeScan->currentIndex()));
    switch (station->getPosType()) {
    case DMS:

        break;
    case UTM:

        break;
    case LatLong:

        break;
    case MGRS:

        break;
    default:
        break;
    }
}

bool ConnectionWin::checkValidateFields()
{
    isValid = true;
    auto setError = [&](QLineEdit *txt) {
        txt->setStyleSheet("border: 2px solid " + gVar->getClrError() + ";");
        isValid = false;
    };
    for (QLineEdit *txt : txtList) {
        if (!txt)
            continue;
        if (txt->text().isEmpty())
            setError(txt);
        else
            txt->setStyleSheet("");
    }
    QVector<StationStruct> result;
    switch (sytemMode) {
    case Role_Monitoring:
        if (txtList[1]->text() == txtList[7]->text())
            setError(txtList[7]);
        result = gVar->getRepStation()->selectByAnyFilter({
            {"name", txtList[0]->text()},
            {"ip", txtList[1]->text()},
            {"ipScan", txtList[7]->text()}
        });
        break;
    case Role_DF:
        result = gVar->getRepStation()->selectByAnyFilter({
            {"name", txtList[0]->text()},
            {"ip", txtList[1]->text()}
        });
        break;
    case Role_HandOff:
        result = gVar->getRepStation()->selectByAnyFilter({
            {"name", txtList[0]->text()},
            {"ip", txtList[1]->text()}
        });
        break;
    default:
        break;
    }
    if (result.isEmpty())
        return isValid;
    const StationStruct &st = result.first();
    auto checkDuplicate = [&](QLineEdit *txt, const QString &value) {
        if (value != txt->text())
            return;
        if (!isEditMode || station->getSysName() != st.name)
            setError(txt);
    };
    checkDuplicate(txtList[0], st.name);
    checkDuplicate(txtList[1], st.ip);
    if (sytemMode == Role_Monitoring)
        checkDuplicate(txtList[7], st.ipScan);
    return isValid;
}


void ConnectionWin::setEditMode(int id)
{
    stationId = id;
    station = gVar->findStationById(stationId);
    if(station)
        isEditMode = true;
    sytemMode = station->getSysMode();
    switch (sytemMode) {
    case Role_Monitoring:
        gbSystem->setVisible(true);
        gbScan->setVisible(true);
        break;
    case Role_DF:
        gbSystem->setVisible(true);
        gbScan->setVisible(false);
        break;
    case Role_HandOff:
        gbSystem->setVisible(true);
        gbScan->setVisible(false);
        break;
    default:
        break;
    }
    slotThemeChange();
    vLayout->invalidate();
    adjustSize();
    txtList[0]->setText(station->getSysName());
    txtList[1]->setText(station->getSysIp());
    txtList[2]->setText(station->getSysMac());
    cmdType->setCurrentIndex(station->getSysConnectionType());
    switch (station->getSysConnectionType()) {
    case TCP_TCP:
        txtList[3]->setText(QString::number(station->getPortTcp()));
        break;
    case OCX_OCX:
        txtList[3]->setText(QString::number(station->getPortOcx()));
        break;
    case UDP_UDP:
        txtList[3]->setText(QString::number(station->getPortUdpSend()));
        txtList[4]->setText(QString::number(station->getPortUdpReceive()));
        break;
    case SER_UDP:
        txtList[3]->setText(station->getPortSerCom());
        txtList[4]->setText(QString::number(station->getPortSerBaudRate()));
        txtList[5]->setText(QString::number(station->getPortUdpReceive()));
        break;
    case SER_SER:
        txtList[3]->setText(station->getPortSerCom());
        txtList[4]->setText(QString::number(station->getPortSerBaudRate()));
        break;
    default:
        break;
    }
    txtList[7]->setText(station->getScanIp());
    txtList[8]->setText(station->getScanMac());
    cmdTypeScan->setCurrentIndex(station->getScanConnectionType());
    switch (station->getScanConnectionType()) {
    case TCP_TCP:
        txtList[9]->setText(QString::number(station->getPortTcp()));
        break;
    case OCX_OCX:
        txtList[9]->setText(QString::number(station->getPortOcx()));
        break;
    case UDP_UDP:
        txtList[9]->setText(QString::number(station->getPortUdpSend()));
        txtList[10]->setText(QString::number(station->getPortUdpReceive()));
        break;
    case SER_UDP:
        txtList[9]->setText(station->getPortSerCom());
        txtList[10]->setText(QString::number(station->getPortSerBaudRate()));
        txtList[11]->setText(QString::number(station->getPortUdpReceive()));
        break;
    case SER_SER:
        txtList[9]->setText(station->getPortSerCom());
        txtList[10]->setText(QString::number(station->getPortSerBaudRate()));
        break;
    default:
        break;
    }
    btnLocation->setStatus(station->getGps());
    cmdTypeLoc->setCurrentIndex(station->getPosType());
    // cmdLatLoc->setCurrentIndex(0);
    // QStringList latParts = _defLat.split("-");
    // txtList[13]->setText(latParts[0]);
    // txtList[14]->setText(latParts[1]);
    // txtList[15]->setText(latParts[2]);
    // cmdLongLoc->setCurrentIndex(0);
    // QStringList longParts = _defLong.split("-");
    // txtList[16]->setText(longParts[0]);
    // txtList[17]->setText(longParts[1]);
    // txtList[18]->setText(longParts[2]);
    btnConnect->setText(tr("Update"));
}

void ConnectionWin::resetDefault()
{
    stationId = -1;
    isEditMode = false;
    sytemMode = Role_Monitoring;
    gbSystem->setVisible(true);
    gbScan->setVisible(true);
    slotThemeChange();
    vLayout->invalidate();
    adjustSize();
    txtList[0]->setText(_defName);
    txtList[1]->setText(_defSystemIp);
    txtList[2]->setText(_defMac);
    cmdType->setCurrentIndex(1);
    txtList[3]->setText(QString::number(_defTcpPort));
    txtList[4]->setText(QString::number(_defUdpReceivePort));
    txtList[5]->setText(QString::number(_defUdpReceivePort));
    txtList[6]->setText(QString::number(_defUdpReceivePort));
    txtList[7]->setText(_defScanIp);
    txtList[8]->setText(_defMac);
    cmdTypeScan->setCurrentIndex(1);
    txtList[9]->setText(QString::number(_defTcpPort));
    txtList[10]->setText(QString::number(_defUdpReceivePort));
    txtList[11]->setText(QString::number(_defUdpReceivePort));
    txtList[12]->setText(QString::number(_defUdpReceivePort));
    btnLocation->setStatus(false);
    cmdTypeLoc->setCurrentIndex(0);
    cmdLatLoc->setCurrentIndex(0);
    QStringList latParts = _defLat.split("-");
    txtList[13]->setText(latParts[0]);
    txtList[14]->setText(latParts[1]);
    txtList[15]->setText(latParts[2]);
    cmdLongLoc->setCurrentIndex(0);
    QStringList longParts = _defLong.split("-");
    txtList[16]->setText(longParts[0]);
    txtList[17]->setText(longParts[1]);
    txtList[18]->setText(longParts[2]);
    btnConnect->setText(tr("Connect"));
}

void ConnectionWin::slotThemeChange()
{
    style = "QDialog { ";
    style += "padding : 0 ; ";
    style += "margin : 0 ; ";
    style += "color : " + gVar->getClrPrimary() + " ; ";
    style += "background : " + gVar->getClrBackground() + " ; " ;
    style += " } ";
    style += "#btnMn { ";
    style += "border-top-left-radius : 5px ; ";
    style += "border-bottom-left-radius : 5px ; ";
    if (sytemMode == Role_Monitoring)
        style += "background : " + gVar->getClrGlow() + " ; " ;
    else
        style += "background : " + gVar->getClrSurface() + " ; " ;
    style += " } ";
    style += "#btnDf { ";
    if (sytemMode == Role_DF)
        style += "background : " + gVar->getClrGlow() + " ; " ;
    else
        style += "background : " + gVar->getClrSurface() + " ; " ;
    style += " } ";
    style += "#btnHo { ";
    style += "border-top-right-radius : 5px ; ";
    style += "border-bottom-right-radius : 5px ; ";
    if (sytemMode == Role_HandOff)
        style += "background : " + gVar->getClrGlow() + " ; " ;
    else
        style += "background : " + gVar->getClrSurface() + " ; " ;
    style += " } ";
    style += "QGroupBox { ";
    style += "border: 2px solid " + gVar->getClrPrimary() + " ; ";
    style += "border-radius: 8px ; ";
    style += "margin-top: 12px ; ";
    style += "padding: 8px ; ";
    style += " } ";
    style += "QGroupBox::title { ";
    style += "subcontrol-origin: margin ; ";
    style += "subcontrol-position: top left ; ";
    style += "padding: 0 8px ; ";
    style += "left: 10px ; ";
    style += "color: " + gVar->getClrPrimary() + " ; ";
    style += " } ";
    setStyleSheet(style);
    lblMn->setPixmap(gVar->changeColorIcon(":/Res/Images/Monitoring.png",
                                           gVar->getClrPrimary(),
                                           QSize(20, 20)));
    lblDf->setPixmap(gVar->changeColorIcon(":/Res/Images/DF.png",
                                           gVar->getClrPrimary(),
                                           QSize(20, 20)));
    lblHo->setPixmap(gVar->changeColorIcon(":/Res/Images/HandOff.png",
                                           gVar->getClrPrimary(),
                                           QSize(20, 20)));
}

void ConnectionWin::changeSystemCmdTypeIndex(int num)
{
    lblList[5]->setVisible(false);
    txtList[4]->setVisible(false);
    lblList[6]->setVisible(false);
    txtList[5]->setVisible(false);
    lblList[7]->setVisible(false);
    txtList[6]->setVisible(false);
    switch (num) {
    case 0:
        lblList[4]->setText("OCX Port : ");
        txtList[3]->setText(QString::number(_defOcxPort));
        break;
    case 1:
        lblList[4]->setText("TCP Port : ");
        txtList[3]->setText(QString::number(_defTcpPort));
        break;
    case 2:
        lblList[4]->setText("UDP Send Port : ");
        txtList[3]->setText(QString::number(_defUdpSendPort));
        lblList[5]->setText("UDP Receive Port : ");
        txtList[4]->setText(QString::number(_defUdpReceivePort));
        lblList[5]->setVisible(true);
        txtList[4]->setVisible(true);
        break;
    case 3:
        lblList[4]->setText("Ser Port : ");
        txtList[3]->setText(_defSerPort);
        lblList[5]->setText("Baud Rate : ");
        txtList[4]->setText(QString::number(_defSerBaud));
        lblList[6]->setText("UDP Receive Port : ");
        txtList[5]->setText(QString::number(_defUdpReceivePort));
        lblList[5]->setVisible(true);
        txtList[4]->setVisible(true);
        lblList[6]->setVisible(true);
        txtList[5]->setVisible(true);
        break;
    case 4:
        lblList[4]->setText("Ser Port : ");
        txtList[3]->setText(_defSerPort);
        lblList[5]->setText("Baud Rate : ");
        txtList[4]->setText(QString::number(_defSerBaud));
        lblList[5]->setVisible(true);
        txtList[4]->setVisible(true);
        break;
    default:
        break;
    }
    gbSystem->updateGeometry();
    vLayout->invalidate();
    adjustSize();
}

void ConnectionWin::changeScanCmdTypeIndex(int num)
{
    lblList[12]->setVisible(false);
    txtList[10]->setVisible(false);
    lblList[13]->setVisible(false);
    txtList[11]->setVisible(false);
    lblList[14]->setVisible(false);
    txtList[12]->setVisible(false);
    switch (num) {
    case 0:
        lblList[11]->setText("OCX Port : ");
        txtList[9]->setText(QString::number(_defOcxPort));
        break;
    case 1:
        lblList[11]->setText("TCP Port : ");
        txtList[9]->setText(QString::number(_defTcpPort));
        break;
    case 2:
        lblList[11]->setText("UDP Send Port : ");
        txtList[9]->setText(QString::number(_defUdpSendPort));
        lblList[12]->setText("UDP Receive Port : ");
        txtList[10]->setText(QString::number(_defUdpReceivePort));
        lblList[12]->setVisible(true);
        txtList[10]->setVisible(true);
        break;
    case 3:
        lblList[11]->setText("Ser Port : ");
        txtList[9]->setText(_defSerPort);
        lblList[12]->setText("Baud Rate : ");
        txtList[10]->setText(QString::number(_defSerBaud));
        lblList[13]->setText("UDP Receive Port : ");
        txtList[11]->setText(QString::number(_defUdpReceivePort));
        lblList[12]->setVisible(true);
        txtList[10]->setVisible(true);
        lblList[13]->setVisible(true);
        txtList[11]->setVisible(true);
        break;
    case 4:
        lblList[11]->setText("Ser Port : ");
        txtList[9]->setText(_defSerPort);
        lblList[12]->setText("Baud Rate : ");
        txtList[10]->setText(QString::number(_defSerBaud));
        lblList[12]->setVisible(true);
        txtList[10]->setVisible(true);
        break;
    default:
        break;
    }
    gbScan->updateGeometry();
    vLayout->invalidate();
    adjustSize();
}

void ConnectionWin::changeLocationOnline(bool check)
{
    cmdTypeLoc->setEnabled(!check);
    lblList[17]->setEnabled(!check);
    lblList[18]->setEnabled(!check);
    lblList[19]->setEnabled(!check);
    lblList[20]->setEnabled(!check);
    lblList[21]->setEnabled(!check);
    lblList[22]->setEnabled(!check);
    cmdLatLoc->setEnabled(!check);
    cmdLongLoc->setEnabled(!check);
    txtList[13]->setEnabled(!check);
    txtList[14]->setEnabled(!check);
    txtList[15]->setEnabled(!check);
    txtList[16]->setEnabled(!check);
    txtList[17]->setEnabled(!check);
    txtList[18]->setEnabled(!check);
}

void ConnectionWin::changeLocationTypeIndex(int num)
{
    lblList[19]->setVisible(false);
    lblList[20]->setVisible(false);
    lblList[22]->setVisible(false);
    lblList[23]->setVisible(false);
    cmdLatLoc->setVisible(false);
    cmdLongLoc->setVisible(false);
    txtList[14]->setVisible(false);
    txtList[15]->setVisible(false);
    txtList[17]->setVisible(false);
    txtList[18]->setVisible(false);
    switch (num) {
    case 0:
        lblList[18]->setText("Lat : ");
        lblList[21]->setText("Long : ");
        txtList[13]->setText("0");
        txtList[14]->setText("0");
        txtList[15]->setText("0.00");
        txtList[16]->setText("0");
        txtList[17]->setText("0");
        txtList[18]->setText("0.00");
        cmdLatLoc->setVisible(true);
        cmdLongLoc->setVisible(true);
        txtList[14]->setVisible(true);
        txtList[15]->setVisible(true);
        txtList[17]->setVisible(true);
        txtList[18]->setVisible(true);
        break;
    case 1:
        lblList[18]->setText("Easting : ");
        lblList[19]->setText("LatZone : ");
        lblList[21]->setText("Northing : ");
        lblList[22]->setText("LongZone : ");
        txtList[13]->setText("0");
        txtList[14]->setText("N");
        txtList[16]->setText("0");
        txtList[17]->setText("31");
        lblList[19]->setVisible(true);
        lblList[22]->setVisible(true);
        txtList[14]->setVisible(true);
        txtList[17]->setVisible(true);

        break;
    case 2:
        lblList[18]->setText("Lat : ");
        lblList[21]->setText("Long : ");
        txtList[13]->setText("0.0");
        txtList[16]->setText("0.0");
        cmdLatLoc->setVisible(true);
        cmdLongLoc->setVisible(true);
        break;
    case 3:
        lblList[18]->setText("Easting : ");
        lblList[19]->setText("EastingID : ");
        lblList[20]->setText("ZoneID : ");
        lblList[21]->setText("Northing : ");
        lblList[22]->setText("NorthingID : ");
        lblList[23]->setText("ZoneChar : ");
        txtList[13]->setText("0");
        txtList[14]->setText("A");
        txtList[15]->setText("31");
        txtList[16]->setText("0");
        txtList[17]->setText("A");
        txtList[18]->setText("N");
        lblList[19]->setVisible(true);
        lblList[20]->setVisible(true);
        lblList[22]->setVisible(true);
        lblList[23]->setVisible(true);
        txtList[14]->setVisible(true);
        txtList[15]->setVisible(true);
        txtList[17]->setVisible(true);
        txtList[18]->setVisible(true);
        break;
    default:
        break;
    }
}

void ConnectionWin::clickBtnConnect()
{
    if (!checkValidateFields())
        return;
    if (!isEditMode) {
        // --- Data Base ---
        StationStruct newStruct;
        newStruct.mode = sytemMode;
        newStruct.name = txtList[0]->text();
        // newStruct.icon = txtList[1]->text();
        newStruct.ip = txtList[1]->text();
        newStruct.mac = txtList[2]->text();
        newStruct.connection = cmdType->currentIndex();
        newStruct.tcp = _defTcpPort;
        newStruct.ocx = _defOcxPort;
        newStruct.udpSend = _defUdpSendPort;
        newStruct.udpReseive = _defUdpReceivePort;
        newStruct.serialCom = _defSerPort;
        newStruct.serialBaudRate = _defSerBaud;
        switch (newStruct.connection) {
        case 0:
            newStruct.tcp = txtList[3]->text().toInt();
            break;
        case 1:
            newStruct.ocx = txtList[3]->text().toInt();
            break;
        case 2:
            newStruct.udpSend = txtList[3]->text().toInt();
            newStruct.udpReseive = txtList[4]->text().toInt();
            break;
        case 3:
            newStruct.serialCom = txtList[3]->text();
            newStruct.serialBaudRate = txtList[4]->text().toInt();
            newStruct.udpReseive = txtList[5]->text().toInt();
            break;
        case 4:
            newStruct.serialCom = txtList[3]->text();
            newStruct.serialBaudRate = txtList[4]->text().toInt();
            break;
        default:
            break;
        }

        newStruct.ipScan = txtList[7]->text();
        newStruct.macScan = txtList[8]->text();
        newStruct.syncScan = false;
        newStruct.connectionScan = cmdTypeScan->currentIndex();
        newStruct.tcpScan = _defTcpPort;
        newStruct.ocxScan = _defOcxPort;
        newStruct.udpSendScan = _defUdpSendPort;
        newStruct.udpReseiveScan = _defUdpReceivePort;
        newStruct.serialComScan = _defSerPort;
        newStruct.serialBaudRateScan = _defSerBaud;
        switch (newStruct.connectionScan) {
        case 0:
            newStruct.tcpScan = txtList[9]->text().toInt();
            break;
        case 1:
            newStruct.ocxScan = txtList[9]->text().toInt();
            break;
        case 2:
            newStruct.udpSendScan = txtList[9]->text().toInt();
            newStruct.udpReseiveScan = txtList[10]->text().toInt();
            break;
        case 3:
            newStruct.serialComScan = txtList[9]->text();
            newStruct.serialBaudRateScan = txtList[10]->text().toInt();
            newStruct.udpReseiveScan = txtList[11]->text().toInt();
            break;
        case 4:
            newStruct.serialComScan = txtList[9]->text();
            newStruct.serialBaudRateScan = txtList[10]->text().toInt();
            break;
        default:
            break;
        }

        newStruct.gps = btnLocation->getStatus();
        newStruct.position = cmdTypeScan->currentIndex();
        newStruct.pLat = txtList[0]->text();
        newStruct.pLong = txtList[0]->text();
        newStruct.isAuto = false;
        newStruct.isDeleted = false;
        newStruct.lastSeen = QDateTime::currentDateTime();
        newStruct.createTime = QDateTime::currentDateTime();
        newStruct.userID = 0;
        int newId = gVar->getRepStation()->insert(newStruct);
        // --- Station List ---
        StationModel *newStation = new StationModel();
        newStation->setID(newId);
        newStation->setSysMode(static_cast<EnumSystemMode>(newStruct.mode));
        newStation->setSysName(newStruct.name);
        newStation->setSysIcon(newStruct.icon);
        newStation->setSysIp(newStruct.ip);
        newStation->setSysMac(newStruct.mac);
        newStation->setSysConnectionType(static_cast<EnumConnectionType>(newStruct.connection));
        newStation->setPortTcp(newStruct.tcp);
        newStation->setPortOcx(newStruct.ocx);
        newStation->setPortUdpSend(newStruct.udpSend);
        newStation->setPortUdpReceive(newStruct.udpReseive);
        newStation->setPortSerCom(newStruct.serialCom);
        newStation->setPortSerBaudRate(newStruct.serialBaudRate);
        newStation->setScanIp(newStruct.ipScan);
        newStation->setScanMac(newStruct.macScan);
        newStation->setScanSync(newStruct.syncScan);
        newStation->setScanConnectionType(static_cast<EnumConnectionType>(newStruct.connectionScan));
        newStation->setPortTcp(newStruct.tcpScan);
        newStation->setPortOcx(newStruct.ocxScan);
        newStation->setPortUdpSend(newStruct.udpSendScan);
        newStation->setPortUdpReceive(newStruct.udpReseiveScan);
        newStation->setPortSerCom(newStruct.serialComScan);
        newStation->setPortSerBaudRate(newStruct.serialBaudRateScan);
        newStation->setGPS(newStruct.gps);
        newStation->setPosType(static_cast<EnumPositionType>(newStruct.position));
        newStation->setPosLat(newStruct.pLat);
        newStation->setPosLong(newStruct.pLong);
        newStation->setSysNote(newStruct.note);
        calcPosition(newStation);
        gVar->addToStationList(newStation);
        emit clickAddNewStation(newStation);
    } else {
        station->setSysMode(sytemMode);
        station->setSysName(txtList[0]->text());
        station->setSysIp(txtList[1]->text());
        station->setSysMac(txtList[2]->text());
        station->setSysConnectionType(static_cast<EnumConnectionType>(cmdType->currentIndex()));
        switch (station->getSysConnectionType()) {
        case TCP_TCP:
            station->setPortTcp(txtList[3]->text().toInt());
            break;
        case OCX_OCX:
            station->setPortOcx(txtList[3]->text().toInt());
            break;
        case UDP_UDP:
            station->setPortUdpSend(txtList[3]->text().toInt());
            station->setPortUdpReceive(txtList[4]->text().toInt());
            break;
        case SER_UDP:
            station->setPortSerCom(txtList[3]->text());
            station->setPortSerBaudRate(txtList[4]->text().toInt());
            station->setPortUdpReceive(txtList[5]->text().toInt());
            break;
        case SER_SER:
            station->setPortSerCom(txtList[3]->text());
            station->setPortSerBaudRate(txtList[4]->text().toInt());
            break;
        default:
            break;
        }
        station->setScanIp(txtList[7]->text());
        station->setScanMac(txtList[8]->text());
        station->setScanConnectionType(static_cast<EnumConnectionType>(cmdTypeScan->currentIndex()));
        switch (station->getScanConnectionType()) {
        case TCP_TCP:
            station->setScanTcp(txtList[9]->text().toInt());
            break;
        case OCX_OCX:
            station->setScanOcx(txtList[9]->text().toInt());
            break;
        case UDP_UDP:
            station->setScanUdpSend(txtList[9]->text().toInt());
            station->setScanUdpReceive(txtList[10]->text().toInt());
            break;
        case SER_UDP:
            station->setScanSerCom(txtList[9]->text());
            station->setScanSerBaudRate(txtList[10]->text().toInt());
            station->setScanUdpReceive(txtList[11]->text().toInt());
            break;
        case SER_SER:
            station->setScanSerCom(txtList[9]->text());
            station->setScanSerBaudRate(txtList[10]->text().toInt());
            break;
        default:
            break;
        }
        station->setGPS(btnLocation->getStatus());
        station->setPosType(static_cast<EnumPositionType>(cmdTypeScan->currentIndex()));
        QMap<QString, QVariant> fields;
        fields["mode"] = station->getSysMode();
        fields["name"] = station->getSysName();
        fields["icon"] = station->getSysIcon();
        fields["ip"] = station->getSysIp();
        fields["mac"] = station->getSysMac();
        fields["connection"] = station->getSysConnectionType();
        fields["tcp"] = station->getPortTcp();
        fields["ocx"] = station->getPortOcx();
        fields["udpSend"] = station->getPortUdpSend();
        fields["udpReseive"] = station->getPortUdpReceive();
        fields["serialCom"] = station->getPortSerCom();
        fields["serialBaudrate"] = station->getPortSerBaudRate();
        fields["ipScan"] = station->getScanIp();
        fields["macScan"] = station->getScanMac();
        fields["connectionScan"] = station->getScanConnectionType();
        fields["tcpScan"] = station->getScanTcp();
        fields["ocxScan"] = station->getScanOcx();
        fields["udpSendScan"] = station->getScanUdpSend();
        fields["udpReseiveScan"] = station->getScanUdpReceive();
        fields["serialComScan"] = station->getScanSerCom();
        fields["serialBaudrateScan"] = station->getScanSerBaudRate();
        fields["gps"] = station->getGps();
        fields["position"] = station->getPosType();
        gVar->getRepStation()->update(stationId, fields);
        emit clickEditNewStation(station);
    }
    accept();
}

void ConnectionWin::resizeEvent(QResizeEvent *event)
{
    cmdTypeLoc->setFixedWidth(cmdType->width());
}

void ConnectionWin::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    cmdTypeLoc->setFixedWidth(cmdType->width());
}

void ConnectionWin::mousePressEvent(QMouseEvent *event)
{
    QPoint point = btnSystem->mapFrom(this, event->pos());
    if (event->button() == Qt::LeftButton) {
        if (btnMn->geometry().contains(point)) {
            sytemMode = Role_Monitoring;
            gbSystem->setVisible(true);
            gbScan->setVisible(true);
        }
        else if (btnDf->geometry().contains(point)) {
            sytemMode = Role_DF;
            gbSystem->setVisible(true);
            gbScan->setVisible(false);
        }
        else if (btnHo->geometry().contains(point)){
            sytemMode = Role_HandOff;
            gbSystem->setVisible(true);
            gbScan->setVisible(false);
        }
    }
    slotThemeChange();
    vLayout->invalidate();
    adjustSize();
}
