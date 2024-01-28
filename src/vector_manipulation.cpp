#pragma once
#include "vector_manipulation.h"
#include <algorithm>

int pointerRaster::get_spot_id(const double& p)
{
	return std::floor(p / rasterSize);
}

pointerRaster::pointerRaster(double3 _posMin, double3 _posMax, double _rasterSize)
{
	posMin = _posMin;
	posMax = _posMax;
	rasterSize = _rasterSize;
	xDim = std::ceil((posMax.x - posMin.x) / rasterSize) + 1;
	yDim = std::ceil((posMax.y - posMin.y) / rasterSize) + 1;
	rasterSize = _rasterSize;
	space.resize(xDim,
		std::vector<std::vector<double3*>>(yDim,
			std::vector<double3*>()));
}

void pointerRaster::emplace_back(double3& pos)
{
	int x = get_spot_id(pos.x - posMin.x);
	int y = get_spot_id(pos.y - posMin.y);
	space[x][y].emplace_back(&pos);
}

std::vector<std::reference_wrapper<std::vector<double3*>>> pointerRaster::get_spheres(double3& basePos, int range)
{
	std::vector<std::reference_wrapper<std::vector<double3*>>> columns;

	int xId = get_spot_id(basePos.x - posMin.x);
	int yId = get_spot_id(basePos.y - posMin.y);

	int xMin = std::max(xId - range, 0);
	int xMax = std::min(xId + range, xDim - 1);
	int yMin = std::max(yId - range, 0);
	int yMax = std::min(yId + range, yDim - 1);

	for (int myX = xMin; myX <= xMax; ++myX)
	{
		for (int myY = yMin; myY <= yMax; ++myY)
		{
			if (!space[myX][myY].empty())
			{
				columns.emplace_back(space[myX][myY]);
			}
		}
	}

	return columns;
}

void pointerRaster::pop_back(double3& basePos)
{
	int xId = get_spot_id(basePos.x - posMin.x);
	int yId = get_spot_id(basePos.y - posMin.y);
	space[xId][yId].pop_back();
}

void shift_vector(std::vector<Sphere>& vecIn, double3 shift)
{
	for (auto& elem : vecIn)
	{
		elem.P = elem.P + shift;
	}
}

std::vector<Sphere> cut_out(std::vector<Sphere>& spheres, std::function<bool(const Sphere&)> condition)
{
	std::vector<Sphere> buffer;
	buffer.reserve(spheres.size());
	for (auto& s : spheres)
	{
		if (condition(s))
		{
			buffer.emplace_back(s);
		}
	}
	buffer.shrink_to_fit();
	return buffer;
}

void rotate_points(std::vector<Sphere>& spheres, double3 axis, double angle)
{
	struct quaternion
	{
		double a;
		double3 d3;

		quaternion(double a, double3 d3) :
			a(a),
			d3(d3) {};

		quaternion(double a, double b, double c, double d) :
			a(a),
			d3(b, c, d) {};


		quaternion operator*(const double& other)
		{
			return quaternion(
				other * a,
				other * d3
			);
		}

		quaternion operator*(const quaternion& other)
		{
			return quaternion(
				a * other.a - d3 * other.d3,
				a * other.d3 + other.a * d3 + d3.cross(other.d3)
			);
		}

		quaternion invert()
		{
			double buffer = 1.0 / (a * a + d3.x * d3.x + d3.y * d3.y + d3.z * d3.z);
			return quaternion(
				a,
				d3 * (-1)
			) * buffer;
		}

		quaternion conjugate()
		{
			return quaternion(
				a,
				d3 * (-1)
			);
		}
	};

	double3 ax = axis / abs(axis);

	quaternion rot(
		std::cos(0.5 * angle),
		ax.x * std::sin(0.5 * angle),
		ax.y * std::sin(0.5 * angle),
		ax.z * std::sin(0.5 * angle)
	);

	for (auto& sphere : spheres)
	{
		quaternion newPoint = (rot * quaternion(0, sphere.P)) * rot.conjugate();
		sphere.P = newPoint.d3;
	}
}

std::vector<double3> reduce_to_biggest_group(std::vector<double3>& positions, double R)
{
	auto doOverlap = [&R](double3& p1, double3& p2) {
		double dist = abs((p1 - p2));
		return (dist < 2.000001 * R);
	};

	double3 minPos, maxPos;
	minPos.x = positions.front().x;
	minPos.y = positions.front().y;
	minPos.z = positions.front().z;
	maxPos = minPos;
	for (auto& Ps : positions)
	{
		minPos.x = minPos.x < Ps.x ? minPos.x : Ps.x;
		minPos.y = minPos.y < Ps.y ? minPos.y : Ps.y;
		minPos.z = minPos.z < Ps.z ? minPos.z : Ps.z;
		maxPos.x = maxPos.x > Ps.x ? maxPos.x : Ps.x;
		maxPos.y = maxPos.y > Ps.y ? maxPos.y : Ps.y;
		maxPos.z = maxPos.z > Ps.z ? maxPos.z : Ps.z;
	}

	std::sort(
		positions.begin(),
		positions.end(),
		[](const double3& lhs, const double3& rhs)
		{
			return lhs.z < rhs.z;
		}
	);

	pointerRaster myRaster(minPos, maxPos, 2.0 * R);

	for (auto& pos : positions)
	{
		myRaster.emplace_back(pos);
	}

	std::vector<contactInfo> allContacts;
	allContacts.resize(positions.size());

	for (int i = positions.size() - 1; i >= 0; --i)
	{
		double3 basePos = positions[i];
		myRaster.pop_back(basePos);
		auto neighbouringSpace = myRaster.get_spheres(basePos, 1);

		for (auto& column : neighbouringSpace)
		{
			for (auto it = column.get().rbegin();
				it != column.get().rend() && (*it)->z >= basePos.z - 2.0 * R;
				++it)
			{
				if (doOverlap(basePos, **it))
				{
					auto contactingId = std::distance(&positions.front(), *it);
					allContacts[i].contacts.emplace_back(contactingId);
					allContacts[contactingId].contacts.emplace_back(i);
				}
			}
		}

	}

	int groupId = -1;
	for (int i = 0; i < allContacts.size(); ++i)
	{
		std::vector<int> idsToCheck;

		if (allContacts[i].group == -1)
		{
			idsToCheck.reserve(100);
			idsToCheck.emplace_back(i);
			++groupId;
		}

		while (!idsToCheck.empty())
		{
			int id = idsToCheck.back();
			idsToCheck.pop_back();
			if (allContacts[id].group == -1)
			{
				allContacts[id].group = groupId;
				idsToCheck.insert(idsToCheck.end(), allContacts[id].contacts.begin(), allContacts[id].contacts.end());
			}
		}
	}

	std::vector<std::vector<int>> groups;
	groups.resize(groupId + 1);

	for (int id = 0; id < allContacts.size(); ++id)
	{
		groups[allContacts[id].group].emplace_back(id);
	}

	std::sort(
		groups.begin(),
		groups.end(),
		[](std::vector<int>& lhs, std::vector<int>& rhs)
		{
			return lhs.size() > rhs.size();
		}
	);

	std::vector<double3> biggestGroup;
	biggestGroup.reserve(groups.front().size());
	for (auto& id : groups.front())
	{
		biggestGroup.emplace_back(positions[id]);
	}

	return biggestGroup;
}
