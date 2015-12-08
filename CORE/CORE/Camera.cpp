#include "Camera.h"

/////////////////////////////////////////////////////////////////////////

Camera* camera = NULL;

/////////////////////////////////////////////////////////////////////////
/*
Function:Camera()
Purpose:constructor.The inititial values for the camera's vectors , the speed of the camera and the cammera offset are set
*/
Camera::Camera(float fFovAngle, float fAspectRatio, float fFrustumNearPlaneZ, float fFrustumFarPlaneZ, bool bIsCameraFree) 
	: m_fFovAngle(fFovAngle)
	, m_fAspectRatio(fAspectRatio)
	, m_fFrustumNearPlaneZ(fFrustumNearPlaneZ)
	, m_fFrustumFarPlaneZ(fFrustumFarPlaneZ)
	, m_bIsCameraFree(bIsCameraFree)
	, m_zoom(-35)
	, m_maxZoom(-80)
	, m_minZoom(-24)
{
	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjectionMatrix);
	D3DXMatrixIdentity(&m_ViewProjectionMatrix);

	m_vPosition	   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRightVector = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vUpVector	   = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_vLookVector  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	BuildProjectionMatrix(fFovAngle,fAspectRatio,fFrustumNearPlaneZ,fFrustumFarPlaneZ);
}

/////////////////////////////////////////////////////////////////////////
/*
Function:getViewMatrix()
Purpose:returns the view matrix for the camera
*/
D3DXMATRIX Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:getProjMatrix
Purpose:returns the projection matrix for the camera
*/
D3DXMATRIX Camera::GetProjMatrix() const
{
	return m_ProjectionMatrix;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:getViewProjMatrix()
Purpose:returns the combined matrix from proj and view matrices
*/
D3DXMATRIX Camera::GetViewProjMatrix() const
{
	return m_ViewProjectionMatrix;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:getRightVector()
Purpose:returns the right vector of the camera
*/
D3DXVECTOR3& Camera::GetRightVector()
{
	return m_vRightVector;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:getUpVector()
Purpose:returns the up vector of the camera
*/
D3DXVECTOR3& Camera::GetUpVector() 
{
	return m_vUpVector;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:getLookVector()
Purpose:return the look vector of the camera
*/
D3DXVECTOR3& Camera::GetLookVector() 
{
	return m_vLookVector;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:pos()
Purpose:returns the current position of the camera
*/
D3DXVECTOR3& Camera::GetPosition()
{
	return m_vPosition;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:pos()
Purpose:returns the current position of the camera
*/
void Camera::SetPosition(D3DXVECTOR3 vNewPos)
{
	m_vPosition = vNewPos;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:setLens()
Purpose:builds the projection matrix
*/
void Camera::BuildProjectionMatrix(float fFovAngle, float fAspectRatio, float fFrustumNearPlaneZ, float fFrustumFarPlaneZ)
{
	//build projection matrix, based on the field of view angle. 
	//The projection matrix is used for converting from view space to screen space
	//nearZ sets the value of the near plane, i.e. everything with smaller value than this wont be seen
	//farZ sets the value of the far plane, i.e. everything with bigger value than this wont be seen
	D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix, fFovAngle, fAspectRatio, fFrustumNearPlaneZ, fFrustumFarPlaneZ);
	BuildFrustumPlanes();
	m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:update()
Purpose:updates the camera position
*/
void Camera::OnUpdate(float dt)
{
	if( m_bIsCameraFree )
	{
		MoveCamera(dt);
	}

	//calculations for the camera vectors are made in Game.cpp, where camera is attached to model in the game.
	BuildViewMatrix();
	BuildFrustumPlanes();
	m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:isVisible()
Purpose:tests if certain bounding box is visible by the camera
*/
bool Camera::IsBoundingBoxVisible(AABB& box) const
{
	D3DXVECTOR3 Q;
	for(int i = 0; i < 6; ++i)
	{
		//For each coordinate x,y,z
		for(int j = 0; j < 3; ++j)
		{
			if( m_FrustumPlanes[i][j] >= 0.0f )
			{
				Q[j] = box.GetMaxPoint()[j];
			}
			else 
			{
				Q[j] = box.GetMinPoint()[j];
			}
		}
		//computes the dot product between the current plane and the point that it is assumed to be in the positive half-space of the plane
		//if the dot product equals zero it means the point is on the plane.
		//if the dot product is less than zero means that the point is negative half space -> its not visible
		//if the dot product is bigger than zero means that the points is in positive half space -> its visible
		if( D3DXPlaneDotCoord(&m_FrustumPlanes[i], &Q) < 0.0f  )
		{
			return false;
		}
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:buildView()
Purpose:builds the view matrix. View matrix represents the inverse matrix of the world matrix.
		Initally all the models in the game are transformed according to the world matrix, so they can be in same space.
		After that we transform all the models according to view matrix, so they can be in view space.
		Later we are transforming them according to a projection matrix, so they can be rendered in the screen.
*/
void Camera::BuildViewMatrix()
{
	//these calcualtions before the filling the matrix are done because 
	//of floating point errors which can cause the vectors to be non-orthogonal
	D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);

	D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector);
	D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);

	D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector);
	D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);

	float x = -D3DXVec3Dot(&m_vPosition, &m_vRightVector);
	float y = -D3DXVec3Dot(&m_vPosition, &m_vUpVector);
	float z = -D3DXVec3Dot(&m_vPosition, &m_vLookVector);

	m_ViewMatrix(0,0) = m_vRightVector.x; 
	m_ViewMatrix(1,0) = m_vRightVector.y; 
	m_ViewMatrix(2,0) = m_vRightVector.z; 
	m_ViewMatrix(3,0) = x;   

	m_ViewMatrix(0,1) = m_vUpVector.x;
	m_ViewMatrix(1,1) = m_vUpVector.y;
	m_ViewMatrix(2,1) = m_vUpVector.z;
	m_ViewMatrix(3,1) = y;  

	m_ViewMatrix(0,2) = m_vLookVector.x; 
	m_ViewMatrix(1,2) = m_vLookVector.y; 
	m_ViewMatrix(2,2) = m_vLookVector.z; 
	m_ViewMatrix(3,2) = z;   

	m_ViewMatrix(0,3) = 0.0f;
	m_ViewMatrix(1,3) = 0.0f;
	m_ViewMatrix(2,3) = 0.0f;
	m_ViewMatrix(3,3) = 1.0f;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:buildFrustumPlanes
Purpose:the camera in the game is represented by a frustum - clipped pyramid. The pyramid is built by 6 planes
	    which can be extracted directly from the combined view and proj matrix.
*/
void Camera::BuildFrustumPlanes()
{
	D3DXMATRIX ViewProj = m_ViewMatrix * m_ProjectionMatrix;

	D3DXVECTOR4 vCol1(ViewProj(0,0), ViewProj(1,0), ViewProj(2,0), ViewProj(3,0));
	D3DXVECTOR4 vCol2(ViewProj(0,1), ViewProj(1,1), ViewProj(2,1), ViewProj(3,1));
	D3DXVECTOR4 vCol3(ViewProj(0,2), ViewProj(1,2), ViewProj(2,2), ViewProj(3,2));
	D3DXVECTOR4 vCol4(ViewProj(0,3), ViewProj(1,3), ViewProj(2,3), ViewProj(3,3));

	m_FrustumPlanes[0] = (D3DXPLANE)(vCol3);        // near plane
	m_FrustumPlanes[1] = (D3DXPLANE)(vCol4 - vCol3); // far plane
	m_FrustumPlanes[2] = (D3DXPLANE)(vCol4 + vCol1); // left plane
	m_FrustumPlanes[3] = (D3DXPLANE)(vCol4 - vCol1); // right plane
	m_FrustumPlanes[4] = (D3DXPLANE)(vCol4 - vCol2); // top plane
	m_FrustumPlanes[5] = (D3DXPLANE)(vCol4 + vCol2); // bottom plane

	for(int i = 0; i < 6; i++)
	{
		D3DXPlaneNormalize(&m_FrustumPlanes[i], &m_FrustumPlanes[i]);
	}
}

/////////////////////////////////////////////////////////////////////////

void Camera::SetCameraMode(ECameraMode eCameraMode)
{
	m_eCameraMode = eCameraMode;
}

/////////////////////////////////////////////////////////////////////////

ECameraMode Camera::GetCameraMode()
{
	return m_eCameraMode;
}

/////////////////////////////////////////////////////////////////////////

//moves the camera according to the pressed buttons
void Camera::MoveCamera(float dt)
{
	//this vector holds the new direction to move
	D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);
	
	if( pDinput->IsKeyDown(DIK_W) )
	{
		dir += m_vLookVector;
	}
	if( pDinput->IsKeyDown(DIK_S) )
	{
		dir -= m_vLookVector;
	}
	if( pDinput->IsKeyDown(DIK_A) )
	{
		dir -= m_vRightVector;
	}
	if( pDinput->IsKeyDown(DIK_D) )
	{
		dir += m_vRightVector;
	}

	
	D3DXVECTOR3 newPos = m_vPosition + dir*100.0*dt;
	m_vPosition = newPos;

	if( pDinput->IsMouseButtonDown(0) )
	{

		float pitch  = pDinput->GetMouseDY() / 150.0;
		float yAngle = pDinput->GetMouseDX() / 150.0f;


		//when we move the mouse up/down we will rotate the camera around the right vector as axis
		D3DXMATRIX R;
		D3DXMatrixRotationAxis(&R, &m_vRightVector, pitch);
		D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);
		D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);


		// Rotate camera axes about the world's y-axis.
		D3DXMatrixRotationY(&R, yAngle);
		D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);
		D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);
		D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);
	}
}

/////////////////////////////////////////////////////////////////////////

bool Camera::IsCameraFree() const
{
	return m_bIsCameraFree;
}

void Camera::SetCameraFree(bool free)
{
	m_bIsCameraFree = free;
}

void Camera::SetZoom(int zoom)
{
	m_zoom = zoom;
}

void Camera::ModifyZoom(int delta)
{
	if( m_zoom + delta > m_minZoom)
	{
		m_zoom = m_minZoom;
		return;
	}

	if( m_zoom + delta < m_maxZoom )
	{
		m_zoom = m_maxZoom;
		return;
	}

	m_zoom += delta;
}

int Camera::GetZoom()
{
	return m_zoom;
}
