#include "menulayout.h"
#include <QPushButton>

MenuLayout::MenuLayout(GlobalVariables *gVar, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
{
    setFrameShape(QFrame::NoFrame);
    vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 5, 0, 0);
    vLayout->setSpacing(0);
    setLayout(vLayout);
    tabWidget = new QTabWidget();
    homeTab = new HomeTab(gVar,this);
    tabWidget->addTab(homeTab, "Home");
    viewTab = new ViewTab(gVar,this);
    tabWidget->addTab(viewTab, "View");
    toolsTab = new ToolsTab(gVar,this);
    tabWidget->addTab(toolsTab, "Tools");
    vLayout->addWidget(tabWidget);
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&MenuLayout::slotThemeChange);
}

MenuLayout::~MenuLayout()
{

}

void MenuLayout::slotThemeChange()
{
    style = "MenuLayout { ";
    style += "background : " + gVar->getClrBackground() + " ; ";
    // style += "border-bottom: 2px solid " + gVar->getClrPrimary() + ";";
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
    tabWidget->setStyleSheet(style);
}

void MenuLayout::resizeEvent(QResizeEvent *event)
{

}
