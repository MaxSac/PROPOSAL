
#include "gtest/gtest.h"

#include "PROPOSAL/decay/DecayChannel.h"
#include "PROPOSAL/decay/LeptonicDecayChannel.h"
#include "PROPOSAL/decay/TwoBodyPhaseSpace.h"

using namespace PROPOSAL;

TEST(Comparison , Comparison_equal ) {
    LeptonicDecayChannel A;
    LeptonicDecayChannel B;
    EXPECT_TRUE(A==B);

    TwoBodyPhaseSpace C(1, 2);
    TwoBodyPhaseSpace D(1, 2);
    EXPECT_TRUE(C==D);

    DecayChannel* E = new LeptonicDecayChannel();
    DecayChannel* F = new LeptonicDecayChannel();

    EXPECT_TRUE(*E==*F);
    delete E;
    delete F;

    E = new TwoBodyPhaseSpace(1, 2);
    F = new TwoBodyPhaseSpace(1, 2);

    EXPECT_TRUE(*E==*F);
    delete E;
    delete F;
}

TEST(Comparison , Comparison_not_equal ) {
    LeptonicDecayChannel A;
    TwoBodyPhaseSpace B(1, 2);
    EXPECT_TRUE(A!=B);

    DecayChannel* E = new LeptonicDecayChannel();
    DecayChannel* F = new TwoBodyPhaseSpace(1, 2);

    EXPECT_TRUE(*E!=*F);
    delete E;
    delete F;

    E = new TwoBodyPhaseSpace(1, 4);
    F = new TwoBodyPhaseSpace(1, 2);

    EXPECT_TRUE(*E!=*F);
    delete E;
    delete F;
}

TEST(Assignment , Copyconstructor ) {
    LeptonicDecayChannel A;
    DecayChannel* B = A.clone();

    EXPECT_TRUE(A==*B);

    delete B;

    DecayChannel* C = new LeptonicDecayChannel();
    DecayChannel* D = C->clone();

    EXPECT_TRUE(*C==*D);

    delete C;
    delete D;

    TwoBodyPhaseSpace E(1, 2);
    DecayChannel* F = E.clone();

    EXPECT_TRUE(E==*F);

    delete F;

    DecayChannel* G = new TwoBodyPhaseSpace(1, 2);
    DecayChannel* H = G->clone();

    EXPECT_TRUE(*G==*H);

    delete G;
    delete H;
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
