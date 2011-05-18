#ifndef EventGenerator_EjectedProtonGun_hh
#define EventGenerator_EjectedProtonGun_hh

//
// Simulate the protons that come from the stopping target when muons capture
// on an Al nucleus.  Use the MECO distribution for the kinetic energy of the
// protons.
//
// $Id: EjectedProtonGun.hh,v 1.11 2011/05/18 02:27:15 wb Exp $
// $Author: wb $
// $Date: 2011/05/18 02:27:15 $
//
//

// C++ includes
#include <memory>

// Mu2e includes
#include "Mu2eUtilities/inc/RandomUnitSphere.hh"
#include "EventGenerator/inc/GeneratorBase.hh"
#include "EventGenerator/inc/FoilParticleGenerator.hh"

// CLHEP includes
#include "CLHEP/Random/RandPoissonQ.h"
#include "CLHEP/Random/RandGeneral.h"

// Forward declarations outside of namespace mu2e
class TH1D;
namespace art {
  class Run;
}

namespace mu2e {

  // Forward reference.
  class SimpleConfig;

  class EjectedProtonGun: public GeneratorBase{

  public:
    EjectedProtonGun( art::Run& run, const SimpleConfig& config );
    virtual ~EjectedProtonGun();

    virtual void generate( ToyGenParticleCollection&  );

  private:

    // Start: parameters that can be configured from the config file.

    double _mean;    // Mean number of protons per event
    double _elow;    // Range of proton energy.
    double _ehi;     //
    double _czmin;   // Range of cos(polar angle)
    double _czmax;
    double _phimin;  // Range of azimuth
    double _phimax;
    int    _nbins;   // number of bins in proton energy pdf

    // Class object to generate position and time of the particle
    std::auto_ptr<FoilParticleGenerator> _fGenerator;

    double _mass; //Particle mass

    double _p; //Particle momentum

    // Limits on the generated time.
    double _tmin;
    double _tmax;

    // Histogram control.
    bool _doHistograms;

    bool _PStoDSDelay;
    bool _pPulseDelay;

    // end: parameters that can be configured from the config file.

    //Random generators
    CLHEP::RandPoissonQ _randPoissonQ;
    RandomUnitSphere    _randomUnitSphere;
    CLHEP::RandGeneral _shape;


    TH1D* _hMultiplicity;
    TH1D* _hKE;
    TH1D* _hKEZoom;
    TH1D* _hMomentumMeV;
    TH1D* _hzPosition;
    TH1D* _hcz;
    TH1D* _hphi;
    TH1D* _htime;


    //Functions used to calculate the energy spectrum of the proton
    std::vector<double> binnedEnergySpectrum();
    double energySpectrum( double e );

  };

} // end namespace mu2e,

#endif /* EventGenerator_EjectedProtonGun_hh */
