#include "Placer.h"
#include <algorithm>
#include <chrono>

SpherePlacer::SpherePlacer(PlacerConfig& config) : config(config) 
{
	inputSpheres = std::shared_ptr<std::vector<Sphere>>(new std::vector<Sphere>());
	
	RNG::uniform_real_distribution xDist(rngEngine());
	RNG::uniform_real_distribution yDist(rngEngine());
	RNG::uniform_real_distribution zDist(rngEngine());

	for (int i = 0; i < config.n; ++i)
	{
		inputSpheres->emplace_back(
			xDist(config.r, config.x_dim - config.r),
			yDist(config.r, config.y_dim - config.r),
			0,
			config.radius_generator(rngEngine)
		);
	}
	double r_max = std::max_element(inputSpheres->begin(), inputSpheres->end(), [](const Sphere& lhs, const Sphere& rhs) {return lhs.R < rhs.R; })->R;

	sphereRaster = std::make_unique<RecursiveRaster>(config.x_dim, config.y_dim, r_max);
	
}

SpherePlacer::dynamicFfInterface::dynamicFfInterface(std::function<double(const Sphere&)> sphereFunction)
{
	sphereToFillingFactor = sphereFunction;
}

std::pair<int, double> SpherePlacer::dynamicFfInterface::get_phi_and_extra_theta_values(const Sphere& sphere)
{
	double wantedFF = sphereToFillingFactor(sphere);
	std::pair<int, double> result;

	if (wantedFF < 0.152)
	{
		result.first = 1;
		result.second = 0.0;
		return result;
	}
	else if (wantedFF > 0.5758)
	{
		result.first = 360;
		result.second = std::numbers::pi;
		return result;
	}

	// Appropriate parameters to achieve this filling factor
	auto p = std::lower_bound(
		params.begin(),
		params.end(),
		wantedFF,
		[](const boltzmannParameters& lhs, double rhs) {
			return lhs.rangeEnd <= rhs;
		});

	// Fitted against Boltzmann function from 181 data points for phi steps = 1,
	// extra theta angles from 0�� do 180��. Function has the form
	// y = (A1 - A2) / (1 + e^((x - x0) / dx)) + A2

	// Invert Boltzmann function to get the needed theta angle for the wanted filling factor
	result.first = p->phiSteps;
	result.second = (p->dx * std::log((wantedFF - p->A1) / (p->A2 - wantedFF)) + p->x0) * std::numbers::pi / 180.0;

	return result;
}

std::reference_wrapper<Sphere> SpherePlacer::stick_to_base_sphere(Raster& sphereRaster, Sphere& newSphere)
{
	auto allSphereVectors = sphereRaster.get_spheres(newSphere.P.x, newSphere.P.y, 1);
	auto baseSphere = std::ref(newSphere);
	double3 offset(0);
	for (auto& wrappedSphereVector : allSphereVectors)
	{
		auto& sphereIt = *wrappedSphereVector.get();
		for (;
			!sphereIt.finished() && (*sphereIt).get_max_z() >= newSphere.P.z - newSphere.R;
			++sphereIt)
		{
			double3& P_n = newSphere.P;
			double3 P_o = (*sphereIt).P;
			double& R_n = newSphere.R;
			double R_o = (*sphereIt).R;

			double overlap = pow(R_n + R_o, 2.0) - pow(P_n.x - P_o.x, 2.0) - pow(P_n.y - P_o.y, 2.0);
			if (overlap > 0.0)
			{
				double newZPosition = P_o.z + std::sqrt(overlap);
				if (newZPosition > P_n.z)
				{
					P_n.z = newZPosition;
					baseSphere = std::ref(*&sphereIt);
					offset = sphereIt.offset();
				}
			}
		}
	}
	newSphere.P = newSphere.P - offset;
	return baseSphere;
}

std::vector<Sphere> SpherePlacer::place_spheres()
{
	double zFloor = 0.0;
	std::reverse(inputSpheres->begin(), inputSpheres->end());

	double phiStep = 2.0 * std::numbers::pi / (double)config.n_phi;
	double extraTheta = config.theta_a * std::numbers::pi / 180.0;

	auto chronoEnd = std::chrono::system_clock::now();

	while(!inputSpheres->empty())
	{
		Sphere& newSphere = inputSpheres->back();
		double3& Pn = newSphere.P;
		double& Rn = newSphere.R;
		Pn.z = 0;
		Sphere newSphereStart = newSphere;

		Sphere& baseSphere = stick_to_base_sphere(*sphereRaster, newSphere);
		if (&baseSphere != &newSphere)
		{
			double3& Pb = baseSphere.P;
			double& Rb = baseSphere.R;
			double3 Pn_r(Pn - Pb);

			double startingPhi = std::atan2(Pn_r.y, Pn_r.x);
			double startingTheta = std::acos(Pn_r.z / (Rn + Rb));

			RangeTracker thetaRange(config.n_phi);

			thetaRange.enter_ranges(*sphereRaster, baseSphere, startingPhi, Rn);

			double appropriateExtraTheta = std::min(startingTheta + extraTheta, std::numbers::pi);
			if (Pb.z + (Rn + Rb) * std::cos(appropriateExtraTheta) < zFloor)
			{
				// If a full rotation by extra theta would bring our sphere under the z-floor,
				// limit the rotation in theta appropriately
				double allowedRotation = zFloor - Pb.z;
				appropriateExtraTheta = std::acos(allowedRotation / (Rn + Rb));
			}
			
			auto phiInfo = thetaRange.find_first_free_value(rngEngine, appropriateExtraTheta);

			
			double phi = startingPhi + phiInfo.first * phiStep;
			double theta = phiInfo.second;

			Pn.x = Pb.x + (Rn + Rb) * std::sin(theta) * std::cos(phi);
			Pn.y = Pb.y + (Rn + Rb) * std::sin(theta) * std::sin(phi);
			Pn.z = Pb.z + (Rn + Rb) * std::cos(theta);
		}

		
		sphereRaster->add(newSphere);
		inputSpheres->pop_back();
		
		if (inputSpheres->size() % 100000 == 0)
		{
			auto chronoNow = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = chronoNow - chronoEnd;
			std::cout << inputSpheres->size() << "\t" << elapsed_seconds.count() << "\n";
			chronoEnd = chronoNow;
		}
	}

	return sphereRaster->get_all_spheres();
}