#include "gtest/gtest.h"
#include "PROPOSAL/ProcessCollection.h"
#include "PROPOSAL/CrossSections.h"
#include <iostream>
#include <string>

using namespace std;




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
    while(in.good())
    {
        if(first)in>>ecut>>vcut>>lpm>>energy>>med>>particleName>> IonizEvents >> BremsEvents >> PhotoEvents >> EpairEvents;
        first=false;

        NumberOfEvents = IonizEvents+BremsEvents+PhotoEvents+EpairEvents;
        cout << "NumberOfEvents: " << NumberOfEvents << endl;
        energy_old = -1;
        Medium *medium = new Medium(med,1.);
        Particle *particle = new Particle(particleName,1.,1.,1,.20,20,1e5,10);
        particle->SetEnergy(energy);
        EnergyCutSettings *cuts = new EnergyCutSettings(ecut,vcut);

        ProcessCollection* ProcColl = new ProcessCollection(particle, medium, cuts);
        for(unsigned int gna = 0; gna < ProcColl->GetCrosssections().size() ; gna++)
        {
            if(ProcColl->GetCrosssections().at(gna)->GetName().compare("Bremsstrahlung") == 0)
            {
                ProcColl->GetCrosssections().at(gna)->SetParametrization(1);
            }
            if(ProcColl->GetCrosssections().at(gna)->GetName().compare("Photonuclear") == 0)
            {
                ProcColl->GetCrosssections().at(gna)->SetParametrization(6);
                //ProcColl->GetCrosssections().at(gna)->SetShadow(2);
            }
        }
        ProcColl->EnableInterpolation();
        //cout << para << "\t" << ecut << "\t" << vcut << "\t" << lpm << "\t" << energy << "\t" << med << "\t" << particleName<< "\t" << e << endl;

        pair<double,string> LossReturn;
        LossReturn.first = 1.0;
        LossReturn.second = "lskflad";
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
            cout << NewBremsEvents << "\t" << NewEpairEvents << "\t" << NewPhotoEvents << "\t" << NewIonizEvents << endl;
            cout << BremsEvents << "\t" << EpairEvents << "\t" << PhotoEvents << "\t" << IonizEvents << endl;
            DevBremsEvents = CalcDev(NumberOfEvents,NewBremsEvents);
            DevEpairEvents = CalcDev(NumberOfEvents,NewEpairEvents);
            DevPhotoEvents = CalcDev(NumberOfEvents,NewPhotoEvents);
            DevIonizEvents = CalcDev(NumberOfEvents,NewIonizEvents);


//            ASSERT_NEAR(BremsEvents, NewBremsEvents, 3*DevBremsEvents);
//            ASSERT_NEAR(EpairEvents, NewEpairEvents, 3*DevEpairEvents);
//            ASSERT_NEAR(PhotoEvents, NewPhotoEvents, 3*DevPhotoEvents);
//            ASSERT_NEAR(IonizEvents, NewIonizEvents, 3*DevIonizEvents);
            }
            in>>ecut>>vcut>>lpm>>energy>>med>>particleName>> IonizEvents >> BremsEvents >> PhotoEvents >> EpairEvents;
        }

        delete cuts;
        delete medium;
        delete particle;
        delete ProcColl;
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
