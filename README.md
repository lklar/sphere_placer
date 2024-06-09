## Rolling Ballistic Deposition with Rolling (RBDR) sphere placing algorithm

This program can generate 3D sphere packings with volume filling factors ranging from 0.15 to 0.59.

### Command line interface usage:
Use the command line as follows: sphere_placer -n 100000 -dims 100.0 100.0 -r 0.5 [...]. All parameters are optional.

| Parameter    | Valus       | Default | Description |
|--------------|-------------|:-------:|-------------|
| -n           | int         |1000| Number of spheres to place |
| -dims        | int int     |10 10| X- and Y- dimensions of the packing |
| -r           | float       |0.5| Radius of the spheres, use for monosized packings |
| -rvar        | float&nbsp;float |0.5 0.5| Minimum and maximum radius of the spheres, use for polysized packings. The distribution of radii is uniform. Overrides -r parameter. |
| -overlap     | double      | 0.0 | Overlap between two spheres in contact. By default they will be placed at a distance of the sum of their radii (e.g. overlap = 0). |
| -theta       | float       |180.0| Parameter $\theta_a$ used to control the filling factor. It describes the additional angle of polar rotation in degrees. |
| -nphi        | int         |1| Parameter $N_\varphi$ used to control the filling factor. It describes the number of tries in azimuthal direction. |
| -o           | string      |"out.csv"| Output path for the .csv files containing X, Y and Z positions as well as the radius. |
| -i           | string      |"" | Input path for a .csv file containing a list of radii. If specified, -n, -r and -rvar parameters are ignored and the number of spheres + their radii are sourced from the specified file. |
