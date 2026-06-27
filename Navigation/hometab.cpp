#include "hometab.h"

HomeTab::HomeTab(GlobalVariables *gVar, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
{
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(5, 5, 5, 5);
    hLayout->setSpacing(5);
    hLayout->setAlignment(Qt::AlignLeft);
    setLayout(hLayout);
    createConnectionBox();
    createMenuLin();
    createFrequencyControl();
    createMenuLin();
    createSignalProcessing();
    createMenuLin();
    hLayout->addStretch();
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&HomeTab::slotThemeChange);
}

HomeTab::~HomeTab()
{

}

void HomeTab::createMenuLin()
{
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setFixedWidth(2);
    linList.append(line);
    hLayout->addWidget(line);
}

QHBoxLayout* HomeTab::createMenuBox(QString txt)
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

void HomeTab::createConnectionBox()
{
    QHBoxLayout *mLayout = createMenuBox(tr("Connection"));
    btnConnection = new BtnConnection(gVar);
    mLayout->addWidget(btnConnection);
    btnPlayPause = new ImgToggle(gVar,
                              ":/Res/Images/Play.png",
                              tr("Play"),
                              ":/Res/Images/Pause.png",
                              tr("Pause"));
    mLayout->addWidget(btnPlayPause);
}

void HomeTab::createFrequencyControl()
{
    QHBoxLayout *mLayout = createMenuBox(tr("Frequency Control"));
    freqBox = new FrequencyBox(gVar);
    mLayout->addWidget(freqBox);
    btnBandwidth = new BtnTextFlip(gVar,
                                   "Bandwidth","BW",
                                   {"50KHz","100KHz","150KHz","200KHz","250KHz","500KHz","1MHz","2MHz","4MHz","10MHz","20MHz","40MHz","80MHz","200MHz"},
                                   "40MHz",this);
    mLayout->addWidget(btnBandwidth);
    btnResolution = new BtnTextFlip(gVar,
                                    "Resolution","RES",
                                    {"512","1024","2048"},
                                    "512",this);
    mLayout->addWidget(btnResolution);
}

void HomeTab::createSignalProcessing()
{
    QHBoxLayout *mLayout = createMenuBox(tr("Signal Processing"));
    btnModulation = new BtnTextFlip(gVar
                                ,"Modulation","MOD"
                                ,{"AM","FM","PM"}
                                ,"AM", this);
    mLayout->addWidget(btnModulation);
    btnIFFilter = new BtnTextFlip(gVar
                              ,"IFFilter","IFF"
                              ,{"1KHz","2Hz","3Hz","7.5Hz","15KHz","125KHz","250KHz"}
                              ,"125KHz", this);
    mLayout->addWidget(btnIFFilter);
    btnRFHead = new BtnTextFlip(gVar
                            ,"RFHead","RFH"
                            ,{"ATT", "HL", "INT", "HS"}
                            ,"HS", this);
    mLayout->addWidget(btnRFHead);
}

void HomeTab::slotThemeChange()
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
