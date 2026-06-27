#ifndef FLOATWINDOW_H
#define FLOATWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include "Includes/globalvariables.h"

class FloatWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit FloatWindow(GlobalVariables *gVar,
                         QWidget *child,
                         const QString &name,
                         QWidget *parent = nullptr);

signals:
    void closed(QWidget *child);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    GlobalVariables *gVar;
    QWidget *childWidget;
    QString name;
};

#endif // FLOATWINDOW_H
