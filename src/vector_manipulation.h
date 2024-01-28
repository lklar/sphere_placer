#pragma once
#include "dim_N_vectors/dim_3_vectors.h"
#include <functional>
#include "Raster.h"

void shift_vector(std::vector<Sphere>& vecIn, double3 shift);

std::vector<Sphere> cut_out(std::vector<Sphere>& spheres, std::function<bool(const Sphere&)> condition);

void rotate_points(std::vector<Sphere>& spheres, double3 axis, double angle);

struct pointerRaster {
private:
	double rasterSize;
	double3 posMin;
	double3 posMax;
	int xDim;
	int yDim;
	std::vector<std::vector<std::vector<double3*>>> space;

	int get_spot_id(const double& p);

public:
	pointerRaster(double3 _posMin, double3 _posMax, double _rasterSize);

	void emplace_back(double3& pos);

	std::vector<std::reference_wrapper<std::vector<double3*>>> get_spheres(double3& basePos, int range);

	void pop_back(double3& basePos);
};

struct contactInfo {
	int group = -1;
	std::vector<int> contacts;
};

std::vector<double3> reduce_to_biggest_group(std::vector<double3>& positions, double R);