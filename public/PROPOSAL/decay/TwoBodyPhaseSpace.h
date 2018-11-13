
#pragma once

#include <vector>

#include "PROPOSAL/decay/DecayChannel.h"
#include "PROPOSAL/math/RootFinder.h"

namespace PROPOSAL
{

class PROPOSALParticle;

class TwoBodyPhaseSpace : public DecayChannel
{
    public:
    TwoBodyPhaseSpace(double m1, double m2);
    TwoBodyPhaseSpace(const TwoBodyPhaseSpace& mode);
    virtual ~TwoBodyPhaseSpace();
    // No copy and assignemnt -> done by clone
    virtual TwoBodyPhaseSpace* clone() { return new TwoBodyPhaseSpace(*this); }

    DecayProducts Decay(PROPOSALParticle*);

    private:
    TwoBodyPhaseSpace& operator=(const TwoBodyPhaseSpace&); // Undefined & not allowed

    bool compare(const DecayChannel&) const;

    double first_daughter_mass_;
    double second_daughter_mass_;
};
;

} /* PROPOSAL */

