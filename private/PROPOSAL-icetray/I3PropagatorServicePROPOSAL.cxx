/**
 * class: I3PropagatorServicePROPOSAL
 *
 * Mainly a copy of:
 * Version $Id: I3PropagatorService.cxx 69179 2010-12-04 01:33:36Z dima $
 *
 *
 * Date 07 Jul 2012
 *
 * (c) 2003 IceCube Collaboration
 */

// class header
#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#include <sstream>
#include <unistd.h> // check for write permissions

#include "dataclasses/physics/I3Particle.h"
#include "simclasses/I3MMCTrack.h"

#include "PROPOSAL-icetray/I3PropagatorServicePROPOSAL.h"
#include "PROPOSAL/Propagator.h"
#include "PROPOSAL/PROPOSALParticle.h"
#include "PROPOSAL/Output.h"

using namespace std;

class Output;

// Now: parametrization_ = 1,  Former: photo_family=1 and photo_param=1 Kokoulin
// Now: parametrization_ = 2,  Former: photo_family=2 and photo_param=1 Kokoulin + hard component
// Now: parametrization_ = 3,  Former: photo_family=1 and photo_param=2 Rhode
// Now: parametrization_ = 4,  Former: photo_family=2 and photo_param=2 Rhode + hard component
// Now: parametrization_ = 5,  Former: photo_family=1 and photo_param=3 Bezrukov/Bugaev
// Now: parametrization_ = 6,  Former: photo_family=2 and photo_param=3 Bezrukov/Bugaev + hard component
// Now: parametrization_ = 7,  Former: photo_family=1 and photo_param=4 Zeus
// Now: parametrization_ = 8,  Former: photo_family=2 and photo_param=4 Zeus + hard component
// Now: parametrization_ = 9,  Former: photo_family=3 and photo_param=1 shadow=1 ALLM 91
// Now: parametrization_ = 10, Former: photo_family=3 and photo_param=1 shadow=2 ALLM 91
// Now: parametrization_ = 11, Former: photo_family=3 and photo_param=2 shadow=1 ALLM 97
// Now: parametrization_ = 12, Former: photo_family=3 and photo_param=2 shadow=2 ALLM 97
// Now: parametrization_ = 13, Former: photo_family=4 and photo_param=1 shadow=1 Butkevich/Mikhailov
// Now: parametrization_ = 14, Former: photo_family=4 and photo_param=1 shadow=2 Butkevich/Mikhailov

int ConvertOldToNewPhotonuclearParametrization(int photo_family,int photo_param,int shadow)
{
    switch(photo_family*100+photo_param*10)
    {
        case 110:   return 1; break;
        case 210:   return 2; break;
        case 120:   return 3; break;
        case 220:   return 4; break;
        case 130:   return 5; break;
        case 230:   return 6; break;
        case 140:   return 7; break;
        case 240:   return 8; break;
    default:
        break;
    }

    switch(photo_family*100+photo_param*10+shadow)
    {
        case 311:   return 9; break;
        case 312:   return 10; break;
        case 321:   return 11; break;
        case 322:   return 12; break;
        case 411:   return 13; break;
        case 412:   return 14; break;
    default:
        break;
    }

    log_warn("No fitting parametrization %i found setting default! 322 / 12!",photo_family*100+photo_param*10+shadow);

    return 12;
}


bool IsWritable(std::string table_dir)
{
    bool writeable = false;

    if (access(table_dir.c_str(), F_OK) == 0)
    {
        if ((access(table_dir.c_str(), R_OK) == 0) && (access(table_dir.c_str(), W_OK) == 0))
        {
            writeable = true;
            log_info("Table directory does exist and has read and write permissions: %s", table_dir.c_str());
        }
        else
        {
            if (access(table_dir.c_str(), R_OK) != 0)
            {
                log_info("Table directory is not readable: %s", table_dir.c_str());
            }
            else
            {
                log_info("Table directory is not writable: %s", table_dir.c_str());
            }
        }
    }
    else
    {
        log_info("Table directory does not exist: %s", table_dir.c_str());
    }

    return writeable;
}

I3PropagatorServicePROPOSAL::I3PropagatorServicePROPOSAL(
      std::string mediadef, std::string tabledir
    , double cylinderRadius, double cylinderHeight
    , I3Particle::ParticleType type, double particleMass
    , BremsstrahlungParametrization brems_param
    , PhotonuclearParametrizationFamily photo_family
    , PhotonuclearParametrization photo_param
    , ShadowingParametrization shadow)
  : particleMass_(particleMass)
  , mediadef_(mediadef)
  , tabledir_(tabledir)
  , cylinderRadius_(cylinderRadius)
  , cylinderHeight_(cylinderHeight)
  , brems_param_(brems_param)
  , photo_family_(photo_family)
  , photo_param_(photo_param)
  , shadow_(shadow)
{
    // TODO(sudojan): type
    //TODO(mario): particeMass Mo 2017/02/20

    // switch(type) {
    //   case I3Particle::MuMinus:
    //   case I3Particle::MuPlus:
    //     break;
    //   case I3Particle::TauMinus:
    //   case I3Particle::TauPlus:
    //     break;
    //   case I3Particle::STauMinus:
    //   case I3Particle::STauPlus:
    //     if (!std::isfinite(particleMass))
    //       log_fatal("You asked for staus of non-finite mass %f", particleMass_);
    //     break;
    //   case I3Particle::Monopole:
    //     if (!std::isfinite(particleMass))
    //       log_fatal("You asked for monopoles of non-finite mass %f", particleMass_);
    //     break;
    //   default:
    //     I3Particle dummy;
    //     dummy.SetType(type);
    //     log_fatal("I don't know how to propagate %s", dummy.GetTypeString().c_str());
    // }

    // particle_type = new PROPOSALParticle(GenerateMMCName(type))


    // ----[ Check table dir and mediadef ]------------------ //

    if (mediadef_.empty())
        mediadef_ = GetDefaultMediaDef();
    if (tabledir_.empty())
        tabledir_ = GetDefaultTableDir();

    namespace fs = boost::filesystem;
    if (!fs::exists(mediadef_))
        log_fatal("The mediadef file '%s' can't be read!", mediadef_.c_str());
    if (!fs::is_directory(tabledir_))
        log_fatal("The table directory '%s' doesn't exist!", tabledir_.c_str());


    // ----[ Check, apply options ]-------------------------- //


    // Define propagator but do not apply option yet
    proposal = new Propagator(mediadef_,false);

    stringstream options;
    int photo = ConvertOldToNewPhotonuclearParametrization(photo_family_, photo_param_, shadow_);

    options << "You choose the following parameter by passing arguments:" << std::endl;
    options << "\tcylinderRadius = " << cylinderRadius_ << std::endl;
    options << "\tcylinderHeight = " << cylinderHeight_ << std::endl;
    options << "\tBremsstrahlungParametrization = " << brems_param_ << std::endl;
    options << "\tPhotonuclearParametrization = " << photo << std::endl;

    if (brems_param_ != proposal->GetBrems())
    {
        options << "\tChosen BremsstrahlungParametrization differs from parametrization in config file!" << std::endl;
        options << "Passed parametrization will be used: " << brems_param_ << std::endl;
    }

    if (photo != proposal->GetPhoto())
    {
        options << "\tChosen PhotonuclearParametrization differs from parametrization in config file!" << std::endl;
        options << "Passed parametrization will be used: " << photo << std::endl;
    }

    log_info(options.str().c_str());

    // if (std::isinf(particleMass_))
    // {
    //     log_fatal("You asked for monopoles of non-finite mass %f", particleMass_);
    // }

    Geometry* geo = new Geometry();
    geo->InitCylinder(0,0,0,cylinderRadius_,0,cylinderHeight_);
    proposal->SetDetector(geo);
    proposal->SetBrems(brems_param_);
    proposal->SetPhoto(photo);
    proposal->SetPath_to_tables(tabledir_);

    // proposal->SetParticle(particle_type)

    proposal->ApplyOptions();

	tearDownPerCall_ = false;
}

std::string I3PropagatorServicePROPOSAL::GetDefaultMediaDef()
{
	const char *I3_BUILD = getenv("I3_BUILD");
	if (!I3_BUILD)
		log_fatal("$I3_BUILD is not set!");
	std::string s(I3_BUILD);

    return s + "/PROPOSAL/resources/configuration";
}


std::string I3PropagatorServicePROPOSAL::GetDefaultTableDir()
{
    std::string append_string  = "/PROPOSAL/resources/tables";
    std::string append_string2 = "/PROPOSAL/tables";

    // --------------------------------------------------------------------- //
    // Environment variable set?
    // --------------------------------------------------------------------- //

    // Initializing a std::string with a NULL ptr is undefined behavior.
    // Why it doens't just return an empty string, I have no idea.
    std::string table_dir(getenv("PROPOSALTABLEDIR") ? getenv("PROPOSALTABLEDIR") : "");

    if (table_dir.empty())
    {
        log_info("$PROPOSALTABLEDIR is not set in env variables. Falling back to defaults.");
    }
    else
    {
        if(IsWritable(table_dir)) return table_dir;
    }

    // --------------------------------------------------------------------- //
    // Writable in I3_TESTDATA?
    // --------------------------------------------------------------------- //

    table_dir = std::string(getenv("I3_TESTDATA") ? getenv("I3_TESTDATA") : "");

    if (table_dir.empty())
    {
        log_warn("$I3_TESTDATA is not set, falling back to build folder!");
    }
    else
    {
        if (IsWritable(table_dir + append_string))
        {
            return table_dir + append_string;
        }
        else
        {
            log_warn("Falling back to build folder!");
        }
    }

    // --------------------------------------------------------------------- //
    // Fall back to I3_BUID
    // --------------------------------------------------------------------- //

    table_dir = std::string(getenv("I3_BUILD") ? getenv("I3_BUILD") : "");

    if (table_dir.empty())
    {
        log_fatal("$I3_BUILD is not set");
    }
    else
    {
        if (IsWritable(table_dir + append_string))
        {
            return table_dir + append_string;
        }
        else if (IsWritable(table_dir + append_string2))
        {
            return table_dir + append_string2;
        }
        else
        {
            log_fatal("No folder availble to fall back! Abort search for table directory.");
        }
    }
}


void I3PropagatorServicePROPOSAL::SetRandomNumberGenerator(I3RandomServicePtr random)
{
  rng_ = random;
  boost::function<double ()> f = boost::bind(&I3RandomService::Uniform, random, 0, 1);

  proposal->SetRandomNumberGenerator(f);

}

I3PropagatorServicePROPOSAL::~I3PropagatorServicePROPOSAL()
{
    delete proposal;
}

/**
 *
 */
std::vector<I3Particle> I3PropagatorServicePROPOSAL::Propagate(I3Particle& p, DiagnosticMapPtr frame){
  // saying where we are
  log_debug("Entering I3PropagatorServicePROPOSAL::Propagate()");

  vector<I3Particle> daughters;

  log_trace("location type = %d",p.GetLocationType());
  if(p.GetLocationType()!=I3Particle::InIce) return std::vector<I3Particle>();

  if((p.GetType()==I3Particle::NuE)||(p.GetType()==I3Particle::NuEBar)||
     (p.GetType()==I3Particle::NuMu)||(p.GetType()==I3Particle::NuMuBar)||
     (p.GetType()==I3Particle::NuTau)||(p.GetType()==I3Particle::NuTauBar)) return std::vector<I3Particle>();

  log_trace("particle to propagate:\n"
	    "type/energy[GeV]/posx[m]/posy[m]/posz[m]/theta[deg]/phi[deg]/length[m]\n"
	    "%d/%.2e/%.2f/%.2f/%.2f/%.2f/%.2f/%.2f",
	    p.GetType(),
	    p.GetEnergy ()/I3Units::GeV,
	    p.GetPos().GetX()/I3Units::m,
	    p.GetPos().GetY()/I3Units::m,
	    p.GetPos().GetZ()/I3Units::m,
	    p.GetZenith ()/I3Units::deg,
	    p.GetAzimuth ()/I3Units::deg,
	    p.GetLength()/I3Units::m);

	if (tearDownPerCall_) {

    delete proposal;
    proposal = new Propagator(mediadef_,false);
    // Apply Settings
    Geometry* geo = new Geometry();
    geo->InitCylinder(0,0,0,cylinderRadius_,0,cylinderHeight_);
    proposal->SetDetector(geo);
    proposal->SetBrems(brems_param_);
    proposal->SetPhoto(ConvertOldToNewPhotonuclearParametrization(photo_family_, photo_param_, shadow_));
    proposal->SetPath_to_tables(tabledir_);
    proposal->ApplyOptions();
    // proposal->SetParticle(particle_type)

    boost::function<double ()> f = boost::bind(&I3RandomService::Uniform, rng_, 0, 1);
    proposal->SetRandomNumberGenerator(f);
	}
  I3MMCTrackPtr mmcTrack = propagate(p, daughters);

  if (mmcTrack && frame) {
    if (!frame->Has("MMCTrackList"))
        frame->Put("MMCTrackList", I3MMCTrackListPtr(new I3MMCTrackList));
    I3MMCTrackListPtr trackList = frame->Get<I3MMCTrackListPtr>("MMCTrackList");
    i3_assert(trackList != NULL);

    trackList->push_back(*mmcTrack);
  }

  return daughters;
}

string I3PropagatorServicePROPOSAL::GenerateMMCName(const I3Particle& p){

  string name;

  if(p.GetType()==I3Particle::MuMinus) name="mu-";
  else if(p.GetType()==I3Particle::MuPlus) name="mu+";
  else if(p.GetType()==I3Particle::NuMu) name="nu_mu";
  else if(p.GetType()==I3Particle::NuMuBar) name="~nu_mu";
  else if(p.GetType()==I3Particle::NuE) name="nu_e";
  else if(p.GetType()==I3Particle::NuEBar) name="~nu_e";
  else if(p.GetType()==I3Particle::NuTau) name="nu_tau";
  else if(p.GetType()==I3Particle::NuTauBar) name="~nu_tau";
  else if((p.GetType()==I3Particle::EMinus) &&
	  (p.GetShape()==I3Particle::TopShower)) name="e-";
  else if((p.GetType()==I3Particle::EPlus) &&
	  (p.GetShape()==I3Particle::TopShower)) name="e+";
  else if(p.GetType()==I3Particle::TauMinus) name="tau-";
  else if(p.GetType()==I3Particle::TauPlus) name="tau+";
  else if((p.GetType()==I3Particle::Hadrons) &&
	  (p.GetShape()==I3Particle::TopShower)) name="hadr";
  else if(p.GetType()==I3Particle::Monopole) name="monopole";
  else if(p.GetType()==I3Particle::STauPlus ||
	  p.GetType()==I3Particle::STauMinus){
    stringstream s;
    s<<"stau="<<particleMass_/I3Units::GeV;
    name=s.str();
  }

  return name;
}

I3MMCTrackPtr I3PropagatorServicePROPOSAL::GenerateMMCTrack(PROPOSALParticle* particle){

    //explicitly specifying the units from MMC
    double xi = particle->GetXi() * I3Units::m;
    double yi = particle->GetYi() * I3Units::m;
    double zi = particle->GetZi() * I3Units::m;
    double ti = particle->GetTi() * I3Units::second;
    double Ei = particle->GetEi() * I3Units::GeV;

    double xf = particle->GetXf() * I3Units::m;
    double yf = particle->GetYf() * I3Units::m;
    double zf = particle->GetZf() * I3Units::m;
    double tf = particle->GetTf() * I3Units::second;
    double Ef = particle->GetEf() * I3Units::GeV;

    double xc = particle->GetXc() * I3Units::m;
    double yc = particle->GetYc() * I3Units::m;
    double zc = particle->GetZc() * I3Units::m;
    double tc = particle->GetTc() * I3Units::second;
    double Ec = particle->GetEc() * I3Units::GeV;

  I3MMCTrackPtr mmcTrack( new I3MMCTrack);
  mmcTrack->SetEnter(xi,yi,zi,ti,Ei);
  mmcTrack->SetCenter(xc,yc,zc,tc,Ec);
  mmcTrack->SetExit(xf,yf,zf,tf,Ef);

  double Elost = particle->GetElost() * I3Units::GeV;
  mmcTrack->SetDepositedEnergy(Elost);
  log_debug("Elost = %f", Elost);

  if(Elost>0) return mmcTrack;
  //return null pointer if Elost <= 0
  return I3MMCTrackPtr();
}

typedef std::map<int, I3Particle::ParticleType> particle_type_conversion_t;

static const particle_type_conversion_t fromRDMCTable =
boost::assign::list_of<std::pair<int, I3Particle::ParticleType> >
(-100, I3Particle::unknown)
(1, I3Particle::Gamma)
(2, I3Particle::EPlus)
(3, I3Particle::EMinus)
(4, I3Particle::Nu)
(5, I3Particle::MuPlus)
(6, I3Particle::MuMinus)
(7, I3Particle::Pi0)
(8, I3Particle::PiPlus)
(9, I3Particle::PiMinus)
(11, I3Particle::KPlus)
(12, I3Particle::KMinus)
(14, I3Particle::PPlus)
(15, I3Particle::PMinus)
(33, I3Particle::TauPlus)
(34, I3Particle::TauMinus)
(41, I3Particle::Monopole)
(201, I3Particle::NuE)
(202, I3Particle::NuMu)
(203, I3Particle::NuTau)
(204, I3Particle::NuEBar)
(205, I3Particle::NuMuBar)
(206, I3Particle::NuTauBar)
(1001, I3Particle::Brems)
(1002, I3Particle::DeltaE)
(1003, I3Particle::PairProd)
(1004, I3Particle::NuclInt)
(1005, I3Particle::MuPair)
(1006, I3Particle::Hadrons);

I3MMCTrackPtr
I3PropagatorServicePROPOSAL::propagate( I3Particle& p, vector<I3Particle>& daughters){

  /**
   * Natural units of MMC is cm, deg, MeV, and s.
   * Therefore we need to convert explicitly to
   * MMC units before passing the propagate method
   */
  double x_0 = p.GetPos().GetX()/I3Units::cm;     // [cm]
  double y_0 = p.GetPos().GetY()/I3Units::cm;    // [cm]
  double z_0 = p.GetPos().GetZ()/I3Units::cm;     // [cm]
  double theta_0 = p.GetDir().CalcTheta()/I3Units::deg; // [deg]
  double phi_0 = p.GetDir().CalcPhi()/I3Units::deg;   // [deg]
  double e_0 = p.GetEnergy()/I3Units::MeV;  // [MeV]
  double t_0 = p.GetTime()/I3Units::s;     // [s]

  string mmcName = GenerateMMCName(p);
  log_debug("MMC name of particle to propagate: %s",mmcName.c_str());

  PROPOSALParticle* particle = new PROPOSALParticle(mmcName, x_0, y_0, z_0, theta_0, phi_0, e_0, t_0);
  if (particle == 0) log_fatal("Error calling the Particle constructor");

  //--- Tomasz
  //vector<PROPOSALParticle*> aobj_l = amanda->propagate(particle);
  proposal->Propagate(particle);

  vector<PROPOSALParticle*> aobj_l = Output::getInstance().GetSecondarys();
  //get the propagated length of the particle
  //double length = particle->r;
    double length = particle->GetPropagatedDistance();
  //--- Tomasz End
  p.SetLength( length * I3Units::cm );
  log_trace(" length = %f cm ", length );

  int nParticles =  aobj_l.size();
  log_trace("nParticles = %d", nParticles);

  I3MMCTrackPtr mmcTrack;
  log_trace("javaClass_ == AMANDA");
  mmcTrack = GenerateMMCTrack(particle);
  if(mmcTrack)
      mmcTrack->SetParticle( p );

  for(int i=0; i < nParticles; i++){

    //Tomasz
    //in mmc the particle relationships are stored
    int type = aobj_l.at(i)->GetType();
    double x = aobj_l.at(i)->GetX() * I3Units::cm;
    double y = aobj_l.at(i)->GetY() * I3Units::cm;
    double z = aobj_l.at(i)->GetZ() * I3Units::cm;
    double theta = aobj_l.at(i)->GetTheta() * I3Units::deg;
    double phi = aobj_l.at(i)->GetPhi() * I3Units::deg;
    double t = aobj_l.at(i)->GetT() * I3Units::s;
    double e = aobj_l.at(i)->GetEnergy() * I3Units::MeV;
    double l = aobj_l.at(i)->GetPropagatedDistance() * I3Units::cm;

    log_trace("MMC DEBUG SEC  \n    type=%d pos=(%g,%g,%g) ang=(%g,%g)  e=%g t=%g  l=%g",
	      type, x, y, z, theta, phi, e, t, l);

    //this should be a stochastic
    I3Particle new_particle;

    //Setting MC Type
    I3Particle::ParticleType mcType(static_cast<I3Particle::ParticleType>(abs(type)));
    particle_type_conversion_t::const_iterator it = fromRDMCTable.find(mcType);
    if (it == fromRDMCTable.end()) {
	cout << "Particle information!" << endl << endl << *(aobj_l.at(i)) << endl << endl;
       log_fatal("unknown RDMC code \"%i\" cannot be converted to a I3Particle::ParticleType.", mcType);
    } else {
       new_particle.SetType(it->second);
    }
    new_particle.SetLocationType(I3Particle::InIce);
    new_particle.SetPos(x, y, z);
    new_particle.SetTime(t);
    new_particle.SetLength(l);
    new_particle.SetThetaPhi(theta,phi);
    new_particle.SetEnergy(e);

    // this is not the particle you're looking for
    // move along...and add it to the daughter list
    daughters.push_back(new_particle);

    //we're done with eobj
    //delete aobj_l.at(i); //Tomasz
  }
  Output::getInstance().ClearSecondaryVector(); //Tomasz
  delete particle;

  return mmcTrack;
}

