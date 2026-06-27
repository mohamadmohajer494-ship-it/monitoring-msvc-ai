#include "quickaccesstab.h"

QuickAccessTab::QuickAccessTab(GlobalVariables *gVar, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
{
    createUi();

    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&QuickAccessTab::slotThemeChange);
}

QuickAccessTab::~QuickAccessTab()
{

}

void QuickAccessTab::createUi()
{
    setFrameShape(QFrame::NoFrame);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(6);

    topBar = new QFrame(this);
    topBar->setFrameShape(QFrame::NoFrame);

    topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(5);
    topLayout->addStretch();

    btnOptions = new QPushButton(topBar);
    btnOptions->setCursor(Qt::PointingHandCursor);
    btnOptions->setFixedSize(32, 32);
    btnOptions->setToolTip("Options");
    btnOptions->setIconSize(QSize(22, 22));

    QPixmap optionsPixmap = gVar->changeColorIcon(
        ":/Res/Images/Options.png",
        QColor(gVar->getClrPrimary()),
        QSize(22, 22)
        );

    btnOptions->setIcon(QIcon(optionsPixmap));

    topLayout->addWidget(btnOptions);
    mainLayout->addWidget(topBar);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    containerWidget = new QWidget(scrollArea);

    vLayout = new QVBoxLayout(containerWidget);
    vLayout->setContentsMargins(5, 5, 5, 5);
    vLayout->setSpacing(7);
    vLayout->addStretch();

    QHBoxLayout *viewLayout = new QHBoxLayout();


    scrollArea->setWidget(containerWidget);
    mainLayout->addWidget(scrollArea);



    createOptionsMenu();

    connect(btnOptions, &QPushButton::clicked,
            this, &QuickAccessTab::showOptionsMenu);
}

void QuickAccessTab::createOptionsMenu()
{
    optionsMenu = new QMenu(this);

    // chkShowView = addOptionCheckBox("Show View", isShowView);
    chkShowFreq = addOptionCheckBox("Show Freq", isShowFreq);
    chkShowBw   = addOptionCheckBox("Show BW",   isShowBw);
    chkShowRes  = addOptionCheckBox("Show Res",  isShowRes);
    chkShowMod  = addOptionCheckBox("Show Mod",  isShowMod);
    chkShowIff  = addOptionCheckBox("Show IFF",  isShowIff);
    chkShowRfh  = addOptionCheckBox("Show RFH",  isShowRfh);

    // connect(chkShowView, &QCheckBox::toggled, this, [this](bool checked) {
    //     isShowView = checked;
    //     updateQuickAccessView();
    // });

    connect(chkShowFreq, &QCheckBox::toggled, this, [this](bool checked) {
        isShowFreq = checked;
        updateQuickAccessView();
    });

    connect(chkShowBw, &QCheckBox::toggled, this, [this](bool checked) {
        isShowBw = checked;
        updateQuickAccessView();
    });

    connect(chkShowRes, &QCheckBox::toggled, this, [this](bool checked) {
        isShowRes = checked;
        updateQuickAccessView();
    });

    connect(chkShowMod, &QCheckBox::toggled, this, [this](bool checked) {
        isShowMod = checked;
        updateQuickAccessView();
    });

    connect(chkShowIff, &QCheckBox::toggled, this, [this](bool checked) {
        isShowIff = checked;
        updateQuickAccessView();
    });

    connect(chkShowRfh, &QCheckBox::toggled, this, [this](bool checked) {
        isShowRfh = checked;
        updateQuickAccessView();
    });

    applyOptionsMenuStyle();
}

void QuickAccessTab::showOptionsMenu()
{
    if (!optionsMenu || !btnOptions)
        return;

    // if (chkShowView) chkShowView->setChecked(isShowView);
    if (chkShowFreq) chkShowFreq->setChecked(isShowFreq);
    if (chkShowBw)   chkShowBw->setChecked(isShowBw);
    if (chkShowRes)  chkShowRes->setChecked(isShowRes);
    if (chkShowMod)  chkShowMod->setChecked(isShowMod);
    if (chkShowIff)  chkShowIff->setChecked(isShowIff);
    if (chkShowRfh)  chkShowRfh->setChecked(isShowRfh);

    QSize menuSize = optionsMenu->sizeHint();

    QPoint pos = btnOptions->mapToGlobal(QPoint(
        btnOptions->width() - menuSize.width(),
        btnOptions->height() + 4
        ));

    optionsMenu->popup(pos);
}

void QuickAccessTab::applyOptionsMenuStyle()
{
    if (!optionsMenu)
        return;

    QString menuStyle;

    menuStyle += "QMenu {";
    menuStyle += "background-color: " + gVar->getClrSurface() + ";";
    menuStyle += "border: 1px solid " + gVar->getClrPrimary() + ";";
    menuStyle += "border-radius: 6px;";
    menuStyle += "padding: 6px;";
    menuStyle += "}";

    menuStyle += "QCheckBox {";
    menuStyle += "background: transparent;";
    menuStyle += "color: " + gVar->getClrPrimary() + ";";
    menuStyle += "font-weight: bold;";
    menuStyle += "spacing: 7px;";
    menuStyle += "padding: 3px 8px;";
    menuStyle += "}";

    menuStyle += "QCheckBox:hover {";
    menuStyle += "background-color: " + gVar->getClrGlow() + ";";
    menuStyle += "border-radius: 4px;";
    menuStyle += "}";

    menuStyle += "QCheckBox::indicator {";
    menuStyle += "width: 13px;";
    menuStyle += "height: 13px;";
    menuStyle += "}";

    menuStyle += "QCheckBox::indicator:unchecked {";
    menuStyle += "border: 1px solid " + gVar->getClrPrimary() + ";";
    menuStyle += "background: transparent;";
    menuStyle += "border-radius: 2px;";
    menuStyle += "}";

    menuStyle += "QCheckBox::indicator:checked {";
    menuStyle += "border: 1px solid " + gVar->getClrPrimary() + ";";
    menuStyle += "background: " + gVar->getClrPrimary() + ";";
    menuStyle += "border-radius: 2px;";
    menuStyle += "}";

    optionsMenu->setStyleSheet(menuStyle);
}

void QuickAccessTab::updateQuickAccessView()
{

}

QCheckBox* QuickAccessTab::addOptionCheckBox(const QString &text, bool checked)
{
    if (!optionsMenu)
        return nullptr;

    QWidget *rowWidget = new QWidget(optionsMenu);

    QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(8, 3, 8, 3);
    rowLayout->setSpacing(6);

    QCheckBox *checkBox = new QCheckBox(text, rowWidget);
    checkBox->setChecked(checked);
    checkBox->setCursor(Qt::PointingHandCursor);

    rowLayout->addWidget(checkBox);
    rowLayout->addStretch();

    QWidgetAction *widgetAction = new QWidgetAction(optionsMenu);
    widgetAction->setDefaultWidget(rowWidget);

    optionsMenu->addAction(widgetAction);

    return checkBox;
}

void QuickAccessTab::slotThemeChange()
{
    QString tabStyle;
    tabStyle += "QuickAccessTab {";
    tabStyle += "background: " + gVar->getClrBackground() + ";";
    tabStyle += "}";

    tabStyle += "QScrollArea {";
    tabStyle += "background: transparent;";
    tabStyle += "border: none;";
    tabStyle += "}";

    tabStyle += "QWidget {";
    tabStyle += "background: transparent;";
    tabStyle += "}";

    setStyleSheet(tabStyle);

    QString btnStyle;
    btnStyle += "QPushButton {";
    btnStyle += "background: " + gVar->getClrSurface() + ";";
    btnStyle += "color: " + gVar->getClrPrimary() + ";";
    btnStyle += "border: 1px solid " + gVar->getClrSurface() + ";";
    btnStyle += "border-radius: 5px;";
    btnStyle += "padding: 0px;";
    btnStyle += "}";

    btnStyle += "QPushButton:hover {";
    btnStyle += "background: " + gVar->getClrGlow() + ";";
    btnStyle += "}";

    btnStyle += "QPushButton:pressed {";
    btnStyle += "background: " + gVar->getClrPrimary() + ";";
    btnStyle += "}";

    if (btnOptions)
    {
        QPixmap optionsPixmap = gVar->changeColorIcon(
            ":/Res/Images/Options.png",
            QColor(gVar->getClrPrimary()),
            QSize(22, 22)
            );

        btnOptions->setIcon(QIcon(optionsPixmap));
        btnOptions->setStyleSheet(btnStyle);
    }

    applyOptionsMenuStyle();
}