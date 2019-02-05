
#include <cmath>

#include "PROPOSAL/crossection/parametrization/MupairProduction.h"

#include "PROPOSAL/medium/Components.h"
#include "PROPOSAL/medium/Medium.h"

#include "PROPOSAL/Constants.h"
#include "PROPOSAL/Output.h"

#define MUPAIR_PARAM_INTEGRAL_IMPL(param)                                                                               \
    Mupair##param::Mupair##param(const ParticleDef& particle_def,                                                        \
                               const Medium& medium,                                                                   \
                               const EnergyCutSettings& cuts,                                                          \
                               double multiplier,                                                                      \
                               bool mupair_enable)                                                                               \
        : MupairProductionRhoIntegral(particle_def, medium, cuts, multiplier, mupair_enable)                                      \
    {                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    Mupair##param::Mupair##param(const Mupair##param& photo)                                                              \
        : MupairProductionRhoIntegral(photo)                                                                            \
    {                                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    Mupair##param::~Mupair##param() {}                                                                                   \
                                                                                                                       \
    const std::string Mupair##param::name_ = "Mupair" #param;

using namespace PROPOSAL;

/******************************************************************************
 *                              MupairProduction                               *
 ******************************************************************************/

// ------------------------------------------------------------------------- //
// Constructor & Destructor
// ------------------------------------------------------------------------- //

MupairProduction::MupairProduction(const ParticleDef& particle_def,
                                 const Medium& medium,
                                 const EnergyCutSettings& cuts,
                                 double multiplier,
                                 bool mupair_enable)
    : Parametrization(particle_def, medium, cuts, multiplier)
    , mupair_enable_(mupair_enable)
{
}

MupairProduction::MupairProduction(const MupairProduction& mupair)
    : Parametrization(mupair)
    , mupair_enable_(mupair.mupair_enable_)
{
}

MupairProduction::~MupairProduction() {}

bool MupairProduction::compare(const Parametrization& parametrization) const
{
    const MupairProduction* pairproduction = static_cast<const MupairProduction*>(&parametrization);

    if (mupair_enable_ != pairproduction->mupair_enable_)
        return false;
    else
        return Parametrization::compare(parametrization);
}

// ------------------------------------------------------------------------- //
// Public methods
// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //
Parametrization::IntegralLimits MupairProduction::GetIntegralLimits(double energy)
{
    IntegralLimits limits;

    //TODO: Check Integral Limits (where do we need MMU, where do we need the particle_def_.mass ?)
    limits.vMin = 2 * MMU / energy;
    limits.vMax = 1 - particle_def_.mass / energy;

    if (limits.vMax < limits.vMin)
    {
        limits.vMax = limits.vMin;
    }

    limits.vUp = std::min(limits.vMax, cut_settings_.GetCut(energy));

    if (limits.vUp < limits.vMin)
    {
        limits.vUp = limits.vMin;
    }

    return limits;
}


/******************************************************************************
 *                          MupairProduction Integral                           *
 ******************************************************************************/

// ------------------------------------------------------------------------- //
MupairProductionRhoIntegral::MupairProductionRhoIntegral(const ParticleDef& particle_def,
                                                       const Medium& medium,
                                                       const EnergyCutSettings& cuts,
                                                       double multiplier,
                                                       bool mupair_enable)
    : MupairProduction(particle_def, medium, cuts, multiplier, mupair_enable)
    , integral_(IROMB, IMAXS, IPREC)
{
}

// ------------------------------------------------------------------------- //
MupairProductionRhoIntegral::MupairProductionRhoIntegral(const MupairProductionRhoIntegral& mupair)
    : MupairProduction(mupair)
    , integral_(mupair.integral_)
{
}

// ------------------------------------------------------------------------- //
MupairProductionRhoIntegral::~MupairProductionRhoIntegral() {}

// ------------------------------------------------------------------------- //
bool MupairProductionRhoIntegral::compare(const Parametrization& parametrization) const
{
    const MupairProductionRhoIntegral* mupair = static_cast<const MupairProductionRhoIntegral*>(&parametrization);

    if (integral_ != mupair->integral_)
        return false;
    else
        return MupairProduction::compare(parametrization);
}

// ------------------------------------------------------------------------- //
double MupairProductionRhoIntegral::DifferentialCrossSection(double energy, double v)
{
    double rMax, aux;

    //TODO: Check Integral Limits (where do we need MMU, where do we need the particle_def_.mass ?)
    aux  = 1 - 2 * MMU / (v * energy);

    if (aux > 0)
    {
        rMax = aux;
    } else
    {
        rMax = 0;
    }

    aux = std::max(1 - rMax, COMPUTER_PRECISION);

    return multiplier_ * medium_->GetMolDensity() * components_[component_index_]->GetAtomInMolecule() *
           particle_def_.charge * particle_def_.charge *
           (integral_.Integrate(
                1 - rMax, aux, std::bind(&MupairProductionRhoIntegral::FunctionToIntegral, this, energy, v, std::placeholders::_1), 2) +
            integral_.Integrate(
                aux, 1, std::bind(&MupairProductionRhoIntegral::FunctionToIntegral, this, energy, v, std::placeholders::_1), 4));
}

// ------------------------------------------------------------------------- //
void MupairProductionRhoIntegral::print(std::ostream& os) const
{
    os << "mupair production enabled: " << mupair_enable_ << '\n';
}

// ------------------------------------------------------------------------- //
size_t MupairProductionRhoIntegral::GetHash() const
{
    size_t seed = Parametrization::GetHash();
    hash_combine(seed, mupair_enable_);

    return seed;
}

/******************************************************************************
 *                          Specifc Parametrizations                           *
 ******************************************************************************/

// ------------------------------------------------------------------------- //
// Define the specific parametrizations
// ------------------------------------------------------------------------- //

MUPAIR_PARAM_INTEGRAL_IMPL(KelnerKokoulinPetrukhin)

// ------------------------------------------------------------------------- //
double MupairKelnerKokoulinPetrukhin::FunctionToIntegral(double energy, double v, double r)
{
    // Parametrization of Kelner/Kokoulin/Petrukhin
    // Physics of Atomic Nuclei, Vol. 63, No. 9, 2000, pp. 1603-1611. Translated from Yadernaya Fizika, Vol. 63, 2000, pp. 1690-1698
    // Original Russian Text Copyright 2000 by Kel'ner, Kokoulin, Petukhin
    // DOI: 10.1134/1.1312894


    double aux, aux1, aux2, r2, rMax, Z3, xi, beta, A_pow, r_mu;
    double phi, U, U_max, X, Y;
    double medium_charge       = components_[component_index_]->GetNucCharge();
    double atomic_weight       = components_[component_index_]->GetAtomInMolecule();
    //double medium_log_constant = components_[component_index_]->GetLogConstant();
    double medium_log_constant = 183; // According to the paper, B is set to 183

    r           = 1 - r; // only for integral optimization - do not forget to swap integration limits!
    r2          = r * r;
    rMax        = 2 * MMU / (v * energy); // TODO: Is this correct with the swap of the integration limits AND with the MMU?
    Z3          = std::pow(medium_charge, -1. / 3);
    aux         = (particle_def_.mass * v) / (2 * MMU);
    xi          = aux * aux * (1 - r2) / (1 - v);
    beta        = (v * v) / (2 * (1 - v));
    A_pow       = std::pow(atomic_weight, -0.27);
    r_mu        = RE * ME / MMU; //classical muon radius

    //Phi Calculation (18)
    aux     = (2 + r2) * (1 + beta) + xi * (3 + r2);
    aux     *= std::log(1 + 1. / xi);

    aux1    = (1 + r2) * (1 + 1.5 * beta) - 1. / xi * (1 + 2 * beta) * (1 - r2);
    aux1    *= std::log(1 + xi);
    aux2    = -1 - 3 * r2 + beta * (1 - 2 * r2);

    phi     = aux + aux1 + aux2; 

    //X Calculation (22)
    Y       = 12 * std::sqrt(MMU / energy); //(21)
    aux     = 0.65 * A_pow * medium_log_constant * Z3 * MMU / ME;
    aux1    = 2 * SQRTE * std::pow(MMU, 2) * medium_log_constant * Z3 * (1 + xi) * (1 + Y);
    aux2    = ME * energy * v * (1 - r2);

    U       = aux / (1 + aux1 / aux2);

    aux2    = ME * energy * v * (1 - rMax * rMax);
    U_max   = aux / (1 + aux1 / aux2);

    X       = 1 + U - U_max;

    //Combine results
    aux     = ALPHA * r_mu * particle_def_.charge * medium_charge;
    aux     *= 2 * aux * phi * (1 - v) / (1.5 * PI * v); //Factor 2: Similar to factor 2 from EPairProduction, probably from symmetry in Rho

    if (X > 0)
    {
        aux *= std::log(X);
    }
    else
    {
        aux = 0;
    }

    if (aux < 0)
    {
        aux = 0;
    }

    return aux;
}


#undef MUPAIR_PARAM_INTEGRAL_IMPL
