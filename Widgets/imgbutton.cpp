#include "imgbutton.h"

ImgButton::ImgButton(GlobalVariables *gVar, QString icon, QString name, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
    , icon(icon)
    , name(name)
{
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(3);
    vLayout->setAlignment(Qt::AlignCenter);
    setLayout(vLayout);
    setCursor(Qt::PointingHandCursor);
    setToolTip(name);
    lblIcon = new QLabel();
    lblIcon->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(lblIcon, 0, Qt::AlignCenter);
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&ImgButton::slotThemeChange);
}

ImgButton::~ImgButton()
{

}

void ImgButton::setActive(bool act)
{
    isActive = act;
    slotThemeChange();
}

void ImgButton::setStyle(bool act)
{
    if (isLockStyle)
        return;
    style = "ImgButton { ";
    style += "border-radius: 5px;";
    if (act)
        style += "background: " + gVar->getClrGlow() + ";";
    else
        style += "background: " + gVar->getClrSurface() + ";";
    style += " } ";
    style += "ImgButton:hover { ";
    style += "background: " + gVar->getClrGlow() + ";";
    style += " } ";
    setStyleSheet(style);
}

void ImgButton::setLockStyle(bool act)
{
    isLockStyle = act;
}

void ImgButton::slotThemeChange()
{
    style = "ImgButton { ";
    style += "border-radius: 5px;";
    if (isActive)
        style += "background: " + gVar->getClrGlow() + ";";
    else
        style += "background: " + gVar->getClrSurface() + ";";
    style += " } ";
    style += "ImgButton:hover { ";
    style += "background: " + gVar->getClrGlow() + ";";
    style += " } ";
    setStyleSheet(style);
    lblIcon->setPixmap(gVar->changeColorIcon(icon,
                                             gVar->getClrPrimary(),
                                             QSize(height() * 0.66, height() * 0.66)));
    style = "background : transparent ;";
    lblIcon->setStyleSheet(style);
}

void ImgButton::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
    setFixedWidth(height());
    lblIcon->setPixmap(gVar->changeColorIcon(icon,
                                             gVar->getClrPrimary(),
                                             QSize(height() * 0.66, height() * 0.66)));
}

void ImgButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        setActive(true);
        emit leftClick(name);
    } else if (event->button() == Qt::RightButton) {
        emit rightClick(name);
    }
}
