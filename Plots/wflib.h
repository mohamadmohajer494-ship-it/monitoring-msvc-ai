#ifndef WFLIB_H
#define WFLIB_H

#include <QObject>
#include "Plots/baseplots.h"

class WfLib : public BasePlots
{
    Q_OBJECT
public:
    explicit WfLib(GlobalVariables* gVar, QObject *parent = nullptr);
    ~WfLib() override;

private:
    QVector<double> waveData;
    QVector<double> fftData;
    double timeSpanSec   = 30.0;
    int    rowsPerSecond = 10;
    int    rowsCap       = 300;
    int    writeRow      = 0;
    int    filled        = 0;
    QVector<float>   rawRB;
    QVector<quint32> imgRGBA;
    QVector<float>   emaRow;
    quint32          palette[256]{};
    GLuint texId = 0;
    int    texW  = 0;
    int    texH  = 0;
    qint64 m_writtenRows  = 0;
    qint64 m_uploadedRows = 0;
    float  denoiseAlpha   = 1.0f;
    float  quantizeDbStep = 0.0f;
    bool useHot = false;

public slots:
    void renderStandalone(const ImVec2& size = ImVec2(-1,-1)) override;
    void setData(const QVector<double>& newRow, int newLen);

private:
    void drawWaveform();
    void drawFFT();

signals:
};

#endif // WFLIB_H
