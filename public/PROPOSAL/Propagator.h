/*
 * Propagator.h
 *
 *  Created on: 2013.05.06
 *      Author: Jan-Hendrik Köhne
 */
#pragma once

/*! \mainpage PROPOSAL:
 * <b>PR</b>opagator with <b>O</b>ptimal <b>P</b>recision and <b>O</b>ptimized <b>S</b>peed for <b>A</b>ll
 * <b>L</b>eptons.
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection requirements
 *
 * \section HowTo
 *
 */

// #include <utility>
#include <boost/property_tree/ptree.hpp>
#include <deque>
#include <vector>
// #include "PROPOSAL/Collection.h"
// #include "PROPOSAL/ScatteringFirstOrder.h"
// #include "PROPOSAL/ScatteringMoliere.h"
#include "PROPOSAL/Output.h"
#include "PROPOSAL/sector/SectorFactory.h"
// #include "PROPOSAL/MathModel.h"
// #include "PROPOSAL/PROPOSALParticle.h"
// #include "PROPOSAL/Geometry.h"
// #include "PROPOSAL/Scattering.h"

namespace PROPOSAL {

class Geometry;
class Sector;
class PROPOSALParticle;
class Vector3D;

class Propagator
{
    public:
    // Constructors
    Propagator();
    Propagator(const std::vector<Sector*>&, const Geometry&);
    // Propagator(const ParticleDef&, const std::vector<SectorFactory::Definition>&, const Geometry&);
    Propagator(const ParticleDef&, const std::string&);
    // Propagator(ParticleDef,
    //            std::string path_to_tables,
    //            bool exact_time = true,
    //            bool lpm = true,
    //            bool integrate = false,
    //            int scattering_model = 0);
    // Propagator(std::string config_file, bool DoApplyOptions=true);
    // Propagator(std::string config_file, PROPOSALParticle* particle, bool DoApplyOptions=true);
    // Propagator(Medium* medium,
    //            EnergyCutSettings* cuts,
    //            ParticleDef,
    //            std::string path_to_tables,
    //            bool moliere = true,
    //            bool continuous_rand = true,
    //            bool exact_time = true,
    //            bool lpm = true,
    //            ParametrizationType::Enum brems = ParametrizationType::BremsKelnerKokoulinPetrukhin,
    //            ParametrizationType::Enum photo = ParametrizationType::PhotoAbramowiczLevinLevyMaor97ShadowButkevich,
    //            double brems_multiplier = 1,
    //            double photo_multiplier = 1,
    //            double ioniz_multiplier = 1,
    //            double epair_multiplier = 1,
    //            bool integrate = false,
    //            int scattering_model = 0);
    Propagator(const Propagator&);
    Propagator& operator=(const Propagator& propagator);
    bool operator==(const Propagator& propagator) const;
    bool operator!=(const Propagator& propagator) const;

    //----------------------------------------------------------------------------//
    // Memberfunctions
    /**
     * Propagates the particle of initial energy e to the distance r.
     * Returns the final energy if the
     * particle has survived or the track length to the
     * point of disappearance with a minus sign otherwise.
     *
     *  \param  distance   maximum track length
     *  \param  energy   initial energy
     *  \return energy at distance OR -(track length)
     */

    // double Propagate(double distance);

    //----------------------------------------------------------------------------//
    /**
     * Propagates the particle through the current set of ProcessCollections
     *  \return vector of secondarys
     */

    // std::vector<PROPOSALParticle*> Propagate( PROPOSALParticle *particle, double MaxDistance_cm = 1e20 );

    //----------------------------------------------------------------------------//
    std::vector<DynamicData*> Propagate( double MaxDistance_cm = 1e20);

    //----------------------------------------------------------------------------//
    /*!
     *  Apply options which are read from configuration file to ProcessCollections
    */
    void ApplyOptions();

    //----------------------------------------------------------------------------//
    /*!
    * advances the particle by the given distance
    * Sets the x,y and z coordinates of particle_
    * and its time and propagation distance
    *
    * \param    dr  flight distance
    * \param    ei  initial energy
    * \param    ef  final energy
    */

    // void AdvanceParticle(double dr, double ei, double ef);

    //----------------------------------------------------------------------------//

    void swap(Propagator& propagator);

    //----------------------------------------------------------------------------//

    void EnableInterpolation(PROPOSALParticle&, std::string path = "", bool raw = false);

    //----------------------------------------------------------------------------//

    void DisableInterpolation();

    //----------------------------------------------------------------------------//

    // void ReadConfigFile(std::string config_file, bool DoApplyOptions=true);


    //----------------------------------------------------------------------------//
    // Getter

    Sector* GetCurrentCollection() const { return current_collection_; }
    //----------------------------------------------------------------------------//

    std::vector<Sector*> GetCollections() const { return collections_; }

    //----------------------------------------------------------------------------//
    // PROPOSALParticle* GetParticle() const
    // {
    //     return particle_;
    // }

    //----------------------------------------------------------------------------//
    // Setter

    /**
     *  Sets the ProcessCollections. Need to execute AplyOptions() afterward.
     */
    // void SetCollections(std::vector<Collection*>);
    /**
     *  Sets the particle for the Propagator and its current ProcessCollection
     */
    // void SetParticle(PROPOSALParticle* particle);

    //----------------------------------------------------------------------------//
    // Destructor
    ~Propagator();

    int GetSeed() const;
    void SetSeed(int seed);
    // ParametrizationType::Enum GetBrems() const;
    // void SetBrems(ParametrizationType::Enum brems);
    // ParametrizationType::Enum GetPhoto() const;
    // void SetPhoto(ParametrizationType::Enum photo);
    // std::string GetPath_to_tables() const;
    // void SetPath_to_tables(const std::string &path_to_tables);
    Geometry* GetDetector() const;
    PROPOSALParticle& GetParticle();
    // void SetDetector(Geometry *detector);
    // bool GetStopping_decay() const;
    // void SetStopping_decay(bool stopping_decay);

    private:

    template<class T>
    void SetMember(T& var, std::string option, boost::property_tree::ptree& pt)
    {
        try
        {
            var = pt.get<T>(option);
        }
        catch(std::exception& ex)
        {
            log_fatal("%s! Use default: %i", ex.what(), var);
        }
    }

    int seed_; //!< seed of the random number generator
    // ParametrizationType::Enum  brems_;                     //!< Bremsstrahlungs parametrization
    // ParametrizationType::Enum  photo_;                     //!< Photonuclear parametrization
    static const double global_ecut_inside_; //!< ecut for inside the detector (it's used when not specified explicit for a sector in
    //!congiguration file)
    static const double global_ecut_infront_; //!< ecut for infront of the detector (it's used when not specified explicit for a
    //!sector in congiguration file)
    static const double global_ecut_behind_; //!< ecut for behind the detector (it's used when not specified explicit for a sector in
    //!congiguration file)
    static const double global_vcut_inside_; //!< vcut for inside the detector (it's used when not specified explicit for a sector in
    //!congiguration file)
    static const double global_vcut_infront_; //!< ecut for infront of the detector (it's used when not specified explicit for a
    //!sector in congiguration file)
    static const double global_vcut_behind_; //!< ecut for behind the detector (it's used when not specified explicit for a sector in
    //!congiguration file)
    static const double global_cont_inside_; //!< continuous randominzation flag for inside the detector (it's used when not
    //!specified explicit for a sector in congiguration file)
    static const double global_cont_infront_; //!< continuous randominzation flag for infront of the detector (it's used when not
    //!specified explicit for a sector in congiguration file)
    static const double global_cont_behind_;  //!< continuous randominzation flag for behind the detector (it's used when not
                                 //!specified explicit for a sector in congiguration file)


    std::vector<Sector*> collections_;
    Sector* current_collection_;

    PROPOSALParticle& particle_;
    Geometry* detector_;

    //----------------------------------------------------------------------------//
    /**
     * Choose the current collection by particle type and location.
     */

    void ChooseCurrentCollection(Vector3D& particle_position, Vector3D& particle_direction);

    //----------------------------------------------------------------------------//
    /**
     * Calculate the distance to propagate and
     * choose if the particle has to propagate through the whole sector
     * or only to the collection border
     */

    double CalculateEffectiveDistance(Vector3D& particle_position, Vector3D& particle_direction);

    //----------------------------------------------------------------------------//

    /*!
    * Initalize a geomtry. Used when reading the values from config file
    *,
    */
    // Geometry* InitGeometry(std::deque<std::string>* token , std::string first_token);
    //----------------------------------------------------------------------------//
    /*!
    * Init ProcessCollection from configuration file. When keyword sector is found in configuration
    * file this function is called and inits ProcessCollections:
    * 3 for muons inside/behind/infront
    * 3 for taus inside/behind/infront
    * 3 for electrons inside/behind/infront
    */
    // void InitProcessCollections(std::ifstream &file);

    // void MoveParticle(double distance);
};

}
