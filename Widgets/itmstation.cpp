#include "itmstation.h"

ItmStation::ItmStation(GlobalVariables *gVar, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
{
    setCursor(Qt::PointingHandCursor);
    hLayout = new QHBoxLayout();
    hLayout->setSpacing(10);
    setLayout(hLayout);
    lblIcon = new QLabel();
    hLayout->addWidget(lblIcon);
    vLayout = new QVBoxLayout();
    hLayout->addLayout(vLayout);
    lblName = new QLabel(name);
    lblName->setVisible(false);
    lblName->setObjectName("lblName");
    vLayout->addWidget(lblName);
    lblIp = new QLabel(ip);
    lblIp->setVisible(false);
    lblIp->setObjectName("lblIp");
    vLayout->addWidget(lblIp);
    hLayout->addStretch();
    lblDf = new QLabel();
    lblDf->setVisible(false);
    hLayout->addWidget(lblDf);
    createStatusCircle();
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&ItmStation::slotThemeChange);
    setOpen(false);
}

ItmStation::~ItmStation()
{

}

void ItmStation::setOpen(bool act)
{
    lblName->setVisible(act);
    lblIp->setVisible(act);
    lblDf->setVisible(isShowDir && act);
}

void ItmStation::setIcon(QString txt)
{
    icon = txt;
    lblIcon->setPixmap(gVar->changeColorIcon(icon,
                                             gVar->getClrPrimary(),
                                             QSize(height()/2, height()/2)));
}

void ItmStation::setName(QString txt)
{
    name = txt;
    lblName->setText(name);
}

void ItmStation::setIp(QString txt)
{
    ip = txt;
    lblIp->setText(ip);
}

void ItmStation::setDirection(int angle)
{
    if (angle == -1 || angle == 0) {
        isShowDir = false;
        lblDf->setVisible(false);
        return;
    } else {
        isShowDir = true;
        lblDf->setVisible(true);
    }
    dfAngle = angle;
    if (dfPixmap.isNull())
        return;
    int fixedSize = qMax(dfPixmap.width(), dfPixmap.height()) * 2;
    QPixmap canvas(fixedSize, fixedSize);
    canvas.fill(Qt::transparent);
    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.translate(fixedSize / 2, fixedSize / 2);
    painter.rotate(angle - 45);
    QRect targetRect(
        -dfPixmap.width() / 2,
        -dfPixmap.height() / 2,
        dfPixmap.width(),
        dfPixmap.height()
        );
    painter.drawPixmap(targetRect, dfPixmap);
    painter.end();
    lblDf->setPixmap(canvas);
}

void ItmStation::setAvailable(bool check)
{
    if (isAvailable == check)
        return;
    isAvailable = check;
    if (!isAvailable) {
        setSystemStatus(Statin_Unavailable);
    } else if (isAvailable &&
               static_cast<int>(systemStatus) < 2)
    {
        setSystemStatus(Statin_Reachable);
    }
}

void ItmStation::setConnected(bool check)
{
    if (isConnected == check)
        return;
    isConnected = check;
    if (!isConnected) {
        setSystemStatus(Statin_Unavailable);
    } else if (isConnected &&
               static_cast<int>(systemStatus) < 3)
    {
        setSystemStatus(Statin_Connected);
    }
}

void ItmStation::setSystemStatus(EnumStatinStatus status)
{
    systemStatus = status;
    switch (systemStatus) {
    case Statin_Unavailable:
        style = "background-color:" + gVar->getClrError() + ";";
        break;
    case Statin_Available:
        style = "background-color:" + gVar->getClrPrimary() + ";";
        break;
    case Statin_Reachable:
        style = "background-color:" + gVar->getClrWarning() + ";";
        break;
    case Statin_Connected:
        style = "background-color:" + gVar->getClrCorrect() + ";";
        break;
    default:
        break;
    }
    style += "border:2px solid " + gVar->getClrBackground() + ";";
    style += "border-radius:7px;";
    statusCircle->setStyleSheet(style);
}

void ItmStation::createStatusCircle()
{
    statusCircle = new QWidget(this);
    statusCircle->setFixedSize(14, 14);
    statusCircle->setAttribute(Qt::WA_TransparentForMouseEvents);
    statusCircle->setToolTip("DisConnect");
    statusCircle->raise();
}

void ItmStation::updateStatusCircle()
{
    QColor circleColor;
    if (isConnected)
        circleColor = QColor(gVar->getClrAccent());
    else if (isAvailable)
        circleColor = QColor(gVar->getClrPrimary());
    else
        circleColor = QColor(gVar->getClrError());
}

void ItmStation::slotThemeChange()
{
    style = "ItmStation { ";
    style += "border-radius: 5px;";
    style += "background: " + gVar->getClrSurface() + ";";
    style += " } ";
    style += "ItmStation:hover { ";
    style += "background: " + gVar->getClrGlow() + ";";
    style += " } ";
    setStyleSheet(style);
    style = "background : transparent ;";
    style += "color : " + gVar->getClrPrimary() + ";";
    style += "font-weight : bold";
    lblName->setStyleSheet(style);
    style = "background : transparent ;";
    style += "color : " + gVar->getClrGlow() + ";";
    style += "font-weight : bold";
    lblIp->setStyleSheet(style);
    lblIcon->setPixmap(gVar->changeColorIcon(icon,
                                             gVar->getClrPrimary(),
                                             QSize(height()/2, height()/2)));
    style = "background : transparent ;";
    lblIcon->setStyleSheet(style);
    dfPixmap = gVar->changeColorIcon(dir,
                                     gVar->getClrPrimary(),
                                     QSize(height()/3, height()/3));
    lblDf->setPixmap(dfPixmap);
    setDirection(dfAngle);
    style = "background : transparent ;";
    lblDf->setStyleSheet(style);
    setSystemStatus(systemStatus);
}

void ItmStation::resizeEvent(QResizeEvent *event)
{
    lblIcon->setPixmap(gVar->changeColorIcon(icon,
                                             gVar->getClrPrimary(),
                                             QSize(height()/2, height()/2)));
    dfPixmap = gVar->changeColorIcon(dir,
                                     gVar->getClrPrimary(),
                                     QSize(height()/3, height()/3));
    setDirection(dfAngle);
    int margin = 5;
    statusCircle->move(margin,
                       height() - statusCircle->height() - margin);
    statusCircle->raise();
}

void ItmStation::showEvent(QShowEvent *event)
{

}

bool ItmStation::eventFilter(QObject *obj, QEvent *event)
{
    return true;
}

void ItmStation::enterEvent(QEnterEvent *event)
{
    style = "background : transparent ;";
    style += "color : " + gVar->getClrSurface() + ";";
    style += "font-weight : bold";
    lblIp->setStyleSheet(style);
    QFrame::enterEvent(event);
}

void ItmStation::leaveEvent(QEvent *event)
{
    style = "background : transparent ;";
    style += "color : " + gVar->getClrGlow() + ";";
    style += "font-weight : bold";
    lblIp->setStyleSheet(style);
    QFrame::leaveEvent(event);
}

void ItmStation::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    QWidgetAction *headerAction = new QWidgetAction(&menu);
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(0);
    QLabel *lblName = new QLabel();
    lblName->setText(name);
    lblName->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(lblName);
    // QLabel *lblStatus = new QLabel();
    // if (isAvailable)
    //     lblStatus->setText("Reachable");
    // else
    //     lblStatus->setText("Unreachable");
    // lblStatus->setAlignment(Qt::AlignCenter);
    // headerLayout->addWidget(lblStatus);
    style = "QWidget {";
    style += "background-color: " + gVar->getClrSurface() + ";";
    style += "padding: 6px 5px;";
    style += "}";
    headerWidget->setStyleSheet(style);
    style = "background: transparent;";
    style += "font-weight: bold;";
    style += "color : " + gVar->getClrPrimary() + ";";
    lblName->setStyleSheet(style);
    if (isAvailable)
        style += "color : " + gVar->getClrAccent() + ";";
    else
        style += "color : " + gVar->getClrError() + ";";
    // lblStatus->setStyleSheet(style);
    headerAction->setDefaultWidget(headerWidget);
    menu.addAction(headerAction);
    QPixmap connectPixmap;
    if (!isConnected)
        connectPixmap = gVar->changeColorIcon(
            ":/Res/Images/Connection.png",
            gVar->getClrPrimary(),
            QSize(18, 18)
            );
    else
        connectPixmap = gVar->changeColorIcon(
            ":/Res/Images/Connection.png",
            gVar->getClrSurface(),
            QSize(18, 18)
            );
    QPixmap closePixmap;
    if (isConnected)
        closePixmap = gVar->changeColorIcon(
            ":/Res/Images/Close.png",
            gVar->getClrPrimary(),
            QSize(18, 18)
            );
    else
        closePixmap = gVar->changeColorIcon(
            ":/Res/Images/Close.png",
            gVar->getClrSurface(),
            QSize(18, 18)
            );
    QPixmap editPixmap;
    if (!isConnected)
        editPixmap = gVar->changeColorIcon(
            ":/Res/Images/Edit.png",
            gVar->getClrPrimary(),
            QSize(18, 18)
            );
    else
        editPixmap = gVar->changeColorIcon(
            ":/Res/Images/Edit.png",
            gVar->getClrSurface(),
            QSize(18, 18)
            );
    QPixmap deletePixmap;
    if (!isConnected)
        deletePixmap = gVar->changeColorIcon(
            ":/Res/Images/Delete.png",
            gVar->getClrPrimary(),
            QSize(18, 18)
            );
    else
        deletePixmap = gVar->changeColorIcon(
            ":/Res/Images/Delete.png",
            gVar->getClrSurface(),
            QSize(18, 18)
            );
    QAction *actConnect = menu.addAction(QIcon(connectPixmap), "Connect");
    actConnect->setEnabled(!isConnected);
    QAction *actClose = menu.addAction(QIcon(closePixmap), "DisConnect");
    actClose->setEnabled(isConnected);
    QAction *actEdit = menu.addAction(QIcon(editPixmap), "Edit");
    actEdit->setEnabled(!isConnected);
    QAction *actDelete = menu.addAction(QIcon(deletePixmap), "Delete");
    actDelete->setEnabled(!isConnected);
    style = "QMenu {";
    style += "background-color: " + gVar->getClrBackground() + ";";
    style += "color: " + gVar->getClrPrimary() + ";";
    style += "border: 1px solid " + gVar->getClrPrimary() + ";";
    style += "padding: 4px 5px;";
    style += "}";
    style += "QMenu::item {";
    // style += "padding: 6px 24px 6px 24px;";
    // style += "padding: 6px 5px;";
    style += "background-color: transparent;";
    style += "}";
    style += "QMenu::item:selected {";
    style += "background-color: " + gVar->getClrSurface() + ";";
    style += "color: " + gVar->getClrPrimary() + ";";
    style += "}";
    style += "QMenu::item:disabled {";
    style += "background-color: transparent;";
    style += "color: " + gVar->getClrSurface() + ";";
    style += "}";
    menu.setStyleSheet(style);
    emit openRightMenu(true);
    QAction *selectedAction = menu.exec(event->globalPos());
    if (selectedAction == actConnect) {
        emit clickRightMenuConnect(iID);
    }
    if (selectedAction == actClose) {
        emit clickRightMenuClose(iID);
    }
    if (selectedAction == actEdit) {
        emit clickRightMenuEdit(iID);
    }
    else if (selectedAction == actDelete) {
        emit clickRightMenuDelete(iID);
    }
    emit openRightMenu(false);
}

void ItmStation::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (!isConnected) {
            emit clickRightMenuConnect(iID);
        }
    }
    QFrame::mouseDoubleClickEvent(event);
}



