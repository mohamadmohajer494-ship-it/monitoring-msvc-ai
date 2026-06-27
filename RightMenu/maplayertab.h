#ifndef MAPLAYERTAB_H
#define MAPLAYERTAB_H

#include <QWidget>
#include <QFrame>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFont>
#include <QBrush>
#include <QColor>
#include <QIcon>
#include <QTimer>
#include <QSignalBlocker>
#include <QSet>
#include <QScrollBar>
#include <QDropEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QAbstractItemModel>
#include "Includes/globalvariables.h"
#include "Includes/GlobalStruct.h"
#include "Includes/GlobalEnum.h"
#include "DataCore/maplayermanager.h"

class MapLayerTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit MapLayerTreeWidget(QWidget *parent = nullptr)
        : QTreeWidget(parent)
    {
    }

signals:
    void orderDropped();

protected:
    void dropEvent(QDropEvent *event) override
    {
        QTreeWidget::dropEvent(event);
        emit orderDropped();
    }
};

class MapLayerTab : public QFrame
{
    Q_OBJECT

public:
    explicit MapLayerTab(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~MapLayerTab() override;

    void setMapLayerManager(MapLayerManager *manager);

private:
    enum TreeColumn {
        ColumnName = 0,
        ColumnDelete = 1
    };

    enum TreeRole {
        RoleKind = Qt::UserRole + 1,
        RoleId,
        RoleOldName,
        RoleOldVisible
    };

    enum RowKind {
        RowLayer = 1,
        RowItem = 2
    };

private:
    GlobalVariables *gVar = nullptr;
    MapLayerManager *mapLayerManager = nullptr;

    QString style;

    QVBoxLayout *vLayout = nullptr;

    QFrame *topBar = nullptr;
    QHBoxLayout *topLayout = nullptr;

    QPushButton *btnAddLayer = nullptr;
    QPushButton *btnExport = nullptr;
    QPushButton *btnImport = nullptr;

    MapLayerTreeWidget *tree = nullptr;

    bool isReloading = false;
    bool isPendingReload = false;
    bool isChangingVisibleFromTree = false;
    bool isSavingOrder = false;

private:
    void createUi();
    void createConnections();

    void reloadTree();
    void scheduleReloadTree();
    QSet<int> collectExpandedLayerIds() const;
    int currentSelectedId() const;
    RowKind currentSelectedKind() const;
    void restoreTreeState(const QSet<int> &expandedLayerIds,
                          int selectedId,
                          RowKind selectedKind,
                          int scrollValue);
    QTreeWidgetItem* createLayerRow(const MapLayerStruct &layer);
    QTreeWidgetItem* createItemRow(QTreeWidgetItem *parentLayer, const MapItemStruct &item);

    QToolButton* createDeleteButton(RowKind kind, int id);

    RowKind itemKind(QTreeWidgetItem *item) const;
    int itemId(QTreeWidgetItem *item) const;

    QTreeWidgetItem* layerItemFor(QTreeWidgetItem *item) const;

    QString itemIconPath(const MapItemStruct &item) const;

    void beginEditCurrent();
    void deleteCurrent();
    void updateActiveLayerStyle();

private slots:
    void onAddLayerClicked();
    void onRenameClicked();
    void onDeleteClicked();
    void onExportClicked();
    void onImportClicked();

    void onTreeItemClicked(QTreeWidgetItem *item, int column);
    void onTreeItemChanged(QTreeWidgetItem *item, int column);

    void onManagerLayersChanged();
    void onManagerActiveLayerChanged(int layerID);

    void onTreeRowsMoved(const QModelIndex &parent,
                         int start,
                         int end,
                         const QModelIndex &destination,
                         int row);
    void saveLayerOrderFromTree();
    void onTreeOrderDropped();

public slots:
    void slotThemeChange();
};

#endif // MAPLAYERTAB_H