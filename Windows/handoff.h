#ifndef HANDOFF_H
#define HANDOFF_H

#include <QWidget>
#include <QLabel>
#include "Windows/basewindows.h"

class Handoff : public BaseWindows
{
    Q_OBJECT
public:
    explicit Handoff(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~Handoff() override;

protected:
    void render() override;

signals:
};

#endif // HANDOFF_H
