#include "RangeTracker.h"
#include <execution>

RangeTracker::RangeTracker(int nmbrOfBins)
{
	ranges.resize(nmbrOfBins);
}

void RangeTracker::enter_range(int bin, double2 newRange)
{
	ranges[bin].emplace(newRange);
}


double RangeTracker::find_first_free_value(int bin, double maxValue)
{
	double foundValue = maxValue;
	auto it = ranges[bin].begin();
	if (it == ranges[bin].end())
	{
		return maxValue;
	}

	while (it != ranges[bin].end())
	{
		if (it->y >= foundValue)
		{
			foundValue = std::min(foundValue, it->x);
		}
		else {
			break;
		}
		++it;
	}
	return foundValue;
}

void RangeTracker::enter_ranges_for_only_one_bin(Raster& raster, Sphere& baseSphere, double phi, double R)
{
	double3& Pb = baseSphere.P;
	double minZ = baseSphere.P.z - baseSphere.R - 2.0 * R;
	auto interactionRange = raster.get_spheres(Pb.x, Pb.y, 2);

	for (auto& wrappedSphereVector : interactionRange)
	{
		for (auto& sphereIt = *wrappedSphereVector.get();
			!sphereIt.finished() && (*sphereIt).get_max_z() >= minZ;
			++sphereIt)
		{
			if (&baseSphere == &sphereIt)
			{
				continue;
			}
			Sphere otherSphere = *sphereIt;
			double3 Ps_r(otherSphere.P - Pb);

			double a = std::cos(phi) * Ps_r.x + std::sin(phi) * Ps_r.y;
			double b = Ps_r.z;
			double c =
				(
					std::pow(baseSphere.R + R, 2) - std::pow(otherSphere.R + R, 2) +
					Ps_r.x * Ps_r.x + Ps_r.y * Ps_r.y + Ps_r.z * Ps_r.z
				) / (2.0 * (baseSphere.R + R));
			double buffer = a * a + b * b - c * c;
			if (buffer > 0)
			{
				double d = std::sqrt(buffer);
				double theta1 = std::atan2(a * c - b * d, b * c + a * d);
				double theta2 = std::atan2(a * c + b * d, b * c - a * d);

				double minTheta = std::min(theta1, theta2);
				double maxTheta = std::max(theta1, theta2);

				double derivative = a * std::cos(minTheta) - b * std::sin(minTheta);

				if (derivative > 0.0)
				{
					// Range from minTheta -> maxTheta is blocked by sphere
					ranges[0].emplace(double2(std::max(0.0, minTheta), maxTheta));
				}
				else {
					// Range from maxTheta -> min Theta is blocked by sphere
					if (minTheta > 0.0)
					{
						ranges[0].emplace(double2(0.0, minTheta));
					}
					ranges[0].emplace(double2(maxTheta, std::numbers::pi));
				}
			}
		}
	}
}

void RangeTracker::enter_ranges_for_multiple_bins(Raster& raster, Sphere& baseSphere, double startingPhi, double R)
{
	double3& Pb = baseSphere.P;
	double minZ = baseSphere.P.z - baseSphere.R - 2.0 * R;
	double phiStep = 2.0 * std::numbers::pi / (double)ranges.size();
	std::for_each(
		std::execution::par_unseq,
		ranges.begin(),
		ranges.begin() + ranges.size() / 2,
		[&](auto&& range)
		{
			auto interactionRange = raster.get_spheres(Pb.x, Pb.y, 2);
			int binId = &range - ranges.data();
			auto& oppositeRange = *(&range + ranges.size() / 2);
			double phi = startingPhi + binId * phiStep;
			for (auto& wrappedSphereVector : interactionRange)
			{
				for (auto& sphereIt = *wrappedSphereVector.get();
					!sphereIt.finished() && (*sphereIt).get_max_z() >= minZ;
					++sphereIt)
				{
					if (&baseSphere == &sphereIt)
					{
						continue;
					}
					Sphere otherSphere = *sphereIt;
					double3 Ps_r(otherSphere.P - Pb);

					double a = std::cos(phi) * Ps_r.x + std::sin(phi) * Ps_r.y;
					double b = Ps_r.z;
					double c =
						(
							std::pow(baseSphere.R + R, 2) - std::pow(otherSphere.R + R, 2) +
							Ps_r.x * Ps_r.x + Ps_r.y * Ps_r.y + Ps_r.z * Ps_r.z
							) / (2.0 * (baseSphere.R + R));
					double buffer = a * a + b * b - c * c;
					if (buffer > 0)
					{
						// In case they do touch, calculate the corresponding theta from a, b, c and d
						double d = std::sqrt(buffer);
						double theta1 = std::atan2(a * c - b * d, b * c + a * d);
						double theta2 = std::atan2(a * c + b * d, b * c - a * d);

						double minTheta = std::min(theta1, theta2);
						double maxTheta = std::max(theta1, theta2);

						// Determine, which range of the polar angle is blocked by taking the derivative
						double derivative = a * std::cos(minTheta) - b * std::sin(minTheta);

						if (derivative > 0.0)
						{
							// Range from minTheta -> maxTheta is blocked by sphere
							if (minTheta >= 0.0)
							{
								// Both theta values are on this side, add the theta range to the corresponding bin
								range.emplace(double2(minTheta, maxTheta));
							}
							else {
								if (maxTheta >= 0.0)
								{
									// minTheta is on the other phi side, but maxTheta is on this side
									// In this case, the range passes over theta = 0Åã, so split it there
									// and add to the appropriate ranges
									oppositeRange.emplace(double2(0.0, -minTheta));
									range.emplace(double2(0.0, maxTheta));
								}
								else {
									// Both are on the opposite side, so add them there
									oppositeRange.emplace(double2(-maxTheta, -minTheta));
								}
							}
						}
						else {
							// Range from maxTheta -> min Theta is blocked by sphere
							if (minTheta >= 0.0)
							{
								// The blocked range passes over theta = 180Åã and blocks the entire opposite side, split into three parts
								range.emplace(double2(maxTheta, std::numbers::pi));
								oppositeRange.emplace(double2(0.0, std::numbers::pi));
								range.emplace(double2(0.0, minTheta));
							}
							else {
								if (maxTheta >= 0.0)
								{
									// The blocked range passes over 180Åã and ends at minTheta, split into two parts
									range.emplace(double2(maxTheta, std::numbers::pi));
									oppositeRange.emplace(double2(-minTheta, std::numbers::pi));
								}
								else {
									// The blocked range only occupies the opposite side, so add it there
									oppositeRange.emplace(double2(-maxTheta, -minTheta));
								}
							}
						}
					}
				}
			}
		}
	);
}

void RangeTracker::enter_ranges(Raster& raster, Sphere& Pb, double startingPhi, double R)
{
	if (ranges.size() == 1)
	{
		enter_ranges_for_only_one_bin(raster, Pb, startingPhi, R);
	}
	else {
		enter_ranges_for_multiple_bins(raster, Pb, startingPhi, R);
	}
}

std::pair<int, double> RangeTracker::find_first_free_value(RNG::xoshiro256ss& rngEngine, double maxValue)
{
	std::vector<int> bestBins;
	bestBins.reserve(ranges.size());

	double firstFreeValue = 0.0;
	for (int bin = 0; bin < ranges.size(); ++bin)
	{
		double theta = find_first_free_value(bin, maxValue);
		if (theta > firstFreeValue)
		{
			bestBins.clear();
			bestBins.emplace_back(bin);
			firstFreeValue = theta;
		}
		else if (theta == firstFreeValue) {
			bestBins.emplace_back(bin);
		}
	}

	if (bestBins.size() == 1)
		return std::make_pair(bestBins.front(), firstFreeValue);
	else {
		int chosenBin = bestBins[rngEngine() % bestBins.size()];
		return std::make_pair(chosenBin, firstFreeValue);
	}
}

RangeTracker::OrderedMultisets& RangeTracker::get_ranges()
{
	return ranges;
}

size_t RangeTracker::size()
{
	return ranges.size();
}

