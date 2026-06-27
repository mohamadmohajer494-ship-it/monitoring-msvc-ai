#ifndef PSDLIB_H
#define PSDLIB_H

#include <QObject>
#include "Plots/baseplots.h"

class PsdLib : public BasePlots
{
    Q_OBJECT
public:
    explicit PsdLib(GlobalVariables* gVar, QObject *parent = nullptr);
    ~PsdLib() override;

private:
    //--- Data ---
    QVector<double> psdData;
    QVector<double> maxData;
    QVector<double> avrData;
    QVector<double> prdData;
    QVector<double> difData;
    bool isShowPsd = true;
    bool isShowMax = true;
    bool isShowAvr = true;
    bool isShowPrd = true;
    bool isShowDif = false;
    float sizeAvr = 2.0f;
    float sizePrd = 2.0f;
    float sizeDif = 2.0f;
    ImVec4 clrPsd = ImVec4(0.196f, 0.827f, 0.074f, 1.0f);
    ImVec4 clrMax = ImVec4(0.545f, 0.0f, 0.0f, 1.0f);
    ImVec4 clrAvr = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
    ImVec4 clrPrd = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 clrDif = ImVec4(0.25f, 0.85f, 1.0f, 1.0f);

public slots:
    void renderStandalone(const ImVec2& size = ImVec2(-1,-1)) override;
    void setPsdData(const QVector<double>& data);
    void setMaxData(const QVector<double>& data);
    void setAvrData(const QVector<double>& data);
    void setPrdData(const QVector<double>& data);
    void setDifData(const QVector<double>& data);
    void setAllData(const QVector<double>& psd,
                    const QVector<double>& max,
                    const QVector<double>& avr,
                    const QVector<double>& prd,
                    const QVector<double>& dif);

private slots:
    void drowPsd();
    void drowMax();
    void drowAvr();
    void drowPrd();
    void drowDif();
    void restartDatas(int size);


signals:
};

#endif // PSDLIB_H
