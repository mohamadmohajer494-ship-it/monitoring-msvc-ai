#include "caretableitem.h"

CareTableItem::CareTableItem(GlobalVariables *gVar,
                             CareTableStruct itemData,
                             QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
    , item(itemData)
{
    setCursor(Qt::PointingHandCursor);
    setFixedHeight(80);
    gLayout = new QGridLayout();
    setLayout(gLayout);

    lblName = new QLabel("Name");
    gLayout->addWidget(lblName,0,0,1,5);

    lblTitleFreq = new QLabel("Freq : ");
    gLayout->addWidget(lblTitleFreq,1,0);
    lblFreq = new QLabel("FreqTxt");
    gLayout->addWidget(lblFreq,1,1);

    lblTitleBw = new QLabel("BW : ");
    gLayout->addWidget(lblTitleBw,2,0);
    lblBw = new QLabel("BwTxt");
    gLayout->addWidget(lblBw,2,1);

    lblTitleThr = new QLabel("Thr : ");
    gLayout->addWidget(lblTitleThr,3,0);
    lblThr = new QLabel("ThrTxt");
    gLayout->addWidget(lblThr,3,1);

    lblTitleLvl = new QLabel("Lvl : ");
    gLayout->addWidget(lblTitleLvl,1,3);
    lblLvl = new QLabel("LvlTxt");
    gLayout->addWidget(lblLvl,1,4);

    lblTitleAzm = new QLabel("Azm : ");
    gLayout->addWidget(lblTitleAzm,2,3);
    lblAzm = new QLabel("AzmTxt");
    gLayout->addWidget(lblAzm,2,4);

    lblTitleSeen = new QLabel("Seen : ");
    gLayout->addWidget(lblTitleSeen,3,3);
    lblSeen = new QLabel("SeenTxt");
    gLayout->addWidget(lblSeen,3,4);

    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&CareTableItem::slotThemeChange);
}

CareTableItem::~CareTableItem()
{

}

void CareTableItem::slotThemeChange()
{
    style = "CareTableItem { ";
    style += "border-radius: 5px;";
    if (isActive)
        style += "background: " + gVar->getClrGlow() + ";";
    else
        style += "background: " + gVar->getClrSurface() + ";";
    style += " } ";
    style += "CareTableItem:hover { ";
    style += "background: " + gVar->getClrGlow() + ";";
    style += " } ";
    setStyleSheet(style);
    style = "background : transparent ;";
    lblTitleFreq->setStyleSheet(style);
    lblFreq->setStyleSheet(style);
    lblTitleBw->setStyleSheet(style);
    lblBw->setStyleSheet(style);
    lblTitleThr->setStyleSheet(style);
    lblThr->setStyleSheet(style);
    lblTitleLvl->setStyleSheet(style);
    lblLvl->setStyleSheet(style);
    lblTitleAzm->setStyleSheet(style);
    lblAzm->setStyleSheet(style);
    lblTitleSeen->setStyleSheet(style);
    lblSeen->setStyleSheet(style);
    lblName->setStyleSheet(style);

}

void CareTableItem::resizeEvent(QResizeEvent *event)
{

}

void CareTableItem::mousePressEvent(QMouseEvent *event)
{

}