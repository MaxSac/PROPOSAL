import pyPROPOSAL as pp
import numpy as np

photo_real = [
    pp.Parametrization.Photonuclear.Zeus,
    pp.Parametrization.Photonuclear.BezrukovBugaev,
    pp.Parametrization.Photonuclear.Rhode,
    pp.Parametrization.Photonuclear.Kokoulin
]

particle_defs = [
    pp.MuMinusDef.get(),
    pp.TauMinusDef.get()#,
    # pp.EMinusDef.get()
]

mediums = [
    pp.Medium.Ice(1.0),
    pp.Medium.Hydrogen(1.0),
    pp.Medium.Uranium(1.0)
]

cuts = [
    pp.EnergyCutSettings(-1, -1),
    pp.EnergyCutSettings(500, -1),
    pp.EnergyCutSettings(-1, 0.05),
    pp.EnergyCutSettings(500, 0.05)
]

multiplier = 1.

hard_components = [0, 1]

photo_q2 = [
    pp.Parametrization.Photonuclear.AbramowiczLevinLevyMaor91,
    pp.Parametrization.Photonuclear.AbramowiczLevinLevyMaor97,
    pp.Parametrization.Photonuclear.ButkevichMikhailov,
    pp.Parametrization.Photonuclear.RenoSarcevicSu
]

photo_q2_interpol = [
    pp.Parametrization.Photonuclear.AbramowiczLevinLevyMaor91Interpolant,
    pp.Parametrization.Photonuclear.AbramowiczLevinLevyMaor97Interpolant,
    pp.Parametrization.Photonuclear.ButkevichMikhailovInterpolant,
    pp.Parametrization.Photonuclear.RenoSarcevicSuInterpolant
]

shadows = [
    pp.Parametrization.Photonuclear.ShadowDuttaRenoSarcevicSeckel(),
    pp.Parametrization.Photonuclear.ShadowButkevichMikhailov()
]

energies = np.logspace(4, 13, num=10)

interpoldef = pp.InterpolationDef()


def create_table_dEdx(interpolate=False):

    with open("Photo_Real_dEdx{}.txt".format("_interpol" if interpolate else ""), "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for hard  in hard_components:
                        for energy in energies:
                            for parametrization in photo_real:

                                photo = parametrization(
                                    particle,
                                    medium,
                                    cut,
                                    multiplier,
                                    hard)

                                if interpolate:
                                    xsection = pp.CrossSection.PhotoInterpolant(photo, interpoldef)
                                else:
                                    xsection = pp.CrossSection.PhotoIntegral(photo)

                                buf = [""]

                                dEdx = xsection.calculate_dEdx(energy)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(energy))
                                buf.append(str(dEdx))
                                buf.append(photo.name)
                                buf.append(str(hard))
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_dNdx(interpolate=False):

    with open("Photo_Real_dNdx{}.txt".format("_interpol" if interpolate else ""), "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for hard  in hard_components:
                        for energy in energies:
                            for parametrization in photo_real:

                                photo = parametrization(
                                    particle,
                                    medium,
                                    cut,
                                    multiplier,
                                    hard)

                                if interpolate:
                                    xsection = pp.CrossSection.PhotoInterpolant(photo, interpoldef)
                                else:
                                    xsection = pp.CrossSection.PhotoIntegral(photo)

                                buf = [""]

                                dNdx = xsection.calculate_dNdx(energy)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(energy))
                                buf.append(str(dNdx))
                                buf.append(photo.name)
                                buf.append(str(hard))
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_dNdx_rnd(interpolate=False):

    pp.RandomGenerator.get().set_seed(1234)

    with open("Photo_Real_dNdx_rnd{}.txt".format("_interpol" if interpolate else ""), "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for hard  in hard_components:
                        for energy in energies:
                            rnd = pp.RandomGenerator.get().random_double()
                            for parametrization in photo_real:

                                photo = parametrization(
                                    particle,
                                    medium,
                                    cut,
                                    multiplier,
                                    hard)

                                if interpolate:
                                    xsection = pp.CrossSection.PhotoInterpolant(photo, interpoldef)
                                else:
                                    xsection = pp.CrossSection.PhotoIntegral(photo)

                                buf = [""]

                                dNdx = xsection.calculate_dNdx_rnd(energy, rnd)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(energy))
                                buf.append(str(rnd))
                                buf.append(str(dNdx))
                                buf.append(photo.name)
                                buf.append(str(hard))
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_stochastic_loss(interpolate=False):

    pp.RandomGenerator.get().set_seed(1234)

    with open("Photo_Real_e{}.txt".format("_interpol" if interpolate else ""), "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for hard  in hard_components:
                        for energy in energies:
                            rnd1 = pp.RandomGenerator.get().random_double()
                            rnd2 = pp.RandomGenerator.get().random_double()
                            for parametrization in photo_real:

                                photo = parametrization(
                                    particle,
                                    medium,
                                    cut,
                                    multiplier,
                                    hard)

                                if interpolate:
                                    xsection = pp.CrossSection.PhotoInterpolant(photo, interpoldef)
                                else:
                                    xsection = pp.CrossSection.PhotoIntegral(photo)

                                buf = [""]

                                stochastic_loss = xsection.calculate_stochastic_loss(energy, rnd1, rnd2)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(energy))
                                buf.append(str(rnd1))
                                buf.append(str(rnd2))
                                buf.append(str(stochastic_loss))
                                buf.append(photo.name)
                                buf.append(str(hard))
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_dEdx_Q2(interpolate=False):

    if interpolate:
        q2 = photo_q2_interpol
    else:
        q2 = photo_q2

    with open("Photo_Q2_dEdx{}.txt".format("_interpol" if interpolate else ""), "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for shadow in shadows:
                        for energy in energies:
                            for parametrization in q2:

                                if interpolate:
                                    photo = parametrization(
                                        particle,
                                        medium,
                                        cut,
                                        multiplier,
                                        shadow,
                                        interpoldef)

                                    xsection = pp.CrossSection.PhotoInterpolant(photo, interpoldef)
                                else:
                                    photo = parametrization(
                                        particle,
                                        medium,
                                        cut,
                                        multiplier,
                                        shadow)

                                    xsection = pp.CrossSection.PhotoIntegral(photo)

                                buf = [""]

                                dEdx = xsection.calculate_dEdx(energy)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(energy))
                                buf.append(str(dEdx))
                                buf.append(photo.name)
                                buf.append(shadow.name)
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_dNdx_Q2(interpolate=False):

    if interpolate:
        q2 = photo_q2_interpol
    else:
        q2 = photo_q2

    with open("Photo_Q2_dNdx{}.txt".format("_interpol" if interpolate else ""), "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for shadow in shadows:
                        for energy in energies:
                            for parametrization in q2:

                                if interpolate:
                                    photo = parametrization(
                                        particle,
                                        medium,
                                        cut,
                                        multiplier,
                                        shadow,
                                        interpoldef)

                                    xsection = pp.CrossSection.PhotoInterpolant(photo, interpoldef)
                                else:
                                    photo = parametrization(
                                        particle,
                                        medium,
                                        cut,
                                        multiplier,
                                        shadow)

                                    xsection = pp.CrossSection.PhotoIntegral(photo)

                                buf = [""]

                                dNdx = xsection.calculate_dNdx(energy)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(energy))
                                buf.append(str(dNdx))
                                buf.append(photo.name)
                                buf.append(shadow.name)
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_dNdx_rnd_Q2(interpolate=False):

    if interpolate:
        q2 = photo_q2_interpol
    else:
        q2 = photo_q2

    with open("Photo_Q2_dNdx_rnd{}.txt".format("_interpol" if interpolate else ""), "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for shadow in shadows:
                        for energy in energies:
                            rnd = pp.RandomGenerator.get().random_double()
                            for parametrization in q2:

                                if interpolate:
                                    photo = parametrization(
                                        particle,
                                        medium,
                                        cut,
                                        multiplier,
                                        shadow,
                                        interpoldef)

                                    xsection = pp.CrossSection.PhotoInterpolant(photo, interpoldef)
                                else:
                                    photo = parametrization(
                                        particle,
                                        medium,
                                        cut,
                                        multiplier,
                                        shadow)

                                    xsection = pp.CrossSection.PhotoIntegral(photo)

                                buf = [""]

                                dNdx = xsection.calculate_dNdx_rnd(energy, rnd)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(energy))
                                buf.append(str(rnd))
                                buf.append(str(dNdx))
                                buf.append(photo.name)
                                buf.append(shadow.name)
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_stochastic_loss_Q2(interpolate=False):

    if interpolate:
        q2 = photo_q2_interpol
    else:
        q2 = photo_q2

    with open("Photo_Q2_e{}.txt".format("_interpol" if interpolate else ""), "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for shadow in shadows:
                        for energy in energies:
                            rnd1 = pp.RandomGenerator.get().random_double()
                            rnd2 = pp.RandomGenerator.get().random_double()
                            for parametrization in q2:

                                if interpolate:
                                    photo = parametrization(
                                        particle,
                                        medium,
                                        cut,
                                        multiplier,
                                        shadow,
                                        interpoldef)

                                    xsection = pp.CrossSection.PhotoInterpolant(photo, interpoldef)
                                else:
                                    photo = parametrization(
                                        particle,
                                        medium,
                                        cut,
                                        multiplier,
                                        shadow)

                                    xsection = pp.CrossSection.PhotoIntegral(photo)

                                buf = [""]

                                stochastic_loss = xsection.calculate_stochastic_loss(energy, rnd1, rnd2)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(energy))
                                buf.append(str(rnd1))
                                buf.append(str(rnd2))
                                buf.append(str(stochastic_loss))
                                buf.append(photo.name)
                                buf.append(shadow.name)
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def main():
    # Integrate
    # create_table_dEdx()
    # create_table_dNdx()
    # create_table_dNdx_rnd()
    # create_table_stochastic_loss()
    # create_table_dEdx_Q2()
    # create_table_dNdx_Q2()
    # create_table_dNdx_rnd_Q2()
    # create_table_stochastic_loss_Q2()

    # Interpolate
    create_table_dEdx(True)
    create_table_dNdx(True)
    create_table_dNdx_rnd(True)
    create_table_stochastic_loss(True)
    create_table_dEdx_Q2(True)
    create_table_dNdx_Q2(True)
    create_table_dNdx_rnd_Q2(True)
    create_table_stochastic_loss_Q2(True)


if __name__ == "__main__":
    main()
