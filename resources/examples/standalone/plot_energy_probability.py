import pyPROPOSAL
import numpy as np

ptype = pyPROPOSAL.ParticleType.MuMinus
mu = pyPROPOSAL.Particle(ptype)
med = pyPROPOSAL.Medium("standard_rock")
cuts = pyPROPOSAL.EnergyCutSettings()

prop = pyPROPOSAL.Propagator(med, cuts, ptype, "../../resources/tables")

statistics = 10
E_max_log = 14

epair_primary_energy = []
epair_secondary_energy = []

brems_primary_energy = []
brems_secondary_energy = []

ioniz_primary_energy = []
ioniz_secondary_energy = []

photo_primary_energy = []
photo_secondary_energy = []

for i in range(statistics):
    prop.reset_particle()
    prop.particle.energy = np.power(10, E_max_log)
    secondarys = prop.propagate()

    for sec in secondarys:
        log_sec_energy = np.log10(sec.energy)
        log_energy = np.log10(sec.parent_particle_energy)

        if sec.type is pyPROPOSAL.ParticleType.EPair:
            epair_primary_energy.append(log_energy)
            epair_secondary_energy.append(log_sec_energy)
        if sec.type is pyPROPOSAL.ParticleType.Brems:
            brems_primary_energy.append(log_energy)
            brems_secondary_energy.append(log_sec_energy)
        if sec.type is pyPROPOSAL.ParticleType.DeltaE:
            ioniz_primary_energy.append(log_energy)
            ioniz_secondary_energy.append(log_sec_energy)
        if sec.type is pyPROPOSAL.ParticleType.NuclInt:
            photo_primary_energy.append(log_energy)
            photo_secondary_energy.append(log_sec_energy)

try:
    import matplotlib.pyplot as plt
    from matplotlib.colors import LogNorm
    from mpl_toolkits.axes_grid1 import make_axes_locatable

except ImportError:
    print("pylab not installed.  no plots for you.")

tex_preamble = [
    r"\usepackage{amsmath}",
    r"\usepackage[utf8]{inputenc}",
    r"\usepackage[T1]{fontenc}",
    r"\usepackage{siunitx}",
]

font_size = 10

params = {
    'backend': 'pdf',
    'font.family': 'serif',
    'font.size': 12,
    'text.usetex': True,
    'text.latex.preamble': tex_preamble,
    'axes.labelsize': font_size,
    'legend.numpoints': 1,
    'legend.shadow': False,
    'legend.fontsize': font_size,
    'xtick.labelsize': font_size,
    'ytick.labelsize': font_size,
    'axes.unicode_minus': True
}

plt.rcParams.update(params)


def plot_hist(ax, prim, sec):
    # ax.set_aspect(0.5)

    hist = ax.hist2d(
        prim,
        sec,
        bins=1000,
        norm=LogNorm(),
    )

    ax.set_xlim(xmin=2, xmax=14)
    ax.set_ylim(ymin=-2, ymax=14)
    ax.grid(ls=":", lw=0.2)

    ax.text(0.05, 0.95, 'count = {:g}'.format(np.sum(hist[0])),
            verticalalignment='top', horizontalalignment='left',
            transform=ax.transAxes, fontsize=10)

    # divider = make_axes_locatable(ax)
    # cax = divider.append_axes("right", size="5%", pad=0.01)

    plt.colorbar(hist[3], ax=ax, pad=0.01)
    # plt.colorbar(hist[3], cax=cax)

    return ax

inch_to_cm = 2.54
golden_ratio = 1.61803
width = 29.7  # cm

fig = plt.figure(figsize=(width / inch_to_cm, width / inch_to_cm / golden_ratio))
fig.subplots_adjust(wspace=0.1, hspace=0.3)

# =========================================================
# 	Ionization
# =========================================================

ax1 = fig.add_subplot(221)
ax1 = plot_hist(ax1, ioniz_primary_energy, ioniz_secondary_energy)
ax1.set_title("ionization")
ax1.set_ylabel(r'secondary particle energy $\log(E/\si{MeV})$')

# =========================================================
# 	Brems
# =========================================================

ax2 = fig.add_subplot(222)
ax2 = plot_hist(ax2, brems_primary_energy, brems_secondary_energy)
ax2.set_title("bremsstrahlung")

# =========================================================
# 	Photonuclear
# =========================================================

ax3 = fig.add_subplot(223)
ax3 = plot_hist(ax3, photo_primary_energy, photo_secondary_energy)
ax3.set_title("photonuclear")
ax3.set_xlabel(r'primary particle energy $\log(E/\si{MeV})$')
ax3.set_ylabel(r'secondary particle energy $\log(E/\si{MeV})$')

# =========================================================
# 	Epair
# =========================================================

ax4 = fig.add_subplot(224)
ax4 = plot_hist(ax4, epair_primary_energy, epair_secondary_energy)
ax4.set_title("pair production")
ax4.set_xlabel(r'primary particle energy $\log(E/\si{MeV})$')

fig.savefig("energy_probability.pdf")
