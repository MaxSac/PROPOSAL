

#include <cmath>

#include "PROPOSAL/crossection/parametrization/PhotoRealPhotonAssumption.h"

#include "PROPOSAL/Constants.h"
#include "PROPOSAL/math/Interpolant.h"

using namespace PROPOSAL;

/******************************************************************************
*                         PhotoRealPhotonAssumption                           *
******************************************************************************/

PhotoRealPhotonAssumption::PhotoRealPhotonAssumption(const ParticleDef& particle_def,
                                                     const Medium& medium,
                                                     const EnergyCutSettings& cuts,
                                                     const RealPhoton& hardBB,
                                                     Definition param_def)
    : Photonuclear(particle_def, medium, cuts, param_def)
    , hardBB_(hardBB.clone())
{
}

PhotoRealPhotonAssumption::PhotoRealPhotonAssumption(const PhotoRealPhotonAssumption& photo)
    : Photonuclear(photo)
    , hardBB_(photo.hardBB_->clone())
{
}

PhotoRealPhotonAssumption::~PhotoRealPhotonAssumption()
{
    delete hardBB_;
}

// ------------------------------------------------------------------------- //
double PhotoRealPhotonAssumption::DifferentialCrossSection(double energy, double v)
{
    double aux, aum, k, G, t, sgn, tmp;

    double nu = v * energy * 1.e-3;

    sgn = CalculateParametrization(nu);

    const double m1 = 0.54;
    const double m2 = 1.80;

    double particle_charge = particle_def_.charge;
    double particle_mass   = particle_def_.mass;
    double atomic_number   = components_[component_index_]->GetAtomicNum();

    k = 1 - 2 / v + 2 / (v * v);

    if (components_[component_index_]->GetNucCharge() == 1)
    {
        G = 1;
    } else
    {

        tmp = 0.00282 * pow(atomic_number, 1. / 3) * sgn;
        G   = (3 / tmp) * (0.5 + ((1 + tmp) * exp(-tmp) - 1) / (tmp * tmp));
    }

    G *= 3;
    aux = v * particle_mass * 1.e-3;
    t   = aux * aux / (1 - v);
    aum = particle_mass * 1.e-3;
    aum *= aum;
    aux = 2 * aum / t;
    aux = G * ((k + 4 * aum / m1) * log(1 + m1 / t) - (k * m1) / (m1 + t) - aux) +
          ((k + 2 * aum / m2) * log(1 + m2 / t) - aux) +
          aux * ((G * (m1 - 4 * t)) / (m1 + t) + (m2 / t) * log(1 + t / m2));

    aux *= ALPHA / (8 * PI) * atomic_number * v * sgn * 1.e-30;

    aux += components_[component_index_]->GetAtomicNum() * 1.e-30 * hardBB_->CalculateHardBB(energy, v);

    return medium_->GetMolDensity() * components_[component_index_]->GetAtomInMolecule() * particle_charge *
           particle_charge * aux;
}

// ------------------------------------------------------------------------- //
double PhotoRealPhotonAssumption::NucleusCrossSectionCaldwell(double nu)
{
    return 49.2 + 11.1 * log(nu) + 151.8 / sqrt(nu);
}

/******************************************************************************
*                            Zeus Parametrization                            *
******************************************************************************/

// ------------------------------------------------------------------------- //
PhotoZeus::PhotoZeus(const ParticleDef& particle_def,
                     const Medium& medium,
                     const EnergyCutSettings& cuts,
                     const RealPhoton& hardBB,
                     Definition param_def)
    : PhotoRealPhotonAssumption(particle_def, medium, cuts, hardBB, param_def)
{
}

PhotoZeus::PhotoZeus(const PhotoZeus& photo)
    : PhotoRealPhotonAssumption(photo)
{
}

PhotoZeus::~PhotoZeus()
{
}

Parametrization* PhotoZeus::create(const ParticleDef& particle_def,
                        const Medium& medium,
                        const EnergyCutSettings& cuts,
                        const RealPhoton& hardBB,
                        Definition def)
{
    return new PhotoZeus(particle_def, medium, cuts, hardBB, def);
}

// ------------------------------------------------------------------------- //
double PhotoZeus::CalculateParametrization(double nu)
{
    double aux;

    aux = nu * 2.e-3 * this->components_[this->component_index_]->GetAverageNucleonWeight();
    aux = 63.5 * pow(aux, 0.097) + 145 * pow(aux, -0.5);

    return aux;
}

/******************************************************************************
*                      Bezrukov Bugaev Parametrization                       *
******************************************************************************/

// ------------------------------------------------------------------------- //
PhotoBezrukovBugaev::PhotoBezrukovBugaev(const ParticleDef& particle_def,
                     const Medium& medium,
                     const EnergyCutSettings& cuts,
                     const RealPhoton& hardBB,
                     Definition param_def)
    : PhotoRealPhotonAssumption(particle_def, medium, cuts, hardBB, param_def)
{
}

PhotoBezrukovBugaev::PhotoBezrukovBugaev(const PhotoBezrukovBugaev& photo)
    : PhotoRealPhotonAssumption(photo)
{
}

PhotoBezrukovBugaev::~PhotoBezrukovBugaev()
{
}

Parametrization* PhotoBezrukovBugaev::create(const ParticleDef& particle_def,
                        const Medium& medium,
                        const EnergyCutSettings& cuts,
                        const RealPhoton& hardBB,
                        Definition def)
{
    return new PhotoBezrukovBugaev(particle_def, medium, cuts, hardBB, def);
}

// ------------------------------------------------------------------------- //
double PhotoBezrukovBugaev::CalculateParametrization(double nu)
{
    double aux;

    aux = log(0.0213 * nu);
    aux = 114.3 + 1.647 * aux * aux;

    return aux;
}

/******************************************************************************
*                           Rhode Parametrization                            *
******************************************************************************/

PhotoRhode::PhotoRhode(const ParticleDef& particle_def,
                       const Medium& medium,
                       const EnergyCutSettings& cuts,
                       const RealPhoton& hardBB,
                       Definition param_def)
    : PhotoRealPhotonAssumption(particle_def, medium, cuts, hardBB, param_def)
    , interpolant_(NULL)
{
    double x_aux[] = { 0,           0.1,         0.144544,   0.20893,     0.301995,    0.436516,    0.630957,
                       0.912011,    1.31826,     1.90546,    2.75423,     3.98107,     5.7544,      8.31764,
                       12.0226,     17.378,      25.1189,    36.3078,     52.4807,     75.8577,     109.648,
                       158.489,     229.087,     331.131,    478.63,      691.831,     1000,        1445.44,
                       2089.3,      3019.95,     4365.16,    6309.58,     9120.12,     13182.6,     19054.6,
                       27542.3,     39810.8,     57544,      83176.4,     120226,      173780,      251188,
                       363078,      524807,      758576,     1.09648e+06, 1.58489e+06, 2.29086e+06, 3.3113e+06,
                       4.78628e+06, 6.91828e+06, 9.99996e+06 };

    double y_aux[] = { 0,       0.0666667, 0.0963626, 159.74,  508.103, 215.77,  236.403, 201.919, 151.381,
                       145.407, 132.096,   128.546,   125.046, 121.863, 119.16,  117.022, 115.496, 114.607,
                       114.368, 114.786,   115.864,   117.606, 120.011, 123.08,  126.815, 131.214, 136.278,
                       142.007, 148.401,   155.46,    163.185, 171.574, 180.628, 190.348, 200.732, 211.782,
                       223.497, 235.876,   248.921,   262.631, 277.006, 292.046, 307.751, 324.121, 341.157,
                       358.857, 377.222,   396.253,   415.948, 436.309, 457.334, 479.025 };

    std::vector<double> x(x_aux, x_aux + sizeof(x_aux) / sizeof(double));
    std::vector<double> y(y_aux, y_aux + sizeof(y_aux) / sizeof(double));

    interpolant_ = new Interpolant(x, y, 4, false, false);
}

PhotoRhode::PhotoRhode(const PhotoRhode& photo)
    : PhotoRealPhotonAssumption(photo)
    , interpolant_(new Interpolant(*photo.interpolant_))
{
}

PhotoRhode::~PhotoRhode()
{
    delete interpolant_;
}


Parametrization* PhotoRhode::create(const ParticleDef& particle_def,
                        const Medium& medium,
                        const EnergyCutSettings& cuts,
                        const RealPhoton& hardBB,
                        Definition def)
{
    return new PhotoRhode(particle_def, medium, cuts, hardBB, def);
}

// ------------------------------------------------------------------------- //
double PhotoRhode::MeasuredSgN(double e)
{
    return interpolant_->InterpolateArray(e);
}

// ------------------------------------------------------------------------- //
double PhotoRhode::CalculateParametrization(double nu)
{
    if (nu <= 200.)
    {
        return MeasuredSgN(nu);
    } else
    {
        return this->NucleusCrossSectionCaldwell(nu);
    }
}

/******************************************************************************
*                          Kokoulin Parametrization                           *
******************************************************************************/

// ------------------------------------------------------------------------- //
PhotoKokoulin::PhotoKokoulin(const ParticleDef& particle_def,
                             const Medium& medium,
                             const EnergyCutSettings& cuts,
                             const RealPhoton& hardBB,
                             Definition param_def)
    : PhotoBezrukovBugaev(particle_def, medium, cuts, hardBB, param_def)
{
}

PhotoKokoulin::PhotoKokoulin(const PhotoKokoulin& brems)
    : PhotoBezrukovBugaev(brems)
{
}

PhotoKokoulin::~PhotoKokoulin()
{
}

Parametrization* PhotoKokoulin::create(const ParticleDef& particle_def,
                        const Medium& medium,
                        const EnergyCutSettings& cuts,
                        const RealPhoton& hardBB,
                        Definition def)
{
    return new PhotoKokoulin(particle_def, medium, cuts, hardBB, def);
}

// ------------------------------------------------------------------------- //
double PhotoKokoulin::CalculateParametrization(double nu)
{
    if (nu <= 200.)
    {
        if (nu <= 17.)
        {
            return 96.1 + 82. / sqrt(nu);
        } else
        {
            return PhotoBezrukovBugaev::CalculateParametrization(nu);
        }
    } else
    {
        return NucleusCrossSectionCaldwell(nu);
    }
}
