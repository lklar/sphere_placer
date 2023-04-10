#pragma once
#include <vector>
#include "dim_3_vectors.h"

struct Sphere {
	double3 P{ 0 };
	double R{ 0 };

	Sphere(
		double x,
		double y,
		double z,
		double R
	) :
		P(x, y, z),
		R(R)
	{};

	Sphere(
		double3 P,
		double R
	) :
		P(P),
		R(R)
	{};

	Sphere(
		std::vector<double> vec
	)
	{
		if (vec.size() > 4)
		{
			P.x = vec[0];
			P.y = vec[1];
			P.z = vec[2];
			R = vec[3];
		}
	};

	operator std::vector<double>() const
	{
		return std::vector<double>{ P.x, P.y, P.z, R };
	}

	double get_max_z() const
	{
		return P.z + R;
	}
};


class BaseWrapper {
protected:
	std::vector<Sphere>::reverse_iterator m_it;
	std::vector<Sphere>::reverse_iterator m_end;
	std::vector<Sphere>& myVec;

public:
	BaseWrapper(std::vector<Sphere>& vec) :
		m_it(vec.rbegin()),
		m_end(vec.rend()),
		myVec(vec)
	{}

	void operator++()
	{
		++m_it;
	}

	virtual Sphere operator*()
	{
		return *m_it;
	}

	Sphere* operator&()
	{
		return &*m_it;
	}

	bool finished()
	{
		return m_it == m_end;
	}

	virtual double3 offset()
	{
		return double3(0);
	}
};

class OffsetWrapper : public BaseWrapper {
private:
	double3 m_offset;
public:
	OffsetWrapper(std::vector<Sphere>& vec, double3 offset) :
		BaseWrapper(vec),
		m_offset(offset)
	{}

	Sphere operator*()
	{
		Sphere sphere(m_it->P + m_offset, m_it->R);
		return sphere;
	}

	double3 offset()
	{
		return m_offset;
	}
};


class Raster {
public:
	typedef std::vector<Sphere> SphereVector;
	typedef std::vector<std::unique_ptr<BaseWrapper>> SphereVectorRange;
protected:
	double rasterSize;
	int xDim;
	int yDim;
	size_t numberOfSpheres = 0ull;

	std::vector<std::vector<SphereVector>> space;

	int get_spot_id(const double& p);

public:
	Raster(double xMax, double yMax, double R);

	virtual void add(Sphere& sphere);

	virtual SphereVectorRange get_spheres(const double& x, const double& y, int range);

	SphereVector get_all_spheres();
};

class RecursiveRaster : public Raster {
private:
	double xMax;
	double yMax;

public:
	RecursiveRaster(double xMax, double yMax, double R) :
		Raster(xMax, yMax, R),
		xMax(xMax),
		yMax(yMax)
	{}

	void add(Sphere& sphere);

	SphereVectorRange get_spheres(const double& x, const double& y, int range);

};