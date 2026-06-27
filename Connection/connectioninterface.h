#ifndef CONNECTIONINTERFACE_H
#define CONNECTIONINTERFACE_H

#include <QObject>
#include "cdfsocket.h"

class ConnectionInterface : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionInterface(QObject *parent = nullptr);
    ~ConnectionInterface();
    int stationId = -1;

public:
    virtual bool isConnected();
    virtual void ConnectToSystem(ConnectionMode Connection,UDPType udptype,QString ip, short port
                         , short PortSendUDP, short PortReciveUPD, short mtu = 1000, QString Comport = "tty1", int Baudrate = 115200);
    virtual void disconnectFromSystem();
    virtual void SendFrequency(double freqMHz);
    virtual void SendThreshold(double thr);
    virtual void SendDefaults(SystemDefualtsStruct SystemDefaults);
    virtual void SendAntennaOffset(QMap<char, double> offsets);
    virtual void SendAtennaState(QMap<char,bool> state);
    //void SendGPSData();
    virtual void SendAGc(char agc);
    virtual void SendMGc(short mgc);
    virtual void SendRFHead(char rfhead);
    virtual void SendDemod(char demod);
    virtual void SendFFTPoint(short fft);
    virtual void SendSquelch(bool state);
    virtual void SendVolume(short vol);
    virtual void SendAverage(double val);
    virtual void SendQuality(double val);
    virtual void SendPhaseAverage(double val);
    virtual void SendAutoThreshold(bool val);
    virtual void SendAzimuthHistogram(bool val);
    virtual void SendClassificationState(ClassificationStruct Classsification);
    virtual void SendBiteState(bool val);
    virtual void SendAmrState(bool val);
    virtual void SendAudioCodecOption(AudioCodecStruct acodec);
    virtual void SendIFLogParam(IFlogParamStruct iflog);
    virtual void SendBandwidth(char bw);
    virtual void SendFilterBand(char filter);
    virtual void SendFilter(char fil);
    virtual void SendBandwidthElint(char bw);
    virtual void SendThresholdLow(int val);
    virtual void SendThresholdHigh(int val = 25000000);
    virtual void SendElintADCRef(short lowRef,short HighRef);
    virtual void SendElintMaxHold(bool state);
    virtual void SendThresholdLimit(int val);
    virtual void SendDFParameters(double Azimuth, double level, double Quality);
    virtual void SendTransferDataRate(TransferDataStruct Tds);
    virtual void SendAutoDetectAntenna(bool state);
    virtual void SendAutoDetectCompass(bool state);
    virtual void SendGPSData(GPSParameters Tds);
    virtual void SendAutoDetectGPS(bool state);
    virtual void SendChannelNumber(short channel);
    virtual void SendPageNumber(short PageNumber);
    virtual void SendElintAntenna(bool isElint);
    virtual void SendSwitchSequence(short seq);
    virtual void SendAudioSate(bool state);
    virtual void SendComputerParameters(ComputersParametersStruct ComputerParams);
    virtual void SendManualMode(ManualModeStruct manual);
    virtual void SendDFlog(DFlogStruct logs);
    virtual void SendDFMode(DFModeFindings_DFMODFND DFmode );
    virtual void SendPositionFinding(PFmodeStruct Pfparam);
    virtual void SendMemoryScanParam(MemoryScanParamStruct MemPar);
    virtual void SendMemoryScanState(MemoryScanStateStruct MemState);
    virtual void SendSubRangeScanParam(SubRangeScanParamStruct SubPar);
    virtual void SendSubRangeScanState(SubRangeScanStateStruct SubState);
    virtual void SendFullRangeParam(FullRangeScanParamStruct Fullpar);
    virtual void SendRemoveScanlist(RemoveScansTables_MODE scant);
    virtual void SendGetAllDFConfig();
    virtual void SendResetSystemConfig(ResetSystem_RESET resettype);
    virtual void SendVersionRequest(std::string vs);
    virtual void SendCalibraionParam(CalibrationStruct calib);

signals:
    void SystemParamEnvet(SystemParametersStruct);
    void SystemParamElintEnvet(SystemParametersElintStruct);
    void XferDFResult(DFResultStruct);
    void XferPSD(const double*,short);
    void XferPSDWide(const double*,short);
    void XferPDW(PulseParametersStruct*,short);
    void XferPSDFullRange(const double* data,short index,short cnt,short len);
    void XferDOA(const double*,double,short);
    void XferDefault(SystemDefualtsStruct);
    void XferAudioData(QByteArray,short);
    void XferPhaseAmp(double measuredPH1[],short len,double phaseAddr[],short lenPhase,double AmpAddr[],short lenAmp);
    void ConnectedState(bool);
    void AntennaStatesChanged(QMap<char,bool>,bool);
    void AntennaOffsetChanged(QMap<char,double>);
    void AgcChanged(char);
    void MGcChange(short);
    void RFHeadChanged(char);
    void DemodChanged(char);
    void QualityChanged(double);
    void FFTPointChanged(char);
    void SquelchChanged(bool);
    void VolumeChanged(short);
    void AverageChange(double);
    void PhaseAverageChange(double);
    void bandwidthChanged(char);
    void FilterBandChanged(char);
    void FilterChanged(char);
    void FrequencyChanged(double);
    void ThresholdChanged(double);
    void TransferDataRate(TransferDataStruct);
    void GPSDataChanged(GPSParameters);
    void AzimuthsMonitoringChanged(AzimuthsDataStruct);
    void GPSParametersChanged(GPSParameters);
    void CompassDataChanged(GPSParameters);
    void ChannelNumberChanged(short);
    void SwitchSequenceChanged(short);
    void PageNumberChanged(short);
    void ElintAntennaChanged(bool);
    void AudioParamChanged(bool);
    void ComputerParametersChanged(ComputersParametersStruct);
    void DFLogChanged(DFlogStruct);
    void AutoThresholdChange(bool);
    void ManualModeChanged(ManualModeStruct);
    void DFModeChanged(DFModeFindings_DFMODFND);
    void PositionFindingModeChanged(PFmodeStruct);
    void TargetAreaAndPositionChanged(TargetDataS TDs);
    void MemoryScanParamChanged(MemoryScanParamStruct);
    void MemoryScanStateChanged(MemoryScanStateStruct);
    void SubRangeScanParamChanged(SubRangeScanParamStruct);
    void SubRangeScanStateChanged(SubRangeScanStateStruct);
    void FullRangeParamChanged(FullRangeScanParamStruct);
    void RemoveScanlistChanged(RemoveScansTables_MODE);
    void GetAllDFConfigEvent();
    void ResetSystemConfigEvent(ResetSystem_RESET);
    void ScanResultData(ScanResultStruct);
    void VersionCommingChanged(std::string);
    void ErrorChanged(std::string);
    void CalibrationParamChanged(CalibrationStruct);
    void ClassificationStateChanged(ClassificationStruct);
    void BiteStatechanged(bool);
    void AmrStatechanged(bool);
    void IFLogParamtersChanged(IFlogParamStruct);
    void AudioCodecParamtersChanged(AudioCodecStruct);
    void SystemStateChanged(DFMode mode);
    void BiteTestResult(QByteArray);
    void ClassificationResult(ClassifiedSignalStruct);
    void ClassificationResultElint(QVector<SGroup>,short size);
    void AmrStereoDataChanged(structAMRResultStereo);
    void AmrMonoDataChanged( std::vector<AmrTargetOutputSimple>);
};

#endif // CONNECTIONINTERFACE_H
