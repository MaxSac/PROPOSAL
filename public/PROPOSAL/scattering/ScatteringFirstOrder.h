/*! \file   Scattering.h
*   \brief  Header file for the Scattering routines.
*
*   For more details see the class documentation.
*
*   \date   2013.06.13
*   \author Tomasz Fuchs
*/
#pragma once

// #include <vector>
// #include <string>

#include "PROPOSAL/scattering/Scattering.h"
#include "PROPOSAL/particle/PROPOSALParticle.h"
#include "PROPOSAL/medium/Medium.h"

namespace PROPOSAL{

/**
  * \brief This class provides the scattering routine provided by moliere.
  *
  * More precise scattering angles will be added soon.
  */
class ScatteringFirstOrder : public Scattering
{
    public:
    ScatteringFirstOrder();
    ScatteringFirstOrder(const ScatteringFirstOrder&);
    ~ScatteringFirstOrder();

    virtual Scattering* clone() const { return new ScatteringFirstOrder(*this); }
    static Scattering* create() { return new ScatteringFirstOrder(); }

    private:
    ScatteringFirstOrder& operator=(const ScatteringFirstOrder&); // Undefined & not allowed

    RandomAngles CalculateRandomAngle(const PROPOSALParticle&, const Medium&, double dr, double disp);
    double CalculateTheta0(const PROPOSALParticle&, const Medium&, double dr);
};

}
