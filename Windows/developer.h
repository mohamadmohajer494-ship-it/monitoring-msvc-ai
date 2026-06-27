#ifndef DEVELOPER_H
#define DEVELOPER_H

#include <QWidget>
#include <QLabel>
#include "Includes/globalvariables.h"

class Developer : public QWidget
{
    Q_OBJECT
public:
    explicit Developer(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~Developer() override;

private:
    GlobalVariables *gVar;
    QString style;

signals:
};

#endif // DEVELOPER_H
