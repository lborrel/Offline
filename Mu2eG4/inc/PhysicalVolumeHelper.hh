#ifndef Mu2eG4_PhysicalVolumeHelper_hh
#define Mu2eG4_PhysicalVolumeHelper_hh
//
// A utility class to do indexolgy related to persistence of
// physical volume information.
//
// $Id: PhysicalVolumeHelper.hh,v 1.3 2011/05/18 02:27:17 wb Exp $
// $Author: wb $
// $Date: 2011/05/18 02:27:17 $
//
// Original author Rob Kutschke
//
// Notes
// 1) See detail notes in the ../src/PhysicalVolumeHelper.cc
//
// 2) This class intrinsically must use two phase construction.  The data to which
//    it points is only guaranteed to be defined during a run.  The implementation
//    protects against trying to use this object outside of a run.

#include "ToyDP/inc/PhysicalVolumeInfoCollection.hh"
#include "Mu2eG4/inc/VolMapType.hh"

class G4Track;
class G4VPhysicalVolume;

namespace mu2e {

  class PhysicalVolumeHelper {

  public:

    // Only a default c'tor.  See note 2.
    PhysicalVolumeHelper();

    // Accept compiler written d'tor, copy c'tor an assignment operator.

    // Bulid _persistentInfo and _volumeMap; valid for duration of the run.
    void beginRun();

    // Clear information at end of run.
    void endRun();

    // Accessors:
    const PhysicalVolumeInfoCollection& persistentInfo() const{
      return _persistentInfo;
    }

    // Return the index into _persistentInfo for the physical volume attached to this track.
    int index( const G4Track* track ) const;

    // Return the index into _persistentInfo for this physical volume.
    int index( G4VPhysicalVolume* physVol ) const;

  private:

    // The persistent info about each volume.
    PhysicalVolumeInfoCollection _persistentInfo;

    // Map used to look up persistent index of each physical volume.
    VolMapType _volumeMap;

  };

} // end namespace mu2e

#endif /* Mu2eG4_PhysicalVolumeHelper_hh */

