#define BOOST_PYTHON_MAX_ARITY 17

// #include <string>

#include <boost/python.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <map>

// #include "PROPOSAL/Propagator.h"
#include "PROPOSAL/PROPOSAL.h"

#define PARTICLE_DEF(cls)                                                                                              \
    class_<cls##Def, boost::shared_ptr<cls##Def>, bases<ParticleDef>, boost::noncopyable>(#cls "Def", no_init)         \
                                                                                                                       \
        .def("get", make_function(&MuMinusDef::Get, return_value_policy<reference_existing_object>()))                 \
        .staticmethod("get");

using namespace PROPOSAL;

// #include "PROPOSAL/PROPOSALParticle.h"
// #include "PROPOSAL/Medium.h"
// #include "PROPOSAL/EnergyCutSettings.h"
// #include "PROPOSAL/ProcessCollection.h"
// #include "PROPOSAL/CrossSections.h"
// #include "PROPOSAL/Photonuclear.h"
// #include "PROPOSAL/Bremsstrahlung.h"
// #include "PROPOSAL/Epairproduction.h"
// #include "PROPOSAL/Ionization.h"
// #include "PROPOSAL/Geometry.h"

// using namespace PROPOSAL;
//
// // struct CrossSectionsWrap : CrossSections, boost::python::wrapper<CrossSections>
// // {
// //     CrossSectionsWrap(): CrossSections(), wrapper<CrossSections>(){}
// //
// //     // virtual double CalculatedEdx()
// //     // {
// //     //     return this->get_override("calculate_dEdx")();
// //     // }
// // };
//
//
// #<{(|*****************************************************************************
// *                             Register functions                              *
// *****************************************************************************|)}>#
//
// // ------------------------------------------------------------------------- //
// // Vectors
// // ------------------------------------------------------------------------- //
//
//
// struct CrossSectionToPython
// {
//     static PyObject* convert(std::vector<CrossSections*> const& vec)
//     {
//         boost::python::list python_list;
//
//         for (unsigned int i = 0; i < vec.size(); ++i)
//         {
//             switch (vec[i]->GetType())
//             {
//                 case ParticleType::Brems:
//                     python_list.append(boost::python::ptr((Bremsstrahlung*)vec[i]));
//                     break;
//                 case ParticleType::NuclInt:
//                     python_list.append(boost::python::ptr((Photonuclear*)vec[i]));
//                     break;
//                 case ParticleType::DeltaE:
//                     python_list.append(boost::python::ptr((Ionization*)vec[i]));
//                     break;
//                 case ParticleType::EPair:
//                     python_list.append(boost::python::ptr((Epairproduction*)vec[i]));
//                     break;
//                 default:
//                     boost::python::object obj(NULL);
//                     break;
//             }
//         }
//
//         PyObject* py = boost::python::incref(python_list.ptr());
//         return py;
//     }
// };
//

template<typename T>
struct VectorToPythonList
{
    static PyObject* convert(std::vector<T> const& vec)
    {
        boost::python::list python_list;
        typename std::vector<T>::const_iterator iter;

        for(iter = vec.begin(); iter != vec.end(); ++iter)
        {
            python_list.append(boost::python::object(*iter));
        }

        return boost::python::incref(python_list.ptr());
    }
};

template<typename T>
struct PVectorToPythonList
{
    static PyObject* convert(std::vector<T> const& vec)
    {
        boost::python::list python_list;
        typename std::vector<T>::const_iterator iter;

        for(iter = vec.begin(); iter != vec.end(); ++iter)
        {
            python_list.append(boost::python::ptr(*iter));
        }

        return boost::python::incref(python_list.ptr());
    }
};


template<typename T>
struct VectorFromPythonList
{

    VectorFromPythonList()
    {
        boost::python::converter::registry::push_back(&VectorFromPythonList<T>::convertible,
                            &VectorFromPythonList<T>::construct,
                            boost::python::type_id<std::vector<T> >());
    }

    // Determine if obj_ptr can be converted in a std::vector<T>
    static void* convertible(PyObject* obj_ptr)
    {
        if (!PyList_Check(obj_ptr))
        {
            return 0;
        }

        return obj_ptr;
    }

    // Convert obj_ptr into a std::vector<T>
    static void construct(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data)
    {
        // Use borrowed to construct the object so that a reference
        // count will be properly handled.
        boost::python::list python_list(boost::python::handle<>(boost::python::borrowed(obj_ptr)));

        // Grab pointer to memory into which to construct the new std::vector<T>
        void* storage = reinterpret_cast<boost::python::converter::rvalue_from_python_storage<std::vector<T> >*>(data)->storage.bytes;

        // in-place construct the new std::vector<T> using the character data
        // extraced from the python object
        std::vector<T>& vec = *(new (storage) std::vector<T>());

        // populate the vector from list contains !!!
        int lenght = boost::python::len(python_list);
        vec.resize(lenght);

        for(int i = 0; i != lenght; ++i)
        {
            vec[i] = boost::python::extract<T>(python_list[i]);
        }

        // Stash the memory chunk pointer for later use by boost.python
        data->convertible = storage;
    }
};

// // ------------------------------------------------------------------------- //
// // Pair
// // ------------------------------------------------------------------------- //
//
//
// template<typename T1, typename T2>
// struct PairToPythonList
// {
//     static PyObject* convert(std::pair<T1, T2> const& p)
//     {
//         boost::python::list python_list;
//
//         python_list.append(boost::python::object(p.first));
//         python_list.append(boost::python::object(p.second));
//         // typename std::pair<T1, T2>::const_iterator iter;
//
//         // for(iter = vec.begin(); iter != vec.end(); ++iter)
//         // {
//         //     python_list.append(boost::python::object(*iter));
//         // }
//
//         return boost::python::incref(python_list.ptr());
//     }
// };
//
//
// template<typename T1, typename T2>
// struct PairFromPythonList
// {
//
//     PairFromPythonList()
//     {
//         boost::python::converter::registry::push_back(&PairFromPythonList<T1, T2>::convertible,
//                             &PairFromPythonList<T1, T2>::construct,
//                             boost::python::type_id<std::pair<T1, T2> >());
//     }
//
//     // Determine if obj_ptr can be converted in a std::pair
//     static void* convertible(PyObject* obj_ptr)
//     {
//         if (!PyList_Check(obj_ptr))
//         {
//             return 0;
//         }
//
//         return obj_ptr;
//     }
//
//     // Convert obj_ptr into a std::pair<T>
//     static void construct(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data)
//     {
//         // Use borrowed to construct the object so that a reference
//         // count will be properly handled.
//         boost::python::list python_list(boost::python::handle<>(boost::python::borrowed(obj_ptr)));
//
//         // Grab pointer to memory into which to construct the new std::pair<T1, T2>
//         void* storage = reinterpret_cast<boost::python::converter::rvalue_from_python_storage<std::pair<T1, T2> >*>(data)->storage.bytes;
//
//         // in-place construct the new std::pair<T1, T2> using the character data
//         // extraced from the python object
//         std::pair<T1, T2>& p = *(new (storage) std::pair<T1, T2>());
//
//         assert(boost::python::len(python_list == 2));
//
//         // Populate pari from python list
//         p.first = boost::python::extract<T1>(python_list[0]);
//         p.second = boost::python::extract<T2>(python_list[1]);
//
//         // Stash the memory chunk pointer for later use by boost.python
//         data->convertible = storage;
//     }
// };
//
// #<{(|*****************************************************************************
// *                               Python Module                                 *
// *****************************************************************************|)}>#

class PythonHardBBTables {};

BOOST_PYTHON_MODULE(pyPROPOSAL)
{
    using namespace boost::python;

    docstring_options doc_options(true, true, false);

    // --------------------------------------------------------------------- //
    // Vector classes
    // --------------------------------------------------------------------- //

    // register the to-python converter
    to_python_converter< std::vector<double>, VectorToPythonList<double> >();
    to_python_converter< std::vector<std::vector<double> >, VectorToPythonList<std::vector<double> > > ();

    to_python_converter< std::vector<std::string>, VectorToPythonList<std::string> >();

    to_python_converter< std::vector<DynamicData*>, PVectorToPythonList<DynamicData*> >();
    to_python_converter< std::vector<PROPOSALParticle*>, PVectorToPythonList<PROPOSALParticle*> >();

    to_python_converter< std::vector<SectorFactory::Definition>, VectorToPythonList<SectorFactory::Definition> >();

    // register the from-python converter
    VectorFromPythonList<double>();
    VectorFromPythonList<std::vector<double> >();
    VectorFromPythonList<std::string>();

    VectorFromPythonList<DynamicData*>();
    VectorFromPythonList<PROPOSALParticle*>();

    VectorFromPythonList<SectorFactory::Definition>();

    // --------------------------------------------------------------------- //
    // PROPOSAL Vector
    // --------------------------------------------------------------------- //

    class_<Vector3D, boost::shared_ptr<Vector3D> >("Vector3D", init<>())

        .def(init<double, double, double>((arg("x"), arg("y"), arg("z"))))
        .def(init<const Vector3D&>())

        .def(self_ns::str(self_ns::self))
        // .def(self_ns::repr(self_ns::self))

        .add_property("x", &Vector3D::GetX)
        .add_property("y", &Vector3D::GetY)
        .add_property("z", &Vector3D::GetZ)
        .add_property("radius", &Vector3D::GetRadius)
        .add_property("phi", &Vector3D::GetPhi)
        .add_property("theta", &Vector3D::GetTheta)

        .def("set_cartesian_coordinates", &Vector3D::SetCartesianCoordinates)
        .def("set_spherical_coordinates", &Vector3D::SetSphericalCoordinates)
        .def("normalize", &Vector3D::normalise)
        .def("magnitude", &Vector3D::magnitude)
        .def("cartesian_from_spherical", &Vector3D::CalculateCartesianFromSpherical)
        .def("spherical_from_cartesian", &Vector3D::CalculateSphericalCoordinates)
    ;

    // --------------------------------------------------------------------- //
    // EnergyCutSettings
    // --------------------------------------------------------------------- //

    class_<EnergyCutSettings, boost::shared_ptr<EnergyCutSettings> >("EnergyCutSettings", init<>())

        .def(init<double, double>((arg("ecut"), arg("vcut"))))
        .def(init<const EnergyCutSettings&>())

        .def(self_ns::str(self_ns::self))
        .def(self_ns::repr(self_ns::self))

        .add_property("ecut", &EnergyCutSettings::GetEcut, &EnergyCutSettings::SetEcut)
        .add_property("vcut", &EnergyCutSettings::GetVcut, &EnergyCutSettings::SetVcut)

        .def("get_cut", &EnergyCutSettings::GetCut, "Return the lower from E*v = e")
    ;

    // --------------------------------------------------------------------- //
    // DecayChannel
    // --------------------------------------------------------------------- //

    class_<DecayChannel, boost::shared_ptr<DecayChannel>, boost::noncopyable >("DecayChannel", no_init)

        // .def(self_ns::str(self_ns::self))
        .def("decay", &DecayChannel::Decay, "Decay the given paritcle")
        // .def("Clone", make_function(&DecayChannel::clone, return_value_policy<manage_new_object>()), "Decay the given paritcle")
    ;

    class_<LeptonicDecayChannel, boost::shared_ptr<LeptonicDecayChannel>, bases<DecayChannel> >("LeptonicDecayChannel", init<>())

        // .def(init<const LeptonicDecayChannel&>())
    ;

    class_<TwoBodyPhaseSpace, boost::shared_ptr<TwoBodyPhaseSpace>, bases<DecayChannel> >("TwoBodyPhaseSpace", init<double, double>())

        // .def(init<const TwoBodyPhaseSpace&>())
    ;

    class_<StableChannel, boost::shared_ptr<StableChannel>, bases<DecayChannel> >("StableChannel", init<>())

        // .def(init<const StableChannel&>())
    ;

    // --------------------------------------------------------------------- //
    // DecayTable
    // --------------------------------------------------------------------- //

    enum_<DecayTable::Mode>("DecayMode")
        .value("LeptonicDecay", DecayTable::LeptonicDecay)
        .value("TwoBodyDecay", DecayTable::TwoBodyDecay)
        .value("Stable", DecayTable::Stable);

    // void (DecayTable::*addChannelFactory)(double, DecayTable::Mode, double, const std::vector<double>&) = &DecayTable::addChannel;
    // void (DecayTable::*addChannel)(double, DecayChannel&);

    class_<DecayTable, boost::shared_ptr<DecayTable> >("DecayTable", init<>())

        .def(init<const DecayTable&>())

        // .def(self_ns::str(self_ns::self))

        // .def("add_channel_factory", addChannelFactory, "Add an decay channel")
        // .def("add_channel", addChannel, "Add an decay channel")
        .def("add_channel", &DecayTable::addChannel, "Add an decay channel")
        .def("select_channel", make_function(&DecayTable::SelectChannel, return_internal_reference<>()), "Select an decay channel according to given branching ratios")
        .def("set_stable", &DecayTable::SetStable, "Define decay table for stable particles")
    ;

    // --------------------------------------------------------------------- //
    // ParticleDef
    // --------------------------------------------------------------------- //


    class_<ParticleDef, boost::shared_ptr<ParticleDef> >("ParticleDef", init<>())

        .def(init<const ParticleDef&>())

        .def(self_ns::str(self_ns::self))

        .def_readwrite("name", &ParticleDef::name)
        .def_readwrite("mass", &ParticleDef::mass)
        .def_readwrite("low", &ParticleDef::low)
        .def_readwrite("charge", &ParticleDef::charge)
        .def_readwrite("decay_table", &ParticleDef::decay_table)
        // .def_readwrite("hardbb_table", &ParticleDef::hardbb_table) //TODO(mario): hardbb Fri 2017/09/22
    ;

    class_<MuMinusDef, boost::shared_ptr<MuMinusDef>, bases<ParticleDef>, boost::noncopyable >("MuMinusDef", no_init)

        .def("get", make_function(&MuMinusDef::Get, return_value_policy<reference_existing_object>()))
        .staticmethod("get")
        ;

    PARTICLE_DEF(MuPlus)

    PARTICLE_DEF(EMinus)
    PARTICLE_DEF(EPlus)

    PARTICLE_DEF(TauMinus)
    PARTICLE_DEF(TauPlus)

    PARTICLE_DEF(StauMinus)
    PARTICLE_DEF(StauPlus)

    PARTICLE_DEF(P0)
    PARTICLE_DEF(PiMinus)
    PARTICLE_DEF(PiPlus)

    PARTICLE_DEF(KMinus)
    PARTICLE_DEF(KPlus)

    PARTICLE_DEF(PMinus)
    PARTICLE_DEF(PPlus)

    PARTICLE_DEF(NuE)
    PARTICLE_DEF(NuEBar)

    PARTICLE_DEF(NuMu)
    PARTICLE_DEF(NuMuBar)

    PARTICLE_DEF(NuTau)
    PARTICLE_DEF(NuTauBar)

    PARTICLE_DEF(Monopole)
    PARTICLE_DEF(Gamma)
    PARTICLE_DEF(StableMassiveParticle)

    // --------------------------------------------------------------------- //
    // DynamicData
    // --------------------------------------------------------------------- //

    enum_<DynamicData::Type>("Data")
        .value("None", DynamicData::None)
        .value("Particle", DynamicData::Particle)
        .value("Brems", DynamicData::Brems)
        .value("DeltaE", DynamicData::DeltaE)
        .value("Epair", DynamicData::Epair)
        .value("NuclInt", DynamicData::NuclInt)
        .value("MuPair", DynamicData::MuPair)
        .value("Hadrons", DynamicData::Hadrons)
        .value("ContinuousEnergyLoss", DynamicData::ContinuousEnergyLoss);

    class_<DynamicData, boost::shared_ptr<DynamicData> >("DynamicData", init<DynamicData::Type>())

        .def(init<const DynamicData&>())

        // .def(self_ns::str(self_ns::self))

        .add_property("id", &DynamicData::GetTypeId)
        .add_property("position", &DynamicData::GetPosition, &DynamicData::SetPosition)
        .add_property("direction", &DynamicData::GetDirection, &DynamicData::SetDirection)
        .add_property("energy", &DynamicData::GetEnergy, &DynamicData::SetEnergy)
        .add_property("parent_particle_energy", &DynamicData::GetParentParticleEnergy, &DynamicData::SetParentParticleEnergy)
        .add_property("time", &DynamicData::GetTime, &DynamicData::SetTime)
        .add_property("propagated_distance", &DynamicData::GetPropagatedDistance, &DynamicData::SetPropagatedDistance)
    ;

    // --------------------------------------------------------------------- //
    // PROPOSALParticle
    // --------------------------------------------------------------------- //

    class_<PROPOSALParticle, boost::shared_ptr<PROPOSALParticle>, bases<DynamicData> >("Particle", init<>())

        .def(init<const ParticleDef&>())
        .def(init<const PROPOSALParticle&>())

        // .def(self_ns::str(self_ns::self))

        .add_property("momentum", &PROPOSALParticle::GetMomentum, &PROPOSALParticle::SetMomentum)
        .add_property("particle_def", make_function(&PROPOSALParticle::GetParticleDef, return_value_policy<reference_existing_object>()))
    ;

    // --------------------------------------------------------------------- //
    // Medium Definition
    // --------------------------------------------------------------------- //

    enum_<MediumFactory::Enum>("MediumType")
        .value("Water", MediumFactory::Water)
        .value("Ice", MediumFactory::Ice)
        .value("Salt", MediumFactory::Salt)
        .value("StandardRock", MediumFactory::StandardRock)
        .value("FrejusRock", MediumFactory::FrejusRock)
        .value("Iron", MediumFactory::Iron)
        .value("Hydrogen", MediumFactory::Hydrogen)
        .value("Lead", MediumFactory::Lead)
        .value("Copper", MediumFactory::Copper)
        .value("Air", MediumFactory::Air)
        .value("Paraffin", MediumFactory::Paraffin)
        .value("AntaresWater", MediumFactory::AntaresWater);

    class_<MediumFactory::Definition, boost::shared_ptr<MediumFactory::Definition> >("MediumDefinition", init<>())

        // .def(self_ns::str(self_ns::self))

        .def_readwrite("type", &MediumFactory::Definition::type)
        .def_readwrite("density_correction", &MediumFactory::Definition::density_correction)
    ;

    // --------------------------------------------------------------------- //
    // Geometry Definition
    // --------------------------------------------------------------------- //

    enum_<GeometryFactory::Enum>("Shape")
        .value("Sphere", GeometryFactory::Sphere)
        .value("Box", GeometryFactory::Box)
        .value("Cylinder", GeometryFactory::Cylinder);

    class_<GeometryFactory::Definition, boost::shared_ptr<GeometryFactory::Definition> >("GeometryDefinition", init<>())

        // .def(self_ns::str(self_ns::self))

        .def_readwrite("shape", &GeometryFactory::Definition::shape)
        .def_readwrite("position", &GeometryFactory::Definition::position)
        .def_readwrite("inner_radius", &GeometryFactory::Definition::inner_radius)
        .def_readwrite("radius", &GeometryFactory::Definition::radius)
        .def_readwrite("width", &GeometryFactory::Definition::width)
        .def_readwrite("height", &GeometryFactory::Definition::height)
        .def_readwrite("depth", &GeometryFactory::Definition::depth)
    ;

    // --------------------------------------------------------------------- //
    // Geometry
    // --------------------------------------------------------------------- //

    class_<Geometry, boost::shared_ptr<Geometry>, boost::noncopyable>("Geometry", no_init)

        .def(self_ns::str(self_ns::self))

        .def("is_infront", &Geometry::IsInfront)
        .def("is_inside", &Geometry::IsInside)
        .def("is_behind", &Geometry::IsBehind)
        .def("distance_to_border", &Geometry::DistanceToBorder)
        .def("distance_to_closet_approach", &Geometry::DistanceToClosestApproach)

        .add_property("name", &Geometry::GetName)
        .add_property("position", &Geometry::GetPosition, &Geometry::SetPosition)
        .add_property("hirarchy", &Geometry::GetHirarchy, &Geometry::SetHirarchy)
    ;

    class_<Sphere, boost::shared_ptr<Sphere>, bases<Geometry> >("Sphere", init<>())

        .def(init<Vector3D, double, double>())
        .def(init<const Sphere&>())

        // .def(self_ns::str(self_ns::self))

        .add_property("inner_radius", &Sphere::GetInnerRadius, &Sphere::SetInnerRadius)
        .add_property("radius", &Sphere::GetRadius, &Sphere::SetRadius)
    ;

    class_<Box, boost::shared_ptr<Box>, bases<Geometry> >("Box", init<>())

        .def(init<Vector3D, double, double, double>())
        .def(init<const Box&>())

        // .def(self_ns::str(self_ns::self))

        .add_property("width", &Box::GetX, &Box::SetX)
        .add_property("height", &Box::GetY, &Box::SetY)
        .add_property("depth", &Box::GetZ, &Box::SetZ)
    ;

    class_<Cylinder, boost::shared_ptr<Cylinder>, bases<Geometry> >("Cylinder", init<>())

        .def(init<Vector3D, double, double, double>())
        .def(init<const Cylinder&>())

        // .def(self_ns::str(self_ns::self))

        .add_property("inner_radius", &Cylinder::GetInnerRadius, &Cylinder::SetInnerRadius)
        .add_property("radius", &Cylinder::GetRadius, &Cylinder::SetRadius)
        .add_property("height", &Cylinder::GetZ, &Cylinder::SetZ)
    ;

    // --------------------------------------------------------------------- //
    // Scattering Definition
    // --------------------------------------------------------------------- //

    enum_<ScatteringFactory::Enum>("ScatteringModel")
        .value("default", ScatteringFactory::Default)
        .value("moliere", ScatteringFactory::Moliere)
        .value("moliere_first_order", ScatteringFactory::MoliereFirstOrder);

    // --------------------------------------------------------------------- //
    // Bremsstrahlung Definition
    // --------------------------------------------------------------------- //

    enum_<BremsstrahlungFactory::Enum>("BremsParametrization")
        .value("PetrukhinShestakov", BremsstrahlungFactory::PetrukhinShestakov)
        .value("KelnerKokoulinPetrukhin", BremsstrahlungFactory::KelnerKokoulinPetrukhin)
        .value("CompleteScreening", BremsstrahlungFactory::CompleteScreening)
        .value("AndreevBezrukovBugaev", BremsstrahlungFactory::AndreevBezrukovBugaev);

    class_<BremsstrahlungFactory::Definition, boost::shared_ptr<BremsstrahlungFactory::Definition> >("BremsDefinition", init<>())

        .def_readwrite("parametrization", &BremsstrahlungFactory::Definition::parametrization)
        .def_readwrite("lpm_effect", &BremsstrahlungFactory::Definition::lpm_effect)
        .def_readwrite("multiplier", &BremsstrahlungFactory::Definition::multiplier)
    ;

    // --------------------------------------------------------------------- //
    // Photonuclear  Definition
    // --------------------------------------------------------------------- //

    enum_<PhotonuclearFactory::Enum>("PhotoParametrization")
        .value("Zeus", PhotonuclearFactory::Zeus)
        .value("BezrukovBugaev", PhotonuclearFactory::BezrukovBugaev)
        .value("Rhode", PhotonuclearFactory::Rhode)
        .value("Kokoulin", PhotonuclearFactory::Kokoulin)
        .value("AbramowiczLevinLevyMaor91", PhotonuclearFactory::AbramowiczLevinLevyMaor91)
        .value("AbramowiczLevinLevyMaor97", PhotonuclearFactory::AbramowiczLevinLevyMaor97)
        .value("ButkevichMikhailov", PhotonuclearFactory::ButkevichMikhailov)
        .value("RenoSarcevicSu", PhotonuclearFactory::RenoSarcevicSu);

    enum_<PhotonuclearFactory::Shadow>("PhotoShadow")
        .value("Dutta", PhotonuclearFactory::ShadowDutta)
        .value("ButkevichMikhailov", PhotonuclearFactory::ShadowButkevichMikhailov);


    class_<PhotonuclearFactory::Definition, boost::shared_ptr<PhotonuclearFactory::Definition> >("PhotoDefinition", init<>())

        .def_readwrite("parametrization", &PhotonuclearFactory::Definition::parametrization)
        .def_readwrite("shadow", &PhotonuclearFactory::Definition::shadow)
        .def_readwrite("hardbb", &PhotonuclearFactory::Definition::hardbb)
        .def_readwrite("multiplier", &PhotonuclearFactory::Definition::multiplier)
    ;

    // --------------------------------------------------------------------- //
    // EpairProduction Definition
    // --------------------------------------------------------------------- //

    class_<EpairProductionFactory::Definition, boost::shared_ptr<EpairProductionFactory::Definition> >("EpairDefinition", init<>())

        .def_readwrite("lpm_effect", &EpairProductionFactory::Definition::lpm_effect)
        .def_readwrite("multiplier", &EpairProductionFactory::Definition::multiplier)
    ;

    // --------------------------------------------------------------------- //
    // Ionization Definition
    // --------------------------------------------------------------------- //

    class_<IonizationFactory::Definition, boost::shared_ptr<IonizationFactory::Definition> >("IonizationDefinition", init<>())

        .def_readwrite("multiplier", &IonizationFactory::Definition::multiplier)
    ;

    // --------------------------------------------------------------------- //
    // Utility Definition
    // --------------------------------------------------------------------- //


    class_<Utility::Definition, boost::shared_ptr<Utility::Definition> >("UtilityDefinition", init<>())

        .def_readwrite("brems_def", &Utility::Definition::brems_def)
        .def_readwrite("photo_def", &Utility::Definition::photo_def)
        .def_readwrite("epair_def", &Utility::Definition::epair_def)
        .def_readwrite("ioniz_def", &Utility::Definition::ioniz_def)
    ;

    // --------------------------------------------------------------------- //
    // Sector Definition
    // --------------------------------------------------------------------- //

    // ----[ Location ]-------------------------------------- //

    enum_<Sector::ParticleLocation::Enum>("ParticleLocation")
        .value("infront_detector", Sector::ParticleLocation::InfrontDetector)
        .value("inside_detector", Sector::ParticleLocation::InsideDetector)
        .value("behind_detector", Sector::ParticleLocation::BehindDetector);

    class_<SectorFactory::Definition, boost::shared_ptr<SectorFactory::Definition> >("SectorDefinition", init<>())

        // .def(self_ns::str(self_ns::self))

        .def_readwrite("e_cut", &SectorFactory::Definition::e_cut)
        .def_readwrite("v_cut", &SectorFactory::Definition::v_cut)
        .def_readwrite("medium_def", &SectorFactory::Definition::medium_def)
        .def_readwrite("geometry_def", &SectorFactory::Definition::geometry_def)
        .def_readwrite("do_weighting", &SectorFactory::Definition::do_weighting)
        .def_readwrite("weighting_order", &SectorFactory::Definition::weighting_order)
        .def_readwrite("do_continuous_randomization", &SectorFactory::Definition::do_continuous_randomization)
        .def_readwrite("do_exact_time_calculation", &SectorFactory::Definition::do_exact_time_calculation)
        .def_readwrite("scattering_model", &SectorFactory::Definition::scattering_model)
        .def_readwrite("particle_location", &SectorFactory::Definition::location)
        .def_readwrite("crosssection_defs", &SectorFactory::Definition::utility_def)
    ;

    // {
    //     // Change the current scope
    //     scope outer = class_<foo>("foo");
    //
    //     // Define a class Y in the current scope, X
    //     class_<foo::bar>("bar")
    //         .def_readwrite("a", &foo::bar::a);
    // }

    // class_<foo::bar, boost::shared_ptr<foo::bar> >("foobar", init<>())
    //
    //     // .def(self_ns::str(self_ns::self))
    //
    //     .def_readwrite("a", &foo::bar::a)
    // ;

    // --------------------------------------------------------------------- //
    // Interpolation Definition
    // --------------------------------------------------------------------- //

    class_<InterpolationDef, boost::shared_ptr<InterpolationDef> >("InterpolationDef", init<>())

        // .def(self_ns::str(self_ns::self))

        .def_readwrite("order_of_interpolation", &InterpolationDef::order_of_interpolation)
        .def_readwrite("path_to_tables", &InterpolationDef::path_to_tables)
        .def_readwrite("raw", &InterpolationDef::raw)
    ;


    // --------------------------------------------------------------------- //
    // Propagator
    // --------------------------------------------------------------------- //

    class_<Propagator, boost::shared_ptr<Propagator> >(
        "Propagator", init<PROPOSALParticle&, const std::vector<SectorFactory::Definition>&, const Geometry&>((arg("partcle"), arg("sector_defs"), arg("detector"))))

        .def(init<PROPOSALParticle&,
                  const std::vector<SectorFactory::Definition>&,
                  const Geometry&,
                  const InterpolationDef&>((
            arg("particle"), arg("sector_defs"), arg("detector"), arg("interpolation_def"))))

        // .def(self_ns::str(self_ns::self))

        .def("propagate", &Propagator::Propagate, (arg("max_distance_cm") = 1e20));

    // --------------------------------------------------------------------- //
    // HardBBTable
    // --------------------------------------------------------------------- //

    boost::python::scope hardbb = class_<PythonHardBBTables>("HardBBTables");
    hardbb.attr("MuonTable") = HardBBTables::MuonTable;
    hardbb.attr("TauTable") = HardBBTables::TauTable;
}

#undef PARTICLE_DEF
