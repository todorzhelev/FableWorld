#pragma once
#include "DirectInput.h"
#include "Terrain.h"
#include "Misc.h"
#include "AABB.h"

//////////////////////////////////////////////////////////////////////////////

enum class ECameraMode
{
	MoveWithoutPressedMouse,
	MoveWithPressedMouse
};

//////////////////////////////////////////////////////////////////////////////

class Camera
{
public:
	Camera(float fFovAngle, float fAspectRatio, float fFrustumNearPlaneZ, float fFrustumFarPlaneZ, bool bIsCameraFree);

	D3DXMATRIX		GetViewMatrix() const;

	D3DXMATRIX		GetProjMatrix() const;

	D3DXMATRIX		GetViewProjMatrix() const;

	D3DXVECTOR3		GetRightVector() const;

	D3DXVECTOR3		GetUpVector() const;

	D3DXVECTOR3		GetLookVector() const;

	D3DXVECTOR3&	GetPosition();

	void			SetPosition(D3DXVECTOR3 vNewPos);

	void			BuildProjectionMatrix(float fFovAngle, float fAspectRatio, float fFrustumNearPlaneZ, float fFrustumFarPlaneZ);

	void			OnUpdate(float dt);

	bool			IsBoundingBoxVisible(AABB& box)const;

	void			SetCameraMode(ECameraMode eCameraMode);

	ECameraMode		GetCameraMode();

	void			MoveCamera(float dt);

	void			RotateRight(float angle);

	void			RotateUp(float angle);

	bool			IsCameraFree() const;

	void			SetCameraFree(bool free);

	void			SetZoom(int zoom);

	void			ModifyZoom(int delta);

	int				GetZoom();

	void			TransformByMatrix(D3DXMATRIX matrix);

private:

	void			BuildFrustumPlanes();

	void			BuildViewMatrix();

	bool		m_bIsCameraFree;

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

	ECameraMode m_eCameraMode;

	int m_maxZoom;
	int m_minZoom;

	int m_zoom;
};

//////////////////////////////////////////////////////////////////////////////

extern Camera* camera;

//////////////////////////////////////////////////////////////////////////////