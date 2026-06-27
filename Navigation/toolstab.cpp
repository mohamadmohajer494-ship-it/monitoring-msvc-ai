#include "toolstab.h"

ToolsTab::ToolsTab(GlobalVariables *gVar, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
{
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(5, 5, 5, 5);
    hLayout->setSpacing(5);
    hLayout->setAlignment(Qt::AlignLeft);
    setLayout(hLayout);
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&ToolsTab::slotThemeChange);
}

ToolsTab::~ToolsTab()
{

}

void ToolsTab::slotThemeChange()
{
    style = "background : " + gVar->getClrBackground() + " ; " ;
    setStyleSheet(style);
}
