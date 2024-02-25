#pragma once
#include "RNG.h"

namespace RNG
{
	xoshiro256pp make_xoshiro256pp(uint64_t seed)
	{
		Splitmix64 s64(seed);
		xoshiro256pp newGenerator(s64(), s64(), s64(), s64());
		return newGenerator;
	}

	xoshiro256pp make_xoshiro256pp(uint64_t seed, uint64_t sequence)
	{
		Splitmix64 s64(seed);
		xoshiro256pp newGenerator(s64(), s64(), s64(), s64());
		newGenerator.perform_jumps(sequence);

		return newGenerator;
	}

	xoshiro256ss make_xoshiro256ss(uint64_t seed)
	{
		Splitmix64 s64(seed);
		xoshiro256ss newGenerator(s64(), s64(), s64(), s64());
		return newGenerator;
	}

	xoshiro256ss make_xoshiro256ss(uint64_t seed, uint64_t sequence)
	{
		Splitmix64 s64(seed);
		xoshiro256ss newGenerator(s64(), s64(), s64(), s64());
		newGenerator.perform_jumps(sequence);

		return newGenerator;
	}
	

	uniform_real_distribution::uniform_real_distribution()
	{
		rngEngine = make_xoshiro256ss(0ull);
	}

	uniform_real_distribution::uniform_real_distribution(uint64_t seed)
	{
		rngEngine = make_xoshiro256ss(seed);
	}

	uniform_real_distribution::uniform_real_distribution(uint64_t seed, double minValue, double maxValue)
	{
		rngEngine = make_xoshiro256ss(seed);
		min = minValue;
		range = maxValue - minValue;
	}

	double uniform_real_distribution::operator()()
	{
		return min + rngEngine.generate_double() * range;
	}

	double uniform_real_distribution::operator()(double minValue, double maxValue)
	{
		return minValue + rngEngine.generate_double() * (maxValue - minValue);
	}
}