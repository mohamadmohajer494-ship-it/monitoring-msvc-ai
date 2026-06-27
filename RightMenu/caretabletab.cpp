#include "caretabletab.h"

CareTableTab::CareTableTab(GlobalVariables *gVar, QWidget *parent)
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

    itemList = new QVector<CareTableItem*>();
    for (int var = 0; var < 20; ++var) {
        CareTableStruct itemData;
        itemData.eID = var;
        itemData.freq = 300 + var;
        CareTableItem *item = new CareTableItem(gVar, itemData, this);
        itemList->append(item);
        vLayout->addWidget(item);
    }

    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&CareTableTab::slotThemeChange);
}

CareTableTab::~CareTableTab()
{

}

void CareTableTab::slotThemeChange()
{
    style = "background : " + gVar->getClrBackground() + " ; " ;
    setStyleSheet(style);
}