#ifndef LAYERTREEWIDGET_H
#define LAYERTREEWIDGET_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QRadioButton>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QHeaderView>
#include <QApplication>
#include <QMouseEvent>
#include "Includes/globalvariables.h"

class LayerTreeItem : public QWidget
{
    Q_OBJECT
public:
    enum ItemType {
        ParentItem,
        ChildItem
    };

    explicit LayerTreeItem(GlobalVariables *gVar,
                           const QString &title,
                           ItemType type,
                           const QString &iconPath = QString(),
                           QWidget *parent = nullptr);
private:
    GlobalVariables *gVar;
    ItemType itemType;
    QString itemName;
    QString itemIconPath;
    QLabel *iconLabel = nullptr;
    QLabel *nameLabel = nullptr;
    QRadioButton *radioSelect = nullptr;
    QPushButton *btnVisibility = nullptr;
    QPushButton *btnDelete = nullptr;
    bool isVisible = true;

public:
    QPushButton* getBtnVisibility() { return btnVisibility; }
    QPushButton* getBtnDelete() { return btnDelete; }
    QRadioButton* getRadioSelect() { return radioSelect; }
    void setName(const QString &name);
    QString getName() const;
    ItemType getType() const;
    void setupUI();
    void updateStyle();
    void setVisibility(bool visible);

private slots:
    void onVisibilityClicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;

signals:
    void visibilityToggled(bool visible);
    void deleteRequested();
    void parentSelected();
    void visibilityChanged(bool act);
    void itemClicked();
    void itemHovered();
};

class LayerTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit LayerTreeWidget(GlobalVariables *gVar, QWidget *parent = nullptr);

private:
    GlobalVariables *gVar;
    QString style;
    QTreeWidgetItem *selectedParent;

public:
    QTreeWidgetItem* addParentLayer(const QString &name);
    QTreeWidgetItem* addChildLayer(QTreeWidgetItem *parent, const MapItemStruct &item);
    QTreeWidgetItem* addChildToSelected(const MapItemStruct &item);
    void removeChild(QTreeWidgetItem *child);
    void removeChild(QTreeWidgetItem *parent, int index);
    QTreeWidgetItem* addNewParent(const QString &name);
    void removeParent(QTreeWidgetItem *parent);
    QTreeWidgetItem* getSelectedParent() const;

private:
    static QString iconPathForShape(EnumMapShapeType type);
    void selectParent(QTreeWidgetItem *parentItem);

public slots:
    void slotThemeChange();

signals:
    void parentSelectionChanged(QTreeWidgetItem *selected);
    void childItemHovered(QTreeWidgetItem *childItem);

};

#endif // LAYERTREEWIDGET_H
