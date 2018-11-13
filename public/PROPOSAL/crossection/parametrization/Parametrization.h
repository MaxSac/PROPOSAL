
#pragma once

#include "PROPOSAL/particle/PROPOSALParticle.h"
#include "PROPOSAL/particle/ParticleDef.h"
#include "PROPOSAL/medium/Medium.h"
#include "PROPOSAL/EnergyCutSettings.h"

namespace PROPOSAL
{

class Parametrization
{
    public:
        Parametrization(const ParticleDef&, const Medium&, const EnergyCutSettings&, double multiplier);
        Parametrization(const Parametrization&);
        virtual ~Parametrization();

        virtual Parametrization* clone() const = 0;

        //bounds of integration
        struct IntegralLimits
        {
            double vMax; //!< upper bound of integration
            double vUp;  //!< lower bound of integration
            double vMin; //!< lowest physical possible bound of integration
        };

        // ----------------------------------------------------------------- //
        // Public methods
        // ----------------------------------------------------------------- //

        virtual double DifferentialCrossSection(double energy, double v) = 0;

        virtual double FunctionToDEdxIntegral(double energy, double v);
        virtual double FunctionToDE2dxIntegral(double energy, double v);
        virtual double FunctionToDNdxIntegral(double energy, double v);

        virtual IntegralLimits GetIntegralLimits(double energy) = 0;

        // ----------------------------------------------------------------- //
        // Getter
        // ----------------------------------------------------------------- //

        const ParticleDef& GetParticleDef() const { return particle_def_; }
        const Medium& GetMedium() const { return *medium_; }
        const EnergyCutSettings& GetEnergyCuts() const { return cut_settings_; }
        double GetMultiplier() const { return multiplier_; }

        // ----------------------------------------------------------------- //
        // Setter
        // ----------------------------------------------------------------- //

        // void SetCurrentComponent(Components::Component* component) {current_component_ = component;}
        void SetCurrentComponent(int index) {component_index_ = index;}

    protected:
        typedef std::vector<Components::Component*> ComponentVec;

        const ParticleDef particle_def_;
        const Medium* medium_;
        const EnergyCutSettings cut_settings_;

        // const Components::Component* current_component_;
        const ComponentVec& components_;
        int component_index_;

        double multiplier_;
};

} /* PROPOSAL */
