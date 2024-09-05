// predicate to extrapolate to foils in the ST
// track changes direction.
#ifndef Mu2eKinKal_ExtrapolateST_hh
#define Mu2eKinKal_ExtrapolateST_hh
#include "Offline/Mu2eKinKal/inc/KKTrack.hh"
#include "KinKal/General/TimeDir.hh"
#include "KinKal/General/TimeRange.hh"
#include "KinKal/Geometry/Intersection.hh"
#include "KinKal/Geometry/ParticleTrajectoryIntersect.hh"
#include "Offline/KinKalGeom/inc/StoppingTarget.hh"
#include "cetlib_except/exception.h"
#include <memory>
#include <vector>
#include <limits>
namespace mu2e {
  using KinKal::TimeDir;
  using KinKal::TimeRange;
  using KinKal::Intersection;
  using KinKalGeom::StoppingTarget;
  using KinKal::Annulus;
  using AnnPtr = std::shared_ptr<KinKal::Annulus>;
  using FoilCol = std::vector<AnnPtr>;
  class ExtrapolateST {
    public:
      ExtrapolateST() : maxDt_(-1.0), tol_(1e10),
      zmin_(std::numeric_limits<float>::max()),
      zmax_(-std::numeric_limits<float>::max()),
      rmin_(std::numeric_limits<float>::max()),
      rmax_(-std::numeric_limits<float>::max()),
      debug_(0){}

      ExtrapolateST(double maxdt, double tol, StoppingTarget const& stoptarg, int debug=0) : maxDt_(maxdt), tol_(tol),
      zmin_( (stoptarg.outer().center() - stoptarg.outer().axis()*stoptarg.outer().halfLength()).Z()),
      zmax_( (stoptarg.outer().center() + stoptarg.outer().axis()*stoptarg.outer().halfLength()).Z()),
      rmin_( stoptarg.inner().radius()), rmax_(stoptarg.outer().radius()),
        foils_(stoptarg.foils()),debug_(debug) {}
      // interface for extrapolation
      double maxDt() const { return maxDt_; } // maximum time to extend the track
      double tolerance() const { return tol_; } // intersection tolerance
      double zmin() const { return zmin_; }
      double zmax() const { return zmax_; }
      double rmin() const { return rmin_; }
      double rmax() const { return rmax_; }
      auto const& foils () const { return foils_; }
      auto const& intersection() const { return inter_; }
      auto const& foil() const { return ann_; }
      auto const& foilId() const { return sid_; }
      int debug() const { return debug_; }
      // extrapolation predicate: the track will be extrapolated until this predicate returns false, subject to the maximum time
      template <class KTRAJ> bool needsExtrapolation(KinKal::Track<KTRAJ> const& ktrk, TimeDir tdir, double time) const;
      // reset between tracks
      void reset() const { inter_ = Intersection(); sid_ = SurfaceId(); ann_ = AnnPtr();}
      // find the nearest foil to a z positionin a given z direction
      size_t nearestFoil(double zpos, double zdir) const;
    private:
      double maxDt_; // maximum extrapolation time
      double tol_; // intersection tolerance (mm)
      mutable Intersection inter_; // cache of most recent intersection
      mutable SurfaceId sid_; // cache of most recent intersection foil SID
      mutable AnnPtr ann_; // cache of most recent intersection foil surface
      // cache of front and back Z positions
      double zmin_, zmax_; // z range of ST volume
      double rmin_, rmax_; // inner and outer radii of the anuli
      FoilCol foils_; // foils
      int debug_; // debug level
  };

  template <class KTRAJ> bool ExtrapolateST::needsExtrapolation(KinKal::Track<KTRAJ> const& ktrk, TimeDir tdir, double time) const {
    auto const& fittraj = ktrk.fitTraj();
    auto const& ktraj = fittraj.nearestPiece(time);
    auto vel = ktraj.speed(time)*ktraj.axis(time).direction();// use helix axis to define the velocity
    auto pos = ktraj.position3(time);
    double zvel = vel.Z()*timeDirSign(tdir); // sign by extrapolation direction
    double zpos = pos.Z();
    double rho = pos.Rho();
    auto const& bnom = fittraj.bnom(time);
    if(debug_ > 2)std::cout << "ST extrap start time " << time << " z " << zpos << " zvel " << zvel << " rho " << rho << std::endl;
    // stop if the particle is heading away from the ST
    if( (zvel > 0 && zpos > zmax_ ) || (zvel < 0 && zpos < zmin_)){
      reset(); // clear any cache
      if(debug_ > 1)std::cout << "Heading away from ST: done" << std::endl;
      return false;
    }
    // if the particle is going in the right direction but haven't yet reached the ST in Z just keep going
    if( (zvel > 0 && zpos < zmin_) || (zvel < 0 && zpos > zmax_) ){
      reset();
      if(debug_ > 2)std::cout << "Heading towards ST, z " << zpos<< std::endl;
      return true;
    }
    // if we get to here we are in the correct Z range
    // check if we are inside the ST volume
    if(rho < rmin_ || rho > rmax_) return true; // keep going
    // we are in the correct radial range too.  Look for an intersection with the foils
    static const double epsilon(1e-2); // small difference to avoid re-intersecting
    double dt = ktraj.range().range() - epsilon; // small difference to avoid re-intersecting
    double tz = 1.0/std::max(fabs(zvel)/(zmax_-zmin_),1.0/maxDt_); // time to cross the ST: protect against reflection (zero z speed)
    TimeRange trange = tdir == TimeDir::forwards ? TimeRange(time-dt,time+tz) : TimeRange(time-tz,time+dt);
    // Use z to determine which foil might be the next hit
    double tstart = tdir == TimeDir::forwards ? trange.begin() : trange.end();
    auto fpos = fittraj.position3(tstart);
    int ifoil = nearestFoil(fpos.Z(),zvel);
    if(debug_ > 2)std::cout << "ST volume rho " << fpos.Rho() <<  " z " << fpos.Z() << " first ST foil " << ifoil << std::endl;
    if(ifoil >= (int)foils_.size())return true;
    if(debug_ > 2)std::cout << "Looping on foils " << std::endl;
    int dfoil = zvel > 0.0 ? 1 : -1; // iteration direction
    // loop over foils
    while(ifoil > 0 && ifoil < (int)foils_.size() && (foils_[ifoil]->center().Z() - zpos)*dfoil < 0.0){
      auto foilptr = foils_[ifoil];
      if(debug_ > 2)std::cout << "foil " << ifoil << " z " << foilptr->center().Z() << std::endl;
      auto newinter = KinKal::intersect(fittraj,*foilptr,trange,tol_,tdir);
      if(debug_ > 2)std::cout << "ST inter " << newinter.time_ << " " << newinter.onsurface_ << " " << newinter.inbounds_ << std::endl;
      bool goodextrap = newinter.onsurface_ && newinter.inbounds_;
      if(goodextrap){
        // if the cached intersection is valid, test this intersection time against it, and
        // if the new intersection time is the same as the last, keep looping on foils
        if(inter_.onsurface_ && inter_.inbounds_ && ( (tdir == TimeDir::forwards && newinter.time_ <= inter_.time_) ||
              (tdir == TimeDir::backwards && newinter.time_ >= inter_.time_) ) ) {
          if(debug_ > 2)std::cout << "ST Skipping duplicate intersection " << std::endl;
          ifoil += dfoil;
          continue;
        }
        // otherwise test if the trajectory extends to the intersection time yet or not. If so we are done
        if ( (tdir == TimeDir::forwards && newinter.time_ < time) ||
            (tdir == TimeDir::backwards && newinter.time_ > time ) ) {
          // update the cache
          inter_ = newinter;
          ann_ = foils_[ifoil];
          sid_ = SurfaceId(SurfaceIdEnum::ST_Foils,ifoil);
          if(debug_ > 0)std::cout << "Good ST intersection found in range, time " << inter_.time_ << " Z " << inter_.pos_.Z()
            << " sid " << sid_ << std::endl;
          return false;
        }
        ifoil += dfoil; // otherwise continue loopin on foils
      } else {
        // move to next foil
        ifoil += dfoil;
      }
    }
    // no more intersections: keep extending in Z till we clear the ST
    reset();
    if(debug_ > 1)std::cout << "Extrapolating to ST edge, z " << zpos << std::endl;
    if(zvel > 0.0)
      return zpos < zmax_;
    else
      return zpos > zmin_;
  }

  size_t ExtrapolateST::nearestFoil(double zpos, double zvel) const {
    size_t retval = foils_.size();
    if(zvel > 0.0){ // going forwards in z
      for(auto ifoil= foils_.begin(); ifoil != foils_.end(); ifoil++){
        auto const& foilptr = *ifoil;
        if(foilptr->center().Z() > zpos){
          retval = std::distance(foils_.begin(),ifoil);
          break;
        }
      }
    } else {
      for(auto ifoil= foils_.rbegin(); ifoil != foils_.rend(); ifoil++){
        auto const& foilptr = *ifoil;
        if(foilptr->center().Z() < zpos){
          auto jfoil = ifoil.base()-1; // points to the equivalent forwards object
          retval = std::distance(foils_.begin(),jfoil);
          break;
        }
      }
    }
    return retval;
  }

}
#endif
