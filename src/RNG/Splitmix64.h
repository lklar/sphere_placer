/*  Written in 2015 by Sebastiano Vigna ([email protected])

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <math.h>

/* This is a fixed-increment version of Java 8's SplittableRandom generator
   See http://dx.doi.org/10.1145/2714064.2660195 and
   http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html

   It is a very fast generator passing BigCrush, and it can be useful if
   for some reason you absolutely want 64 bits of state. */

namespace RNG
{
	class Splitmix64
	{
	private:
		uint64_t x;

	public:
		Splitmix64();

		Splitmix64(uint64_t seed);

		uint64_t operator()();
	};
}