/*  Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */
#pragma once
#include <stdint.h>

/* This is xoshiro256** 1.0, one of our all-purpose, rock-solid
   generators. It has excellent (sub-ns) speed, a state (256 bits) that is
   large enough for any parallel application, and it passes all tests we
   are aware of.

   For generating just floating-point numbers, xoshiro256+ is even faster.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */

namespace RNG
{
	class xoshiro256ss
	{
	private:
		uint64_t s[4];

		inline uint64_t rotl(const uint64_t x, int k);

		uint64_t next(void);

		void jump(void);

		void long_jump(void);

	public:
		xoshiro256ss();

		xoshiro256ss(uint64_t s1, uint64_t s2, uint64_t s3, uint64_t s4);

		void perform_jumps(uint64_t N);

		void perform_long_jumps(uint64_t N);

		uint64_t operator()();

		double generate_double();

		uint64_t min();

		uint64_t max();
	};

}