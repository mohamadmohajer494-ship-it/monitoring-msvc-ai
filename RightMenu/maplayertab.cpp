#include "maplayertab.h"

MapLayerTab::MapLayerTab(GlobalVariables *gVar, QWidget *parent)
    : QFrame{parent}
    , gVar(gVar)
{
    createUi();
    createConnections();

    slotThemeChange();

    connect(gVar, &GlobalVariables::ThemeChange,
            this, &MapLayerTab::slotThemeChange);
}

MapLayerTab::~MapLayerTab()
{
}

void MapLayerTab::setMapLayerManager(MapLayerManager *manager)
{
    if (mapLayerManager == manager)
        return;

    if (mapLayerManager)
        disconnect(mapLayerManager, nullptr, this, nullptr);

    mapLayerManager = manager;

    if (mapLayerManager) {
        connect(mapLayerManager, &MapLayerManager::layersChanged,
                this, &MapLayerTab::onManagerLayersChanged);

        connect(mapLayerManager, &MapLayerManager::mapChanged,
                this, &MapLayerTab::onManagerLayersChanged);

        connect(mapLayerManager, &MapLayerManager::activeLayerChanged,
                this, &MapLayerTab::onManagerActiveLayerChanged);
    }

    reloadTree();
}

void MapLayerTab::createUi()
{
    setFrameShape(QFrame::NoFrame);

    vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(5, 5, 5, 5);
    vLayout->setSpacing(6);

    topBar = new QFrame(this);
    topBar->setFrameShape(QFrame::NoFrame);

    topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(5);

    btnAddLayer = new QPushButton(topBar);
    btnAddLayer->setCursor(Qt::PointingHandCursor);
    btnAddLayer->setFixedSize(32, 32);
    btnAddLayer->setToolTip("New Layer");
    QPixmap addPixmap = gVar->changeColorIcon(
        ":/Res/Images/Add.png",
        QColor(gVar->getClrPrimary()),
        QSize(24, 24)
        );
    btnAddLayer->setIcon(QIcon(addPixmap));
    btnAddLayer->setStyleSheet("background: transparent;");
    btnAddLayer->setIconSize(QSize(24, 24));
    topLayout->addWidget(btnAddLayer);
    topLayout->addStretch();
    btnExport = new QPushButton(topBar);
    btnExport->setCursor(Qt::PointingHandCursor);
    btnExport->setFixedSize(32, 32);
    btnExport->setToolTip("Export");
    QPixmap exportPixmap = gVar->changeColorIcon(
        ":/Res/Images/Export.png",
        QColor(gVar->getClrPrimary()),
        QSize(24, 24)
        );
    btnExport->setIcon(QIcon(exportPixmap));
    btnExport->setStyleSheet("background: transparent;");
    btnExport->setIconSize(QSize(24, 24));
    topLayout->addWidget(btnExport);
    btnImport = new QPushButton(topBar);
    btnImport->setCursor(Qt::PointingHandCursor);
    btnImport->setFixedSize(32, 32);
    btnImport->setToolTip("Import");
    QPixmap importPixmap = gVar->changeColorIcon(
        ":/Res/Images/Import.png",
        QColor(gVar->getClrPrimary()),
        QSize(24, 24)
        );
    btnImport->setIcon(QIcon(importPixmap));
    btnImport->setStyleSheet("background: transparent;");
    btnImport->setIconSize(QSize(24, 24));
    topLayout->addWidget(btnImport);
    tree = new MapLayerTreeWidget(this);
    tree->setColumnCount(2);
    tree->setHeaderHidden(true);
    tree->setRootIsDecorated(true);
    tree->setIndentation(14);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    tree->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    tree->setFocusPolicy(Qt::StrongFocus);

    tree->setDragEnabled(true);
    tree->setAcceptDrops(true);
    tree->setDropIndicatorShown(true);
    tree->setDragDropMode(QAbstractItemView::InternalMove);
    tree->setDefaultDropAction(Qt::MoveAction);
    tree->setDragDropOverwriteMode(false);

    tree->header()->setStretchLastSection(false);
    tree->header()->setSectionResizeMode(ColumnName, QHeaderView::Stretch);
    tree->header()->setSectionResizeMode(ColumnDelete, QHeaderView::Fixed);
    tree->setColumnWidth(ColumnDelete, 32);

    vLayout->addWidget(topBar);
    vLayout->addWidget(tree);
}

void MapLayerTab::createConnections()
{
    connect(btnAddLayer, &QPushButton::clicked,
            this, &MapLayerTab::onAddLayerClicked);

    connect(btnExport, &QPushButton::clicked,
            this, &MapLayerTab::onExportClicked);

    connect(btnImport, &QPushButton::clicked,
            this, &MapLayerTab::onImportClicked);

    connect(tree, &QTreeWidget::itemClicked,
            this, &MapLayerTab::onTreeItemClicked);

    connect(tree, &QTreeWidget::itemChanged,
            this, &MapLayerTab::onTreeItemChanged);

    connect(tree, &MapLayerTreeWidget::orderDropped,
            this, &MapLayerTab::onTreeOrderDropped);
}

void MapLayerTab::reloadTree()
{
    if (!tree)
        return;

    const int scrollValue = tree->verticalScrollBar()
                                ? tree->verticalScrollBar()->value()
                                : 0;

    const QSet<int> expandedLayerIds = collectExpandedLayerIds();
    const int selectedId = currentSelectedId();
    const RowKind selectedKind = currentSelectedKind();

    isReloading = true;

    QSignalBlocker blocker(tree);

    tree->clear();

    if (!mapLayerManager) {
        isReloading = false;
        return;
    }

    const QVector<MapLayerStruct> &layers = mapLayerManager->layers();

    for (const MapLayerStruct &layer : layers) {
        QTreeWidgetItem *layerRow = createLayerRow(layer);

        for (const MapItemStruct &item : layer.items) {
            createItemRow(layerRow, item);
        }

        layerRow->setExpanded(expandedLayerIds.contains(layer.eID));
    }

    updateActiveLayerStyle();

    restoreTreeState(expandedLayerIds, selectedId, selectedKind, scrollValue);

    isReloading = false;
}

void MapLayerTab::scheduleReloadTree()
{
    if (isPendingReload)
        return;

    isPendingReload = true;

    QTimer::singleShot(0, this, [this]() {
        isPendingReload = false;
        reloadTree();
    });
}

QSet<int> MapLayerTab::collectExpandedLayerIds() const
{
    QSet<int> expandedIds;

    if (!tree)
        return expandedIds;

    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *layerRow = tree->topLevelItem(i);

        if (!layerRow)
            continue;

        if (itemKind(layerRow) != RowLayer)
            continue;

        if (layerRow->isExpanded())
            expandedIds.insert(itemId(layerRow));
    }

    return expandedIds;
}

int MapLayerTab::currentSelectedId() const
{
    if (!tree || !tree->currentItem())
        return -1;

    return itemId(tree->currentItem());
}

void MapLayerTab::restoreTreeState(const QSet<int> &expandedLayerIds,
                                   int selectedId,
                                   RowKind selectedKind,
                                   int scrollValue)
{
    if (!tree)
        return;

    QTreeWidgetItem *selectedItem = nullptr;

    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *layerRow = tree->topLevelItem(i);

        if (!layerRow)
            continue;

        const int layerId = itemId(layerRow);

        layerRow->setExpanded(expandedLayerIds.contains(layerId));

        if (selectedKind == RowLayer && layerId == selectedId) {
            selectedItem = layerRow;
        }

        for (int j = 0; j < layerRow->childCount(); ++j) {
            QTreeWidgetItem *child = layerRow->child(j);

            if (!child)
                continue;

            if (selectedKind == RowItem && itemId(child) == selectedId) {
                selectedItem = child;
            }
        }
    }

    if (selectedItem) {
        selectedItem->setSelected(true);
        tree->setCurrentItem(selectedItem, ColumnName);
    }

    if (tree->verticalScrollBar()) {
        QTimer::singleShot(0, this, [this, scrollValue]() {
            if (tree && tree->verticalScrollBar()) {
                tree->verticalScrollBar()->setValue(scrollValue);
            }
        });
    }
}

MapLayerTab::RowKind MapLayerTab::currentSelectedKind() const
{
    if (!tree || !tree->currentItem())
        return RowItem;

    return itemKind(tree->currentItem());
}

QTreeWidgetItem* MapLayerTab::createLayerRow(const MapLayerStruct &layer)
{
    QTreeWidgetItem *row = new QTreeWidgetItem(tree);

    row->setText(ColumnName, layer.name);
    row->setCheckState(ColumnName, layer.isVisible ? Qt::Checked : Qt::Unchecked);

    row->setData(ColumnName, RoleKind, RowLayer);
    row->setData(ColumnName, RoleId, layer.eID);
    row->setData(ColumnName, RoleOldName, layer.name);
    row->setData(ColumnName, RoleOldVisible, layer.isVisible);

    row->setFlags(Qt::ItemIsEnabled |
                  Qt::ItemIsSelectable |
                  Qt::ItemIsUserCheckable |
                  Qt::ItemIsEditable |
                  Qt::ItemIsDragEnabled);

    row->setIcon(ColumnName, QIcon(":/Res/Images/Map/Layer.png"));

    QToolButton *deleteButton = createDeleteButton(RowLayer, layer.eID);
    tree->setItemWidget(row, ColumnDelete, deleteButton);

    return row;
}

QTreeWidgetItem* MapLayerTab::createItemRow(QTreeWidgetItem *parentLayer,
                                            const MapItemStruct &item)
{
    QTreeWidgetItem *row = new QTreeWidgetItem(parentLayer);

    row->setText(ColumnName, item.name);
    row->setCheckState(ColumnName, item.isVisible ? Qt::Checked : Qt::Unchecked);

    row->setData(ColumnName, RoleKind, RowItem);
    row->setData(ColumnName, RoleId, item.eID);
    row->setData(ColumnName, RoleOldName, item.name);
    row->setData(ColumnName, RoleOldVisible, item.isVisible);

    row->setFlags(Qt::ItemIsEnabled |
                  Qt::ItemIsSelectable |
                  Qt::ItemIsUserCheckable |
                  Qt::ItemIsEditable);

    row->setIcon(ColumnName, QIcon(itemIconPath(item)));

    QToolButton *deleteButton = createDeleteButton(RowItem, item.eID);
    tree->setItemWidget(row, ColumnDelete, deleteButton);

    return row;
}

QToolButton* MapLayerTab::createDeleteButton(RowKind kind, int id)
{
    QToolButton *button = new QToolButton(tree);
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedSize(24, 24);
    button->setAutoRaise(true);
    button->setToolTip("Delete");
    QPixmap deletePixmap = gVar->changeColorIcon(
        ":/Res/Images/Delete.png",
        QColor(gVar->getClrPrimary()),
        QSize(16, 16)
        );
    button->setIcon(QIcon(deletePixmap));
    button->setStyleSheet("background: transparent;");
    button->setIconSize(QSize(16, 16));

    connect(button, &QToolButton::clicked, this, [this, kind, id]() {
        if (!mapLayerManager)
            return;

        if (kind == RowLayer) {
            mapLayerManager->removeLayer(id);
        } else {
            mapLayerManager->removeItem(id);
        }
    });

    return button;
}

MapLayerTab::RowKind MapLayerTab::itemKind(QTreeWidgetItem *item) const
{
    if (!item)
        return RowItem;

    return static_cast<RowKind>(item->data(ColumnName, RoleKind).toInt());
}

int MapLayerTab::itemId(QTreeWidgetItem *item) const
{
    if (!item)
        return -1;

    return item->data(ColumnName, RoleId).toInt();
}

QTreeWidgetItem* MapLayerTab::layerItemFor(QTreeWidgetItem *item) const
{
    if (!item)
        return nullptr;

    if (itemKind(item) == RowLayer)
        return item;

    if (item->parent() && itemKind(item->parent()) == RowLayer)
        return item->parent();

    return nullptr;
}

QString MapLayerTab::itemIconPath(const MapItemStruct &item) const
{
    if (item.type == Marker && !item.style.iconPath.trimmed().isEmpty())
        return item.style.iconPath;

    switch (item.type) {
    case Marker:
        return ":/Res/Images/Map/Marker.png";

    case Text:
        return ":/Res/Images/Map/Text.png";

    case Line:
    case Polyline:
        return ":/Res/Images/Map/Line.png";

    case Rectangle:
    case Circle:
    case Ellipse:
    case Polygon:
    case Freehand:
    case Sector:
        return ":/Res/Images/Map/Area.png";

    case Measure:
        return ":/Res/Images/Map/Distance.png";

    default:
        return ":/Res/Images/Map/Layer.png";
    }
}

void MapLayerTab::beginEditCurrent()
{
    if (!tree)
        return;

    QTreeWidgetItem *item = tree->currentItem();

    if (!item)
        return;

    tree->editItem(item, ColumnName);
}

void MapLayerTab::deleteCurrent()
{
    if (!mapLayerManager || !tree)
        return;

    QTreeWidgetItem *item = tree->currentItem();

    if (!item)
        return;

    const int id = itemId(item);

    if (id <= 0)
        return;

    if (itemKind(item) == RowLayer) {
        mapLayerManager->removeLayer(id);
    } else {
        mapLayerManager->removeItem(id);
    }
}

void MapLayerTab::updateActiveLayerStyle()
{
    if (!tree || !mapLayerManager)
        return;

    const int activeLayerID = mapLayerManager->activeLayerID();

    int selectedItemID = -1;
    QTreeWidgetItem *current = tree->currentItem();

    if (current && itemKind(current) == RowItem) {
        selectedItemID = itemId(current);
    }

    const QColor clrActive(gVar->getClrPrimary());
    const QColor clrInactive(gVar->getClrGlow());

    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *layerRow = tree->topLevelItem(i);

        if (!layerRow)
            continue;

        const bool isActiveLayer = itemId(layerRow) == activeLayerID;

        QFont layerFont = layerRow->font(ColumnName);
        layerFont.setBold(true);
        layerRow->setFont(ColumnName, layerFont);

        layerRow->setForeground(
            ColumnName,
            QBrush(isActiveLayer ? clrActive : clrInactive)
            );

        for (int j = 0; j < layerRow->childCount(); ++j) {
            QTreeWidgetItem *child = layerRow->child(j);

            if (!child)
                continue;

            const bool isSelectedItem = itemId(child) == selectedItemID;

            QFont childFont = child->font(ColumnName);
            childFont.setBold(true);
            child->setFont(ColumnName, childFont);

            child->setForeground(
                ColumnName,
                QBrush(isSelectedItem ? clrActive : clrInactive)
                );
        }
    }
}

void MapLayerTab::onAddLayerClicked()
{
    if (!mapLayerManager)
        return;

    const QString baseName = "New Layer";
    QString name = baseName;

    int index = 1;
    bool exists = true;

    while (exists) {
        exists = false;

        for (const MapLayerStruct &layer : mapLayerManager->layers()) {
            if (layer.name == name) {
                exists = true;
                break;
            }
        }

        if (exists)
            name = QString("%1 %2").arg(baseName).arg(index++);
    }

    mapLayerManager->addLayer(name, 0);
}

void MapLayerTab::onRenameClicked()
{
    beginEditCurrent();
}

void MapLayerTab::onDeleteClicked()
{
    deleteCurrent();
}

void MapLayerTab::onExportClicked()
{
    if (!mapLayerManager) {
        QMessageBox::warning(this, "Export", "MapLayerManager not available.");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Export Map Layers",
        "",
        "Omni Map Layer (*.omlayer)"
        );

    if (filePath.isEmpty())
        return;

    if (!filePath.endsWith(".omlayer", Qt::CaseInsensitive))
        filePath += ".omlayer";

    if (!mapLayerManager->exportLayersBinary(filePath)) {
        QMessageBox::critical(this, "Export", "Failed to export layers.");
        return;
    }

    QMessageBox::information(this, "Export", "Layers exported successfully.");
}

void MapLayerTab::onImportClicked()
{
    if (!mapLayerManager) {
        QMessageBox::warning(this, "Import", "MapLayerManager not available.");
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Import Map Layers",
        "",
        "Omni Map Layer (*.omlayer)"
        );

    if (filePath.isEmpty())
        return;

    QMessageBox::StandardButton mode = QMessageBox::question(
        this,
        "Import",
        "Replace current layers?\nYes = replace all\nNo = append",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
        QMessageBox::No
        );

    if (mode == QMessageBox::Cancel)
        return;

    bool replace = mode == QMessageBox::Yes;

    if (!mapLayerManager->importLayersBinary(filePath, replace)) {
        QMessageBox::critical(this, "Import", "Failed to import layers. File may be corrupted.");
        return;
    }

    QMessageBox::information(this, "Import", "Layers imported successfully.");
}

void MapLayerTab::onTreeItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)

    if (!mapLayerManager || !item)
        return;

    QTreeWidgetItem *layerRow = layerItemFor(item);

    if (!layerRow)
        return;

    const int layerID = itemId(layerRow);

    if (layerID > 0)
        mapLayerManager->setActiveLayer(layerID);
}

void MapLayerTab::onTreeItemChanged(QTreeWidgetItem *item, int column)
{
    if (isReloading || !mapLayerManager || !item)
        return;

    if (column != ColumnName)
        return;

    const int id = itemId(item);

    if (id <= 0)
        return;

    const RowKind kind = itemKind(item);

    const bool oldVisible = item->data(ColumnName, RoleOldVisible).toBool();
    const bool newVisible = item->checkState(ColumnName) == Qt::Checked;

    if (oldVisible != newVisible) {
        item->setData(ColumnName, RoleOldVisible, newVisible);

        isChangingVisibleFromTree = true;

        if (kind == RowLayer) {
            mapLayerManager->setLayerVisible(id, newVisible);
        } else {
            mapLayerManager->setItemVisible(id, newVisible);
        }

        isChangingVisibleFromTree = false;

        return;
    }

    const QString oldName = item->data(ColumnName, RoleOldName).toString();
    const QString newName = item->text(ColumnName).trimmed();

    if (newName.isEmpty()) {
        isReloading = true;
        item->setText(ColumnName, oldName);
        isReloading = false;
        return;
    }

    if (newName == oldName)
        return;

    item->setData(ColumnName, RoleOldName, newName);

    if (kind == RowLayer) {
        mapLayerManager->updateLayerName(id, newName);
        return;
    }

    MapItemStruct *oldItem = mapLayerManager->findItem(id);

    if (!oldItem)
        return;

    MapItemStruct updatedItem = *oldItem;
    updatedItem.name = newName;

    if (updatedItem.type == Text)
        updatedItem.note = newName;

    mapLayerManager->updateItem(updatedItem);
}

void MapLayerTab::onManagerLayersChanged()
{
    if (isChangingVisibleFromTree)
        return;

    scheduleReloadTree();
}

void MapLayerTab::onManagerActiveLayerChanged(int layerID)
{
    Q_UNUSED(layerID)
    updateActiveLayerStyle();
}

void MapLayerTab::onTreeRowsMoved(const QModelIndex &parent,
                                  int start,
                                  int end,
                                  const QModelIndex &destination,
                                  int row)
{
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    Q_UNUSED(destination)
    Q_UNUSED(row)

    saveLayerOrderFromTree();
}

void MapLayerTab::saveLayerOrderFromTree()
{
    if (isReloading || isSavingOrder || !mapLayerManager || !tree)
        return;

    QVector<int> orderedLayerIds;
    orderedLayerIds.reserve(tree->topLevelItemCount());

    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *layerRow = tree->topLevelItem(i);

        if (!layerRow || itemKind(layerRow) != RowLayer) {
            scheduleReloadTree();
            return;
        }

        const int layerId = itemId(layerRow);

        if (layerId <= 0) {
            scheduleReloadTree();
            return;
        }

        orderedLayerIds.push_back(layerId);
    }

    isSavingOrder = true;

    const bool ok = mapLayerManager->reorderLayersByIds(orderedLayerIds);

    isSavingOrder = false;

    if (!ok) {
        qDebug() << "Failed to save layer order";
        scheduleReloadTree();
    }
}

void MapLayerTab::onTreeOrderDropped()
{
    if (isReloading || isSavingOrder || !mapLayerManager || !tree)
        return;

    saveLayerOrderFromTree();
}

void MapLayerTab::slotThemeChange()
{
    style = "MapLayerTab {";
    style += "background : " + gVar->getClrBackground() + " ;";
    style += "}";
    setStyleSheet(style);

    style = "QPushButton {";
    style += "background : " + gVar->getClrSurface() + " ;";
    style += "color : " + gVar->getClrPrimary() + " ;";
    style += "border : 1px solid " + gVar->getClrSurface() + " ;";
    style += "border-radius : 5px ;";
    style += "padding : 4px 8px ;";
    style += "font-weight : bold ;";
    style += "}";
    style += "QPushButton:hover {";
    style += "background : " + gVar->getClrGlow() + " ;";
    style += "}";

    if (btnAddLayer)
        btnAddLayer->setStyleSheet(style);
    if (btnExport)
        btnExport->setStyleSheet(style);
    if (btnImport)
        btnImport->setStyleSheet(style);
\
    style = "QTreeWidget {";
    style += "background : " + gVar->getClrBackground() + " ;";
    style += "color : " + gVar->getClrPrimary() + " ;";
    style += "border : none ;";
    style += "outline : none ;";
    style += "}";
    style += "QTreeWidget::item {";
    style += "height : 28px ;";
    style += "padding : 2px ;";
    style += "}";
    style += "QTreeWidget::item:selected {";
    style += "background : " + gVar->getClrSurface() + " ;";
    style += "color : " + gVar->getClrPrimary() + " ;";
    style += "}";

    if (tree)
        tree->setStyleSheet(style);
}