
#include "TrackerConditions/inc/StrawResponseMaker.hh"
// data products
#include <cmath>
#include <algorithm>
#include <TMath.h>
#include "cetlib_except/exception.h"
#include "DataProducts/inc/StrawId.hh"
#include "TrackerConditions/inc/StrawDrift.hh"

#include "BFieldGeom/inc/BFieldManager.hh"
#include "BTrk/BField/BField.hh"
#include "GeometryService/inc/DetectorSystem.hh"
#include "GeometryService/inc/GeomHandle.hh"
#include "CLHEP/Matrix/Vector.h"


using namespace std;
namespace mu2e {
  StrawResponse::ptr_t StrawResponseMaker::fromFcl(
                     StrawDrift::cptr_t strawDrift,
		     StrawElectronics::cptr_t strawElectronics,
		     StrawPhysics::cptr_t strawPhysics)     {
    auto thresh = StrawElectronics::thresh;
    auto adc = StrawElectronics::adc;
    
    // if these value are not defined in fcl, take them
    // from StrawElectronics and StrawPhysics
    double x;
    double electronicsTimeDelay = strawElectronics->electronicsTimeDelay();
    if(_config.electronicsTimeDelay(x)) electronicsTimeDelay = x;
    double gasGain = strawPhysics->strawGain();
    if(_config.gasGain(x)) gasGain = x;
    std::array<double,StrawElectronics::npaths> analognoise;
    analognoise[thresh] = strawElectronics->analogNoise(thresh);
    if(_config.thresholdAnalogNoise(x)) analognoise[thresh] = x;
    analognoise[adc] = strawElectronics->analogNoise(adc);
    if(_config.adcAnalogNoise(x)) analognoise[adc] = x;
    std::array<double,StrawElectronics::npaths> dVdI;
    dVdI[thresh] = strawElectronics->currentToVoltage(StrawId(0,0,0),thresh);
    if(_config.defaultThresholddVdI(x)) dVdI[thresh] = x;
    dVdI[adc] = strawElectronics->currentToVoltage(StrawId(0,0,0),adc);
    if(_config.defaultAdcdVdI(x)) dVdI[adc] = x;
    double vsat = strawElectronics->saturationVoltage();
    if(_config.saturationVoltage(x)) vsat = x;
    double ADCped = strawElectronics->ADCPedestal(StrawId(0,0,0));
    if(_config.ADCPedestal(x)) ADCped = x;
    
    double pmpEnergyScaleAvg = 0;
    std::array<double, StrawId::_nustraws> pmpEnergyScale;
    if (_config.peakMinusPedestalEnergyScale().size() == 0){
      pmpEnergyScale.fill(_config.defaultPeakMinusPedestalEnergyScale());
    }else{
      for (size_t i=0;i<pmpEnergyScale.size();i++) {
        pmpEnergyScale[i] = _config.peakMinusPedestalEnergyScale()[i];
      }
    }
    for (size_t i=0;i<pmpEnergyScale.size();i++) {
      pmpEnergyScaleAvg += pmpEnergyScale[i];
    }
    pmpEnergyScaleAvg /= (double) pmpEnergyScale.size();

    std::vector<double> _parDriftDocas, _parDriftOffsets, _parDriftRes;

    double sigma = _config.parameterizedDriftSigma();
    double tau = _config.parameterizedDriftTau();
    int parameterizedDriftBins = _config.parameterizedDriftBins();

    _parDriftDocas.reserve(parameterizedDriftBins);
    _parDriftOffsets.reserve(parameterizedDriftBins);
    _parDriftRes.reserve(parameterizedDriftBins);
    
    for (int i=0;i<parameterizedDriftBins;i++){
      double doca = i*2.5/parameterizedDriftBins;
      double hypotenuse = sqrt(pow(doca,2) + pow(tau*_config.linearDriftVelocity(),2));
      double tau_eff = hypotenuse/_config.linearDriftVelocity() - doca/_config.linearDriftVelocity();

      double sumw = 0;
      double sumwx = 0;
      double sumwx2 = 0;
      double tresid = -20.005;
      for (int it=0;it<10000;it++){
        double weight = exp(sigma*sigma/(2*tau_eff*tau_eff)-tresid/tau_eff)*(1-TMath::Erf((sigma*sigma-tau_eff*tresid)/(sqrt(2)*sigma*tau_eff)));
        sumw += weight;
        sumwx += weight*tresid;
        sumwx2 += weight*tresid*tresid;
        tresid += 0.01;
      }
      double mean = sumwx/sumw;
      double stddev = sqrt(sumwx2/sumw-mean*mean);

      _parDriftDocas.push_back(doca);
      _parDriftOffsets.push_back(mean);
      _parDriftRes.push_back(stddev);
    }
    
    std::vector<double> edep;
    for (int i=0;i<_config.eBins();i++)
      edep.push_back(_config.eBinWidth()*i);

    if ((int) _config.halfPropVelocity().size() != _config.eBins() ||
        (int) _config.tdCentralRes().size() != _config.eBins() ||
        (int) _config.tdResSlope().size() != _config.eBins() ||
        (int) _config.totDriftTime().size() != _config.totTBins()*_config.totEBins()){
      throw cet::exception("BADCONFIG")
        << "StrawResponse calibration vector lengths incorrect" << "\n";
    }

 
    
    auto ptr = std::make_shared<StrawResponse>(
	 strawDrift,strawElectronics,strawPhysics,
         _config.eBins(), _config.eBinWidth(), edep, _config.halfPropVelocity(), 
	 _config.centralWirePos(), _config.tdCentralRes(), 
	 _config.tdResSlope(), _config.totTBins(), _config.totTBinWidth(),
         _config.totEBins(), _config.totEBinWidth(), _config.totDriftTime(), 
	 _config.useDriftErrorCalibration(), _config.driftErrorParameters(), 
         _config.useParameterizedDriftErrors(), _parDriftDocas, _parDriftOffsets, _parDriftRes,
	 _config.wireLengthBuffer(), _config.strawLengthFactor(), 
	 _config.errorFactor(), _config.useNonLinearDrift(), 
	 _config.linearDriftVelocity(), _config.minDriftRadiusResolution(), 
	 _config.maxDriftRadiusResolution(), 
	 _config.driftRadiusResolutionRadius(), _config.minT0DOCA(), 
	 _config.t0shift(), pmpEnergyScale, 
	 electronicsTimeDelay, 
	  gasGain, analognoise, dVdI, vsat, ADCped,
	 pmpEnergyScaleAvg );

    std::array<double, StrawId::_nupanels> timeOffsetPanel;
    std::array<double, StrawId::_nustraws> timeOffsetStrawHV, timeOffsetStrawCal;
    if (_config.timeOffsetPanel().size() > 0){
      for (size_t i=0;i<timeOffsetPanel.size();i++)
        timeOffsetPanel[i] = _config.timeOffsetPanel()[i];
    }else{
      for (size_t i=0;i<timeOffsetPanel.size();i++)
        timeOffsetPanel[i] = strawElectronics->getTimeOffsetPanel(i);
    }
    if (_config.timeOffsetStrawHV().size() > 0){
      for(size_t i=0;i<timeOffsetStrawHV.size();i++){
        timeOffsetStrawHV[i] = _config.timeOffsetStrawHV()[i];
        timeOffsetStrawCal[i] = _config.timeOffsetStrawCal()[i];
      }
    }else{
      for (size_t i=0;i<timeOffsetStrawHV.size();i++){
        timeOffsetStrawHV[i] = strawElectronics->getTimeOffsetStrawHV(i);
        timeOffsetStrawCal[i] = strawElectronics->getTimeOffsetStrawCal(i);
      }
    }
    ptr->setOffsets( timeOffsetPanel,
        timeOffsetStrawHV,
        timeOffsetStrawCal );

    return ptr;
  }

}
