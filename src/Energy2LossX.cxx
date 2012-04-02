/*
 * Energy2LossX.cxx
 *
 *  Created on: 24.06.2010
 *      Author: koehne
 */

#include "Energy2LossX.h"


#include "Ionizationloss.h"
#include "IonizContinuous.h"
#include "IonizStochastic.h"


#include "EpairStochastic.h"
#include "EpairContinuous.h"


#include "BremsContinuous.h"
#include "BremsStochastic.h"

#include "PhotoStochastic.h"
#include "PhotoContinuous.h"

#include "Decay.h"

#include "Medium.h"

#include "algorithm"



using namespace std;


        Energy2LossX::Energy2LossX(Energy2Loss e2loss): Energy2Loss(e2loss)
        {
            jt_ = false;
            integral_ = new Integral(IROMB, IMAXS, IPREC);
        }

        //----------------------------------------------------------------------------------------------------//

        double Energy2LossX::function(double v)
        {
//            int i; // ?????
//            double result; // ?????
            switch(name)
            {
                case 0:	return v*v*cros->get_ionization()->get_Stochastic()->function(v);
                case 1: return v*v*cros->get_bremsstrahlung()->get_Stochastic()->function(v);
                case 2: return v*v*cros->get_photonuclear()->get_Stochastic()->function(v);
                case 3: return v*v*cros->get_epairproduction()->stochastic_->function(v);
                default: return 0;
            }
        }

        //----------------------------------------------------------------------------------------------------//

        double Energy2LossX::dE2dx()
        {
            if(jt_)
            {
                return max(interpolateJ_->interpolate(particle_->e), 0.0);
            }

            double sum;//, aux;
            cros->get_ionization()->setEnergy();

            name=0;
            sum=integral_->integrateOpened(cros->get_ionization()->get_vMin(), cros->get_ionization()->get_vUp(), this);

            for(int i=0; i<medium_->get_numCompontents(); i++)
            {
                cros->get_bremsstrahlung()->setEnergy(i);
                name=1;
                sum+=integral_->integrateOpened(0, cros->get_bremsstrahlung()->get_vUp(), this);
            }

            for(int i=0; i<medium_->get_numCompontents(); i++)
            {
                cros->get_photonuclear()->setEnergy(i); name=2;
                sum+=integral_->integrateOpened(cros->get_photonuclear()->get_vMin(), cros->get_photonuclear()->get_vUp(), this);
            }

            for(int i=0; i<medium_->get_numCompontents(); i++)
            {
                cros->get_epairproduction()->setEnergy(i);
                name=3;
                sum+=integral_->integrateOpened(cros->get_epairproduction()->get_vMin(), cros->get_epairproduction()->get_vUp(), this);
            }

            return particle_->e*particle_->e*sum;
        }

        //----------------------------------------------------------------------------------------------------//

        /**
         * 1d parametrization - interface to Interpolate
         */

        double Energy2LossX::functionInt(double e)
        {
            particle_->setEnergy(e);
            return dE2dx();
        }


      // Setter

        void Energy2LossX::set_jt(bool newJT){ jt_ = newJT;}

