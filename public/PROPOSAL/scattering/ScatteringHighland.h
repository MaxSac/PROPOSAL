/*! \file   Scattering.h
*   \brief  Header file for the Scattering routines.
*
*   For more details see the class documentation.
*
*   \date   2013.06.13
*   \author Tomasz Fuchs
*/
#pragma once


#include "PROPOSAL/scattering/Scattering.h"

namespace PROPOSAL{

// class PROPSALParticle;
// class Medium;

/**
  * \brief This class provides the scattering routine provided by moliere.
  *
  * More precise scattering angles will be added soon.
  */
class ScatteringHighland : public Scattering
{
    public:
    ScatteringHighland(Particle&, const Medium&);
    ScatteringHighland(Particle&, const ScatteringHighland&);
    ScatteringHighland(const ScatteringHighland&);
    ~ScatteringHighland();

    virtual Scattering* clone() const { return new ScatteringHighland(*this); }
    virtual Scattering* clone(Particle& particle) const { return new ScatteringHighland(particle, *this); }
    static Scattering* create(Particle& particle, const Medium& medium) { return new ScatteringHighland(particle, medium); }

    private:
    ScatteringHighland& operator=(const ScatteringHighland&); // Undefined & not allowed

    bool compare(const Scattering&) const;

    RandomAngles CalculateRandomAngle(double dr, double ei, double ef);
    double CalculateTheta0(double dr);

    const Medium* medium_;
};

}
