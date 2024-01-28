#pragma once
#include <vector>
#include <random>
#include <list>
#include <set>
#include <functional>
#include <numbers>
#include "dim_N_vectors/dim_3_vectors.h"
#include "RNG/RNG.h"
#include "Raster.h"
#include "RangeTracker.h"
#include "PlacerConfig.h"

class SpherePlacer
{
	struct dynamicFfInterface;
private:
	std::unique_ptr<Raster> sphereRaster;
	std::unique_ptr<dynamicFfInterface> dfi;
	std::shared_ptr<std::vector<Sphere>> inputSpheres;
	RNG::xoshiro256ss rngEngine = RNG::make_xoshiro256ss(time(NULL));

	PlacerConfig config;

public:
	//SpherePlacer();

	SpherePlacer(PlacerConfig& config);

	std::vector<Sphere> place_spheres();

private:
	std::reference_wrapper<Sphere> stick_to_base_sphere(Raster& sphereRaster, Sphere& newSphere);

	struct dynamicFfInterface
	{
	private:
		struct boltzmannParameters {
		public:
			int phiSteps;
			double rangeEnd;
			double A1;
			double A2;
			double x0;
			double dx;
	
			boltzmannParameters(int phiSteps, double rangeEnd, double A1, double A2, double x0, double dx) :
				phiSteps(phiSteps),
				rangeEnd(rangeEnd),
				A1(A1),
				A2(A2),
				x0(x0),
				dx(dx) {}
		};
	
		std::function<double(const Sphere&)> sphereToFillingFactor;
		std::vector<boltzmannParameters> params{
			{1, 0.2815, 0.12549, 0.38633, 34.72845, 15.75773},
			{1, 0.3936, 0.02337, 0.39363, 24.65909, 19.52869},
			{2, 0.4344, 0.12549, 0.43442, 30.77826, 17.01983},
			{4, 0.4657, 0.12549, 0.46575, 31.29721, 13.35188},
			{8, 0.4939, 0.12549, 0.49393, 34.41568, 10.27088},
			{18, 0.5295, 0.12549, 0.52955, 37.56393, 8.7814},
			{360, 0.5758, 0.12549, 0.57588, 39.68789, 8.209}
			//{360, 1000.0, 0.000, 1000.0, 180.0000, 0.00000}
		};
	
	public:
		dynamicFfInterface(std::function<double(const Sphere&)> sphereFunction);
	
		std::pair<int, double> get_phi_and_extra_theta_values(const Sphere& sphere);
	};
};





