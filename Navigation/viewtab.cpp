#include "viewtab.h"

ViewTab::ViewTab(GlobalVariables *gVar, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
{
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(5, 5, 5, 5);
    hLayout->setSpacing(5);
    hLayout->setAlignment(Qt::AlignLeft);
    setLayout(hLayout);
    createViewsBox();
    createMenuLin();
    hLayout->addStretch();
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&ViewTab::slotThemeChange);
}

ViewTab::~ViewTab()
{

}

ImgButton *ViewTab::getViewButton(const QString &name)
{
    if (name == "Overview")      return btnOverview;
    if (name == "Panorama")      return btnPanorama;
    if (name == "Analysis")      return btnAnalysis;
    if (name == "Position")      return btnPosition;
    if (name == "Handoff")       return btnHandoff;
    if (name == "Direction01")   return btnDirection01;
    if (name == "Direction02")   return btnDirection02;
    if (name == "Direction03")   return btnDirection03;
    if (name == "Direction04")   return btnDirection04;
    if (name == "Direction05")   return btnDirection05;
    if (name == "Direction06")   return btnDirection06;
    if (name == "Direction07")   return btnDirection07;
    if (name == "Direction08")   return btnDirection08;
    if (name == "Direction09")   return btnDirection09;

    return nullptr;
}

QList<ImgButton *> ViewTab::getAllViewButtons()
{
    return {
        btnOverview,
        btnPanorama,
        btnAnalysis,
        btnPosition,
        btnHandoff,
        btnDirection01,
        btnDirection02,
        btnDirection03,
        btnDirection04,
        btnDirection05,
        btnDirection06,
        btnDirection07,
        btnDirection08,
        btnDirection09
    };
}

void ViewTab::createMenuLin()
{
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setFixedWidth(2);
    linList.append(line);
    hLayout->addWidget(line);
}

QHBoxLayout *ViewTab::createMenuBox(QString txt)
{
    QWidget *box = new QWidget();
    hLayout->addWidget(box);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(2);
    box->setLayout(vLayout);
    QWidget *cont = new QWidget();
    cont->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QHBoxLayout *mLayout = new QHBoxLayout();
    mLayout->setContentsMargins(0, 0, 0, 0);
    mLayout->setSpacing(5);
    cont->setLayout(mLayout);
    vLayout->addWidget(cont, 1);
    QLabel *lbl = new QLabel(txt);
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    lblList.append(lbl);
    vLayout->addWidget(lbl, 0, Qt::AlignCenter);
    return mLayout;
}

void ViewTab::createViewsBox()
{
    QHBoxLayout *mLayout = createMenuBox(tr("Views"));
    btnOverview = new ImgButton(gVar,":/Res/Images/Overview.png","Overview");
    btnOverview->setStyle(true);
    mLayout->addWidget(btnOverview);
    btnPanorama = new ImgButton(gVar,":/Res/Images/Panorama.png","Panorama");
    mLayout->addWidget(btnPanorama);
    btnAnalysis = new ImgButton(gVar,":/Res/Images/Analysis.png","Analysis");
    mLayout->addWidget(btnAnalysis);
    btnPosition = new ImgButton(gVar,":/Res/Images/PF.png","Position");
    mLayout->addWidget(btnPosition);
    btnHandoff = new ImgButton(gVar,":/Res/Images/HandOff.png","Handoff");
    mLayout->addWidget(btnHandoff);
    btnDirection01 = new ImgButton(gVar,":/Res/Images/DF.png","Direction01");
    mLayout->addWidget(btnDirection01);
    btnDirection02 = new ImgButton(gVar,":/Res/Images/DF.png","Direction02");
    mLayout->addWidget(btnDirection02);
    btnDirection03 = new ImgButton(gVar,":/Res/Images/DF.png","Direction03");
    mLayout->addWidget(btnDirection03);
    btnDirection04 = new ImgButton(gVar,":/Res/Images/DF.png","Direction04");
    mLayout->addWidget(btnDirection04);
    btnDirection05 = new ImgButton(gVar,":/Res/Images/DF.png","Direction05");
    mLayout->addWidget(btnDirection05);
    btnDirection06 = new ImgButton(gVar,":/Res/Images/DF.png","Direction06");
    mLayout->addWidget(btnDirection06);
    btnDirection07 = new ImgButton(gVar,":/Res/Images/DF.png","Direction07");
    mLayout->addWidget(btnDirection07);
    btnDirection08 = new ImgButton(gVar,":/Res/Images/DF.png","Direction08");
    mLayout->addWidget(btnDirection08);
    btnDirection09 = new ImgButton(gVar,":/Res/Images/DF.png","Direction09");
    mLayout->addWidget(btnDirection09);
}

void ViewTab::slotThemeChange()
{
    style = "background : " + gVar->getClrBackground() + " ; " ;
    setStyleSheet(style);
    style = "background : " + gVar->getClrGlow() + ";";
    for (int var = 0; var < linList.size(); ++var)
        linList[var]->setStyleSheet(style);
    QString primaryColor = gVar->getClrPrimary();
    QColor color(primaryColor);
    style = QString("color: rgba(%1, %2, %3, 0.3);")
                .arg(color.red())
                .arg(color.green())
                .arg(color.blue());
    style += "background : transparent ;";
    style += "font-weight : bold ;";
    style += "font-size : 6 ;";
    for (int var = 0; var < lblList.size(); ++var)
        lblList[var]->setStyleSheet(style);
}
