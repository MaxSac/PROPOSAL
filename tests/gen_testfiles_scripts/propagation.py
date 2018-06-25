import pyPROPOSAL as pp
import random


def create_table(dir_name):
    """TODO: Docstring for create_table.
    Returns: TODO

    """

    statistics = 100

    prop = pp.Propagator(
        pp.MuMinusDef.get(),
        # "../../resources/config_ice.json"
        "resources/config_ice.json"
    )

    mu = prop.particle

    mu.energy = 1e8
    mu.propagated_distance = 0
    mu.position = pp.Vector3D(0, 0, 0)
    mu.direction = pp.Vector3D(0, 0, -1)

    with open(dir_name + "Propagator_propagation.txt", "a") as f:

        buf = [""]
        buf.append("name")
        buf.append("lenght")
        buf.append("energy")
        buf.append("x")
        buf.append("y")
        buf.append("z")
        buf.append("dx")
        buf.append("dy")
        buf.append("dz")
        buf.append("\n")
        buf.append(str(statistics))
        buf.append(str(mu.energy))
        buf.append("\n")

        f.write("\t".join(buf))

        for i in range(statistics):
            mu.energy = 1e8
            mu.propagated_distance = 0
            mu.position = pp.Vector3D(0, 0, 0)
            mu.direction = pp.Vector3D(0, 0, -1)

            daughters = prop.propagate()

            for d in daughters:
                buf = [""]
                if d.id == pp.Data.Particle:
                    buf.append(d.particle_def.name)
                else:
                    buf.append(str(d.id))
                buf.append(str(d.propagated_distance))
                buf.append(str(d.energy))
                buf.append(str(d.position.x))
                buf.append(str(d.position.y))
                buf.append(str(d.position.z))
                buf.append(str(d.direction.x))
                buf.append(str(d.direction.y))
                buf.append(str(d.direction.z))
                buf.append("\n")

                f.write("\t".join(buf))

        f.close()


def main(dir_name):
    create_table(dir_name)


if __name__ == "__main__":

    dir_name = "TestFiles/"

    try:
        os.makedirs(dir_name)
        print("Directory {} created".format(dir_name))
    except OSError:
        print("Directory {} already exists".format(dir_name))

    main(dir_name)
