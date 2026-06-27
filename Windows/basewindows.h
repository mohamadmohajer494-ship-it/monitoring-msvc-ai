#ifndef BASEWINDOWS_H
#define BASEWINDOWS_H

#include <QWidget>
#include "glviewport.h"
#include "Includes/globalvariables.h"

class BaseWindows : public QWidget
{
    Q_OBJECT
public:
    explicit BaseWindows(GlobalVariables *gVar,
                         const QString& debugName,
                         QWidget *parent = nullptr);
    ~BaseWindows() override;

private:
    GlobalVariables *gVar;
    QString style;
    QVBoxLayout *vLayout;
    GLViewport *vp;
    SystemDefualtsStruct sysDefualts;

public:
    GlobalVariables* getGVar() { return gVar; }
    void setGVar(GlobalVariables *gv) { gVar = gv; }
    GLViewport* getVp() { return vp; }
    void setViewportFps(int fps);

public slots:
    virtual void onXferDefault(SystemDefualtsStruct def);
    virtual void onSystemParam(SystemParametersStruct param);
    virtual void onBandwidthChanged(QString str, double num);
    virtual void onResolutionChanged(QString str, double num);
    virtual void onModulationChanged(QString str);
    virtual void onIFFilterChanged(QString str);
    virtual void onRFHeadChanged(QString str);
    virtual void onFrequencyChanged(double num);
    virtual void onThresholdChanged(double num);

protected:
    void connectRepaint(QObject *obj);
    virtual void render() = 0;
    void attachRenderer();
    void showEvent(QShowEvent *event) override;

signals:
};

#endif // BASEWINDOWS_H
