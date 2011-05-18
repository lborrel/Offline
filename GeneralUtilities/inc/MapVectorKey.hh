#ifndef GeneralUtilities_MapVectorKey_hh
#define GeneralUtilities_MapVectorKey_hh
//
// An object to be the key in a MapVector.
//
// $Id: MapVectorKey.hh,v 1.4 2011/05/18 02:27:16 wb Exp $
// $Author: wb $
// $Date: 2011/05/18 02:27:16 $
//
// Original author Rob Kutschke
//
// The main point of this class is to force a compile time
// error if a user tries to use an integer, rather than
// a MapVectorKey as an argument to one of the accessors of
// a MapVector.
//

#include <ostream>

class MapVectorKey{

public:

  // Default c'tor needed for ROOT persistency.
  // Otherwise I would prefer that we not have one.
  MapVectorKey():_key(-1){}

  // No automatic conversion of int or unsigned int to MapVectorKey.
  explicit MapVectorKey(int key):
    _key(key){
  }

  explicit MapVectorKey(unsigned int key):
    _key(static_cast<int32_t>(key)){
  }

  // Compiler generated versions are OK for:
  // copy c'tor, destructor, operator=

  // If we want to stick a MapVectorKey into a plain-old-ntuple,
  // or to make a histogram of the keys, we need these accessors.
  // But we do not want automatic conversion to int:
  //  - Therefore do not implement operator(int);
  int32_t  asInt() const  { return _key;}
  uint32_t asUint() const { return static_cast<uint32_t>(_key);}

  bool operator==( MapVectorKey const& rhs) const{
    return (_key == rhs._key);
  }

  bool operator<( MapVectorKey const& rhs) const{
    return ( _key < rhs._key);
  }

  bool operator>( MapVectorKey const& rhs) const{
    return ( _key > rhs._key);
  }

  void print (std::ostream& ost ) const {
    ost << _key;
  }

private:

  int32_t _key;
};

inline std::ostream& operator<<( std::ostream& ost,
                                 MapVectorKey const& i){
  i.print(ost);
  return ost;
}

inline bool operator!=( MapVectorKey const& lhs,
                        MapVectorKey const& rhs) {
  return !( lhs == rhs);
}

#endif /* GeneralUtilities_MapVectorKey_hh */
