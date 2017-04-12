/*
* Decay.cxx
*
*  Created on: 24.06.2010
*      Author: koehne
*/

#include "PROPOSAL/Decay.h"
#include "PROPOSAL/Constants.h"
#include "PROPOSAL/methods.h"
#include "PROPOSAL/Output.h"
#include <algorithm>
#include <cmath>
#include "boost/bind.hpp"

using namespace std;


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//-------------------------public member functions----------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Decay::MakeDecay()
{

    if(multiplier_ <= 0 || particle_->GetLifetime() < 0)
    {
        return 0;
    }

    return multiplier_/max((particle_->GetMomentum()/particle_->GetMass())*particle_->GetLifetime()*SPEED, XRES);
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Decay::CalculateProductEnergy( double ernd, double arnd, double srnd )
{
    if(particle_->GetLifetime()<0)
    {
        out_ = ParticleType::unknown;
        return 0;
    }
    double emax, x0, f0, el, lm, pl;
    ParticleType::Enum out1 = ParticleType::NuMu;
    ParticleType::Enum out2 = ParticleType::NuMu;

    // Tau Decay
    if (particle_->GetType() == ParticleType::TauMinus || particle_->GetType() == ParticleType::TauPlus)
    {

        const double brmu   =   0.1737;         // ratio: tau- ---> mu + anti-munu + nu-tau
        const double brel   =   0.1783+brmu;    // ratio: tau- ---> electron + anti-electron +nu-tau
        const double brpi   =   0.1109+brel;    // ratio: tau- ---> pi- + nu-tau
        const double br2p   =   0.2540+brpi;    // ratio: tau- ---> pi- + pi0 + nu-tau
        const double br3p   =   0.1826+br2p;    // ratio: tau- ---> pi- + pi0 + pi0 + nutau and pi- + pi+ + pi- + nu-tau

        if( srnd<brmu )
        {
            lm  =   MMU;

            if (particle_->GetType() == ParticleType::TauPlus)
            {
                out_ = ParticleType::MuPlus;
                if(store_neutrinos_)
                {
                    out1 = ParticleType::NuTauBar;
                    out2 = ParticleType::NuMu;
                }
            }
            else if (particle_->GetType() == ParticleType::TauMinus)
            {
                out_ = ParticleType::MuMinus;
                if(store_neutrinos_)
                {
                    out1 = ParticleType::NuTau;
                    out2 = ParticleType::NuMuBar;
                }
            }
            else
            {
                log_fatal("You should never end here: tau- or tau+?");
            }
        }
        else if( srnd<brel )
        {
            lm  =   ME;

            if (particle_->GetType() == ParticleType::TauPlus)
            {
                out_ = ParticleType::EPlus;

                if(store_neutrinos_)
                {
                    out1 = ParticleType::NuTauBar;
                    out2 = ParticleType::NuE;
                }
            }
            else if (particle_->GetType() == ParticleType::TauMinus)
            {
                out_ = ParticleType::EMinus;

                if(store_neutrinos_)
                {
                    out1 = ParticleType::NuTau;
                    out2 = ParticleType::NuEBar;
                }
            }
            else
            {
                log_fatal("You should never end here: tau- or tau+?");
            }
        }
        else
        {
            if( srnd<brpi )
            {
                lm  =   MPI;
            }
            else if( srnd<br2p )
            {
                lm  =   MRH;
            }
            else if( srnd<br3p )
            {
                lm  =   MA1;
            }
            else
            {
                lm  =   MRS;
            }
            el  =   (MTAU*MTAU + lm*lm) / (2*MTAU);

            out_ = ParticleType::Hadrons;

            el  =   el * (particle_->GetEnergy()/particle_->GetMass()) + sqrt(el*el - lm*lm) * (particle_->GetMomentum()/particle_->GetMass()) * (2*arnd-1);

            if(store_neutrinos_)
            {
                if( EndsWith(particle_->GetName(),"+") )
                {
                    out1 = ParticleType::NuTauBar;
                }
                else if( EndsWith(particle_->GetName(),"-") )
                {
                    out1 = ParticleType::NuTau;
                }
                else
                {
                    out1 = ParticleType::NuTau;
                }
               // o->output(1, out1, particle_->GetEnergy()-el, 0);
            }
            return el;
        }
    }

    //Muon Decay
    else
    {
        lm  =   ME;
        if (particle_->GetType() == ParticleType::MuPlus)
        {
            out_ = ParticleType::EPlus;

            if(store_neutrinos_)
            {
                out1 = ParticleType::NuMuBar;
                out2 = ParticleType::NuE;
            }
        }
        else if (particle_->GetType() == ParticleType::MuMinus)
        {
            out_ = ParticleType::EMinus;

            if(store_neutrinos_)
            {
                out1 = ParticleType::NuMu;
                out2 = ParticleType::NuEBar;
            }
        }
        else
        {
            log_fatal("You should never end here: mu- or mu+?");
        }
    }
    emax    =   (particle_->GetMass()*particle_->GetMass() + lm*lm) / (2*particle_->GetMass());
    x0      =   lm/emax;
    f0      =   x0*x0;
    f0      =   f0*x0 - f0*f0/2;
    el      =   max(root_finder_->FindRoot(
                        x0, 1.0, 0.5,
                        boost::bind(&Decay::Function, this, _1),
                        boost::bind(&Decay::DifferentiatedFunction, this, _1),
                        f0+(0.5-f0)*ernd) *emax
                    , lm);
    pl      =   sqrt(el*el - lm*lm);

    if(store_neutrinos_)
    {
        int sign;
        double cp, sp, ct, st, en, En1, En2;
        cp  =   pl/(particle_->GetMass() - el);
        sp  =   sqrt(1 - cp*cp);

        if(arnd<0.5)
        {
            sign    =   1;
            arnd    =   2*arnd;
        }
        else
        {
            sign    =   -1;
            arnd    =   2*arnd-1;
        }
        ct  =   2*arnd - 1;
        st  =   sqrt(1 - ct*ct);
        en  =   (particle_->GetMass() - el)/2;
        En1 =   -cp*ct - sign*sp*st;
        En2 =   -cp*ct + sign*sp*st;
        En1 =   en * ((particle_->GetEnergy()/particle_->GetMass()) + (particle_->GetMomentum()/particle_->GetMass()) * En1);
        En2 =   en * ((particle_->GetEnergy()/particle_->GetMass()) + (particle_->GetMomentum()/particle_->GetMass()) * En2);
        //o->output(1, out1, En1, 0);
        //o->output(1, out2, En2, 0);
    }

    return el * (particle_->GetEnergy()/particle_->GetMass()) + pl * (particle_->GetMomentum()/particle_->GetMass()) * (2*arnd - 1);
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//--------------------------------constructors--------------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


Decay::Decay()
    :out_               ( )
    ,store_neutrinos_   ( false )
    ,multiplier_        ( 1 )
{
    root_finder_    =   new RootFinder(IMAXS, IPREC);
    particle_       =   new PROPOSALParticle();
    backup_particle_= particle_;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


Decay::Decay(PROPOSALParticle* particle)
    :particle_          ( particle )
    ,out_               ( )
    ,store_neutrinos_   ( false )
    ,multiplier_        ( 1 )
{
    root_finder_    =   new RootFinder(IMAXS, IPREC);
    backup_particle_= particle_;

}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


Decay::Decay(const Decay &decay)
    :root_finder_       ( new RootFinder(*decay.root_finder_) )
    ,particle_          ( new PROPOSALParticle(*decay.particle_) )
    ,backup_particle_   ( new PROPOSALParticle(*decay.backup_particle_) )
    ,out_               ( decay.out_)
    ,store_neutrinos_   ( decay.store_neutrinos_ )
    ,multiplier_        ( decay.multiplier_ )
{

}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//-------------------------operators and swap function------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


Decay& Decay::operator=(const Decay &decay)
{
    if (this != &decay)
    {
      Decay tmp(decay);
      swap(tmp);
    }
    return *this;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


bool Decay::operator==(const Decay &decay) const
{
    if( *particle_          != *decay.particle_ )       return false;
    if( store_neutrinos_    != decay.store_neutrinos_ ) return false;
    if( multiplier_         != decay.multiplier_ )      return false;
    if( *root_finder_       != *decay.root_finder_ )    return false;

    if( out_ !=  decay.out_)                            return false;

    //else
    return true;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


bool Decay::operator!=(const Decay &decay) const
{
    return !(*this == decay);
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


void Decay::swap(Decay &decay)
{
    using std::swap;

    swap( store_neutrinos_  ,   decay.store_neutrinos_ );
    swap( multiplier_       ,   decay.multiplier_ );

    particle_->swap( *decay.particle_ );
    root_finder_->swap( *decay.root_finder_ );
    swap(out_, decay.out_);
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//-------------------------Functions to Rootfinder----------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Decay::Function(double x)
{
    double x2;
    x2  =   x*x;
    return  x*x2 - x2*x2/2;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Decay::DifferentiatedFunction(double x)
{
    return (3 - 2*x) * x*x;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//---------------------------------Setter-------------------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


void Decay::SetOut(ParticleType::Enum out){
    out_    =   out;
}

void Decay::SetRootFinder(RootFinder *root_finder){
    root_finder_    =   root_finder;
}

void Decay::SetParticle(PROPOSALParticle* particle){
    particle_   =   particle;
}

void Decay::SetStoreNeutrinos(bool store_neutrinos){
    store_neutrinos_    =   store_neutrinos;
}

void Decay::SetMultiplier(double multiplier){
    multiplier_ =   multiplier;
}

PROPOSALParticle *Decay::GetBackup_particle() const
{
    return backup_particle_;
}

void Decay::SetBackup_particle(PROPOSALParticle *backup_particle)
{
    backup_particle_ = backup_particle;
}

void Decay::RestoreBackup_particle()
{
    particle_ = backup_particle_;
}
