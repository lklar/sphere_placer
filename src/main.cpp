﻿// sphere_placer.cpp : Defines the entry point for the application.
//

#include "main.h"

using namespace std;

int main(int argc, char** argv)
{
	// Use the command line as follows: sphere_placer -n 100000 -dims 100.0 100.0 -r 0.5 ...
	// Valid command line options are:
	// -n <int>:				Number of spheres to place
	// -dims <double double>:	X- and Y-Dimensions of the area where the spheres will be placed onto
	// -r <double>:				Constant radius of a sphere
	// -rvar <double double>:	Minimum and maximum radius of a sphere, the radius will be uniformly chosen in between
	// -theta <double>:			Maximum additional rotation in polar direction in degrees
	// -nphi <int>:				Number of azimuthal angles considered
	// -o <string>:				Output path
	PlacerConfig config(argc, argv);
	
	// Apart from the command line interface, parameters can be set like this:
	/*config.r_min = 0.1;
	config.r_max = 0.5;
	config.n = 1000000;*/

	// Example for generating a packing with a normal distribution of radii
	/*config.radius_generator = [&config](RNG::xoshiro256ss& rng) {
		static std::normal_distribution<double> dist(1.0, 0.2); // R_mean = 1.0, R_sigma = 0.2
		return std::abs(dist(rng)); // You should not return negative radii
	};*/

	SpherePlacer sp(config);
	auto result = sp.place_spheres();
	
	FileInterface fi(config.output_path, FileInterface::FI_CSV);
	fi.write(result);

	return 0;
}