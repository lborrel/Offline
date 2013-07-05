#ifndef BeamlineGeom_TSSection_hh
#define BeamlineGeom_TSSection_hh

//
// Abstract base class for TS sections
//
#include <memory>

#include "BeamlineGeom/inc/TSSection.hh"

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"

namespace mu2e {

  class TSSection {

  public:

    virtual ~TSSection(){}

    virtual CLHEP::Hep3Vector  const & getGlobal()   const { return _origin; }
    virtual CLHEP::HepRotation const * getRotation() const { return &_rotation; }

  protected:

    CLHEP::Hep3Vector  _origin;
    CLHEP::HepRotation _rotation;

};

}
#endif /* BeamlineGeom_TSSection_hh */
