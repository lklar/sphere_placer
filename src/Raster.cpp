#include "Raster.h"
#include <algorithm>

Raster::Raster(double xMax, double yMax, double R)
{
	yDim = std::floor(yMax / (2.0 * R));
	rasterSize = yMax / (double)yDim;
	xDim = std::ceil(xMax / rasterSize);

	space.resize(xDim, std::vector<SphereVector>(yDim, SphereVector()));

	for (auto& row : space)
	{
		for (auto& col : row)
		{
			col.reserve(100);
		}
	}
}

int Raster::get_spot_id(const double& p)
{
	return std::floor(p / rasterSize);
}

void Raster::add(Sphere& sphere)
{
	int x = get_spot_id(sphere.P.x);
	int y = get_spot_id(sphere.P.y);

	auto& sv = space[x][y];
	auto found = std::lower_bound(
		sv.rbegin(),
		sv.rend(),
		sphere,
		[](const Sphere& lhs, const Sphere& rhs) { return lhs.get_max_z() > rhs.get_max_z(); }
	);
	sv.insert(found.base(), sphere);
	++numberOfSpheres;
}

Raster::SphereVectorRange Raster::get_spheres(const double& x, const double& y, int range)
{
	SphereVectorRange svr;

	int xId = get_spot_id(x);
	int yId = get_spot_id(y);

	int xMin = std::max(xId - range, 0);
	int xMax = std::min(xId + range, xDim - 1);
	int yMin = std::max(yId - range, 0);
	int yMax = std::min(yId + range, yDim - 1);

	if (!space[xId][yId].empty())
		svr.emplace_back(new BaseWrapper(space[xId][yId]));

	for (int myX = xMin; myX <= xMax; ++myX)
	{
		for (int myY = yMin; myY <= yMax; ++myY)
		{
			if (
				!space[myX][myY].empty() &&
				(myX != xId || myY != yId)
				)
			{
				svr.emplace_back(new BaseWrapper(space[myX][myY]));
			}
		}
	}

	return svr;
}

Raster::SphereVector Raster::get_all_spheres()
{
	SphereVector allSpheres;
	allSpheres.reserve(numberOfSpheres);
	for (auto& row : space)
	{
		for (auto& col : row)
		{
			for (auto& pos : col)
			{
				allSpheres.emplace_back(pos);
			}
		}
	}
	return allSpheres;
}

void RecursiveRaster::add(Sphere& sphere)
{

	sphere.P.x -= std::floor(sphere.P.x / xMax) * xMax;
	sphere.P.y -= std::floor(sphere.P.y / yMax) * yMax;
	int x = get_spot_id(sphere.P.x);
	int y = get_spot_id(sphere.P.y);

	auto& sv = space[x][y];
	auto found = std::lower_bound(
		sv.rbegin(),
		sv.rend(),
		sphere,
		[](const Sphere& lhs, const Sphere& rhs) { return lhs.get_max_z() > rhs.get_max_z(); }
	);
	sv.insert(found.base(), sphere);
	++numberOfSpheres;
}

Raster::SphereVectorRange RecursiveRaster::get_spheres(const double& x, const double& y, int range)
{
	SphereVectorRange svr;

	int xId = get_spot_id(x);
	int yId = get_spot_id(y);

	if(!space[xId][yId].empty())
		svr.emplace_back(new BaseWrapper(space[xId][yId]));
	for (int dx = -range; dx <= range; ++dx)
	{
		int myX = xId + dx;
		double3 offset(0);
		if (myX < 0)
		{
			offset.x = -xMax;
			myX += xDim;
		} else if (myX >= xDim)
		{
			offset.x = xMax;
			myX -= xDim;
		}

		for (int dy = -range; dy <= range; ++dy)
		{
			int myY = yId + dy;
			if (myY < 0)
			{
				offset.y = -yMax;
				myY += yDim;
			} else if (myY >= yDim)
			{
				offset.y = yMax;
				myY -= yDim;
			}
			
			if (
				!space[myX][myY].empty() &&
				(myX != xId || myY != yId)
				)
			{
				if (offset == double3(0))
				{
					svr.emplace_back(new BaseWrapper(space[myX][myY]));
				}
				else {
					svr.emplace_back(new OffsetWrapper(space[myX][myY], offset));
				}

			}
		}
	}

	return svr;
}