#ifndef PCHLIB_H
#define PCHLIB_H

#include <QObject>
#include "Plots/baseplots.h"

class PchLib : public BasePlots
{
    Q_OBJECT
public:
    explicit PchLib(GlobalVariables* gVar, QObject *parent = nullptr);
    ~PchLib() override;

private:
    QVector<quint8>  occHeat;
    QVector<double>  lastPowerBuf;
    EnumPCHTheme paletteKind = EnumPCHTheme::Current;
    bool liveTrace = true;
    int occMax = 255;
    int occYBins = 64;
    int fadeTauMs = 1200;
    float occGamma = 0.55f;
    float liveWidth = 2.0f;
    bool occAutoScale = true;
    float occAutoAlpha = 0.98f;
    bool isShowMidLine = false;
    bool isShowColorMap = true;

public slots:
    void renderStandalone(const ImVec2& size = ImVec2(-1,-1)) override;
    void setData(const quint8* occ, int yBins, int newLen, const double* lastPower);

private:
    void drawPolychrome();
    void drawLiveTrace();

    inline ImU32 colorForOcc(quint8 v) const;
    inline ImU32 colorForOccTheme(quint8 v, EnumPCHTheme th) const;
    void fillPalette256(EnumPCHTheme kind, quint32 out[256]) const;

signals:
};

#endif // PCHLIB_H
