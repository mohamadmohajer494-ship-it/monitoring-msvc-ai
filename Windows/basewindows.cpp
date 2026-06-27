#include "basewindows.h"

BaseWindows::BaseWindows(GlobalVariables *gVar,
                         const QString &debugName,
                         QWidget *parent)
    : QWidget{parent}
    , gVar(gVar)
{
    vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    setLayout(vLayout);
    vp = new GLViewport(gVar,this);
    vp->setDebugName(debugName);
    vLayout->addWidget(vp);
}

BaseWindows::~BaseWindows()
{
    if (vp)
        vp->clearRenderers();
}

void BaseWindows::setViewportFps(int fps)
{
    if (vp)
        vp->setTargetFps(fps);
}

void BaseWindows::onXferDefault(SystemDefualtsStruct def)
{
    sysDefualts = def;
}

void BaseWindows::onSystemParam(SystemParametersStruct param)
{

}

void BaseWindows::onBandwidthChanged(QString str, double num)
{

}

void BaseWindows::onResolutionChanged(QString str, double num)
{

}

void BaseWindows::onModulationChanged(QString str)
{

}

void BaseWindows::onIFFilterChanged(QString str)
{

}

void BaseWindows::onRFHeadChanged(QString str)
{

}

void BaseWindows::onFrequencyChanged(double num)
{

}

void BaseWindows::onThresholdChanged(double num)
{

}

void BaseWindows::connectRepaint(QObject *obj)
{
    // QObject::connect(obj, SIGNAL(needsRepaint()),
    //                  vp, SLOT(update()),
    //                  Qt::QueuedConnection);
    QObject::connect(obj, SIGNAL(needsRepaint()),
                     vp, SLOT(requestRepaint()),
                     Qt::QueuedConnection);
}

void BaseWindows::attachRenderer()
{
    vp->clearRenderers();
    vp->addRenderer([this](){
        render();
    });
    vp->update();
}

void BaseWindows::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (vp)
        vp->update();
}
