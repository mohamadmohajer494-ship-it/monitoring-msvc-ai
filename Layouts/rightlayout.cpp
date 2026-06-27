#include "rightlayout.h"

RightLayout::RightLayout(GlobalVariables *gVar, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
{
    setFrameShape(QFrame::NoFrame);
    vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    vSplitter = new QSplitter(Qt::Vertical, this);
    vLayout->addWidget(vSplitter);
    createTopBox();
    createBotBox();
    vSplitter->setSizes({2, 1});
    vSplitter->setStretchFactor(0, 1);
    vSplitter->setStretchFactor(1, 1);
    slotThemeChange();
    connect(gVar, &GlobalVariables::ThemeChange,
            this, &RightLayout::slotThemeChange);
}


RightLayout::~RightLayout()
{

}

void RightLayout::createTopBox()
{
    topBox = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(topBox);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    tabWidgetTop = new QTabWidget(topBox);
    quickAccessTab = new QuickAccessTab(gVar, tabWidgetTop);
    tabWidgetTop->addTab(quickAccessTab, "Quick Access");
    mapLayerTab = new MapLayerTab(gVar, tabWidgetTop);
    tabWidgetTop->addTab(mapLayerTab, "Map Layer");
    layout->addWidget(tabWidgetTop);
    vSplitter->addWidget(topBox);
}

void RightLayout::createBotBox()
{
    botBox = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(botBox);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    tabWidgetBot = new QTabWidget(botBox);
    careTableTab = new CareTableTab(gVar, tabWidgetBot);
    tabWidgetBot->addTab(careTableTab, "CareTable");
    layout->addWidget(tabWidgetBot);
    vSplitter->addWidget(botBox);
}


void RightLayout::slotThemeChange()
{
    style = "RightLayout { ";
    style += "border-left: 2px solid " + gVar->getClrGlow() + ";";
    style += " } ";
    setStyleSheet(style);
    style = "QTabWidget::pane {";
    style += "border-top : 2px solid " + gVar->getClrGlow() + " ; ";
    style += "border-bottom : 2px solid " + gVar->getClrGlow() + " ; ";
    style += "}";
    style += "QTabBar::tab {";
    style += "background : " + gVar->getClrGlow() + " ; ";
    style += "color : " + gVar->getClrPrimary() + " ; ";
    style += "border: 1px solid " + gVar->getClrGlow() + " ; ";
    style += "border-bottom: none ; ";
    style += "border-top-left-radius : 5px ; ";
    style += "border-top-right-radius : 5px ; ";
    style += "padding: 2px 20px ; ";
    style += "margin: 0 4px 0 0 ; ";
    style += "}";
    style += "QTabBar::tab:hover {";
    style += "background : " + gVar->getClrSurface() + " ; ";
    style += "color : " + gVar->getClrPrimary() + " ; ";
    style += "}";
    style += "QTabBar::tab:pressed {";
    style += "background : " + gVar->getClrSurface() + " ; ";
    style += "color : " + gVar->getClrPrimary() + " ; ";
    style += "}";
    style += "QTabBar::tab:selected {";
    style += "background : " + gVar->getClrSurface() + " ; ";
    style += "color : " + gVar->getClrPrimary() + " ; ";
    style += "border-bottom: 1px solid " + gVar->getClrGlow() + " ; ";
    style += "}";
    style += "QTabBar::tab:!selected {";
    style += "margin-top: 2px ; ";
    style += "}";
    tabWidgetTop->setStyleSheet(style);
    tabWidgetBot->setStyleSheet(style);
}

void RightLayout::showEvent(QShowEvent *event)
{
    const int h = vSplitter->height();
    vSplitter->setSizes({h * 2 / 3, h / 3});
}
