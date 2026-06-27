// layertreewidget.cpp
#include "layertreewidget.h"

// LayerTreeItem Implementation
LayerTreeItem::LayerTreeItem(GlobalVariables *gVar,
                           const QString &title,
                           ItemType type,
                           const QString &iconPath,
                           QWidget *parent)
    : QWidget(parent)
    , gVar(gVar)
    , itemType(type)
    , itemName(title)
    , itemIconPath(iconPath)
    , isVisible(true)
{
    setupUI();
    updateStyle();
}

void LayerTreeItem::setName(const QString &name)
{
    itemName = name;
    nameLabel->setText(name);
}

QString LayerTreeItem::getName() const
{
    return itemName;
}

LayerTreeItem::ItemType LayerTreeItem::getType() const
{
    return itemType;
}

void LayerTreeItem::setupUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(4);
    if (itemType == ParentItem) {
        radioSelect = new QRadioButton(this);
        radioSelect->setCursor(Qt::PointingHandCursor);
        radioSelect->setFixedSize(18, 18);
        radioSelect->setStyleSheet(R"(
            QRadioButton { spacing: 0px; }
            QRadioButton::indicator {
                width: 14px; height: 14px;
                border-radius: 7px;
                border: 2px solid #7a8a9a;
                background: transparent;
            }
            QRadioButton::indicator:hover { border-color: #4fc3f7; }
            QRadioButton::indicator:checked {
                border: 2px solid #4fc3f7;
                background: qradialgradient(cx:0.5, cy:0.5, radius:0.4,
                    fx:0.5, fy:0.5,
                    stop:0 #4fc3f7, stop:0.55 #4fc3f7,
                    stop:0.56 transparent);
            }
        )");
        connect(radioSelect, &QRadioButton::clicked, this, &LayerTreeItem::parentSelected);
        layout->addWidget(radioSelect);
    }
    if (itemType == ChildItem) {
        iconLabel = new QLabel(this);
        iconLabel->setFixedSize(20, 20);
        QString path = itemIconPath.isEmpty() ? ":/Res/Images/Map/Marker.png" : itemIconPath;
        iconLabel->setPixmap(QPixmap(path).scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        layout->addWidget(iconLabel);
    }
    nameLabel = new QLabel(itemName, this);
    nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(nameLabel, 1);
    btnVisibility = new QPushButton(this);
    btnVisibility->setFlat(true);
    btnVisibility->setCursor(Qt::PointingHandCursor);
    connect(btnVisibility, &QPushButton::clicked, this, &LayerTreeItem::onVisibilityClicked);
    layout->addWidget(btnVisibility);
    if (itemType == ChildItem) {
        btnDelete = new QPushButton(this);
        btnDelete->setFlat(true);
        btnDelete->setCursor(Qt::PointingHandCursor);
        connect(btnDelete, &QPushButton::clicked, this, &LayerTreeItem::deleteRequested);
        layout->addWidget(btnDelete);
    }
}

void LayerTreeItem::updateStyle()
{
    if (!btnVisibility) return;
    QString textColor = gVar->getClrPrimary();
    setStyleSheet(QString(
                      "QWidget { background: transparent; color: %1; }"
                      "QPushButton { background: transparent; border: none; }"
                      "QPushButton:hover { background: rgba(255,255,255,0.1); border-radius: 4px; }"
                      "QRadioButton { color: %1; }"
                      ).arg(textColor));
    QSize iconSize(16, 16);
    QString visibilityText = isVisible
                                 ? "Show"
                                 : "Hide";
    QString visibilityPath = isVisible
                                 ? ":/Res/Images/Eye.png"
                                 : ":/Res/Images/Hide.png";
    QPixmap visibilityPixmap = gVar->changeColorIcon(
        visibilityPath,
        QColor(textColor),
        iconSize
        );
    btnVisibility->setToolTip(visibilityText);
    btnVisibility->setIcon(QIcon(visibilityPixmap));
    btnVisibility->setIconSize(iconSize);
    btnVisibility->setFixedSize(20, 20);
    if (btnDelete) {
        QPixmap deletePixmap = gVar->changeColorIcon(
            ":/Res/Images/Delete.png",
            QColor(textColor),
            iconSize
            );
        btnDelete->setToolTip("Delete");
        btnDelete->setIcon(QIcon(deletePixmap));
        btnDelete->setIconSize(iconSize);
        btnDelete->setFixedSize(20, 20);
    }
}

void LayerTreeItem::setVisibility(bool visible)
{
    if (isVisible == visible)
        return;
    isVisible = visible;
    updateStyle();
    emit visibilityChanged(visible);
}

void LayerTreeItem::onVisibilityClicked()
{
    isVisible = !isVisible;
    updateStyle();
    emit visibilityToggled(isVisible);
    void itemClicked();
}

void LayerTreeItem::mousePressEvent(QMouseEvent *event)
{
    emit itemClicked();
    QWidget::mousePressEvent(event);
}

void LayerTreeItem::enterEvent(QEnterEvent *event)
{
    emit itemHovered();
    QWidget::enterEvent(event);
}

LayerTreeWidget::LayerTreeWidget(GlobalVariables *gVar, QWidget *parent)
    : QTreeWidget(parent)
    , gVar(gVar)
    , selectedParent(nullptr)
{
    setHeaderHidden(true);
    setIndentation(12);
    setDragEnabled(false);
    setAcceptDrops(false);
    setDragDropMode(QAbstractItemView::NoDragDrop);
    setSelectionMode(QAbstractItemView::NoSelection);
    setFocusPolicy(Qt::NoFocus);
    header()->setSectionResizeMode(QHeaderView::Stretch);
    setStyleSheet(
        "QTreeWidget { border: none; background: transparent; outline: none; }"
        "QTreeWidget::item { padding: 2px 0px; border: none; outline: none; }"
        "QTreeWidget::item:hover { background: rgba(255,255,255,0.05); }"
        "QTreeWidget::item:selected { background: transparent; border: none; }"
        "QTreeWidget::item:focus { border: none; outline: none; }"
        );
}

QTreeWidgetItem* LayerTreeWidget::addParentLayer(const QString &name)

{
    QTreeWidgetItem *item = new QTreeWidgetItem(this);
    LayerTreeItem *widget = new LayerTreeItem(gVar, name, LayerTreeItem::ParentItem);
    addTopLevelItem(item);
    setItemWidget(item, 0, widget);
    item->setExpanded(true);
    item->setFlags(Qt::ItemIsEnabled);
    connect(widget, &LayerTreeItem::parentSelected, this, [this, item]() {
        selectParent(item);
    });
    connect(widget, &LayerTreeItem::visibilityToggled, this, [this, item](bool visible) {
        for (int j = 0; j < item->childCount(); ++j) {
            LayerTreeItem *childWidget = qobject_cast<LayerTreeItem*>(
                itemWidget(item->child(j), 0));
            if (childWidget)
                childWidget->setVisibility(visible);
        }
    });
    connect(widget, &LayerTreeItem::itemClicked, this, [this, item]() {
        selectParent(item);
    });
    connect(widget, &LayerTreeItem::itemHovered, this, [this, item]() {
        emit childItemHovered(item);
    });
    if (!selectedParent) {
        selectParent(item);
    }
    return item;
}

QTreeWidgetItem* LayerTreeWidget::addChildLayer(QTreeWidgetItem *parent,
                                                const MapItemStruct &item)
{
    QTreeWidgetItem *childItem = new QTreeWidgetItem(parent);
    LayerTreeItem *widget = new LayerTreeItem(gVar,
                                              item.name,
                                              LayerTreeItem::ChildItem,
                                              iconPathForShape(item.type));
    setItemWidget(childItem, 0, widget);
    childItem->setFlags(Qt::ItemIsEnabled);

    connect(widget, &LayerTreeItem::deleteRequested, this, [this, childItem]() {
        removeChild(childItem);
    });
    connect(widget, &LayerTreeItem::itemClicked, this, [this, parent]() {
        selectParent(parent);
    });
    return childItem;
}

QTreeWidgetItem *LayerTreeWidget::addChildToSelected(const MapItemStruct &item)
{
    if (!selectedParent)
        return nullptr;
    return addChildLayer(selectedParent, item);
}

void LayerTreeWidget::removeChild(QTreeWidgetItem *child)
{
    if (!child || !child->parent())
        return;
    QTreeWidgetItem *parent = child->parent();
    int idx = parent->indexOfChild(child);
    parent->takeChild(idx);
    delete child;
}

void LayerTreeWidget::removeChild(QTreeWidgetItem *parent, int index)
{
    if (!parent || index < 0 || index >= parent->childCount())
        return;
    QTreeWidgetItem *child = parent->takeChild(index);
    delete child;
}

QTreeWidgetItem *LayerTreeWidget::addNewParent(const QString &name)
{
    return addParentLayer(name);
}

void LayerTreeWidget::removeParent(QTreeWidgetItem *parent)
{
    if (!parent)
        return;
    int idx = indexOfTopLevelItem(parent);
    if (idx < 0)
        return;
    bool wasSelected = (parent == selectedParent);
    takeTopLevelItem(idx);
    delete parent;
    if (wasSelected) {
        selectedParent = nullptr;
        if (topLevelItemCount() > 0) {
            selectParent(topLevelItem(0));
        }
    }
}

QTreeWidgetItem *LayerTreeWidget::getSelectedParent() const
{
    return selectedParent;
}

QString LayerTreeWidget::iconPathForShape(EnumMapShapeType type)
{
    switch (type) {
    case Marker:
        return QStringLiteral(":/Res/Images/Map/Marker.png");
    case Line:
        return QStringLiteral(":/Res/Images/Map/Line.png");
    case Rectangle:
    case Circle:
    case Polygon:
    case Freehand:
    default:
        return QStringLiteral(":/Res/Images/Map/Area.png");
    }
}

void LayerTreeWidget::selectParent(QTreeWidgetItem *parentItem)
{
    if (selectedParent) {
        LayerTreeItem *oldWidget = qobject_cast<LayerTreeItem*>(itemWidget(selectedParent, 0));
        if (oldWidget && oldWidget->getRadioSelect()) {
            oldWidget->getRadioSelect()->setChecked(false);
        }
    }
    selectedParent = parentItem;
    LayerTreeItem *newWidget = qobject_cast<LayerTreeItem*>(itemWidget(selectedParent, 0));
    if (newWidget && newWidget->getRadioSelect()) {
        newWidget->getRadioSelect()->setChecked(true);
    }
    emit parentSelectionChanged(selectedParent);
}

void LayerTreeWidget::slotThemeChange()
{
    for (int i = 0; i < topLevelItemCount(); ++i) {
        QTreeWidgetItem *pItem = topLevelItem(i);
        LayerTreeItem *pWidget = qobject_cast<LayerTreeItem*>(itemWidget(pItem, 0));
        if (pWidget)
            pWidget->updateStyle();
        for (int j = 0; j < pItem->childCount(); ++j) {
            QTreeWidgetItem *cItem = pItem->child(j);
            LayerTreeItem *cWidget = qobject_cast<LayerTreeItem*>(itemWidget(cItem, 0));
            if (cWidget)
                cWidget->updateStyle();
        }
    }
}