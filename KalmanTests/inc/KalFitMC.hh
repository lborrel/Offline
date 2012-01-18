//
// MC functions associated with KalFit
// $Id: KalFitMC.hh,v 1.10 2012/01/18 01:25:16 brownd Exp $
// $Author: brownd $ 
// $Date: 2012/01/18 01:25:16 $
//
#ifndef KalFitMC_HH
#define KalFitMC_HH

// data
#include "art/Framework/Principal/fwd.h"
#include "RecoDataProducts/inc/StrawHit.hh"
#include "MCDataProducts/inc/StrawHitMCTruth.hh"
#include "MCDataProducts/inc/PtrStepPointMCVectorCollection.hh"
#include "MCDataProducts/inc/StrawHitMCTruthCollection.hh"
#include "MCDataProducts/inc/StepPointMCCollection.hh"
#include "MCDataProducts/inc/SimParticleCollection.hh"
#include "MCDataProducts/inc/SimParticle.hh"
// tracker
#include "TrackerGeom/inc/Tracker.hh"
#include "TrackerGeom/inc/Straw.hh"
// BaBar
#include "BaBar/BaBar.hh"
#include "BaBar/PdtPid.hh"
#include "KalmanTests/inc/TrkDef.hh"
#include "KalmanTests/inc/TrkStrawHit.hh"
#include "KalmanTests/inc/KalFit.hh"
//CLHEP
#include "CLHEP/Units/PhysicalConstants.h"
// root 
#include "Rtypes.h"
#include "TTree.h"
#include "TClass.h"
#include <vector>

namespace mu2e 
{  
 // some convenient typedefs    
  typedef art::Ptr<SimParticle> SPPtr;
// structs
  struct TrkSum {
    double _esum;
    unsigned _count;
    art::Ptr<SimParticle> _spp;
    int _pdgid;
    int _gid,_pid;
    double _time;
    CLHEP::Hep3Vector _pos;
    TrkSum() : _esum(0.0),_count(0),_pdgid(0),_gid(0),_time(0.0) {}
    TrkSum(StepPointMC const& mchit,art::Ptr<SimParticle>& spp) : _esum(mchit.eDep()),_count(1),
    _spp(spp),_pdgid(0),_gid(-1),_pid(-1),
    _time(mchit.time()),
    _pos(mchit.position()){
      if(spp.isNonnull() ){
	_pdgid = spp->pdgId();
	_pid = spp->creationCode();
	if( spp->genParticle().isNonnull())
	  _gid = spp->genParticle()->generatorId().id();
      }
    }
    bool operator == (const TrkSum& other) { return _spp == other._spp; }
    bool operator < (const TrkSum& other) { return _spp < other._spp; }
    void append(StepPointMC const& mchit)  {
      double eold = _esum;
      _esum += mchit.eDep();
      _time = _time*eold/_esum + mchit.time()*mchit.eDep()/_esum;
      _pos = _pos*(eold/_esum) + mchit.position()*(mchit.eDep()/_esum);
      _count++;
    }
// comparison functor for ordering according to energy
    struct ecomp : public std::binary_function<TrkSum,TrkSum, bool> {
      bool operator()(TrkSum const& t1, TrkSum const& t2) { return t1._esum < t2._esum; }
    };
  };
  typedef std::vector<TrkSum> MCHitSum;
 // simple structs
  struct threevec {
    Float_t _x,_y,_z;
    threevec(): _x(0.0),_y(0.0),_z(0.0) {}
    threevec(const CLHEP::Hep3Vector& vec) : _x(vec.x()),_y(vec.y()),_z(vec.z()) {}
    threevec& operator = (const CLHEP::Hep3Vector& vec){ _x =vec.x(); _y =vec.y(); _z= vec.z(); return *this; }
  };

  struct helixpar {
    Float_t _d0, _p0, _om, _z0, _td;
    helixpar() : _d0(0.0),_p0(0.0),_om(0.0),_z0(0.0),_td(0.0) {}
    helixpar(const HepVector& pvec) : _d0(pvec[0]),_p0(pvec[1]),_om(pvec[2]),_z0(pvec[3]),_td(pvec[4]) {}
    helixpar(const HepSymMatrix& pcov) : _d0(sqrt(pcov.fast(1,1))),_p0(sqrt(pcov.fast(2,2))),_om(sqrt(pcov.fast(3,3))),
      _z0(sqrt(pcov.fast(4,4))),_td(sqrt(pcov.fast(5,5))) {}
  };

  struct TrkStrawHitInfo {
    Int_t _active,_usable;
    Float_t _resid, _residerr, _rdrift, _rdrifterr, _trklen;
    UInt_t _mcn, _mcnunique, _mcpdg, _mcgen, _mcproc;
// root 
    ClassDef(TrkStrawHitInfo,1)
  };

  struct MCEvtData {
    MCEvtData(const StrawHitMCTruthCollection* mcstrawhits,
      const PtrStepPointMCVectorCollection* mchitptr,
      const StepPointMCCollection *mcsteps,
      const StepPointMCCollection *mcvdsteps) : _mcstrawhits(mcstrawhits),_mchitptr(mchitptr),
      _mcsteps(mcsteps),_mcvdsteps(mcvdsteps){}
    void clear() {_mcstrawhits = 0; _mchitptr = 0; _mcsteps = 0; _mcvdsteps = 0; _simparts = 0; }
    MCEvtData() {clear();}
    bool good() { return _mcstrawhits != 0 && _mchitptr != 0 && _mcsteps != 0 && _mcvdsteps != 0; }
    const StrawHitMCTruthCollection* _mcstrawhits;
    const PtrStepPointMCVectorCollection* _mchitptr;
    const StepPointMCCollection *_mcsteps, *_mcvdsteps;
    const SimParticleCollection *_simparts;
  };
  
  typedef StepPointMCCollection::const_iterator MCStepItr;
//  struct test : public binary_function<double,double,bool> {
//    bool operator()(double x, double y) { return x < y;}
//  };

//  Simple helper class to find MC information within collections
  
  class KalFitMC {
  public:
    enum TRACKERPOS {trackerEnt=0,trackerMid};
    explicit KalFitMC(fhicl::ParameterSet const&);
    virtual ~KalFitMC();
// find MC data in the event.  This must be called each event, before the other functions
    bool findMCData(const art::Event& evt);
// create a track definition object based on MC true particle
    bool trkFromMC(cet::map_vector_key const& trkid, TrkDef& mytrk);
// diagnostic comparison of reconstructed tracks with MC truth
    void trkDiag(TrkKalFit const& myfit);
    void trkDiag(TrkRecoTrk const& mytrk);
    void kalDiag(const KalRep* krep);
    void hitsDiag(std::vector<TrkStrawHit*> const& hits);
    void mcTrkInfo();
    void hitDiag(const TrkStrawHit* strawhit);
// allow creating the trees
    TTree* createTrkDiag();
    TTree* createHitDiag();
// General StrawHit MC functions: these should move to more general class, FIXME!!
    const MCHitSum& mcHitSummary(size_t ihit) const { return _mchitsums[ihit]; }
// access to MC data
    MCEvtData const& mcData() const { return _mcdata; }
// access to event-specific MC truth for conversion electron
    double MCT0(TRACKERPOS tpos) const { return tpos == trackerEnt ? _mcentt0 : _mcmidt0 ; }
    double MCMom(TRACKERPOS tpos) const { return tpos == trackerEnt ? _mcentmom : _mcmidmom ; }
    const helixpar& MCHelix(TRACKERPOS tpos) const { return tpos == trackerEnt ? _mcentpar : _mcmidpar ; }
    double MCBrems() const { return _bremsesum; }
  private:
// cache of event data
    MCEvtData _mcdata;
// cache of hit summary
   std::vector<MCHitSum> _mchitsums;
// event data labels
    std::string _mcstrawhitslabel;
    std::string _mcptrlabel;
    std::string _mcstepslabel;
    std::string _simpartslabel;
// helper functions
    void findMCSteps(StepPointMCCollection const* mcsteps, cet::map_vector_key const& trkid, std::vector<int> const& vids,
      std::vector<MCStepItr>& steps);
    static void findRelatives(PtrStepPointMCVector const& mcptr,std::map<SPPtr,SPPtr>& mdmap );
    static void fillMCHitSum(PtrStepPointMCVector const& mcptr,MCHitSum& summary );
    void fillMCHitSummary();
// config parameters
    double _mintrkmom; // minimum true momentum at z=0 to create a track from
    double _mct0err;
    int _debug;
    unsigned _minnhits,_maxnhits;
    bool _purehits;
// vector of detector Ids corresponding to entrance and midplane
    std::vector<int> _midvids;
    std::vector<int> _entvids;
// tracker ID
    int _trackerid;
// trk tuple variables
    TTree *_trkdiag;
    UInt_t _eventid;
    UInt_t _trkid;
    Int_t _fitstatus;
    Float_t _t00;
    Float_t _t00err;
    Float_t _t0;
    Float_t _t0err;
    Float_t _mct0;
    Float_t _mcentt0;
    Float_t _mcmidt0;
    Int_t _nhits;
    Int_t _ndof;
    UInt_t _niter;
    UInt_t _nt0iter;
    UInt_t _nweediter;
    Int_t _nactive;
    Int_t _ncactive;
    Float_t _chisq;
    Float_t _fitcon;
    Float_t _fitmom;
    Float_t _fitmomerr;
    Float_t _mcmom;
    Float_t _mccost;
    Float_t _mcentmom;
    Float_t _mcmidmom;
    Float_t _seedmom;
    helixpar _fitpar;
    helixpar _fiterr;
    helixpar _mcentpar;
    helixpar _mcmidpar;
    Float_t _bremsesum;
    Float_t _bremsemax;
    Float_t _bremsz;
 
    std::vector<TrkStrawHitInfo> _tshinfo;

// hit tuple variables
    TTree *_hitdiag;
    threevec _shpos;
    Float_t _dmid;
    Float_t _dmiderr;
    Float_t _hitt0;
    Float_t _hitt0err;
    Float_t _rdrift;
    Float_t _rdrifterr;
    Float_t _resid;
    Float_t _residerr;
    Float_t _edep;
    Int_t _amb;
    Float_t _hflt;
    Float_t _trkflt;
    Bool_t _active;
    Int_t _use;
    UInt_t _nmcsteps;
    threevec _mcpos;
    Float_t _mcdmid;
    Float_t _mchitt0;
    Float_t _mcrdrift;
    Float_t _pdist;
    Float_t _pperp;
    Float_t _pmom;
  };
}

#endif

