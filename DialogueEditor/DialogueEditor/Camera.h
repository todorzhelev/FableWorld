#pragma once
#include "DirectInput.h"
//#include "Terrain.h"
#include "Misc.h"

//////////////////////////////////////////////////////////////////////////////

enum ECameraMode
{
	ECameraMode_MoveWithoutPressedMouse,
	ECameraMode_MoveWithPressedMouse
};

//////////////////////////////////////////////////////////////////////////////

class Camera
{
public:
	Camera(float fFovAngle, float fAspectRatio, float fFrustumNearPlaneZ, float fFrustumFarPlaneZ);

	D3DXMATRIX		GetViewMatrix() const;

	D3DXMATRIX		GetProjMatrix() const;

	D3DXMATRIX		GetViewProjMatrix() const;

	D3DXVECTOR3&	GetRightVector();

	D3DXVECTOR3&	GetUpVector();

	D3DXVECTOR3&	GetLookVector();

	D3DXVECTOR3&	GetPosition();

	D3DXVECTOR3&	GetOffset();

	void			BuildProjectionMatrix(float fFovAngle, float fAspectRatio, float fFrustumNearPlaneZ, float fFrustumFarPlaneZ);

	void			OnUpdate(float dt);

	bool			IsBoundingBoxVisible(const AABB& box)const;

	void			SetCameraMode(ECameraMode eCameraMode);

	ECameraMode		GetCameraMode();

	void		    MoveCamera(float dt);

private:

	void			BuildFrustumPlanes();

	void			BuildViewMatrix();

private:

	float		m_fFovAngle;
	float		m_fAspectRatio;
	float		m_fFrustumNearPlaneZ;
	float		m_fFrustumFarPlaneZ;

	D3DXPLANE  m_FrustumPlanes[6];

	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjectionMatrix;
	D3DXMATRIX m_ViewProjectionMatrix;

	D3DXVECTOR3 m_vPosition;
	D3DXVECTOR3 m_vRightVector;
	D3DXVECTOR3 m_vUpVector;
	D3DXVECTOR3 m_vLookVector;

	//the offset from a certain object to the camera
	D3DXVECTOR3 m_vOffset;

	ECameraMode m_eCameraMode;
};

//////////////////////////////////////////////////////////////////////////////

extern Camera* camera;

//////////////////////////////////////////////////////////////////////////////