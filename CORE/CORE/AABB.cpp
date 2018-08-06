#include"AABB.h"

AABB::AABB()
{
	minPoint = D3DXVECTOR3(-INF, -INF, -INF);
	maxPoint = D3DXVECTOR3(INF, INF, INF);
}

D3DXVECTOR3 AABB::GetCenter() const
{
	return (minPoint + maxPoint)*0.5f;
}

D3DXVECTOR3& AABB::GetMinPoint()
{
	return minPoint;
}

D3DXVECTOR3& AABB::GetMaxPoint()
{
	return maxPoint;
}

//transforms bounding box according to a matrix
AABB AABB::TransformByMatrix(const D3DXMATRIX& M)
{
	std::vector<D3DXVECTOR3> corners;
	corners.resize(8);
	//using the min and max points of the untransformed bounding box we 
	//can calculate the 8 corners of the bounding box
	corners[0] = D3DXVECTOR3(minPoint.x, minPoint.y, minPoint.z); // min point
	corners[1] = D3DXVECTOR3(maxPoint.x, minPoint.y, minPoint.z);
	corners[2] = D3DXVECTOR3(minPoint.x, maxPoint.y, minPoint.z);
	corners[3] = D3DXVECTOR3(maxPoint.x, maxPoint.y, minPoint.z);
	corners[4] = D3DXVECTOR3(minPoint.x, minPoint.y, maxPoint.z);
	corners[5] = D3DXVECTOR3(maxPoint.x, minPoint.y, maxPoint.z);
	corners[6] = D3DXVECTOR3(minPoint.x, maxPoint.y, maxPoint.z);
	corners[7] = D3DXVECTOR3(maxPoint.x, maxPoint.y, maxPoint.z); // max point

	//transform the 8 corners of the bounding box by the matrix
	for (auto& corner : corners)
	{
		D3DXVec3TransformCoord(&corner, &corner, &M);
	}

	AABB box;

	//initialize min and max point with the first of the
	//transformed coordinates
	box.minPoint = box.maxPoint = corners[0];

	//after the transformation we dont know which is the min or max point
	for (auto& corner : corners)
	{
		//this way the point could be outside of the bounding box.
		//we assume that after the transformation there will be only one point 
		//with each coordinates smaller than the other points (respectivly for max point)
		if (corner.x < box.minPoint.x)
			box.minPoint.x = corner.x;

		if (corner.x > box.maxPoint.x)
			box.maxPoint.x = corner.x;

		if (corner.y < box.minPoint.y)
			box.minPoint.y = corner.y;

		if (corner.y > box.maxPoint.y)
			box.maxPoint.y = corner.y;

		if (corner.z < box.minPoint.z)
			box.minPoint.z = corner.z;

		if (corner.z > box.maxPoint.z)
			box.maxPoint.z = corner.z;

		/*if (corner.x < box.minPoint.x)
			box.minPoint = corner;

		if (corner.x > box.maxPoint.x)
			box.maxPoint = corner;

		if (corner.y < box.minPoint.y)
			box.minPoint = corner;

		if (corner.y > box.maxPoint.y)
			box.maxPoint = corner;

		if (corner.z < box.minPoint.z)
			box.minPoint = corner;

		if (corner.z > box.maxPoint.z)
			box.maxPoint = corner;*/
	}

	//minPoint = box.minPoint;
	//maxPoint = box.maxPoint;

	//cout << "Min Point:" << "(" << box.minPoint.x << "," << box.minPoint.y << "," << box.minPoint.z << ")" << endl;
	//cout << "Max Point:" << "(" << box.maxPoint.x << "," << box.maxPoint.y << "," << box.maxPoint.z << ")" << endl;
	//cout << endl;
	return box;
}

bool AABB::Collide(const AABB& other)
{
	/*if (maxPoint.x > other.minPoint.x &&
		maxPoint.y > other.minPoint.y &&
		maxPoint.z > other.minPoint.z &&
		minPoint.x < other.maxPoint.x &&
		minPoint.y < other.maxPoint.y &&
		minPoint.z < other.maxPoint.z)
	{
		return true;
	}*/

	if (maxPoint.x < other.minPoint.x || minPoint.x > other.maxPoint.x) return false;
	if (maxPoint.y < other.minPoint.y || minPoint.y > other.maxPoint.y) return false;
	if (maxPoint.z < other.minPoint.z || minPoint.z > other.maxPoint.z) return false;

	return true;
}