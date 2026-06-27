#include "btntextflip.h"

BtnTextFlip::BtnTextFlip(GlobalVariables *gVar,
                         QString name,
                         QString shortName,
                         QStringList itemList,
                         QString selectedText,
                         QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
    , name(name)
    , shortName(shortName)
    , itemList(itemList)
    , selectedText(selectedText)

{
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(3);
    vLayout->setAlignment(Qt::AlignCenter);
    setLayout(vLayout);
    setCursor(Qt::PointingHandCursor);
    setToolTip(name);
    lblTitel = new QLabel();
    lblTitel->setText(shortName);
    lblTitel->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(lblTitel, 0, Qt::AlignCenter);
    lblText = new QLabel();
    setSelectedItem(selectedText);
    lblText->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(lblText, 0, Qt::AlignCenter);
    slotThemeChange();
    connect(gVar,&GlobalVariables::ThemeChange,
            this,&BtnTextFlip::slotThemeChange);
}

BtnTextFlip::~BtnTextFlip()
{

}

void BtnTextFlip::removeItems(const QStringList &itemsToRemove)
{
    for (const QString &item : itemsToRemove) {
        itemList.removeAll(item);
    }
    if (!itemList.contains(selectedText)) {
        if (!itemList.isEmpty()) {
            selectedText = itemList.first();
            lblText->setText(selectedText);
        } else {
            selectedText.clear();
            lblText->setText("");
        }
    }
    rebuildContextMenu();
}

void BtnTextFlip::setItemList(const QStringList &newList)
{
    itemList = newList;
    if (!itemList.contains(selectedText)) {
        if (!itemList.isEmpty()) {
            selectedText = itemList.first();
            lblText->setText(selectedText);
        } else {
            selectedText.clear();
            lblText->setText("");
        }
    }
    rebuildContextMenu();
}

void BtnTextFlip::setSelectedItem(int index)
{
    selectedIndex = index;
    lblText->setText(itemList[selectedIndex]);
}

void BtnTextFlip::setSelectedItem(QString text)
{
    selectedIndex = itemList.indexOf(text);
    lblText->setText(itemList[selectedIndex]);
}

void BtnTextFlip::createContextMenu()
{
    if (contextMenu)
        return;
    contextMenu = new QMenu(this);
    createContextStyle();
    contextMenu->setFixedWidth(width()-2);
    if (itemList.isEmpty()) return;
    for (int var = 0; var < itemList.size(); ++var) {
        QString itemText = itemList[var];
        QWidget *actionWidget = new QWidget();
        QLabel *label = new QLabel(itemText, actionWidget);
        label->setCursor(Qt::PointingHandCursor);
        style += "QLabel {";
        style += " padding: 10px 0px;";
        style += "font-size: 14px;";
        style += " background: " + gVar->getClrSurface() + ";";
        style += "}";
        style += "QLabel:hover {";
        style += " background: " + gVar->getClrGlow() + ";";
        style += "}";
        label->setStyleSheet(style);
        label->setAlignment(Qt::AlignCenter);
        QVBoxLayout *layout = new QVBoxLayout(actionWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(label);
        QWidgetAction *widgetAction = new QWidgetAction(this);
        widgetAction->setDefaultWidget(actionWidget);
        contextMenu->addAction(widgetAction);
        connect(widgetAction, &QAction::triggered, this, [=]() {
            if (selectedText != itemText) {
                selectedText = itemText;
                setSelectedItem(selectedText);
                lblText->setStyleSheet(setStyle(gVar->getClrAccent()));
                emit changeSelected(selectedText);
            }
        });
    }
}

void BtnTextFlip::createContextStyle()
{
    style = "QMenu {";
    style += " background: " + gVar->getClrSurface() + ";";
    style += " border-top: none;";
    style += " border-left: 1px solid " + gVar->getClrPrimary() + ";";
    style += " border-right: 1px solid " + gVar->getClrPrimary() + ";";
    style += " border-bottom: 1px solid " + gVar->getClrPrimary() + ";";
    style += " border-top-left-radius: 0px;";
    style += " border-top-right-radius: 0px;";
    style += " border-bottom-left-radius: 6px;";
    style += " border-bottom-right-radius: 6px;";
    style += " padding: 0px;";
    style += "}";
    style += "QMenu::item {";
    style += " background: transparent;";
    style += " color: " + gVar->getClrPrimary() + ";";
    style += " text-align: center;";
    style += " min-width: " + QString::number(width()-2) + ";";
    style += "}";
    style += "QMenu::item:selected {";
    style += " background: " + gVar->getClrHighlight() + ";";
    style += " color: " + gVar->getClrPrimary() + ";";
    style += "}";
    contextMenu->setStyleSheet(style);
}

void BtnTextFlip::rebuildContextMenu()
{
    if (contextMenu) {
        contextMenu->deleteLater();
        contextMenu = nullptr;
    }
}

QString BtnTextFlip::setStyle(QString color)
{
    style = "background : transparent ;";
    style += "color: " + color + ";";
    style += "font-size: 16px;";
    style += "font-weight : normal ; ";
    return style;
}

void BtnTextFlip::slotThemeChange()
{
    style = "BtnTextFlip { ";
    style += "border-radius: 5px;";
    style += "background: " + gVar->getClrSurface() + ";";
    style += " } ";
    setStyleSheet(style);
    lblText->setStyleSheet(setStyle(gVar->getClrPrimary()));
    style += "font-weight : bold ; ";
    lblTitel->setStyleSheet(style);
}

void BtnTextFlip::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
    setFixedWidth(height());
}

void BtnTextFlip::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        selectedIndex++;
        if (selectedIndex > itemList.size() - 1)
            selectedIndex = 0;
        setSelectedItem(selectedIndex);
        lblText->setStyleSheet(setStyle(gVar->getClrAccent()));
        emit changeSelected(lblText->text());
    }
    QFrame::mousePressEvent(event);
}

void BtnTextFlip::contextMenuEvent(QContextMenuEvent *event)
{
    if (!contextMenu) {
        createContextMenu();
    }
    if (contextMenu) {
        QPoint pos = this->mapToGlobal(QPoint(0, height()));
        contextMenu->exec(pos);
    }
}
