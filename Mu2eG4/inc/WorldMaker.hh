#ifndef Mu2eG4_WorldMaker_hh
#define Mu2eG4_WorldMaker_hh
//
// The Mu2e version of G4VUserDetectorConstruction.
//
// $Id: WorldMaker.hh,v 1.3 2011/05/18 02:27:17 wb Exp $
// $Author: wb $
// $Date: 2011/05/18 02:27:17 $
//
// Original author Rob Kutschke
//
// This receives calls from G4 and forwards them to
// the class Mu2eWorld that actually constructs the
// Mu2e world.  This class manages the lifetime of
// the Mu2eWorld and ConstructMaterials objects.
//

#include <string>
#include <memory>

// Forward references.
class G4VPhysicalVolume;

#include "G4VUserDetectorConstruction.hh"

namespace mu2e {

  // Forward references within the Mu2e namespace.
  class ConstructMaterials;
  class Mu2eWorld;
  class WorldInfo;

  class WorldMaker : public G4VUserDetectorConstruction
  {
  public:

    WorldMaker();
    ~WorldMaker();

    // This is the required method prescribed by G4.
    G4VPhysicalVolume* Construct();

    // Accessors.
    Mu2eWorld const* getWorld()     { return _world.get(); }
    WorldInfo const* getWorldInfo() { return _worldInfo; }

  private:

    void Clean();

    WorldInfo const* _worldInfo;

    std::auto_ptr<ConstructMaterials> _materials;
    std::auto_ptr<Mu2eWorld>          _world;

  };

} // end namespace mu2e
#endif /* Mu2eG4_WorldMaker_hh */

