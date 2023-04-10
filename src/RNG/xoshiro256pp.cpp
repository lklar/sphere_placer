/*  Written in 2019 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

#pragma once
#include "xoshiro256pp.h"

/* This is xoshiro256++ 1.0, one of our all-purpose, rock-solid generators.
   It has excellent (sub-ns) speed, a state (256 bits) that is large
   enough for any parallel application, and it passes all tests we are
   aware of.

   For generating just floating-point numbers, xoshiro256+ is even faster.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */

namespace RNG
{

	inline uint64_t xoshiro256pp::rotl(const uint64_t x, int k) {
		return (x << k) | (x >> (64 - k));
	}

	uint64_t xoshiro256pp::next(void) {
		const uint64_t result = rotl(s[0] + s[3], 23) + s[0];

		const uint64_t t = s[1] << 17;

		s[2] ^= s[0];
		s[3] ^= s[1];
		s[1] ^= s[2];
		s[0] ^= s[3];

		s[2] ^= t;

		s[3] = rotl(s[3], 45);

		return result;
	}

	void xoshiro256pp::jump(void) {
		static const uint64_t JUMP[] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };

		uint64_t s0 = 0;
		uint64_t s1 = 0;
		uint64_t s2 = 0;
		uint64_t s3 = 0;
		for (int i = 0; i < sizeof JUMP / sizeof * JUMP; i++)
			for (int b = 0; b < 64; b++) {
				if (JUMP[i] & UINT64_C(1) << b) {
					s0 ^= s[0];
					s1 ^= s[1];
					s2 ^= s[2];
					s3 ^= s[3];
				}
				next();
			}

		s[0] = s0;
		s[1] = s1;
		s[2] = s2;
		s[3] = s3;
	}

	void xoshiro256pp::long_jump(void) {
		static const uint64_t LONG_JUMP[] = { 0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635 };

		uint64_t s0 = 0;
		uint64_t s1 = 0;
		uint64_t s2 = 0;
		uint64_t s3 = 0;
		for (int i = 0; i < sizeof LONG_JUMP / sizeof * LONG_JUMP; i++)
			for (int b = 0; b < 64; b++) {
				if (LONG_JUMP[i] & UINT64_C(1) << b) {
					s0 ^= s[0];
					s1 ^= s[1];
					s2 ^= s[2];
					s3 ^= s[3];
				}
				next();
			}

		s[0] = s0;
		s[1] = s1;
		s[2] = s2;
		s[3] = s3;
	}

	xoshiro256pp::xoshiro256pp()
	{
		s[0] = 0ull;
		s[1] = 0ull;
		s[2] = 0ull;
		s[3] = 0ull;
	}

	xoshiro256pp::xoshiro256pp(uint64_t s1, uint64_t s2, uint64_t s3, uint64_t s4)
	{
		s[0] = s1;
		s[1] = s2;
		s[2] = s3;
		s[3] = s4;
	}

	void xoshiro256pp::perform_jumps(uint64_t N)
	{
		for (int i = 0; i < N; ++i)
		{
			jump();
		}
	}

	void xoshiro256pp::perform_long_jumps(uint64_t N)
	{
		for (int i = 0; i < N; ++i)
		{
			long_jump();
		}
	}

	uint64_t xoshiro256pp::operator()()
	{
		return next();
	}
}