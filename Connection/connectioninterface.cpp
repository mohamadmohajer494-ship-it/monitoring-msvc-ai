#include "connectioninterface.h"

ConnectionInterface::ConnectionInterface(QObject *parent)
    : QObject{parent}
{

}

ConnectionInterface::~ConnectionInterface()
{

}

bool ConnectionInterface::isConnected()
{
    return false;
}

void ConnectionInterface::ConnectToSystem(ConnectionMode Connection, UDPType udptype, QString ip, short port, short PortSendUDP, short PortReciveUPD, short mtu, QString Comport, int Baudrate)
{

}

void ConnectionInterface::disconnectFromSystem()
{

}

void ConnectionInterface::SendFrequency(double freqMHz)
{

}

void ConnectionInterface::SendThreshold(double thr)
{

}

void ConnectionInterface::SendDefaults(SystemDefualtsStruct SystemDefaults)
{

}

void ConnectionInterface::SendAntennaOffset(QMap<char, double> offsets)
{

}

void ConnectionInterface::SendAtennaState(QMap<char, bool> state)
{

}

void ConnectionInterface::SendAGc(char agc)
{

}

void ConnectionInterface::SendMGc(short mgc)
{

}

void ConnectionInterface::SendRFHead(char rfhead)
{

}

void ConnectionInterface::SendDemod(char demod)
{

}

void ConnectionInterface::SendFFTPoint(short fft)
{

}

void ConnectionInterface::SendSquelch(bool state)
{

}

void ConnectionInterface::SendVolume(short vol)
{

}

void ConnectionInterface::SendAverage(double val)
{

}

void ConnectionInterface::SendQuality(double val)
{

}

void ConnectionInterface::SendPhaseAverage(double val)
{

}

void ConnectionInterface::SendAutoThreshold(bool val)
{

}

void ConnectionInterface::SendAzimuthHistogram(bool val)
{

}

void ConnectionInterface::SendClassificationState(ClassificationStruct Classsification)
{

}

void ConnectionInterface::SendBiteState(bool val)
{

}

void ConnectionInterface::SendAmrState(bool val)
{

}

void ConnectionInterface::SendAudioCodecOption(AudioCodecStruct acodec)
{

}

void ConnectionInterface::SendIFLogParam(IFlogParamStruct iflog)
{

}

void ConnectionInterface::SendBandwidth(char bw)
{

}

void ConnectionInterface::SendFilterBand(char filter)
{

}

void ConnectionInterface::SendFilter(char fil)
{

}

void ConnectionInterface::SendBandwidthElint(char bw)
{

}

void ConnectionInterface::SendThresholdLow(int val)
{

}

void ConnectionInterface::SendThresholdHigh(int val)
{

}

void ConnectionInterface::SendElintADCRef(short lowRef, short HighRef)
{

}

void ConnectionInterface::SendElintMaxHold(bool state)
{

}

void ConnectionInterface::SendThresholdLimit(int val)
{

}

void ConnectionInterface::SendDFParameters(double Azimuth, double level, double Quality)
{

}

void ConnectionInterface::SendTransferDataRate(TransferDataStruct Tds)
{

}

void ConnectionInterface::SendAutoDetectAntenna(bool state)
{

}

void ConnectionInterface::SendAutoDetectCompass(bool state)
{

}

void ConnectionInterface::SendGPSData(GPSParameters Tds)
{

}

void ConnectionInterface::SendAutoDetectGPS(bool state)
{

}

void ConnectionInterface::SendChannelNumber(short channel)
{

}

void ConnectionInterface::SendPageNumber(short PageNumber)
{

}

void ConnectionInterface::SendElintAntenna(bool isElint)
{

}

void ConnectionInterface::SendSwitchSequence(short seq)
{

}

void ConnectionInterface::SendAudioSate(bool state)
{

}

void ConnectionInterface::SendComputerParameters(ComputersParametersStruct ComputerParams)
{

}

void ConnectionInterface::SendManualMode(ManualModeStruct manual)
{

}

void ConnectionInterface::SendDFlog(DFlogStruct logs)
{

}

void ConnectionInterface::SendDFMode(DFModeFindings_DFMODFND DFmode)
{

}

void ConnectionInterface::SendPositionFinding(PFmodeStruct Pfparam)
{

}

void ConnectionInterface::SendMemoryScanParam(MemoryScanParamStruct MemPar)
{

}

void ConnectionInterface::SendMemoryScanState(MemoryScanStateStruct MemState)
{

}

void ConnectionInterface::SendSubRangeScanParam(SubRangeScanParamStruct SubPar)
{

}

void ConnectionInterface::SendSubRangeScanState(SubRangeScanStateStruct SubState)
{

}

void ConnectionInterface::SendFullRangeParam(FullRangeScanParamStruct Fullpar)
{

}

void ConnectionInterface::SendRemoveScanlist(RemoveScansTables_MODE scant)
{

}

void ConnectionInterface::SendGetAllDFConfig()
{

}

void ConnectionInterface::SendResetSystemConfig(ResetSystem_RESET resettype)
{

}

void ConnectionInterface::SendVersionRequest(string vs)
{

}

void ConnectionInterface::SendCalibraionParam(CalibrationStruct calib)
{

}
