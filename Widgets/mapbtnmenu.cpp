#include "mapbtnmenu.h"

MapBtnMenu::MapBtnMenu(GlobalVariables *gVar, QString icon, QString name, QStringList rightMenu, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
    , icon(icon)
    , name(name)
    , rightMenu(rightMenu)
{
    setFixedSize(48,48);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(3);
    vLayout->setAlignment(Qt::AlignCenter);
    setLayout(vLayout);
    setCursor(Qt::PointingHandCursor);
    setToolTip(name);
    lblIcon = new QLabel();
    lblIcon->setAlignment(Qt::AlignCenter);
    lblIcon->setPixmap(icon);
    vLayout->addWidget(lblIcon, 0, Qt::AlignCenter);
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&MapBtnMenu::slotThemeChange);
}

MapBtnMenu::~MapBtnMenu()
{

}

void MapBtnMenu::setActive(bool act)
{
    isActive = act;
    slotThemeChange();
}

void MapBtnMenu::setStyle(bool act)
{
    if (isLockStyle)
        return;
    style = "MapBtnMenu { ";
    style += "border-radius: 5px;";
    if (act)
        style += "background: " + gVar->getClrGlow() + ";";
    else
        style += "background: " + gVar->getClrSurface() + ";";
    style += " } ";
    style += "MapBtnMenu:hover { ";
    style += "background: " + gVar->getClrGlow() + ";";
    style += " } ";
    setStyleSheet(style);
}

void MapBtnMenu::setLockStyle(bool act)
{
    isLockStyle = act;
}

void MapBtnMenu::showUpwardMenu()
{
    if (rightMenu.size() == 0)
        return;
    QMenu menu(this);
    for (int i = rightMenu.size() - 1; i >= 0; --i) {
        QAction *act = menu.addAction(rightMenu[i]);
        connect(act, &QAction::triggered, this, [this, text = rightMenu[i]]() {
            emit rightClick(text);
        });
    }
    menu.ensurePolished();
    QSize sz = menu.sizeHint();
    QPoint topLeft = mapToGlobal(QPoint(0, 0));
    QPoint menuPos(topLeft.x(), topLeft.y() - sz.height());
    menu.exec(menuPos);
}

void MapBtnMenu::slotThemeChange()
{
    style = "MapBtnMenu { ";
    style += "border-radius: 5px;";
    if (isActive)
        style += "background: " + gVar->getClrGlow() + ";";
    else
        style += "background: " + gVar->getClrSurface() + ";";
    style += " } ";
    style += "MapBtnMenu:hover { ";
    style += "background: " + gVar->getClrGlow() + ";";
    style += " } ";
    setStyleSheet(style);
    style = "background : transparent ;";
    lblIcon->setStyleSheet(style);
}

void MapBtnMenu::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
}

void MapBtnMenu::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        setActive(true);
        emit leftClick(name);
    } else if (event->button() == Qt::RightButton) {
        showUpwardMenu();
    }
}

