
#pragma once

#include "PROPOSAL/crossection/parametrization/Parametrization.h"

namespace PROPOSAL {
class Ionization : public Parametrization
{
    public:
    Ionization(const ParticleDef&, const Medium&, const EnergyCutSettings&, double multiplier);
    Ionization(const Ionization&);
    virtual ~Ionization();

    virtual Parametrization* clone() const { return new Ionization(*this); }

    // ----------------------------------------------------------------- //
    // Public methods
    // ----------------------------------------------------------------- //

    double DifferentialCrossSection(double energy, double v);

    double FunctionToDEdxIntegral(double energy, double v);
    double FunctionToDNdxIntegral(double energy, double v);

    IntegralLimits GetIntegralLimits(double energy);

    double InelCorrection(double energy, double v);

    // --------------------------------------------------------------------- //
    // Getter
    // --------------------------------------------------------------------- //

    const std::string& GetName() const { return name_; }

    private:
    static const std::string name_;
};


} /* PROPOSAL */
