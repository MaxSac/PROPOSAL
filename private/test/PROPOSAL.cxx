#include <iostream>
#include "PROPOSAL/Bremsstrahlung.h"
#include "PROPOSAL/Integral.h"
#include "PROPOSAL/Medium.h"

using namespace std;

double integrate(double min, double max, int N, double (*func)(double) ){
    double dx   =   max-min;
    dx          /=  N;
    double  integral    = func(min) + func(max);
            integral    /=2;

    for(int i=1;i<N;i++)integral += func(dx*i+min);

    return integral*dx;
}

double X2(double x){
    return x*x;
}

double _3X_2(double x){
    return 3*x + 2;
}

int main(){
    cout.precision(16);
    cout << "baslsadkadsadasdh" << endl;
    double min, max;
    min = 1;
    max = 3;
    int N = 10000000;
    cout << "min: " << min << endl;
    cout << "max: " << max << endl;
    cout << "--------------------------" << endl;
    cout << "integral x*x: "        << integrate(min,max,N,X2)      << endl;
    cout << "integral 3*x + 2: "    << integrate(min,max,N,_3X_2)   << endl;

    Medium *med = new Medium("ice",1.);
    Particle *particle = new Particle("mu",1.,1.,1,.20,20,1e5,10);
    Bremsstrahlung *brems = new Bremsstrahlung();
    brems->SetMedium(med);


    for(int i = 0 ; i < 13 ; i++){
        brems->SetParametrization(4);
        particle->SetEnergy(pow(10,i));
        brems->SetParticle(particle);
        cout<<brems->ElasticBremsstrahlungCrossSection(0.5,0)<<endl;


    }


    brems->SetParametrizationLimit(1.);

    Integral* Int = new Integral();
//    cout << "IntegralKlasse x*x: " << Int->IntegrateClosed(0,3,X2) << endl;


    return 0;
}





