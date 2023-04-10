#pragma once
#include "Splitmix64.h"
#include "xoshiro256pp.h"
#include "xoshiro256ss.h"

namespace RNG
{
	xoshiro256pp make_xoshiro256pp(uint64_t seed);

	xoshiro256pp make_xoshiro256pp(uint64_t seed, uint64_t sequence);

	xoshiro256ss make_xoshiro256ss(uint64_t seed);

	xoshiro256ss make_xoshiro256ss(uint64_t seed, uint64_t sequence);
	
	struct uniform_real_distribution
	{
	private:
		xoshiro256ss rngEngine;
		double min = 0.0;
		double range = 1.0;

	public:
		uniform_real_distribution();

		uniform_real_distribution(uint64_t seed);

		uniform_real_distribution(uint64_t seed, double minValue, double maxValue);

		double operator()();
		
		double operator()(double minValue, double maxValue);
	};
}