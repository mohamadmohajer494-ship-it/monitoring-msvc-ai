#ifndef OCXHANDLER_H
#define OCXHANDLER_H

#include <QObject>
#include <QAxWidget>
#include <QAxObject>
#include "ConnectionInterface.h"

class OCXHandler : public ConnectionInterface
{
    Q_OBJECT
public:
    explicit OCXHandler(QObject *parent = nullptr);

private:
    QAxWidget *activex = nullptr;

public:
    QAxWidget* getActiveX() const { return activex; }
    bool isConnected() override;
    void ConnectToSystem(ConnectionMode Connection,UDPType udptype,QString ip, short port
                         , short PortSendUDP, short PortReciveUPD, short mtu = 1000, QString Comport = "tty1", int Baudrate = 115200) override;
    void SendFrequency(double freqMHz) override;
    void SendThreshold(double thr) override;
    void SendDefaults(SystemDefualtsStruct SystemDefaults) override;
    void SendAntennaOffset(QMap<char, double> offsets) override;
    void SendAtennaState(QMap<char,bool> state) override;
    //void SendGPSData();
    void SendAGc(char agc) override;
    void SendMGc(short mgc) override;
    void SendRFHead(char rfhead) override;
    void SendDemod(char demod) override;
    void SendFFTPoint(short fft) override;
    void SendSquelch(bool state) override;
    void SendVolume(short vol) override;
    void SendAverage(double val) override;
    void SendQuality(double val) override;
    void SendPhaseAverage(double val) override;
    void SendAutoThreshold(bool val) override;
    void SendAzimuthHistogram(bool val) override;
    void SendClassificationState(ClassificationStruct Classsification) override;
    void SendBiteState(bool val) override;
    void SendAmrState(bool val) override;
    void SendAudioCodecOption(AudioCodecStruct acodec) override;
    void SendIFLogParam(IFlogParamStruct iflog) override;
    void SendBandwidth(char bw) override;
    void SendFilterBand(char filter) override;
    void SendFilter(char fil) override;
    void SendBandwidthElint(char bw) override;
    void SendThresholdLow(int val) override;
    void SendThresholdHigh(int val = 25000000) override;
    void SendElintADCRef(short lowRef,short HighRef) override;
    void SendElintMaxHold(bool state) override;
    void SendThresholdLimit(int val) override;
    void SendDFParameters(double Azimuth, double level, double Quality) override;
    void SendTransferDataRate(TransferDataStruct Tds) override;
    void SendAutoDetectAntenna(bool state) override;
    void SendAutoDetectCompass(bool state) override;
    void SendGPSData(GPSParameters Tds) override;
    void SendAutoDetectGPS(bool state) override;
    void SendChannelNumber(short channel) override;
    void SendPageNumber(short PageNumber) override;
    void SendElintAntenna(bool isElint) override;
    void SendSwitchSequence(short seq) override;
    void SendAudioSate(bool state) override;
    void SendComputerParameters(ComputersParametersStruct ComputerParams) override;
    void SendManualMode(ManualModeStruct manual) override;
    void SendDFlog(DFlogStruct logs) override;
    void SendDFMode(DFModeFindings_DFMODFND DFmode ) override;
    void SendPositionFinding(PFmodeStruct Pfparam) override;
    void SendMemoryScanParam(MemoryScanParamStruct MemPar) override;
    void SendMemoryScanState(MemoryScanStateStruct MemState) override;
    void SendSubRangeScanParam(SubRangeScanParamStruct SubPar) override;
    void SendSubRangeScanState(SubRangeScanStateStruct SubState) override;
    void SendFullRangeParam(FullRangeScanParamStruct Fullpar) override;
    void SendRemoveScanlist(RemoveScansTables_MODE scant) override;
    void SendGetAllDFConfig() override;
    void SendResetSystemConfig(ResetSystem_RESET resettype) override;
    void SendVersionRequest(std::string vs) override;
    void SendCalibraionParam(CalibrationStruct calib) override;

signals:
};

#endif // OCXHANDLER_H
