/*! \file   ContinuousRandomization.h
*   \brief  Headerfile for the randomization of continous energy losses.
*
*   For more details see the class documentation.
*
*   \date   2013.05.28
*   \author Jan-Hednrik Köhne
*/

#ifndef CONTINUOUSRANDOMIZATION_H_
#define CONTINUOUSRANDOMIZATION_H_

#include "PROPOSAL/PROPOSALParticle.h"
#include "PROPOSAL/Medium.h"
#include "PROPOSAL/CrossSections.h"
#include "PROPOSAL/StandardNormal.h"
#include "PROPOSAL/Integral.h"
#include "PROPOSAL/Interpolant.h"

/**
 * \brief Class containing the functions to randomize the continuous energy losses
 *
 */


class ContinuousRandomization
{

private:

    PROPOSALParticle*   particle_;
    PROPOSALParticle*   backup_particle_;
    Medium*     medium_;
    std::vector<CrossSections*> cross_sections_;

    bool do_dE2dx_Interpolation_; //!< Enables the interpolation of -dE2/dx
    bool do_dE2de_Interpolation_; //!< Enables the interpolation of -dE2/de

    Integral* dE2dx_integral_;
    Integral* dE2de_integral_;

    Interpolant* dE2dx_interpolant_;
    Interpolant* dE2de_interpolant_;
    Interpolant* dE2de_interpolant_diff_;

    double FunctionToDE2dxIntegral(double v);
    double FunctionToDE2deIntegral(double energy);

    double FunctionToBuildDE2dxInterplant(double energy);
    double FunctionToBuildDE2deInterplant(double energy);
    double FunctionToBuildDE2deInterplantDiff(double energy);

    double DE2dx();
    double DE2de( double ei, double ef );

    int which_cross_;
    int order_of_interpolation_;


public:

 //----------------------------------------------------------------------------//
    // Constructors

    ContinuousRandomization();
    ContinuousRandomization(PROPOSALParticle* particle, Medium* medium, std::vector<CrossSections*> cross_sections);
    ContinuousRandomization(const ContinuousRandomization&);
    ContinuousRandomization& operator=(const ContinuousRandomization& continuous_randomization);
    bool operator==(const ContinuousRandomization &continuous_randomization) const;
    bool operator!=(const ContinuousRandomization &continuous_randomization) const;
    friend std::ostream& operator<<(std::ostream& os, ContinuousRandomization const &continuous_randomization);
//----------------------------------------------------------------------------//

    //Memberfunction

    double Randomize(double initial_energy, double final_energy, double rnd);

//----------------------------------------------------------------------------//

    void swap(ContinuousRandomization &continuous_randomization);

//----------------------------------------------------------------------------//

    void EnableDE2dxInterpolation(std::string path ="", bool raw=false);

//----------------------------------------------------------------------------//

    void EnableDE2deInterpolation(std::string path ="", bool raw=false);

//----------------------------------------------------------------------------//

    void DisableDE2dxInterpolation();

//----------------------------------------------------------------------------//

    void DisableDE2deInterpolation();

//----------------------------------------------------------------------------//
    //Getter

    PROPOSALParticle* GetParticle() const
    {
        return particle_;
    }

//----------------------------------------------------------------------------//
    Medium* GetMedium() const
    {
        return medium_;
    }

//----------------------------------------------------------------------------//

    std::vector<CrossSections*> GetCrosssections() const {
        return cross_sections_;
    }

//----------------------------------------------------------------------------//
    //Setter

    void SetParticle(PROPOSALParticle *particle);

//----------------------------------------------------------------------------//

    void SetMedium(Medium *medium);

//----------------------------------------------------------------------------//

    void SetCrosssections(std::vector<CrossSections*> crosssections);

    PROPOSALParticle *GetBackup_particle() const;
    void SetBackup_particle(PROPOSALParticle *backup_particle);
    void RestoreBackup_particle();

};

#endif /* CONTINUOUSRANDOMIZATION_H_ */
