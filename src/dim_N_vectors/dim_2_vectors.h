#pragma once
#include <iostream>
#include <math.h>
#include <vector>

template<typename T>
class dim_2_vector;

typedef dim_2_vector<double> double2;
typedef dim_2_vector<bool> bool2;
typedef dim_2_vector<int> int2;

template<typename T>
class dim_2_vector
{
public:
	T x;
	T y;

	dim_2_vector() :
		x(0),
		y(0) {}

	dim_2_vector(const dim_2_vector& other) :
		x(other.x),
		y(other.y) {}

	dim_2_vector(T x, T y) :
		x(x),
		y(y) {}

	dim_2_vector(std::vector<T>& vec) :
		x(vec[0]),
		y(vec[1]) {}

	inline dim_2_vector operator+(const dim_2_vector& other)
	{
		return dim_2_vector(
			x + other.x,
			y + other.y
		);
	}

	template <typename T2>
	inline dim_2_vector operator+(T2 other)
	{
		return dim_2_vector(
			x + other,
			y + other
		);
	}

	inline dim_2_vector operator-(const dim_2_vector& other)
	{
		return dim_2_vector(
			x - other.x,
			y - other.y
		);
	}

	template <typename T2>
	inline dim_2_vector operator-(T2 other)
	{
		return dim_2_vector(
			x - other,
			y - other
		);
	}

	template <typename T2>
	inline auto operator*(const dim_2_vector<T2>& other)
	{
		return
			x * other.x +
			y * other.y;

	}

	template <typename O>
	inline dim_2_vector operator*(O other)
	{
		return dim_2_vector(
			x * other,
			y * other
		);
	}

	inline dim_2_vector operator/(const dim_2_vector& other)
	{
		return dim_2_vector(
			x / other.x,
			y / other.y
		);
	}

	template <typename O>
	inline dim_2_vector operator/(O other)
	{
		return dim_2_vector(
			x / other,
			y / other
		);
	}
};

inline double abs(const double2& d2)
{
	return std::sqrt(d2.x * d2.x + d2.y * d2.y);
}

inline int abs(const int2& i2)
{
	return i2.x + i2.y;
}

inline int abs(const bool2& b2)
{
	return (int)b2.x + b2.y;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const dim_2_vector<T>& d3)
{
	return os << d3.x << "\t" << d3.y;
}