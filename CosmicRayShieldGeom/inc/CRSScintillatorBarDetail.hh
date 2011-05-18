#ifndef CosmicRayShieldGeom_CRSScintillatorBarDetail_hh
#define CosmicRayShieldGeom_CRSScintillatorBarDetail_hh

//
// Representation of common properties of the Scintillator Bars etc...
//
// $Id: CRSScintillatorBarDetail.hh,v 1.3 2011/05/18 02:27:15 wb Exp $
// $Author: wb $
// $Date: 2011/05/18 02:27:15 $
//
// Original author KLG; somewhat based on Rob Kutschke's StrawDetail
//

#include <vector>
#include <string>

namespace mu2e {

  class CRSScintillatorBarDetail{

    friend class CosmicRayShieldMaker;

  public:

    CRSScintillatorBarDetail():
      _id(-1),
      _materialNames(),
      _halfLengths()
    {};

    CRSScintillatorBarDetail( int32_t const id,
                              std::vector<std::string> const & materialNames,
                              std::vector<double> const & halfLengths
                              );

    // Compiler generated versions are OK for destructor
    // and for copy and assignment constructors.

    int32_t const  Id() const { return  _id; }

    std::string const & getMaterialName(int idx) const { return _materialNames.at(idx);}

    std::vector<std::string> const & getMaterialNames() const { return _materialNames;}

    std::vector<double> const & getHalfLengths() const { return _halfLengths;}

    std::string name( std::string const & base ) const;

  private:

    // Identifier for this type of CRSScintillatorBar.
    int32_t _id;

    // Order of materials is:

    // _scintillatorBarMaterialName
    // _scintillatorBarPigmentationMaterialName
    // _scintillatorModuleOuterSheetMaterialName
    // _scintillatorModuleInterLayerSheetMaterialName

    std::vector<std::string> _materialNames;

    // outer dimensions
    std::vector<double> _halfLengths;

  };

}  //namespace mu2e

#endif /* CosmicRayShieldGeom_CRSScintillatorBarDetail_hh */
