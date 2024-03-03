#pragma once
#include <vector>
#include "dim_N_vectors/dim_2_vectors.h"
#include "dim_N_vectors/dim_3_vectors.h"
#include "RNG/RNG.h"
#include "Raster.h"
#include <set>
#include <numbers>

struct OrderDouble2ByY {
	bool operator() (const double2& lhs, const double2& rhs) const
	{
		return lhs.y > rhs.y;
	}
};

class RangeTracker
{
	typedef std::vector<std::multiset<double2, OrderDouble2ByY>> OrderedMultisets;

private:
	OrderedMultisets ranges;

	double find_first_free_value(int bin, double maxValue);

	void enter_ranges_for_only_one_bin(Raster& raster, Sphere& Pb, double phi, double R);

	void enter_ranges_for_multiple_bins(Raster& raster, Sphere& Pb, double startingPhi, double R);

public:
	RangeTracker(int nmbrOfBins);

	void enter_range(int bin, double2 newRange);

	void enter_ranges(Raster& raster, Sphere& Pb, double startingPhi, double R);

	std::pair<int, double> find_first_free_value(RNG::xoshiro256ss& rngEngine, double maxValue);

	OrderedMultisets& get_ranges();

	size_t size();
};

