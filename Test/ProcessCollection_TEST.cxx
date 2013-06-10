#include "gtest/gtest.h"
#include "PROPOSAL/ProcessCollection.h"
#include "PROPOSAL/CrossSections.h"
#include <iostream>
#include <string>

using namespace std;


class RndFromFile{
private:
    double rnd_;
    string Path_;
    ifstream in_;

public:
    RndFromFile(string Path){
        Path_ = Path;
        in_.open(Path_.c_str());
        in_>>rnd_;
        if(!in_.good())cout << "less than one rnd_number!" << endl;
    }

    double rnd(){
        in_>>rnd_;
        if(!in_.good()){
            in_.close();
            in_.clear();
            in_.open(Path_.c_str());
            in_>>rnd_;
        }
        return rnd_;
    }
};

std::vector<Medium*>                CombOfMedium;
std::vector<Particle*>              CombOfParticle;
std::vector<EnergyCutSettings*>     CombOfEnergyCutSettings;
std::vector<ProcessCollection*>         CombOfProcColl;

TEST(Epairproduction , Set_Up ) {
    ifstream in;
    in.open("bin/TestFiles/ProcColl_Stoch.txt");

    char firstLine[256];
    in.getline(firstLine,256);

    double e;
    double e_new;
    double energy;
    double ecut;
    double vcut;
    string med;
    string particleName;
    bool lpm;
    int para;

    cout.precision(16);
    double energy_old=-1;


    bool first = true;
    int NumberOfEvents, IonizEvents,BremsEvents,PhotoEvents,EpairEvents;
    int i = -1;
    while(in.good())
    {
        if(first)in>>ecut>>vcut>>lpm>>energy>>med>>particleName>> IonizEvents >> BremsEvents >> PhotoEvents >> EpairEvents;
        first=false;
        energy_old = -1;

        i++;
        CombOfMedium.push_back(new Medium(med,1.));
        CombOfParticle.push_back(new Particle(particleName,1.,1.,1,.20,20,1e5,10));
        CombOfParticle.at(i)->SetEnergy(energy);
        CombOfEnergyCutSettings.push_back(new EnergyCutSettings(ecut,vcut));
        CombOfProcColl.push_back(new ProcessCollection(CombOfParticle.at(i), CombOfMedium.at(i), CombOfEnergyCutSettings.at(i)));

        for(unsigned int gna = 0; gna <CombOfProcColl.at(i)->GetCrosssections().size() ; gna++)
        {
            if(CombOfProcColl.at(i)->GetCrosssections().at(gna)->GetName().compare("Bremsstrahlung") == 0)
            {
                CombOfProcColl.at(i)->GetCrosssections().at(gna)->SetParametrization(1);
            }
            if(CombOfProcColl.at(i)->GetCrosssections().at(gna)->GetName().compare("Photonuclear") == 0)
            {
                CombOfProcColl.at(i)->GetCrosssections().at(gna)->SetParametrization(12);
            }
        }

        CombOfProcColl.at(i)->EnableInterpolation();
        cout << ecut << "\t" << vcut << "\t" << lpm << "\t" << energy << "\t" << med << "\t" << particleName <<  endl;
        while(energy_old < energy)
        {
            energy_old = energy;
            in>>ecut>>vcut>>lpm>>energy>>med>>particleName>> IonizEvents >> BremsEvents >> PhotoEvents >> EpairEvents;
        }
    }
}


int CalcDev(int N, int ni)
{
    double p = (1.0*ni) / N;
    return (1 + (int)(sqrt(N*p*(1-p))) );
}

TEST(ProcessCollection , Stochasticity)
{
    ifstream in;
    in.open("bin/TestFiles/ProcColl_Stoch.txt");

    char firstLine[256];
    in.getline(firstLine,256);

    double e;
    double e_new;
    double energy;
    double ecut;
    double vcut;
    string med;
    string particleName;
    bool lpm;
    int para;

    cout.precision(16);
    double energy_old=-1;

    vector<string> XSecNames;

    XSecNames.push_back("Bremsstrahlung");
    XSecNames.push_back("Epairproduction");
    XSecNames.push_back("Photonuclear");
    XSecNames.push_back("Ionization");

    bool first = true;
    int NumberOfEvents, IonizEvents,BremsEvents,PhotoEvents,EpairEvents;
    int NewIonizEvents,NewBremsEvents,NewPhotoEvents,NewEpairEvents;
    int DevIonizEvents,DevBremsEvents,DevPhotoEvents,DevEpairEvents;
    int foundXSecAt;

    RndFromFile* Rand = new RndFromFile("bin/TestFiles/rnd.txt");
    RndFromFile* Rand2 = new RndFromFile("bin/TestFiles/rnd.txt");
    Rand2->rnd();
            ProcessCollection* ProcColl;
    while(in.good())
    {
        if(first)in>>ecut>>vcut>>lpm>>energy>>med>>particleName>> IonizEvents >> BremsEvents >> PhotoEvents >> EpairEvents;
        first=false;

        NumberOfEvents = IonizEvents+BremsEvents+PhotoEvents+EpairEvents;
        energy_old = -1;

        Medium *medium = new Medium(med,1.);
        Particle *particle = new Particle(particleName,1.,1.,1,.20,20,1e5,10);
        particle->SetEnergy(energy);
        EnergyCutSettings *cuts = new EnergyCutSettings(ecut,vcut);

        int i=0;
        //cout << ecut << "\t" << vcut << "\t" << lpm << "\t" << energy << "\t" << med << "\t" << particleName <<  endl;
        while(i< CombOfProcColl.size())
        {
            if(         !particle->GetName().compare(CombOfParticle.at(i)->GetName())
                    &&  !medium->GetName().compare(CombOfMedium.at(i)->GetName())
                    &&  *cuts == *CombOfEnergyCutSettings.at(i))
                break;
            i++;
        }

        if(i<CombOfProcColl.size())
        {
            ProcColl = CombOfProcColl.at(i);
            //cout << "found cross Section!" << endl;
            //cout << CombOfEnergyCutSettings.at(i)->GetEcut() << "\t" << CombOfEnergyCutSettings.at(i)->GetVcut() << "\t" << CombOfMedium.at(i)->GetName() << "\t" << CombOfParticle.at(i)->GetName() << endl;
        }
        else
        {
            ProcColl = new ProcessCollection(particle, medium, cuts);
        }

        pair<double,string> LossReturn;
        while(energy_old < energy){
            energy_old = energy;

            NewIonizEvents=0;
            NewBremsEvents=0;
            NewPhotoEvents=0;
            NewEpairEvents=0;

            if(NumberOfEvents!=0)
            {
            for(int i = 0; i< NumberOfEvents ; i++)
            {
                ProcColl->GetParticle()->SetEnergy(energy);
                LossReturn = ProcColl->MakeStochasticLoss();
                foundXSecAt = 0;
                while(foundXSecAt < XSecNames.size())
                {
                    if(LossReturn.second.compare(XSecNames.at(foundXSecAt)) == 0)break;
                    foundXSecAt++;
                }

                switch (foundXSecAt) {
                case 0:
                    NewBremsEvents++;
                    break;
                case 1:
                    NewEpairEvents++;
                    break;
                case 2:
                    NewPhotoEvents++;
                    break;
                case 3:
                    NewIonizEvents++;
                    break;
                default:
                    break;
                }
            }

            DevBremsEvents = CalcDev(NumberOfEvents,NewBremsEvents);
            DevEpairEvents = CalcDev(NumberOfEvents,NewEpairEvents);
            DevPhotoEvents = CalcDev(NumberOfEvents,NewPhotoEvents);
            DevIonizEvents = CalcDev(NumberOfEvents,NewIonizEvents);

            DevBremsEvents += CalcDev(NumberOfEvents,BremsEvents);
            DevEpairEvents += CalcDev(NumberOfEvents,EpairEvents);
            DevPhotoEvents += CalcDev(NumberOfEvents,PhotoEvents);
            DevIonizEvents += CalcDev(NumberOfEvents,IonizEvents);

            int NSigmaPruf = 3;
//            int NSigmaCout = 1;

//            if(         fabs(BremsEvents-NewBremsEvents) > NSigmaCout*DevBremsEvents
//                    ||  fabs(EpairEvents-NewEpairEvents) > NSigmaCout*DevEpairEvents
//                    ||  fabs(PhotoEvents-NewPhotoEvents) > NSigmaCout*DevPhotoEvents
//                    ||  fabs(IonizEvents-NewIonizEvents) > NSigmaCout*DevIonizEvents)
//            {
//                cout << NewBremsEvents << "\t" << NewEpairEvents << "\t" << NewPhotoEvents << "\t" << NewIonizEvents << endl;
//                cout << BremsEvents << "\t" << EpairEvents << "\t" << PhotoEvents << "\t" << IonizEvents << endl;
//            }

            ASSERT_NEAR(BremsEvents, NewBremsEvents, NSigmaPruf*DevBremsEvents);
            ASSERT_NEAR(EpairEvents, NewEpairEvents, NSigmaPruf*DevEpairEvents);
            ASSERT_NEAR(PhotoEvents, NewPhotoEvents, NSigmaPruf*DevPhotoEvents);
            ASSERT_NEAR(IonizEvents, NewIonizEvents, NSigmaPruf*DevIonizEvents);
            }
            in>>ecut>>vcut>>lpm>>energy>>med>>particleName>> IonizEvents >> BremsEvents >> PhotoEvents >> EpairEvents;
        }

        delete cuts;
        delete medium;
        delete particle;
    }
}

TEST(ProcessCollection , Displacement)
{
    ifstream in;
    in.open("bin/TestFiles/ProcColl_Disp.txt");

    char firstLine[256];
    in.getline(firstLine,256);
    if(!in.good())
    {
        cerr << "File ProcColl_Disp.txt not found!" << endl;
        EXPECT_TRUE(false);
    }
    double energy;
    double ecut;
    double vcut;
    string med;
    string particleName;
    bool lpm;

    cout.precision(16);
    double energy_old=-1;

    bool first = true;

    double ef,dx,dx_new,dist=1;
    ProcessCollection* ProcColl;
    double RelError = 1E-2;
    while(in.good())
    {
        if(first)in>>ecut>>vcut>>lpm>>med>>particleName>>energy>>ef>>dx;
        first=false;

        energy_old = -1;
        Medium *medium = new Medium(med,1.);
        Particle *particle = new Particle(particleName,1.,1.,1,.20,20,1e5,10);
        particle->SetEnergy(energy);
        EnergyCutSettings *cuts = new EnergyCutSettings(ecut,vcut);

        int i=0;
        //cout << ecut << "\t" << vcut << "\t" << lpm << "\t" << energy << "\t" << med << "\t" << particleName <<  endl;
        while(i< CombOfProcColl.size())
        {
            if(         !particle->GetName().compare(CombOfParticle.at(i)->GetName())
                    &&  !medium->GetName().compare(CombOfMedium.at(i)->GetName())
                    &&  *cuts == *CombOfEnergyCutSettings.at(i))
                break;
            i++;
        }

        if(i<CombOfProcColl.size())
        {
            ProcColl = CombOfProcColl.at(i);
            //cout << "found cross Section!" << endl;
            //cout << CombOfEnergyCutSettings.at(i)->GetEcut() << "\t" << CombOfEnergyCutSettings.at(i)->GetVcut() << "\t" << CombOfMedium.at(i)->GetName() << "\t" << CombOfParticle.at(i)->GetName() << endl;
        }
        else
        {
            ProcColl = new ProcessCollection(particle, medium, cuts);
        }

        while(energy_old < energy){
            energy_old = energy;

            ProcColl->GetParticle()->SetEnergy(energy);
            dx_new = ProcColl->CalculateDisplacement(energy,ef,dist);

//            if(fabs(1-dx_new/dx) > 1E-2)
//            {
//                cout << ecut << "\t" << vcut << "\t" << lpm << "\t" << med << "\t" << particleName << "\t" << energy << "\t" << ef << "\t" << dx << endl;
//                cout << dx << "\t" << dx_new << "\t" << fabs(1-dx_new/dx) << endl;
//            }
            ASSERT_NEAR(dx, dx_new, RelError*dx_new);

            in>>ecut>>vcut>>lpm>>med>>particleName>>energy>>ef>>dx;
        }

        delete cuts;
        delete medium;
        delete particle;
    }
}


TEST(ProcessCollection , TrackingIntegral)
{
    ifstream in;
    in.open("bin/TestFiles/ProcColl_Tracking.txt");

    char firstLine[256];
    in.getline(firstLine,256);
    if(!in.good())
    {
        cerr << "File ProcColl_Tracking.txt not found!" << endl;
        EXPECT_TRUE(false);
    }
    double energy;
    double ecut;
    double vcut;
    string med;
    string particleName;
    bool lpm;

    cout.precision(16);
    double energy_old=-1;

    bool first = true;

    double tracking,tracking_new,rnd=0.1;
    ProcessCollection* ProcColl;
    bool particle_interaction;

    //RelError is mostly better than 1E-2.
    //It differs just 1.8E-3 for electrons in air and
    //in most cases is in the order of the integration precision 1E-6.
    double RelError = 1E-2;
    while(in.good())
    {
        if(first)in>>ecut>>vcut>>lpm>>med>>particleName>> particle_interaction >> energy>>tracking;
        first=false;

        energy_old = -1;
        Medium *medium = new Medium(med,1.);
        Particle *particle = new Particle(particleName,1.,1.,1,.20,20,1e5,10);
        particle->SetEnergy(energy);
        EnergyCutSettings *cuts = new EnergyCutSettings(ecut,vcut);

        int i=0;
        //cout << ecut << "\t" << vcut << "\t" << lpm << "\t" << energy << "\t" << med << "\t" << particleName <<  endl;
        while(i< CombOfProcColl.size())
        {
            if(         !particle->GetName().compare(CombOfParticle.at(i)->GetName())
                    &&  !medium->GetName().compare(CombOfMedium.at(i)->GetName())
                    &&  *cuts == *CombOfEnergyCutSettings.at(i))
                break;
            i++;
        }

        if(i<CombOfProcColl.size())
        {
            ProcColl = CombOfProcColl.at(i);
            //cout << "found cross Section!" << endl;
            //cout << CombOfEnergyCutSettings.at(i)->GetEcut() << "\t" << CombOfEnergyCutSettings.at(i)->GetVcut() << "\t" << CombOfMedium.at(i)->GetName() << "\t" << CombOfParticle.at(i)->GetName() << endl;
        }
        else
        {
            ProcColl = new ProcessCollection(particle, medium, cuts);
        }

        double rndtmp = 0.1;
        while(energy_old < energy){
            energy_old = energy;

            ProcColl->GetParticle()->SetEnergy(energy);
            tracking_new = ProcColl->CalculateTrackingIntegal(energy,rndtmp,particle_interaction);

//            if(fabs(1-tracking_new/tracking) > 1E-5)
//            {
//                cout << ecut << "\t" << vcut << "\t" << lpm << "\t" << med << "\t" << particleName << "\t" << particle_interaction << "\t" << energy << "\t" << tracking << endl;
//                cout << tracking << "\t" << tracking_new << "\t" << fabs(1-tracking_new/tracking)  << endl;
//            }
            ASSERT_NEAR(tracking, tracking_new, RelError*tracking_new);

            in>>ecut>>vcut>>lpm>>med>>particleName>> particle_interaction >> energy>>tracking;
        }

        delete cuts;
        delete medium;
        delete particle;
    }
}

TEST(ProcessCollection , FinalEnergyDist)
{
    ifstream in;
    in.open("bin/TestFiles/ProcColl_FinalEnergyDist.txt");

    char firstLine[256];
    in.getline(firstLine,256);
    if(!in.good())
    {
        cerr << "File ProcColl_FinalEnergyDist.txt not found!" << endl;
        EXPECT_TRUE(false);
    }
    double energy;
    double ecut;
    double vcut;
    string med;
    string particleName;
    bool lpm;

    cout.precision(16);
    double energy_old=-1;

    bool first = true;

    double finalenergy,finalenergy_new,rnd=0.1;
    double dist;
    ProcessCollection* ProcColl;


    //RelError is mostly better than 1E-2.
    //It differs just 1.8E-3 for electrons in air and
    //in most cases is in the order of the integration precision 1E-6.
    double RelError = 1E-2;
    while(in.good())
    {
        if(first)in>>ecut>>vcut>>lpm>>med>>particleName>> dist >> energy>>finalenergy;
        first=false;

        energy_old = -1;
        Medium *medium = new Medium(med,1.);
        Particle *particle = new Particle(particleName,1.,1.,1,.20,20,1e5,10);
        particle->SetEnergy(energy);
        EnergyCutSettings *cuts = new EnergyCutSettings(ecut,vcut);

        int i=0;
        //cout << ecut << "\t" << vcut << "\t" << lpm << "\t" << energy << "\t" << med << "\t" << particleName <<  endl;
        while(i< CombOfProcColl.size())
        {
            if(         !particle->GetName().compare(CombOfParticle.at(i)->GetName())
                    &&  !medium->GetName().compare(CombOfMedium.at(i)->GetName())
                    &&  *cuts == *CombOfEnergyCutSettings.at(i))
                break;
            i++;
        }

        if(i<CombOfProcColl.size())
        {
            ProcColl = CombOfProcColl.at(i);
            //cout << "found cross Section!" << endl;
            //cout << CombOfEnergyCutSettings.at(i)->GetEcut() << "\t" << CombOfEnergyCutSettings.at(i)->GetVcut() << "\t" << CombOfMedium.at(i)->GetName() << "\t" << CombOfParticle.at(i)->GetName() << endl;
        }
        else
        {
            ProcColl = new ProcessCollection(particle, medium, cuts);
        }


        while(energy_old < energy){
            energy_old = energy;

            ProcColl->GetParticle()->SetEnergy(energy);
            finalenergy_new = ProcColl->CalculateFinalEnergy(energy, dist);

            if(fabs(1-finalenergy_new/finalenergy) > 1E-5)
            {
                cout << ecut << "\t" << vcut << "\t" << lpm << "\t" << med << "\t" << particleName << "\t" << dist << "\t" << energy << "\t" << finalenergy << endl;
                cout << finalenergy << "\t" << finalenergy_new << "\t" << fabs(1-finalenergy_new/finalenergy)  << endl;
            }
            ASSERT_NEAR(finalenergy, finalenergy_new, RelError*finalenergy_new);

            in>>ecut>>vcut>>lpm>>med>>particleName>> dist >> energy>>finalenergy;
        }

        delete cuts;
        delete medium;
        delete particle;
    }
}

TEST(Comparison , Comparison_equal ) {
    double dNdx;

    Medium *medium = new Medium("hydrogen",1.);
    Particle *particle = new Particle("mu",1.,1.,1,.20,20,1e5,10);
    EnergyCutSettings *cuts = new EnergyCutSettings(500,-1);
    ProcessCollection *A = new ProcessCollection(particle, medium, cuts);
    ProcessCollection *B = new ProcessCollection(particle, medium, cuts);
    A->EnableInterpolation();
    B->EnableInterpolation();
    EXPECT_TRUE(*A==*B);

    ProcessCollection *C = new ProcessCollection();
    ProcessCollection *D = new ProcessCollection();

    EXPECT_TRUE(*C==*D);



    EXPECT_TRUE(*C==*D);

    A->GetParticle()->SetEnergy(1e6);
    B->GetParticle()->SetEnergy(1e6);
    EXPECT_TRUE(*A==*B);

    dNdx = A->GetCrosssections().at(0)->CalculatedNdx();
    dNdx = B->GetCrosssections().at(0)->CalculatedNdx();
    EXPECT_TRUE(*A==*B);

}

TEST(Comparison , Comparison_not_equal ) {
    double dEdx;
    Medium *medium = new Medium("air",1.);
    Medium *medium2 = new Medium("water",1.);
    Particle *particle = new Particle("mu",1.,1.,1,20,20,1e5,10);
    Particle *particle2 = new Particle("tau",1.,1.,1,20,20,1e5,10);
    EnergyCutSettings *cuts = new EnergyCutSettings(500,-1);
    ProcessCollection *A = new ProcessCollection(particle, medium, cuts);
    ProcessCollection *B = new ProcessCollection(particle, medium2, cuts);
    ProcessCollection *C = new ProcessCollection(particle2, medium, cuts);
    ProcessCollection *D = new ProcessCollection(particle2, medium2, cuts);
    ProcessCollection *E = new ProcessCollection(particle2, medium2, cuts);

    EXPECT_TRUE(*A!=*B);
    EXPECT_TRUE(*C!=*D);
    EXPECT_TRUE(*B!=*D);
    EXPECT_TRUE(*D==*E);

    E->SetParticle(particle);
    EXPECT_TRUE(*D!=*E);
    D->SetParticle(particle);
    EXPECT_TRUE(*D==*E);
    D->GetCrosssections().at(2)->SetParametrization(6);

    EXPECT_TRUE(*D!=*E);


}

TEST(Assignment , Copyconstructor ) {
    ProcessCollection A;
    ProcessCollection B =A;

    EXPECT_TRUE(A==B);

}

TEST(Assignment , Copyconstructor2 ) {
    Medium *medium = new Medium("air",1.);
    Particle *particle = new Particle("mu",1.,1.,1,.20,20,1e5,10);
    EnergyCutSettings *cuts = new EnergyCutSettings(500,-1);

    ProcessCollection A(particle, medium, cuts);
    ProcessCollection B(A);

    EXPECT_TRUE(A==B);

}

TEST(Assignment , Operator ) {
    Medium *medium = new Medium("air",1.);
    Particle *particle = new Particle("mu",1.,1.,1,.20,20,1e5,10);
    EnergyCutSettings *cuts = new EnergyCutSettings(500,-1);
    ProcessCollection A(particle, medium, cuts);
    ProcessCollection B(particle, medium, cuts);
    A.GetCrosssections().at(2)->SetParametrization(6);

    EXPECT_TRUE(A!=B);

    B=A;

    EXPECT_TRUE(A==B);

    Medium *medium2 = new Medium("water",1.);
    Particle *particle2 = new Particle("tau",1.,1.,1,.20,20,1e5,10);
    EnergyCutSettings *cuts2 = new EnergyCutSettings(200,-1);
    ProcessCollection *C = new ProcessCollection(particle2, medium2, cuts2);
    EXPECT_TRUE(A!=*C);

    A=*C;

    EXPECT_TRUE(A==*C);

}

TEST(Assignment , Swap ) {
    Medium *medium = new Medium("hydrogen",1.);
    Medium *medium2 = new Medium("hydrogen",1.);
    Particle *particle = new Particle("mu",1.,1.,1,.20,20,1e5,10);
    Particle *particle2 = new Particle("mu",1.,1.,1,.20,20,1e5,10);
    EnergyCutSettings *cuts = new EnergyCutSettings(500,-1);
    EnergyCutSettings *cuts2 = new EnergyCutSettings(500,-1);
    ProcessCollection A(particle, medium, cuts);
    ProcessCollection B(particle2, medium2, cuts2);
    A.EnableContinuousRandomization();
    B.EnableContinuousRandomization();
    A.EnableInterpolation();
    B.EnableInterpolation();
    EXPECT_TRUE(A==B);

    Medium *medium3 = new Medium("water",1.);
    Medium *medium4 = new Medium("water",1.);
    Particle *particle3 = new Particle("tau",1.,1.,1,.20,20,1e5,10);
    Particle *particle4 = new Particle("tau",1.,1.,1,.20,20,1e5,10);
    EnergyCutSettings *cuts3 = new EnergyCutSettings(200,-1);
    EnergyCutSettings *cuts4 = new EnergyCutSettings(200,-1);
    ProcessCollection *C = new ProcessCollection(particle3, medium3, cuts3);
    ProcessCollection *D = new ProcessCollection(particle4, medium4, cuts4);
    EXPECT_TRUE(*C==*D);

    A.swap(*C);

    EXPECT_TRUE(A==*D);
    EXPECT_TRUE(*C==B);


}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
