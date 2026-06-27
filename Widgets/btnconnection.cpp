#include "btnconnection.h"

BtnConnection::BtnConnection(GlobalVariables *gVar, QWidget *parent)
    : QFrame {parent}
    , gVar(gVar)
{
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(3);
    vLayout->setAlignment(Qt::AlignCenter);
    setLayout(vLayout);
    setCursor(Qt::PointingHandCursor);
    setToolTip(tr("Connection"));
    lblIcon = new QLabel();
    lblIcon->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(lblIcon, 0, Qt::AlignCenter);
    lblText = new QLabel();
    lblText->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(lblText, 0, Qt::AlignCenter);
    lblText->setText(tr("Connection"));
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&BtnConnection::slotThemeChange);
}

BtnConnection::~BtnConnection()
{

}

void BtnConnection::slotThemeChange()
{
    style = "BtnConnection { ";
    style += "border-radius: 5px;";
    style += "background: " + gVar->getClrSurface() + ";";
    style += " } ";
    style += "BtnConnection:hover { ";
    style += "background: " + gVar->getClrGlow() + ";";
    style += " } ";
    setStyleSheet(style);
    lblIcon->setPixmap(gVar->changeColorIcon(icon,
                                             gVar->getClrPrimary(),
                                             QSize(height()/2, height()/2)));
    style = "background : transparent ;";
    lblIcon->setStyleSheet(style);
    style += "font-weight: bold;";
    lblText->setStyleSheet(style);
}

void BtnConnection::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
    setFixedWidth(height());
    lblIcon->setPixmap(gVar->changeColorIcon(icon,
                                             gVar->getClrPrimary(),
                                             QSize(height()/2, height()/2)));
}

void BtnConnection::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit leftClick();
    } else if (event->button() == Qt::RightButton) {
        emit rightClick();
    }
}
