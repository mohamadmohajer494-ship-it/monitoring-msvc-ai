#ifndef PANORAMA_H
#define PANORAMA_H

#include <QWidget>
#include <QLabel>
#include "Windows/basewindows.h"

class Panorama : public BaseWindows
{
    Q_OBJECT
public:
    explicit Panorama(GlobalVariables *gVar, QWidget *parent = nullptr);
    ~Panorama() override;

protected:
    void render() override;

signals:
};

#endif // PANORAMA_H
