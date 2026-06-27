#ifndef BTNTEXTFLIP_H
#define BTNTEXTFLIP_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPoint>
#include <QWidgetAction>
#include "Includes/globalvariables.h"

class BtnTextFlip : public QFrame
{
    Q_OBJECT
public:
    explicit BtnTextFlip(GlobalVariables *gVar,
                         QString name,
                         QString shortName,
                         QStringList itemList,
                         QString selectedText,
                         QWidget *parent = nullptr);
    ~BtnTextFlip();

private:
    GlobalVariables *gVar;
    QString style;
    QString name;
    QString shortName;
    QStringList itemList;
    QString selectedText;
    QLabel *lblTitel;
    QLabel *lblText;
    QMenu *contextMenu = nullptr;
    int selectedIndex = 0;


public:
    void removeItems(const QStringList &itemsToRemove);
    void setItemList(const QStringList &newList);
    void setSelectedItem(int index);
    void setSelectedItem(QString text);

private:
    void createContextMenu();
    void createContextStyle();
    void rebuildContextMenu();
    QString setStyle(QString color);

public slots:
    void slotThemeChange();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void changeSelected(QString item);
};

#endif // BTNTEXTFLIP_H
