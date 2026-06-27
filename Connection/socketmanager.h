#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#include <QObject>
#include "Includes/stationmodel.h"
#include "ConnectionInterface.h"
#include "libhandler.h"
#include "ocxhandler.h"

class SocketManager : public ConnectionInterface
{
    Q_OBJECT

public:
    enum ConnectionType {
        LIB_Mode = 0,
        OCX_Mode = 1,
    };

public:
    explicit SocketManager(QObject *parent = nullptr);
    ~SocketManager();

private:
    ConnectionInterface *handler = nullptr;
    EnumSocketState status = Socket_Free;
    int stationId = -1;
    EnumSystemMode socketRole = Role_NoMode;
    StationModel *station = nullptr;

public:
    EnumSocketState getStatus() { return status; }
    void setStatus(EnumSocketState stat) { status = stat; }
    void setHandler(ConnectionType newHandler);
    ConnectionInterface* getHandler() const;
    int getStationId() { return stationId; }
    void setStationId(int id) { stationId = id; }
    EnumSystemMode getSocketRole() { return socketRole; }
    void setStation(EnumSystemMode sock) { socketRole = sock; }
    StationModel* getStation() { return station; }
    void setStation(StationModel *stat) { station = stat; }

public:
    bool isConnected() override;
    void ConnectToSystem(ConnectionMode Connection,UDPType udptype,QString ip, short port
                         , short PortSendUDP, short PortReciveUPD, short mtu = 1000, QString Comport = "tty1", int Baudrate = 115200) override;
    void disconnectFromSystem() override;
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

#endif // SOCKETMANAGER_H
