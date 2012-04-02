/*
 * BremsStochastic.h
 *
 *  Created on: 21.06.2010
 *      Author: koehne
 */

#ifndef BremsStochastic_H
#define BremsStochastic_H

#include "Bremsstrahlung.h"
#include "Integral.h"
#include "Interpolate.h"
#include <vector>
#include <algorithm>
#include <cmath>

/*!
   \brief class contains functions necessary for calculation of bremsstrahlung losses
 */


class BremsStochastic: public Bremsstrahlung{


	protected:

                std::vector<Integral*> VectorOfIntegral_;
                double* h_;
		double sum_;


		bool jt_;	// set to false in constructor
		double rnd_;


	public:
                Interpolate* interpolateJo_;


                Interpolate* interpolateJ_;
		// constructors

                /*!
                Create a BremsStochastic.
                */

		BremsStochastic();

                /*!
                initializes an integral class separate from that in Propagate
                */
		BremsStochastic(Bremsstrahlung *cros);


		// Memberfunctions

                //---------------------------------------------------------------------------------------------------------//

                /*!
                this is what d2N/dv,dx is equal to - interface to Integral;
                \f$return=c_b\cdot \sigma \f$
                */

                double function(double v);

                //---------------------------------------------------------------------------------------------------------//

                /*!
                this is what the sum over all components of dNdx is equal to
                \f[\frac{dN}{dX}= \sum_{numC} \int_{v_{Up}}^{v_{max}} c_b \cdot  \sigma dv\f]
                */

                double dNdx();

                //---------------------------------------------------------------------------------------------------------//

                /*!
                this is what the sum over all components of dNdx is equal to
                */

                double dNdx(double rnd);

                //---------------------------------------------------------------------------------------------------------//

                /*!
                this is the value of \f$e=v \cdot E\f$, corresponding to rnd in the call to dNdx
                for the component chosen with rnd in the argument
                */

                double e(double rnd);

                //---------------------------------------------------------------------------------------------------------//

                /*!
                 2d parametrization - interface to Interpolate;
                 if \f$v_{Up}=v_{Max}\f$: \f[return=0;\f]
                 else: \f$v=v_{Up}\exp\big(v\log\big(\frac{v_{Max}}{v_{Up}}\big)\big)\f$
                 \f[return=\int_{v_{up}}^{v} c_b \sigma_{el} dv \f]
                 */

                double functionInt(double e, double v);

                //---------------------------------------------------------------------------------------------------------//

                /*!
                1d parametrization - interface to Iterpolate
                */

                double functionInt(double e);

                //---------------------------------------------------------------------------------------------------------//

                // Getter

                std::vector<Integral*> get_integral() const {return VectorOfIntegral_;}
                double* get_h() const {return h_;}
                double get_sum() const {return sum_;}

                Interpolate* get_interpolateJ() const {return interpolateJ_;}
                bool get_jt() const {return jt_;}
                //double get_rnd() const {return rnd_;}

                Interpolate* get_interpolateJo() const {return interpolateJo_;}


                // Setter

                void set_integral(std::vector<Integral*> integral);
                void set_h(double* h);
                void set_sum(double sum);

                void set_interpolateJ(Interpolate* interpolateJ);
                void set_jt(bool newjt);
                void set_rnd(double rnd);

                void set_interpolateJo(Interpolate* interpolateJo);


		// destructors

		///@brief Crush this BremsStochastic.
		virtual ~BremsStochastic();


};



#endif //BremsStochastic_H
