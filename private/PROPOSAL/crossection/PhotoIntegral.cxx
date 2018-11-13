
#include <boost/bind.hpp>

#include "PROPOSAL/crossection/PhotoIntegral.h"
#include "PROPOSAL/crossection/parametrization/Parametrization.h"
#include "PROPOSAL/Constants.h"

using namespace PROPOSAL;

PhotoIntegral::PhotoIntegral(const Parametrization& param): CrossSectionIntegral(param)
{
}

PhotoIntegral::PhotoIntegral(const PhotoIntegral& brems): CrossSectionIntegral(brems)
{
}

PhotoIntegral::~PhotoIntegral()
{
}

// ----------------------------------------------------------------- //
// Public methods
// ----------------------------------------------------------------- //

double PhotoIntegral::CalculatedEdx(double energy)
{
    if (parametrization_->GetMultiplier() <= 0)
    {
        return 0;
    }

    double sum = 0;

    for(unsigned int i=0; i < components_.size(); i++)
    {
        parametrization_->SetCurrentComponent(i);
        Parametrization::IntegralLimits limits = parametrization_->GetIntegralLimits(energy);

        sum +=  dedx_integral_.Integrate(limits.vMin, limits.vUp, boost::bind(&Parametrization::FunctionToDEdxIntegral, parametrization_, energy, _1),4);
    }

    return parametrization_->GetMultiplier()*energy*sum;
}
