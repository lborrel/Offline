#ifndef CosmicRayShieldGeom_CRSScintillatorLayer_hh
#define CosmicRayShieldGeom_CRSScintillatorLayer_hh
//
// Representation of one Scintillator Layer in  CosmicRayShield
//
// $Id: CRSScintillatorLayer.hh,v 1.4 2011/05/18 02:27:15 wb Exp $
// $Author: wb $
// $Date: 2011/05/18 02:27:15 $
//
// Original author KLG; somewhat based on  Rob Kutschke's Layer
//

#include <vector>
#include <deque>

#include "CosmicRayShieldGeom/inc/CRSScintillatorLayerId.hh"
#include "CosmicRayShieldGeom/inc/CRSScintillatorBar.hh"

#include "CLHEP/Vector/ThreeVector.h"

namespace mu2e {

  class CRSScintillatorLayer{

    friend class CRSScintillatorModule;
    friend class CRSScintillatorShield;
    friend class CosmicRayShieldMaker;

  public:

    CRSScintillatorLayer();

    CRSScintillatorLayer(CRSScintillatorLayerId const & id);

    CRSScintillatorLayer(CRSScintillatorLayerId const & id,
                         int const nBars,
                         CLHEP::Hep3Vector   const & localOffset, // wrt Shield
                         std::vector<double> const & globalRotationAngles, // should be same as for the shield
                         CLHEP::Hep3Vector   const & globalOffset // wrt Mu2e
                         );

    CRSScintillatorLayer(CRSScintillatorLayerId const & id,
                         int nBars
                         );

    // Accept the compiler generated destructor, copy constructor and assignment operators

    CRSScintillatorLayerId const & Id() const { return _id;}

    int nBars() const { return _nBars; }

    CLHEP::Hep3Vector const & getLocalOffset() const {return _localOffset;}

    std::vector<double> const & getGlobalRotationAngles() const {return _globalRotationAngles;}

    CLHEP::Hep3Vector const & getGlobalOffset() const {return _globalOffset;}


    std::vector<double> const & getHalfLengths() {
      return _halfLengths;
    }

    void setHalfLengths( std::vector<double> const & v ){
      _halfLengths = v;
    }

    CRSScintillatorBar const & getBar( int n ) const {
      return *_bars.at(n);
    }

    CRSScintillatorBar const & getBar( const CRSScintillatorBarId& id ) const {
      return getBar(id.getBarNumber());
    }

    const std::vector<const CRSScintillatorBar*>& getBars() const { return _bars; }

    // Formatted string embedding the id of the layer.
    std::string name( std::string const & base ) const;

    // Return Id of the last CRSScintillatorBar in the layer.
    // Return an illegal id if there are no bars.
    //  - which should never happen.
    CRSScintillatorBarId getIdLastBar() const{

      return ( _bars.size() != 0 )?
        _bars.back()->Id():
        CRSScintillatorBarId();
    }

  private:

    CRSScintillatorLayerId _id;

    // Number of bars.  Needed because of 2 phase construction.
    // The member _bars is not filled until the second phase
    // but this is neede beforehand. Keep it strictly private.

    // there are two types of layers, as there are two types of modules
    // the number of bars in the half module is, yes, half that of the full module

    int _nBars;

    // Mid-point of the ScintillatorLayer, a.k.a. localOffset
    CLHEP::Hep3Vector _localOffset;

    std::vector<double> _globalRotationAngles;

    // Mid-point of the ScintillatorLayer, in Mu2e
    CLHEP::Hep3Vector _globalOffset;

    // outer dimensions (they depend on the number of bars); not used for now
    std::vector<double> _halfLengths;

    // Pointers to the bars in this layer.
    // These pointers do not own the bars to which they point.
    // These are not persisted and may need to be recomputed after readback.

    // CosmicRayShield "owns" the bars

    mutable std::vector<const CRSScintillatorBar*> _bars;
    std::vector<CRSScintillatorBarIndex> _indices;


  };
}

#endif /* CosmicRayShieldGeom_CRSScintillatorLayer_hh */
