#include "PROPOSAL/Ionization.h"
#include <algorithm>

using namespace std;

namespace po	= boost::program_options;

//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//-------------------------public member functions----------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::CalculatedEdx()
{

    if(multiplier_<=0)
    {
        return 0;
    }

    if(do_dedx_Interpolation_)
    {
        return max(dedx_interpolant_->Interpolate(particle_->GetEnergy()), 0.);
    }

    double result, aux;

    SetIntegralLimits(0);

    aux     =   beta_*gamma_/(1.e-6*medium_->GetI());
    result  =   log(vUp_*(2*ME*particle_->GetEnergy()))+2*log(aux);
    aux     =   vUp_/(2*(1 + 1/gamma_));
    result  +=  aux*aux;
    aux     =   beta_*beta_;
    result  -=  aux*(1 + vUp_/vMax_) + Delta();

    if(result>0)
    {
        result*=IONK*particle_->GetCharge()*particle_->GetCharge()*medium_->GetZA()/(2*aux);
    }
    else
    {
        result=0;
    }
    return multiplier_*(medium_->GetMassDensity()*result
                        + particle_->GetEnergy()*(integral_->Integrate(vMin_, vUp_, boost::bind(&Ionization::FunctionToDEdxIntegral, this, _1),4)));
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::CalculatedNdx()
{
    if(multiplier_<=0)
    {
        return 0;
    }

    if(do_dndx_Interpolation_)
    {
        return sum_of_rates_ = max(dndx_interpolant_1d_->Interpolate(particle_->GetEnergy()), 0.);
    }
    else{
        SetIntegralLimits(0);
        return sum_of_rates_ = integral_->Integrate(vUp_,vMax_,boost::bind(&Ionization::FunctionToDNdxIntegral, this, _1),3,1);
    }
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::CalculatedNdx(double rnd)
{
    if(multiplier_<=0)
    {
        return 0;
    }

    if(do_dndx_Interpolation_)
    {
        return sum_of_rates_ = max(dndx_interpolant_1d_->Interpolate(particle_->GetEnergy()), 0.);
    }
    else
    {
        SetIntegralLimits(0);
        return sum_of_rates_ = integral_->IntegrateWithSubstitution(vUp_,vMax_,boost::bind(&Ionization::FunctionToDNdxIntegral, this, _1),1,rnd);
    }
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::CalculateStochasticLoss(double rnd)
{
    double rand, rsum;

    rand=medium_->GetSumCharge()*rnd;
    rsum=0;

    for(int i=0; i<medium_->GetNumCompontents(); i++){
        rsum+=medium_->GetAtomInMolecule().at(i)* medium_->GetNucCharge().at(i);

        if(rsum>rand){

            if(do_dndx_Interpolation_)
            {
                SetIntegralLimits(0);
                if(vUp_==vMax_){
                    return particle_->GetEnergy()*vUp_;
                }
                return particle_->GetEnergy()*(vUp_*exp(dndx_interpolant_2d_->FindLimit(particle_->GetEnergy(), rnd*sum_of_rates_)*log(vMax_/vUp_)));
            }
            else
            {
                return particle_->GetEnergy()*integral_->GetUpperLimit();
            }
        }
    }

    cerr<<"Error (in IonizStochastic/e): m.totZ was not initialized correctly"<<endl;
        return 0;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::CalculateStochasticLoss(double rnd1, double rnd2)
{
    double rand, rsum;

    double sum = this->CalculatedNdx(rnd1);

    rand=medium_->GetSumCharge()*rnd2;
    rsum=0;

    for(int i=0; i<medium_->GetNumCompontents(); i++){
        rsum+=medium_->GetAtomInMolecule().at(i)* medium_->GetNucCharge().at(i);

        if(rsum>rand){

            if(do_dndx_Interpolation_)
            {
                SetIntegralLimits(0);
                if(vUp_==vMax_){
                    return particle_->GetEnergy()*vUp_;
                }
                return particle_->GetEnergy()*(vUp_*exp(dndx_interpolant_2d_->FindLimit(particle_->GetEnergy(), rnd1*sum)*log(vMax_/vUp_)));
            }
            else
            {
                return particle_->GetEnergy()*integral_->GetUpperLimit();
            }
        }
    }

    cerr<<"Error (in IonizStochastic/e): m.totZ was not initialized correctly"<<endl;
        return 0;
}



//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//-----------------------Enable and disable interpolation---------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


void Ionization::EnableDNdxInterpolation()
{
    double energy = particle_->GetEnergy();

    dndx_interpolant_2d_ = new Interpolant(NUM1, particle_->GetLow(), BIGENERGY, NUM1, 0, 1, boost::bind(&Ionization::FunctionToBuildDNdxInterpolant2D, this, _1, _2),order_of_interpolation_, false, false, true, order_of_interpolation_, false, false, false, order_of_interpolation_, true, false, false);
    dndx_interpolant_1d_ = new Interpolant(NUM1, particle_->GetLow(), BIGENERGY, boost::bind(&Ionization::FunctionToBuildDNdxInterpolant, this, _1),order_of_interpolation_, false, false, true, order_of_interpolation_, true, false, false);

    do_dndx_Interpolation_=true;
    particle_->SetEnergy(energy);
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


void Ionization::EnableDEdxInterpolation()
{
    double energy = particle_->GetEnergy();
    dedx_interpolant_ = new Interpolant(NUM1, particle_->GetLow(), BIGENERGY, boost::bind(&Ionization::FunctionToBuildDEdxInterpolant, this, _1), order_of_interpolation_, true, false, true, order_of_interpolation_, false, false, true);
    do_dedx_Interpolation_=true;
    particle_->SetEnergy(energy);
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


void Ionization::DisableDNdxInterpolation()
{
    do_dndx_Interpolation_  =   false;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


void Ionization::DisableDEdxInterpolation()
{
    do_dedx_Interpolation_  =   false;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//--------------------------Set and validate options--------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


boost::program_options::options_description Ionization::CreateOptions()
{
    po::options_description ionization("Ionization options");
    ionization.add_options()
        ("ionization.interpol_dedx",    po::value<bool>(&do_dedx_Interpolation_)->implicit_value(false),  "Enables interpolation for dEdx")
        ("ionization.interpol_dndx",    po::value<bool>(&do_dndx_Interpolation_)->implicit_value(false),  "Enables interpolation for dNdx")
        ("ionization.multiplier",       po::value<double>(&multiplier_)->default_value(1.),               "modify the cross section by this factor")
        ("ionization.interpol_order",   po::value<int>(&order_of_interpolation_)->default_value(5),       "number of interpolation points");

   return ionization;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


void Ionization::ValidateOptions()
{
    if(order_of_interpolation_ < 2)
    {
        order_of_interpolation_ = 5;
        cerr<<"Ionization: Order of Interpolation is not a vaild number\t"<<"Set to 5"<<endl;
    }
    if(order_of_interpolation_ > 6)
    {
        cerr<<"Ionization: Order of Interpolation is set to "<<order_of_interpolation_
            <<".\t Note a order of interpolation > 6 will slow down the program"<<endl;
    }
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//--------------------------------constructors--------------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


Ionization::Ionization()
    :beta_                  ( 0 )
    ,gamma_                 ( 0 )
    ,integral_              (  )
{
    name_                  = "Ionization";

    dedx_interpolant_      = NULL;
    dndx_interpolant_1d_   = NULL;
    dndx_interpolant_2d_   = NULL;
    integral_              = new Integral();
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


Ionization::Ionization(const Ionization &ioniz)
    :CrossSections          ( ioniz  )
    ,beta_                  ( ioniz.beta_ )
    ,gamma_                 ( ioniz.gamma_ )
    ,integral_              ( new Integral(*ioniz.integral_) )
{
    if(ioniz.dedx_interpolant_ != NULL)
    {
        dedx_interpolant_ = new Interpolant(*ioniz.dedx_interpolant_) ;
    }
    else
    {
        dedx_interpolant_ = NULL;
    }

    if(ioniz.dndx_interpolant_1d_ != NULL)
    {
        dndx_interpolant_1d_ = new Interpolant(*ioniz.dndx_interpolant_1d_) ;
    }
    else
    {
        dndx_interpolant_1d_ = NULL;
    }

    if(ioniz.dndx_interpolant_2d_ != NULL)
    {
        dndx_interpolant_2d_ = new Interpolant(*ioniz.dndx_interpolant_2d_) ;
    }
    else
    {
        dndx_interpolant_2d_ = NULL;
    }
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


Ionization::Ionization(Particle* particle,
                             Medium* medium,
                             EnergyCutSettings* cut_settings)
    :CrossSections(particle, medium, cut_settings)
    ,beta_  ( 0 )
    ,gamma_ ( 0 )
{
    name_                       = "Ionization";
    vMax_                       = 0;
    vUp_                        = 0;
    vMin_                       = 0;
    ebig_                       = BIGENERGY;
    do_dedx_Interpolation_      = false;
    do_dndx_Interpolation_      = false;
    multiplier_                 = 1.;


    integral_              = new Integral(IROMB, IMAXS, IPREC);
    dedx_interpolant_      = NULL;
    dndx_interpolant_1d_   = NULL;
    dndx_interpolant_2d_   = NULL;

}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//-------------------------operators and swap function------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


Ionization& Ionization::operator=(const Ionization &ioniz)
{
    if (this != &ioniz)
    {
      Ionization tmp(ioniz);
      swap(tmp);
    }
    return *this;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


bool Ionization::operator==(const Ionization &ioniz) const
{
    if( this->CrossSections::operator !=(ioniz) )               return false;

    if( beta_                   !=  ioniz.beta_)                return false;
    if( gamma_                  !=  ioniz.gamma_)               return false;
    if( *integral_              != *ioniz.integral_)            return false;

    if( dedx_interpolant_ != NULL && ioniz.dedx_interpolant_ != NULL)
    {
        if( *dedx_interpolant_          != *ioniz.dedx_interpolant_)        return false;
    }
    else if( dedx_interpolant_ != ioniz.dedx_interpolant_)                  return false;

    if( dndx_interpolant_1d_ != NULL && ioniz.dndx_interpolant_1d_ != NULL)
    {
        if( *dndx_interpolant_1d_   != *ioniz.dndx_interpolant_1d_)         return false;
    }
    else if( dndx_interpolant_1d_ != ioniz.dndx_interpolant_1d_)            return false;

    if( dndx_interpolant_2d_ != NULL && ioniz.dndx_interpolant_2d_ != NULL)
    {
        if( *dndx_interpolant_2d_   != *ioniz.dndx_interpolant_2d_)         return false;
    }
    else if( dndx_interpolant_2d_ != ioniz.dndx_interpolant_2d_)            return false;


    //else
    return true;

}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


bool Ionization::operator!=(const Ionization &ioniz) const
{
    return !(*this == ioniz);
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


void Ionization::swap(Ionization &ioniz)
{
    using std::swap;

    this->CrossSections::swap(ioniz);

    swap( beta_ , ioniz.beta_);
    swap( gamma_, ioniz.gamma_);
    integral_->swap( *ioniz.integral_);

    if( dedx_interpolant_ != NULL && ioniz.dedx_interpolant_ != NULL)
    {
        dedx_interpolant_->swap(*ioniz.dedx_interpolant_);
    }
    else if( dedx_interpolant_ == NULL && ioniz.dedx_interpolant_ != NULL)
    {
        dedx_interpolant_ = new Interpolant(*ioniz.dedx_interpolant_);
        ioniz.dedx_interpolant_ = NULL;
    }
    else if( dedx_interpolant_ != NULL && ioniz.dedx_interpolant_ == NULL)
    {
        ioniz.dedx_interpolant_ = new Interpolant(*dedx_interpolant_);
        dedx_interpolant_ = NULL;
    }


    if( dndx_interpolant_1d_ != NULL && ioniz.dndx_interpolant_1d_ != NULL)
    {
        dndx_interpolant_1d_->swap(*ioniz.dndx_interpolant_1d_);
    }
    else if( dndx_interpolant_1d_ == NULL && ioniz.dndx_interpolant_1d_ != NULL)
    {
        dndx_interpolant_1d_ = new Interpolant(*ioniz.dndx_interpolant_1d_);
        ioniz.dndx_interpolant_1d_ = NULL;
    }
    else if( dndx_interpolant_1d_ != NULL && ioniz.dndx_interpolant_1d_ == NULL)
    {
        ioniz.dndx_interpolant_1d_ = new Interpolant(*dndx_interpolant_1d_);
        dndx_interpolant_1d_ = NULL;
    }


    if( dndx_interpolant_2d_ != NULL && ioniz.dndx_interpolant_2d_ != NULL)
    {
        dndx_interpolant_2d_->swap(*ioniz.dndx_interpolant_2d_);
    }
    else if( dndx_interpolant_2d_ == NULL && ioniz.dndx_interpolant_2d_ != NULL)
    {
        dndx_interpolant_2d_ = new Interpolant(*ioniz.dndx_interpolant_2d_);
        ioniz.dndx_interpolant_2d_ = NULL;
    }
    else if( dndx_interpolant_2d_ != NULL && ioniz.dndx_interpolant_2d_ == NULL)
    {
        ioniz.dndx_interpolant_2d_ = new Interpolant(*dndx_interpolant_2d_);
        dndx_interpolant_2d_ = NULL;
    }


}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//----------------------Cross section / limit / private-----------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


void Ionization::SetIntegralLimits(int component)
{
    double aux;

    beta_    =   particle_->GetMomentum()/particle_->GetEnergy();
    gamma_   =   particle_->GetEnergy()/particle_->GetMass();
    vMin_    =   (1.e-6*medium_->GetI())/particle_->GetEnergy();
    aux      =   ME/particle_->GetMass();
    vMax_    =   2*ME*(gamma_*gamma_-1)/((1 + 2*gamma_*aux + aux*aux)*particle_->GetEnergy());
    vMax_    =   min(vMax_, 1. - particle_->GetMass()/particle_->GetEnergy());

    if(vMax_<vMin_)
    {
        vMax_    =   vMin_;
    }

    vUp_ =   min(vMax_, cut_settings_->GetCut(particle_->GetEnergy()));

    if(vUp_<vMin_)
    {
        vUp_    =   vMin_;
    }
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::Delta()
{
    double X;

    X   =   log(beta_*gamma_)/log(10);

    if( X < medium_->GetX0())
    {
        return medium_->GetD0()*pow(10 , 2*(X - medium_->GetX0()));
    }
    else if(X < medium_->GetX1())
    {
        return medium_->GetC1() * X + medium_->GetC()
                + medium_->GetA() * pow(medium_->GetX1() - X , medium_->GetM());
    }
    else
    {
        return medium_->GetC1()*X + medium_->GetC();
    }
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::D2Ndvdx(double v)
{
    double result, aux, aux2;

    aux     =   beta_*beta_;
    aux2    =   v/(1 + 1/gamma_);
    aux2    *=  0.5*aux2;
    result  =   1 - aux*(v/vMax_) + aux2;
    result  *=  IONK*particle_->GetCharge()*particle_->GetCharge()*medium_->GetZA()/(2*aux*particle_->GetEnergy()*v*v);

    return medium_->GetMassDensity()*result;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::InelCorrection(double v)
{
    double result, a, b, c;

    a       =   log(1 + 2*v*particle_->GetEnergy()/ME);
    b       =   log((1 - v/vMax_)/(1 - v));
    c       =   log((2*gamma_*(1 - v)*ME)/(particle_->GetMass()*v));
    result  =   a*(2*b + c) - b*b;

    return (ALPHA/(2*PI))*result;
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//-------------------------Functions to interpolate---------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::FunctionToBuildDEdxInterpolant(double energy)
{
    particle_->SetEnergy(energy);
    return CalculatedEdx();
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::FunctionToBuildDNdxInterpolant(double energy)
{
    return dndx_interpolant_2d_->Interpolate(energy, 1.0);
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::FunctionToBuildDNdxInterpolant2D(double energy , double v)
{
    particle_->SetEnergy(energy);
    SetIntegralLimits(0);

    if(vUp_==vMax_){
        return 0;
    }
    v=vUp_*exp(v*log(vMax_/vUp_));

    return integral_->Integrate(vUp_, v,boost::bind(&Ionization::FunctionToDNdxIntegral, this, _1),3,1);
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//--------------------------Functions to integrate----------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::FunctionToDEdxIntegral(double variable)
{
    return variable*D2Ndvdx(variable)*InelCorrection(variable);
}


//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


double Ionization::FunctionToDNdxIntegral(double variable)
{
    return multiplier_ * D2Ndvdx(variable) * (1+InelCorrection(variable));
}



//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//---------------------------------Setter-------------------------------------//
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//

void Ionization::SetParametrization(int parametrization){
    parametrization_ = parametrization;
    cerr<<"Warning: This has no effect. Till now only one parametrization for Ionization implemented"<<endl;
}

void Ionization::SetBeta(double beta) {
    beta_ = beta;
}

void Ionization::SetDedxInterpolant(Interpolant* dedxInterpolant) {
    dedx_interpolant_ = dedxInterpolant;
}

void Ionization::SetDndxInterpolant1d(Interpolant* dndxInterpolant1d) {
    dndx_interpolant_1d_ = dndxInterpolant1d;
}

void Ionization::SetDndxInterpolant2d(Interpolant* dndxInterpolant2d) {
    dndx_interpolant_2d_ = dndxInterpolant2d;
}

void Ionization::SetGamma(double gamma) {
    gamma_ = gamma;
}

void Ionization::SetIntegral(Integral* integral) {
    integral_ = integral;
}
