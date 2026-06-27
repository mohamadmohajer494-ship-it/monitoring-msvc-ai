#include "socketmanager.h"
#include <QDebug>

SocketManager::SocketManager(QObject *parent)
    : ConnectionInterface{parent}
{

}

SocketManager::~SocketManager()
{

}

void SocketManager::setHandler(ConnectionType newHandler)
{
    if (handler) {
        handler->disconnect(this);
        handler->deleteLater();
        handler = nullptr;
    }
    switch (newHandler) {
    case LIB_Mode:
        handler = new LIBHandler(this);
        break;
    case OCX_Mode:
        handler = new OCXHandler(this);
        break;
    default:
        handler = nullptr;
        break;
    }
    qDebug() << "SocketManager::setHandler, handler =" << handler
             << "thread =" << QThread::currentThread();
    if (!handler)
        return;
    connect(handler, &ConnectionInterface::SystemParamEnvet,
            this, &SocketManager::SystemParamEnvet);
    connect(handler, &ConnectionInterface::SystemParamElintEnvet,
            this, &SocketManager::SystemParamElintEnvet);
    connect(handler, &ConnectionInterface::XferDFResult,
            this, &SocketManager::XferDFResult);
    connect(handler, &ConnectionInterface::XferPSD,
            this, &SocketManager::XferPSD);
    connect(handler, &ConnectionInterface::XferPSDWide,
            this, &SocketManager::XferPSDWide);
    connect(handler, &ConnectionInterface::XferPDW,
            this, &SocketManager::XferPDW);
    connect(handler, &ConnectionInterface::XferPSDFullRange,
            this, &SocketManager::XferPSDFullRange);
    connect(handler, &ConnectionInterface::XferDOA,
            this, &SocketManager::XferDOA);
    connect(handler, &ConnectionInterface::XferDefault,
            this, &SocketManager::XferDefault);
    connect(handler, &ConnectionInterface::XferAudioData,
            this, &SocketManager::XferAudioData);
    connect(handler, &ConnectionInterface::XferPhaseAmp,
            this, &SocketManager::XferPhaseAmp);
    connect(handler, &ConnectionInterface::ConnectedState,
            this, &SocketManager::ConnectedState);
    connect(handler, &ConnectionInterface::AntennaStatesChanged,
            this, &SocketManager::AntennaStatesChanged);
    connect(handler, &ConnectionInterface::AntennaOffsetChanged,
            this, &SocketManager::AntennaOffsetChanged);
    connect(handler, &ConnectionInterface::AgcChanged,
            this, &SocketManager::AgcChanged);
    connect(handler, &ConnectionInterface::MGcChange,
            this, &SocketManager::MGcChange);
    connect(handler, &ConnectionInterface::RFHeadChanged,
            this, &SocketManager::RFHeadChanged);
    connect(handler, &ConnectionInterface::DemodChanged,
            this, &SocketManager::DemodChanged);
    connect(handler, &ConnectionInterface::QualityChanged,
            this, &SocketManager::QualityChanged);
    connect(handler, &ConnectionInterface::FFTPointChanged,
            this, &SocketManager::FFTPointChanged);
    connect(handler, &ConnectionInterface::SquelchChanged,
            this, &SocketManager::SquelchChanged);
    connect(handler, &ConnectionInterface::VolumeChanged,
            this, &SocketManager::VolumeChanged);
    connect(handler, &ConnectionInterface::AverageChange,
            this, &SocketManager::AverageChange);
    connect(handler, &ConnectionInterface::PhaseAverageChange,
            this, &SocketManager::PhaseAverageChange);
    connect(handler, &ConnectionInterface::bandwidthChanged,
            this, &SocketManager::bandwidthChanged);
    connect(handler, &ConnectionInterface::FilterBandChanged,
            this, &SocketManager::FilterBandChanged);
    connect(handler, &ConnectionInterface::FilterChanged,
            this, &SocketManager::FilterChanged);
    connect(handler, &ConnectionInterface::FrequencyChanged,
            this, &SocketManager::FrequencyChanged);
    connect(handler, &ConnectionInterface::ThresholdChanged,
            this, &SocketManager::ThresholdChanged);
    connect(handler, &ConnectionInterface::TransferDataRate,
            this, &SocketManager::TransferDataRate);
    connect(handler, &ConnectionInterface::GPSDataChanged,
            this, &SocketManager::GPSDataChanged);
    connect(handler, &ConnectionInterface::AzimuthsMonitoringChanged,
            this, &SocketManager::AzimuthsMonitoringChanged);
    connect(handler, &ConnectionInterface::GPSParametersChanged,
            this, &SocketManager::GPSParametersChanged);
    connect(handler, &ConnectionInterface::CompassDataChanged,
            this, &SocketManager::CompassDataChanged);
    connect(handler, &ConnectionInterface::ChannelNumberChanged,
            this, &SocketManager::ChannelNumberChanged);
    connect(handler, &ConnectionInterface::SwitchSequenceChanged,
            this, &SocketManager::SwitchSequenceChanged);
    connect(handler, &ConnectionInterface::PageNumberChanged,
            this, &SocketManager::PageNumberChanged);
    connect(handler, &ConnectionInterface::ElintAntennaChanged,
            this, &SocketManager::ElintAntennaChanged);
    connect(handler, &ConnectionInterface::AudioParamChanged,
            this, &SocketManager::AudioParamChanged);
    connect(handler, &ConnectionInterface::ComputerParametersChanged,
            this, &SocketManager::ComputerParametersChanged);
    connect(handler, &ConnectionInterface::DFLogChanged,
            this, &SocketManager::DFLogChanged);
    connect(handler, &ConnectionInterface::AutoThresholdChange,
            this, &SocketManager::AutoThresholdChange);
    connect(handler, &ConnectionInterface::ManualModeChanged,
            this, &SocketManager::ManualModeChanged);
    connect(handler, &ConnectionInterface::DFModeChanged,
            this, &SocketManager::DFModeChanged);
    connect(handler, &ConnectionInterface::PositionFindingModeChanged,
            this, &SocketManager::PositionFindingModeChanged);
    connect(handler, &ConnectionInterface::TargetAreaAndPositionChanged,
            this, &SocketManager::TargetAreaAndPositionChanged);
    connect(handler, &ConnectionInterface::MemoryScanParamChanged,
            this, &SocketManager::MemoryScanParamChanged);
    connect(handler, &ConnectionInterface::MemoryScanStateChanged,
            this, &SocketManager::MemoryScanStateChanged);
    connect(handler, &ConnectionInterface::SubRangeScanParamChanged,
            this, &SocketManager::SubRangeScanParamChanged);
    connect(handler, &ConnectionInterface::SubRangeScanStateChanged,
            this, &SocketManager::SubRangeScanStateChanged);
    connect(handler, &ConnectionInterface::FullRangeParamChanged,
            this, &SocketManager::FullRangeParamChanged);
    connect(handler, &ConnectionInterface::RemoveScanlistChanged,
            this, &SocketManager::RemoveScanlistChanged);
    connect(handler, &ConnectionInterface::GetAllDFConfigEvent,
            this, &SocketManager::GetAllDFConfigEvent);
    connect(handler, &ConnectionInterface::ResetSystemConfigEvent,
            this, &SocketManager::ResetSystemConfigEvent);
    connect(handler, &ConnectionInterface::ScanResultData,
            this, &SocketManager::ScanResultData);
    connect(handler, &ConnectionInterface::VersionCommingChanged,
            this, &SocketManager::VersionCommingChanged);
    connect(handler, &ConnectionInterface::ErrorChanged,
            this, &SocketManager::ErrorChanged);
    connect(handler, &ConnectionInterface::CalibrationParamChanged,
            this, &SocketManager::CalibrationParamChanged);
    connect(handler, &ConnectionInterface::ClassificationStateChanged,
            this, &SocketManager::ClassificationStateChanged);
    connect(handler, &ConnectionInterface::BiteStatechanged,
            this, &SocketManager::BiteStatechanged);
    connect(handler, &ConnectionInterface::AmrStatechanged,
            this, &SocketManager::AmrStatechanged);
    connect(handler, &ConnectionInterface::IFLogParamtersChanged,
            this, &SocketManager::IFLogParamtersChanged);
    connect(handler, &ConnectionInterface::AudioCodecParamtersChanged,
            this, &SocketManager::AudioCodecParamtersChanged);
    connect(handler, &ConnectionInterface::SystemStateChanged,
            this, &SocketManager::SystemStateChanged);
    connect(handler, &ConnectionInterface::BiteTestResult,
            this, &SocketManager::BiteTestResult);
    connect(handler, &ConnectionInterface::ClassificationResult,
            this, &SocketManager::ClassificationResult);
    connect(handler, &ConnectionInterface::ClassificationResultElint,
            this, &SocketManager::ClassificationResultElint);
    connect(handler, &ConnectionInterface::AmrStereoDataChanged,
            this, &SocketManager::AmrStereoDataChanged);
    connect(handler, &ConnectionInterface::AmrMonoDataChanged,
            this, &SocketManager::AmrMonoDataChanged);

}

bool SocketManager::isConnected()
{
    if (!handler)
        return false;
    return handler->isConnected();
}

void SocketManager::ConnectToSystem(ConnectionMode Connection,
                                    UDPType udptype,
                                    QString ip,
                                    short port,
                                    short PortSendUDP,
                                    short PortReciveUPD,
                                    short mtu,
                                    QString Comport,
                                    int Baudrate)
{
    if (!handler)
        return;
    handler->ConnectToSystem(Connection,
                           udptype,
                           ip,
                           port,
                           PortSendUDP,
                           PortReciveUPD,
                           mtu,
                           Comport,
                           Baudrate);
}

void SocketManager::disconnectFromSystem()
{

}

void SocketManager::SendFrequency(double freqMHz)
{
    if (!handler)
        return;
    handler->SendFrequency(freqMHz);
}

void SocketManager::SendThreshold(double thr)
{
    if (!handler)
        return;
    handler->SendThreshold(thr);
}

void SocketManager::SendDefaults(SystemDefualtsStruct SystemDefaults)
{
    if (!handler)
        return;
    handler->SendDefaults(SystemDefaults);
}

void SocketManager::SendAntennaOffset(QMap<char, double> offsets)
{
    if (!handler)
        return;
    handler->SendAntennaOffset(offsets);
}

void SocketManager::SendAtennaState(QMap<char, bool> state)
{
    if (!handler)
        return;
    handler->SendAtennaState(state);
}

void SocketManager::SendAGc(char agc)
{
    if (!handler)
        return;
    handler->SendAGc(agc);
}

void SocketManager::SendMGc(short mgc)
{
    if (!handler)
        return;
    handler->SendMGc(mgc);
}

void SocketManager::SendRFHead(char rfhead)
{
    if (!handler)
        return;
    handler->SendRFHead(rfhead);
}

void SocketManager::SendDemod(char demod)
{
    if (!handler)
        return;
    handler->SendDemod(demod);
}

void SocketManager::SendFFTPoint(short fft)
{
    if (!handler)
        return;
    handler->SendFFTPoint(fft);
}

void SocketManager::SendSquelch(bool state)
{
    if (!handler)
        return;
    handler->SendSquelch(state);
}

void SocketManager::SendVolume(short vol)
{
    if (!handler)
        return;
    handler->SendVolume(vol);
}

void SocketManager::SendAverage(double val)
{
    if (!handler)
        return;
    handler->SendAverage(val);
}

void SocketManager::SendQuality(double val)
{
    if (!handler)
        return;
    handler->SendQuality(val);
}

void SocketManager::SendPhaseAverage(double val)
{
    if (!handler)
        return;
    handler->SendPhaseAverage(val);
}

void SocketManager::SendAutoThreshold(bool val)
{
    if (!handler)
        return;
    handler->SendAutoThreshold(val);
}

void SocketManager::SendAzimuthHistogram(bool val)
{
    if (!handler)
        return;
    handler->SendAzimuthHistogram(val);
}

void SocketManager::SendClassificationState(ClassificationStruct Classsification)
{
    if (!handler)
        return;
    handler->SendClassificationState(Classsification);
}

void SocketManager::SendBiteState(bool val)
{
    if (!handler)
        return;
    handler->SendBiteState(val);
}

void SocketManager::SendAmrState(bool val)
{
    if (!handler)
        return;
    handler->SendAmrState(val);
}

void SocketManager::SendAudioCodecOption(AudioCodecStruct acodec)
{
    if (!handler)
        return;
    handler->SendAudioCodecOption(acodec);
}

void SocketManager::SendIFLogParam(IFlogParamStruct iflog)
{
    if (!handler)
        return;
    handler->SendIFLogParam(iflog);
}

void SocketManager::SendBandwidth(char bw)
{
    if (!handler)
        return;
    handler->SendBandwidth(bw);
}

void SocketManager::SendFilterBand(char filter)
{
    if (!handler)
        return;
    handler->SendFilterBand(filter);
}

void SocketManager::SendFilter(char fil)
{
    if (!handler)
        return;
    handler->SendFilter(fil);
}

void SocketManager::SendBandwidthElint(char bw)
{
    if (!handler)
        return;
    handler->SendBandwidthElint(bw);
}

void SocketManager::SendThresholdLow(int val)
{
    if (!handler)
        return;
    handler->SendThresholdLow(val);
}

void SocketManager::SendThresholdHigh(int val)
{
    if (!handler)
        return;
    handler->SendThresholdHigh(val);
}

void SocketManager::SendElintADCRef(short lowRef, short HighRef)
{
    if (!handler)
        return;
    handler->SendElintADCRef(lowRef, HighRef);
}

void SocketManager::SendElintMaxHold(bool state)
{
    if (!handler)
        return;
    handler->SendElintMaxHold(state);
}

void SocketManager::SendThresholdLimit(int val)
{
    if (!handler)
        return;
    handler->SendThresholdLimit(val);
}

void SocketManager::SendDFParameters(double Azimuth, double level, double Quality)
{
    if (!handler)
        return;
    handler->SendDFParameters(Azimuth, level, Quality);
}

void SocketManager::SendTransferDataRate(TransferDataStruct Tds)
{
    if (!handler)
        return;
    handler->SendTransferDataRate(Tds);
}

void SocketManager::SendAutoDetectAntenna(bool state)
{
    if (!handler)
        return;
    handler->SendAutoDetectAntenna(state);
}

void SocketManager::SendAutoDetectCompass(bool state)
{
    if (!handler)
        return;
    handler->SendAutoDetectCompass(state);
}

void SocketManager::SendGPSData(GPSParameters Tds)
{
    if (!handler)
        return;
    handler->SendGPSData(Tds);
}

void SocketManager::SendAutoDetectGPS(bool state)
{
    if (!handler)
        return;
    handler->SendAutoDetectGPS(state);
}

void SocketManager::SendChannelNumber(short channel)
{
    if (!handler)
        return;
    handler->SendChannelNumber(channel);
}

void SocketManager::SendPageNumber(short PageNumber)
{
    if (!handler)
        return;
    handler->SendPageNumber(PageNumber);
}

void SocketManager::SendElintAntenna(bool isElint)
{
    if (!handler)
        return;
    handler->SendElintAntenna(isElint);
}

void SocketManager::SendSwitchSequence(short seq)
{
    if (!handler)
        return;
    handler->SendSwitchSequence(seq);
}

void SocketManager::SendAudioSate(bool state)
{
    if (!handler)
        return;
    handler->SendAudioSate(state);
}

void SocketManager::SendComputerParameters(ComputersParametersStruct ComputerParams)
{
    if (!handler)
        return;
    handler->SendComputerParameters(ComputerParams);
}

void SocketManager::SendManualMode(ManualModeStruct manual)
{
    if (!handler)
        return;
    handler->SendManualMode(manual);
}

void SocketManager::SendDFlog(DFlogStruct logs)
{
    if (!handler)
        return;
    handler->SendDFlog(logs);
}

void SocketManager::SendDFMode(DFModeFindings_DFMODFND DFmode)
{
    if (!handler)
        return;
    handler->SendDFMode(DFmode);
}

void SocketManager::SendPositionFinding(PFmodeStruct Pfparam)
{
    if (!handler)
        return;
    handler->SendPositionFinding(Pfparam);
}

void SocketManager::SendMemoryScanParam(MemoryScanParamStruct MemPar)
{
    if (!handler)
        return;
    handler->SendMemoryScanParam(MemPar);
}

void SocketManager::SendMemoryScanState(MemoryScanStateStruct MemState)
{
    if (!handler)
        return;
    handler->SendMemoryScanState(MemState);
}

void SocketManager::SendSubRangeScanParam(SubRangeScanParamStruct SubPar)
{
    if (!handler)
        return;
    handler->SendSubRangeScanParam(SubPar);
}

void SocketManager::SendSubRangeScanState(SubRangeScanStateStruct SubState)
{
    if (!handler)
        return;
    handler->SendSubRangeScanState(SubState);
}

void SocketManager::SendFullRangeParam(FullRangeScanParamStruct Fullpar)
{
    if (!handler)
        return;
    handler->SendFullRangeParam(Fullpar);
}

void SocketManager::SendRemoveScanlist(RemoveScansTables_MODE scant)
{
    if (!handler)
        return;
    handler->SendRemoveScanlist(scant);
}

void SocketManager::SendGetAllDFConfig()
{
    if (!handler)
        return;
    handler->SendGetAllDFConfig();
}

void SocketManager::SendResetSystemConfig(ResetSystem_RESET resettype)
{
    if (!handler)
        return;
    handler->SendResetSystemConfig(resettype);
}

void SocketManager::SendVersionRequest(string vs)
{
    if (!handler)
        return;
    handler->SendVersionRequest(vs);
}

void SocketManager::SendCalibraionParam(CalibrationStruct calib)
{
    if (!handler)
        return;
    handler->SendCalibraionParam(calib);
}
