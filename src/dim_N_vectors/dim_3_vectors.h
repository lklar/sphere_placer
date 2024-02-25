#pragma once
#include <iostream>
#include <math.h>
#include <vector>

template<typename T>
class dim_3_vector;
#ifndef __VECTOR_TYPES_H__
typedef dim_3_vector<double> double3;
typedef dim_3_vector<int> int3;
typedef dim_3_vector<bool> bool3;
#else
typedef dim_3_vector<double> custom_double3;
typedef dim_3_vector<int> custom_int3;
typedef dim_3_vector<bool> custom_bool3;
#endif

template<typename T>
class dim_3_vector
{
public:
	T x;
	T y;
	T z;

	dim_3_vector() :
		x(0),
		y(0),
		z(0) {}

	dim_3_vector(const dim_3_vector& other) :
		x(other.x),
		y(other.y),
		z(other.z) {}

	dim_3_vector(T c) :
		x(c),
		y(c),
		z(c) {}

	dim_3_vector(T x, T y, T z) :
		x(x),
		y(y),
		z(z) {}

	dim_3_vector(std::vector<T>& vec) :
		x(vec[0]),
		y(vec[1]),
		z(vec[2]) {}
	
	operator std::vector<double>() const 
	{
		return std::vector<double>{ x,y,z };
	}

	inline dim_3_vector operator+(const dim_3_vector& other)
	{
		return dim_3_vector(
			x + other.x,
			y + other.y,
			z + other.z
		);
	}

	template <typename T2>
	inline dim_3_vector operator+(T2 other)
	{
		return dim_3_vector(
			x + other,
			y + other,
			z + other
		);
	}

	inline dim_3_vector operator-(const dim_3_vector& other)
	{
		return dim_3_vector(
			x - other.x,
			y - other.y,
			z - other.z
		);
	}

	template <typename T2>
	inline dim_3_vector operator-(T2 other)
	{
		return dim_3_vector(
			x - other,
			y - other,
			z - other
		);
	}

	// template <typename T2>
	// inline auto operator*(const dim_3_vector<T2>& other)
	// {
	// 	return
	// 		x * other.x +
	// 		y * other.y +
	// 		z * other.z;

	// }

	// template <typename T2>
	// inline dim_3_vector operator*(T2 other)
	// {
	// 	return dim_3_vector(
	// 		x * other,
	// 		y * other,
	// 		z * other
	// 	);
	// }

	template <typename T2>
	inline dim_3_vector cross(dim_3_vector<T2> other)
	{
		return dim_3_vector(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	inline dim_3_vector operator/(const dim_3_vector& other)
	{
		return dim_3_vector(
			x / other.x,
			y / other.y,
			z / other.z
		);
	}

	template <typename O>
	inline dim_3_vector operator/(O other)
	{
		return dim_3_vector(
			x / other,
			y / other,
			z / other
		);
	}

	inline bool operator==(const dim_3_vector& other)
	{
		return
			x == other.x &&
			y == other.y &&
			z == other.z;
	}
};

template <typename T1, typename T2>
inline auto operator+(const dim_3_vector<T1>& lhs, const dim_3_vector<T2>& rhs)
{
	return dim_3_vector<T1>(
		lhs.x + rhs.x,
		lhs.y + rhs.y,
		lhs.z + rhs.z
	);
}

template <typename T1, typename T2>
inline auto operator-(const dim_3_vector<T1>& lhs, const dim_3_vector<T2>& rhs)
{
	return dim_3_vector<T1>(
		lhs.x - rhs.x,
		lhs.y - rhs.y,
		lhs.z - rhs.z
	);
}

template <typename T1, typename T2>
inline auto operator*(T1 lhs, const dim_3_vector<T2>& rhs)
{
	return dim_3_vector<T1>(
		lhs * rhs.x,
		lhs * rhs.y,
		lhs * rhs.z
	);
}

template <typename T1, typename T2>
inline auto operator*(const dim_3_vector<T1>& lhs, const dim_3_vector<T2>& rhs)
{
	return (
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z
	);
}

template <typename T1, typename T2>
inline bool operator<(const dim_3_vector<T1>& lhs, const dim_3_vector<T2>& rhs)
{
	return
		lhs.x < rhs.x &&
		lhs.y < rhs.y &&
		lhs.z < rhs.z;
}

template <typename T1, typename T2>
inline bool operator<=(const dim_3_vector<T1>& lhs, const dim_3_vector<T2>& rhs)
{
	return
		lhs.x <= rhs.x&&
		lhs.y <= rhs.y&&
		lhs.z <= rhs.z;
}

template <typename T1, typename T2>
inline bool operator>(const dim_3_vector<T1>& lhs, const dim_3_vector<T2>& rhs)
{
	return
		lhs.x > rhs.x&&
		lhs.y > rhs.y&&
		lhs.z > rhs.z;
}

template <typename T1, typename T2>
inline bool operator>=(const dim_3_vector<T1>& lhs, const dim_3_vector<T2>& rhs)
{
	return
		lhs.x >= rhs.x &&
		lhs.y >= rhs.y &&
		lhs.z >= rhs.z;
}

inline double abs(const dim_3_vector<double>& d3)
{
	return std::sqrt(d3.x * d3.x + d3.y * d3.y + d3.z * d3.z);
}

inline int abs(const dim_3_vector<int>& i3)
{
	return i3.x + i3.y + i3.z;
}

inline int abs(const dim_3_vector<bool>& b3)
{
	return (int)b3.x + b3.y + b3.z;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const dim_3_vector<T>& d3)
{
	return os << d3.x << "\t" << d3.y << "\t" << d3.z;
}