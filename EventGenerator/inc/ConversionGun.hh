#ifndef EventGenerator_ConversionGun_hh
#define EventGenerator_ConversionGun_hh

//
// Generate an electron with the conversion energy
// Uses FoilParticleGenerator to extract a random spot
// within the target system at
// a random time during the accelerator cycle.
//
// $Id: ConversionGun.hh,v 1.12 2011/05/18 02:27:15 wb Exp $
// $Author: wb $
// $Date: 2011/05/18 02:27:15 $
//

// C++ includes
#include <memory>

// Mu2e includes
#include "EventGenerator/inc/GeneratorBase.hh"
#include "Mu2eUtilities/inc/RandomUnitSphere.hh"
#include "EventGenerator/inc/FoilParticleGenerator.hh"

// Forward declarations in other namespaces.
namespace art {
  class Run;
}


// More forward declarations.
class TH1F;
class TH2F;

namespace mu2e {

  // Forward reference.
  class SimpleConfig;

  class ConversionGun: public GeneratorBase{

  public:
    ConversionGun( art::Run& run, const SimpleConfig& config );
    virtual ~ConversionGun();

    virtual void generate( ToyGenParticleCollection&  );

  private:

    // Conversion momentum.
    double _p;

    // Limits on the generated direction.
    double _czmin;
    double _czmax;
    double _phimin;
    double _phimax;

    bool _PStoDSDelay;
    bool _pPulseDelay;

    // Limits on the generated time.
    double _tmin;
    double _tmax;

    // Control histograms.
    bool _doHistograms;

    //Utility to generate direction of the momentum, random on the unit sphere.
    RandomUnitSphere _randomUnitSphere;

    // Class object to generate position and time of the particle
    std::auto_ptr<FoilParticleGenerator> _fGenerator;

    //Particle mass
    double _mass;

    // Histograms.
    TH1F* _hMultiplicity;
    TH1F* _hcz;
    TH1F* _hphi;
    TH1F* _hmomentum;
    TH1F* _hradius;
    TH1F* _hzPos;
    TH1F* _htime;
    TH2F* _hxyPos;
    TH2F* _hrzPos;

    void bookHistograms();

  };

} // end namespace mu2e,

#endif /* EventGenerator_ConversionGun_hh */


