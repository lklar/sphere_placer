#pragma once
#include <iostream>
#include <fstream>
#include "Raster.h"
#include "Placer.h"
#include "RNG.h";
#include "file_interface.h"
#include "vector_manipulation.h"
#include "PlacerConfig.h"

int main(int argc, char** argv)
{
	PlacerConfig config;
	if (argc == 1)
	{
		config = PlacerConfig("config.json");
	}
	else {
		config = PlacerConfig(argc, argv);
	}


	SpherePlacer sp(config);
	auto result = sp.place_spheres();

	FileInterface fi(config["output_path"], FileInterface::FI_CSV);

	fi.write(result);
	
	

	return 0;
}