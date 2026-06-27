#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <QWidget>
#include <QLabel>
#include "Windows/basewindows.h"

class Analysis : public BaseWindows
{
    Q_OBJECT
public:
    explicit Analysis(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~Analysis() override;

protected:
    void render() override;

signals:
};

#endif // ANALYSIS_H
