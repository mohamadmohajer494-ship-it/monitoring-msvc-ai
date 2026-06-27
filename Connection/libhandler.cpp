#include "libhandler.h"

LIBHandler::LIBHandler(QObject *parent)
    : ConnectionInterface{parent}
{
    if (!socket)
        socket = new CDFSocket();
    connect(socket, &CDFSocket::SystemParamEnvet,
            this, &ConnectionInterface::SystemParamEnvet);
    connect(socket, &CDFSocket::SystemParamElintEnvet,
            this, &ConnectionInterface::SystemParamElintEnvet);
    connect(socket, &CDFSocket::XferDFResult,
            this, &ConnectionInterface::XferDFResult);
    connect(socket, &CDFSocket::XferPSD,
            this, &ConnectionInterface::XferPSD);
    connect(socket, &CDFSocket::XferPSDWide,
            this, &ConnectionInterface::XferPSDWide);
    connect(socket, &CDFSocket::XferPDW,
            this, &ConnectionInterface::XferPDW);
    connect(socket, &CDFSocket::XferPSDFullRange,
            this, &ConnectionInterface::XferPSDFullRange);
    connect(socket, &CDFSocket::XferDOA,
            this, &ConnectionInterface::XferDOA);
    connect(socket, &CDFSocket::XferDefault,
            this, &ConnectionInterface::XferDefault);
    connect(socket, &CDFSocket::XferAudioData,
            this, &ConnectionInterface::XferAudioData);
    connect(socket, &CDFSocket::XferPhaseAmp,
            this, &ConnectionInterface::XferPhaseAmp);
    connect(socket, &CDFSocket::ConnectedState,
            this, &ConnectionInterface::ConnectedState);
    connect(socket, &CDFSocket::AntennaStatesChanged,
            this, &ConnectionInterface::AntennaStatesChanged);
    connect(socket, &CDFSocket::AntennaOffsetChanged,
            this, &ConnectionInterface::AntennaOffsetChanged);
    connect(socket, &CDFSocket::AgcChanged,
            this, &ConnectionInterface::AgcChanged);
    connect(socket, &CDFSocket::MGcChange,
            this, &ConnectionInterface::MGcChange);
    connect(socket, &CDFSocket::RFHeadChanged,
            this, &ConnectionInterface::RFHeadChanged);
    connect(socket, &CDFSocket::DemodChanged,
            this, &ConnectionInterface::DemodChanged);
    connect(socket, &CDFSocket::QualityChanged,
            this, &ConnectionInterface::QualityChanged);
    connect(socket, &CDFSocket::FFTPointChanged,
            this, &ConnectionInterface::FFTPointChanged);
    connect(socket, &CDFSocket::SquelchChanged,
            this, &ConnectionInterface::SquelchChanged);
    connect(socket, &CDFSocket::VolumeChanged,
            this, &ConnectionInterface::VolumeChanged);
    connect(socket, &CDFSocket::AverageChange,
            this, &ConnectionInterface::AverageChange);
    connect(socket, &CDFSocket::PhaseAverageChange,
            this, &ConnectionInterface::PhaseAverageChange);
    connect(socket, &CDFSocket::bandwidthChanged,
            this, &ConnectionInterface::bandwidthChanged);
    // connect(socket, &CDFSocket::FilterBandChanged,
    //         this, &ConnectionInterface::FilterBandChanged);
    connect(socket, &CDFSocket::FilterChanged,
            this, &ConnectionInterface::FilterChanged);
    connect(socket, &CDFSocket::FrequencyChanged,
            this, &ConnectionInterface::FrequencyChanged);
    connect(socket, &CDFSocket::ThresholdChanged,
            this, &ConnectionInterface::ThresholdChanged);
    connect(socket, &CDFSocket::TransferDataRate,
            this, &ConnectionInterface::TransferDataRate);
    connect(socket, &CDFSocket::GPSDataChanged,
            this, &ConnectionInterface::GPSDataChanged);
    connect(socket, &CDFSocket::AzimuthsMonitoringChanged,
            this, &ConnectionInterface::AzimuthsMonitoringChanged);
    connect(socket, &CDFSocket::GPSParametersChanged,
            this, &ConnectionInterface::GPSParametersChanged);
    connect(socket, &CDFSocket::CompassDataChanged,
            this, &ConnectionInterface::CompassDataChanged);
    connect(socket, &CDFSocket::ChannelNumberChanged,
            this, &ConnectionInterface::ChannelNumberChanged);
    connect(socket, &CDFSocket::SwitchSequenceChanged,
            this, &ConnectionInterface::SwitchSequenceChanged);
    connect(socket, &CDFSocket::PageNumberChanged,
            this, &ConnectionInterface::PageNumberChanged);
    connect(socket, &CDFSocket::ElintAntennaChanged,
            this, &ConnectionInterface::ElintAntennaChanged);
    connect(socket, &CDFSocket::AudioParamChanged,
            this, &ConnectionInterface::AudioParamChanged);
    connect(socket, &CDFSocket::ComputerParametersChanged,
            this, &ConnectionInterface::ComputerParametersChanged);
    connect(socket, &CDFSocket::DFLogChanged,
            this, &ConnectionInterface::DFLogChanged);
    connect(socket, &CDFSocket::AutoThresholdChange,
            this, &ConnectionInterface::AutoThresholdChange);
    connect(socket, &CDFSocket::ManualModeChanged,
            this, &ConnectionInterface::ManualModeChanged);
    connect(socket, &CDFSocket::DFModeChanged,
            this, &ConnectionInterface::DFModeChanged);
    connect(socket, &CDFSocket::PositionFindingModeChanged,
            this, &ConnectionInterface::PositionFindingModeChanged);
    connect(socket, &CDFSocket::TargetAreaAndPositionChanged,
            this, &ConnectionInterface::TargetAreaAndPositionChanged);
    connect(socket, &CDFSocket::MemoryScanParamChanged,
            this, &ConnectionInterface::MemoryScanParamChanged);
    connect(socket, &CDFSocket::MemoryScanStateChanged,
            this, &ConnectionInterface::MemoryScanStateChanged);
    connect(socket, &CDFSocket::SubRangeScanParamChanged,
            this, &ConnectionInterface::SubRangeScanParamChanged);
    connect(socket, &CDFSocket::SubRangeScanStateChanged,
            this, &ConnectionInterface::SubRangeScanStateChanged);
    connect(socket, &CDFSocket::FullRangeParamChanged,
            this, &ConnectionInterface::FullRangeParamChanged);
    connect(socket, &CDFSocket::RemoveScanlistChanged,
            this, &ConnectionInterface::RemoveScanlistChanged);
    connect(socket, &CDFSocket::GetAllDFConfigEvent,
            this, &ConnectionInterface::GetAllDFConfigEvent);
    connect(socket, &CDFSocket::ResetSystemConfigEvent,
            this, &ConnectionInterface::ResetSystemConfigEvent);
    connect(socket, &CDFSocket::ScanResultData,
            this, &ConnectionInterface::ScanResultData);
    connect(socket, &CDFSocket::VersionCommingChanged,
            this, &ConnectionInterface::VersionCommingChanged);
    connect(socket, &CDFSocket::ErrorChanged,
            this, &ConnectionInterface::ErrorChanged);
    connect(socket, &CDFSocket::CalibrationParamChanged,
            this, &ConnectionInterface::CalibrationParamChanged);
    connect(socket, &CDFSocket::ClassificationStateChanged,
            this, &ConnectionInterface::ClassificationStateChanged);
    connect(socket, &CDFSocket::BiteStatechanged,
            this, &ConnectionInterface::BiteStatechanged);
    connect(socket, &CDFSocket::AmrStatechanged,
            this, &ConnectionInterface::AmrStatechanged);
    connect(socket, &CDFSocket::IFLogParamtersChanged,
            this, &ConnectionInterface::IFLogParamtersChanged);
    connect(socket, &CDFSocket::AudioCodecParamtersChanged,
            this, &ConnectionInterface::AudioCodecParamtersChanged);
    connect(socket, &CDFSocket::SystemStateChanged,
            this, &ConnectionInterface::SystemStateChanged);
    connect(socket, &CDFSocket::BiteTestResult,
            this, &ConnectionInterface::BiteTestResult);
    connect(socket, &CDFSocket::ClassificationResult,
            this, &ConnectionInterface::ClassificationResult);
    connect(socket, &CDFSocket::ClassificationResultElint,
            this, &ConnectionInterface::ClassificationResultElint);
    connect(socket, &CDFSocket::AmrStereoDataChanged,
            this, &ConnectionInterface::AmrStereoDataChanged);
    connect(socket, &CDFSocket::AmrMonoDataChanged,
            this, &ConnectionInterface::AmrMonoDataChanged);
}


bool LIBHandler::isConnected()
{
    if (!socket)
        return false;
    return socket->isConnected();
}

void LIBHandler::ConnectToSystem(ConnectionMode Connection,
                                 UDPType udptype,
                                 QString ip,
                                 short port,
                                 short PortSendUDP,
                                 short PortReciveUPD,
                                 short mtu,
                                 QString Comport,
                                 int Baudrate)
{
    if (!socket)
        return;
    socket->ConnectToSystem(Connection,
                            udptype,
                            ip,
                            port,
                            PortSendUDP,
                            PortReciveUPD,
                            mtu,
                            Comport,
                            Baudrate);
}

void LIBHandler::SendFrequency(double freqMHz)
{
    if (!socket)
        return;
    socket->SendFrequency(freqMHz);
}

void LIBHandler::SendThreshold(double thr)
{
    if (!socket)
        return;
    socket->SendThreshold(thr);
}

void LIBHandler::SendDefaults(SystemDefualtsStruct SystemDefaults)
{
    if (!socket)
        return;
    socket->SendDefaults(SystemDefaults);
}

void LIBHandler::SendAntennaOffset(QMap<char, double> offsets)
{
    if (!socket)
        return;
    socket->SendAntennaOffset(offsets);
}

void LIBHandler::SendAtennaState(QMap<char, bool> state)
{
    if (!socket)
        return;
    socket->SendAtennaState(state);
}

void LIBHandler::SendAGc(char agc)
{
    if (!socket)
        return;
    socket->SendAGc(agc);
}

void LIBHandler::SendMGc(short mgc)
{
    if (!socket)
        return;
    socket->SendMGc(mgc);
}

void LIBHandler::SendRFHead(char rfhead)
{
    if (!socket)
        return;
    socket->SendRFHead(rfhead);
}

void LIBHandler::SendDemod(char demod)
{
    if (!socket)
        return;
    socket->SendDemod(demod);
}

void LIBHandler::SendFFTPoint(short fft)
{
    if (!socket)
        return;
    socket->SendFFTPoint(fft);
}

void LIBHandler::SendSquelch(bool state)
{
    if (!socket)
        return;
    socket->SendSquelch(state);
}

void LIBHandler::SendVolume(short vol)
{
    if (!socket)
        return;
    socket->SendVolume(vol);
}

void LIBHandler::SendAverage(double val)
{
    if (!socket)
        return;
    socket->SendAverage(val);
}

void LIBHandler::SendQuality(double val)
{
    if (!socket)
        return;
    socket->SendQuality(val);
}

void LIBHandler::SendPhaseAverage(double val)
{
    if (!socket)
        return;
    socket->SendPhaseAverage(val);
}

void LIBHandler::SendAutoThreshold(bool val)
{
    if (!socket)
        return;
    socket->SendAutoThreshold(val);
}

void LIBHandler::SendAzimuthHistogram(bool val)
{
    if (!socket)
        return;
    socket->SendAzimuthHistogram(val);
}

void LIBHandler::SendClassificationState(ClassificationStruct Classsification)
{
    if (!socket)
        return;
    socket->SendClassificationState(Classsification);
}

void LIBHandler::SendBiteState(bool val)
{
    if (!socket)
        return;
    socket->SendBiteState(val);
}

void LIBHandler::SendAmrState(bool val)
{
    if (!socket)
        return;
    socket->SendAmrState(val);
}

void LIBHandler::SendAudioCodecOption(AudioCodecStruct acodec)
{
    if (!socket)
        return;
    socket->SendAudioCodecOption(acodec);
}

void LIBHandler::SendIFLogParam(IFlogParamStruct iflog)
{
    if (!socket)
        return;
    socket->SendIFLogParam(iflog);
}

void LIBHandler::SendBandwidth(char bw)
{
    if (!socket)
        return;
    socket->SendBandwidth(bw);
}

void LIBHandler::SendFilterBand(char filter)
{
    if (!socket)
        return;
    // socket->SendFilterBand(filter);
}

void LIBHandler::SendFilter(char fil)
{
    if (!socket)
        return;
    socket->SendFilter(fil);
}

void LIBHandler::SendBandwidthElint(char bw)
{
    if (!socket)
        return;
    socket->SendBandwidthElint(bw);
}

void LIBHandler::SendThresholdLow(int val)
{
    if (!socket)
        return;
    socket->SendThresholdLow(val);
}

void LIBHandler::SendThresholdHigh(int val)
{
    if (!socket)
        return;
    socket->SendThresholdHigh(val);
}

void LIBHandler::SendElintADCRef(short lowRef, short HighRef)
{
    if (!socket)
        return;
    socket->SendElintADCRef(lowRef, HighRef);
}

void LIBHandler::SendElintMaxHold(bool state)
{
    if (!socket)
        return;
    socket->SendElintMaxHold(state);
}

void LIBHandler::SendThresholdLimit(int val)
{
    if (!socket)
        return;
    socket->SendThresholdLimit(val);
}

void LIBHandler::SendDFParameters(double Azimuth, double level, double Quality)
{
    if (!socket)
        return;
    // socket->SendDFParameters(Azimuth, level, Quality);
}

void LIBHandler::SendTransferDataRate(TransferDataStruct Tds)
{
    if (!socket)
        return;
    socket->SendTransferDataRate(Tds);
}

void LIBHandler::SendAutoDetectAntenna(bool state)
{
    if (!socket)
        return;
    socket->SendAutoDetectAntenna(state);
}

void LIBHandler::SendAutoDetectCompass(bool state)
{
    if (!socket)
        return;
    socket->SendAutoDetectCompass(state);
}

void LIBHandler::SendGPSData(GPSParameters Tds)
{
    if (!socket)
        return;
    socket->SendGPSData(Tds);
}

void LIBHandler::SendAutoDetectGPS(bool state)
{
    if (!socket)
        return;
    socket->SendAutoDetectGPS(state);
}

void LIBHandler::SendChannelNumber(short channel)
{
    if (!socket)
        return;
    socket->SendChannelNumber(channel);
}

void LIBHandler::SendPageNumber(short PageNumber)
{
    if (!socket)
        return;
    socket->SendPageNumber(PageNumber);
}

void LIBHandler::SendElintAntenna(bool isElint)
{
    if (!socket)
        return;
    socket->SendElintAntenna(isElint);
}

void LIBHandler::SendSwitchSequence(short seq)
{
    if (!socket)
        return;
    socket->SendSwitchSequence(seq);
}

void LIBHandler::SendAudioSate(bool state)
{
    if (!socket)
        return;
    socket->SendAudioSate(state);
}

void LIBHandler::SendComputerParameters(ComputersParametersStruct ComputerParams)
{
    if (!socket)
        return;
    socket->SendComputerParameters(ComputerParams);
}

void LIBHandler::SendManualMode(ManualModeStruct manual)
{
    if (!socket)
        return;
    socket->SendManualMode(manual);
}

void LIBHandler::SendDFlog(DFlogStruct logs)
{
    if (!socket)
        return;
    socket->SendDFlog(logs);
}

void LIBHandler::SendDFMode(DFModeFindings_DFMODFND DFmode)
{
    if (!socket)
        return;
    socket->SendDFMode(DFmode);
}

void LIBHandler::SendPositionFinding(PFmodeStruct Pfparam)
{
    if (!socket)
        return;
    socket->SendPositionFinding(Pfparam);
}

void LIBHandler::SendMemoryScanParam(MemoryScanParamStruct MemPar)
{
    if (!socket)
        return;
    socket->SendMemoryScanParam(MemPar);
}

void LIBHandler::SendMemoryScanState(MemoryScanStateStruct MemState)
{
    if (!socket)
        return;
    socket->SendMemoryScanState(MemState);
}

void LIBHandler::SendSubRangeScanParam(SubRangeScanParamStruct SubPar)
{
    if (!socket)
        return;
    socket->SendSubRangeScanParam(SubPar);
}

void LIBHandler::SendSubRangeScanState(SubRangeScanStateStruct SubState)
{
    if (!socket)
        return;
    socket->SendSubRangeScanState(SubState);
}

void LIBHandler::SendFullRangeParam(FullRangeScanParamStruct Fullpar)
{
    if (!socket)
        return;
    socket->SendFullRangeParam(Fullpar);
}

void LIBHandler::SendRemoveScanlist(RemoveScansTables_MODE scant)
{
    if (!socket)
        return;
    socket->SendRemoveScanlist(scant);
}

void LIBHandler::SendGetAllDFConfig()
{
    if (!socket)
        return;
    socket->SendGetAllDFConfig();
}

void LIBHandler::SendResetSystemConfig(ResetSystem_RESET resettype)
{
    if (!socket)
        return;
    socket->SendResetSystemConfig(resettype);
}

void LIBHandler::SendVersionRequest(string vs)
{
    if (!socket)
        return;
    socket->SendVersionRequest(vs);
}

void LIBHandler::SendCalibraionParam(CalibrationStruct calib)
{
    if (!socket)
        return;
    socket->SendCalibraionParam(calib);
}

