#pragma once

#include"Application.h"
#include"Misc.h"

class AABB
{
public:

	AABB();

	D3DXVECTOR3 GetCenter() const;
	AABB TransformByMatrix(const D3DXMATRIX& M);
	D3DXVECTOR3& GetMinPoint();
	D3DXVECTOR3& GetMaxPoint();
	bool Collide(const AABB& other);

	D3DXMATRIX  m_transformationMatrix;

private:

	D3DXVECTOR3 minPoint;
	D3DXVECTOR3 maxPoint;
	
};