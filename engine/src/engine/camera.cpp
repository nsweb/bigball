
#include "../bigball.h"
#include "camera.h"

namespace bigball
{

Camera::Camera()
{
	m_fParameters[eParam_FOV] = 50.0f * F_DEG_TO_RAD;
	m_fParameters[eParam_ASPECTRATIO] = 1.333f;
	m_fParameters[eParam_NEARPLANE] = 30.0f;
	m_fParameters[eParam_FARPLANE] = 2500.0f;

	m_mTarget[eReference_CURRENT] = mat4(1.0f);
	m_mTarget[eReference_DESIRED] = mat4(1.0f);

	/*vec3 Up( 0.0f, 0.0f, 1.0f );
	SetUpVector( Up );
	vec3 LookAt( 1.0f, 0.0f, 0.0f );
	SetTargetPosition( LookAt, true );
	vec3 Ref( 0.0f, 0.0f, 0.0f );
	SetPosition( Ref, true );*/
}

Camera::~Camera()
{
}


//-------------------------------------------------------------------------------
void Camera::SetParam( eParam _eParam, float _fValue )
{
	m_fParameters[_eParam] = _fValue;
}

//-------------------------------------------------------------------------------
float Camera::GetParam( eParam _eParam ) const
{
	return m_fParameters[_eParam];
}

//-------------------------------------------------------------------------------
/*void Camera::SetPosition( const vec3& _Position, bool _bImmediate )
{
	m_Position[eReference_DESIRED] = _Position;
	UpdateYawPitchTargetDist( eReference_DESIRED );
	if( _bImmediate ) 
	{
		m_Position[eReference_CURRENT] = _Position;
		UpdateYawPitchTargetDist( eReference_CURRENT );
	}
}

//-------------------------------------------------------------------------------
const vec3& Camera::GetPosition( bool _bCurrent ) const
{
	return ( _bCurrent ? m_Position[eReference_CURRENT] : m_Position[eReference_DESIRED] );
}
*/
//-------------------------------------------------------------------------------
/*void Camera::SetUpVector( const vec3& _UpVector, bool _bImmediate )
{
	m_UpVector[eReference_DESIRED] = _UpVector;
	UpdateYawPitchTargetDist( eReference_DESIRED );
	if( _bImmediate ) 
	{
		m_UpVector[eReference_CURRENT] = _UpVector;
		UpdateYawPitchTargetDist( eReference_CURRENT );
	}
}*/

//-------------------------------------------------------------------------------
/*const vec3& Camera::GetUpVector( bool _bCurrent ) const
{
	return ( _bCurrent ? m_UpVector[eReference_CURRENT] : m_UpVector[eReference_DESIRED] );
}*/

//-------------------------------------------------------------------------------
/*void Camera::SetTargetPosition( const vec3& _TargetPosition, bool _bImmediate )
{
	m_TargetPosition[eReference_DESIRED] = _TargetPosition;
	if( _bImmediate ) 
		m_TargetPosition[eReference_CURRENT] = _TargetPosition;
}
*/
//-------------------------------------------------------------------------------
const vec3& Camera::GetTargetPosition() const
{
	return m_TargetPosition;
}

//-------------------------------------------------------------------------------
void Camera::SetTarget( const mat4& _mTarget, bool _bImmediate )
{
	m_mTarget[eReference_DESIRED] = _mTarget;
	if( _bImmediate ) 
		m_mTarget[eReference_CURRENT] = _mTarget;
}

//-------------------------------------------------------------------------------
void Camera::SetYaw( float _fValue, bool _bImmediate )
{
	m_fYaw[eReference_DESIRED] = _fValue;
	//UpdatePosition( eReference_DESIRED );
	if( _bImmediate ) 
	{
		m_fYaw[eReference_CURRENT] = _fValue;
		//UpdatePosition( eReference_CURRENT );
	}
}
float Camera::GetYaw( bool _bCurrent ) const
{
	return ( _bCurrent ? m_fYaw[eReference_CURRENT] : m_fYaw[eReference_DESIRED] );
}
//-------------------------------------------------------------------------------
void Camera::SetPitch( float _fValue, bool _bImmediate )
{
	m_fPitch[eReference_DESIRED] = _fValue;
	//UpdatePosition( eReference_DESIRED );
	if( _bImmediate ) 
	{
		m_fPitch[eReference_CURRENT] = _fValue;
		//UpdatePosition( eReference_CURRENT );
	}
}
float Camera::GetPitch( bool _bCurrent ) const
{
	return ( _bCurrent ? m_fPitch[eReference_CURRENT] : m_fPitch[eReference_DESIRED] );
}
//-------------------------------------------------------------------------------
void Camera::SetTargetDist( float _fValue, bool _bImmediate )
{
	m_fTargetDist[eReference_DESIRED] = _fValue;
	//UpdatePosition( eReference_DESIRED );
	if( _bImmediate ) 
	{
		m_fTargetDist[eReference_CURRENT] = _fValue;
		//UpdatePosition( eReference_CURRENT );
	}
}
float Camera::GetTargetDist( bool _bCurrent ) const
{
	return ( _bCurrent ? m_fTargetDist[eReference_CURRENT] : m_fTargetDist[eReference_DESIRED] );
}

//-------------------------------------------------------------------------------
void Camera::Tick( float DeltaSeconds )
{
	const float fINTERPOLATIONFACTOR = 4.0f;

	float fPrecalc = ( 1.0f - exp( -DeltaSeconds * fINTERPOLATIONFACTOR ) );

	m_mTarget[eReference_CURRENT] = m_mTarget[eReference_DESIRED];
	m_TargetPosition = m_mTarget[eReference_CURRENT][3].xyz;

	// Lerp on targetdist
	m_fTargetDist[eReference_CURRENT] += fPrecalc*(m_fTargetDist[eReference_DESIRED] - m_fTargetDist[eReference_CURRENT]);

	// Lerp sur le yaw et le pitch
	float fYawDist = m_fYaw[eReference_DESIRED] - m_fYaw[eReference_CURRENT];
	if( fYawDist < 0.0f )	fYawDist += F_PI*2.0f;
	// Prendre le sens de rotation le plus court
	if( fYawDist > F_PI )
		fYawDist = fYawDist - F_PI*2.0f;
	m_fYaw[eReference_CURRENT] = fmod( m_fYaw[eReference_CURRENT] + fPrecalc*fYawDist, F_PI*2.0f );
	//if( m_fYaw[eReference_CURRENT] > F_PI )	
	//	m_fYaw[eReference_CURRENT] -= 2.0f*F_PI;
	if( m_fYaw[eReference_CURRENT] < 0.0f )
		m_fYaw[eReference_CURRENT] += 2.0f*F_PI;


	// Lerp sur le yaw et le pitch
	/*float fYawDist = vYaw[EReference_DESIRED] - vYaw[EReference_CURRENT];
	if( fYawDist < 0.0f )	fYawDist += rwPI*2.0f;
	// Prendre le sens de rotation le plus court
	if( fYawDist > rwPI )
		fYawDist = fYawDist - rwPI*2.0f;
	float fYaw = fmodf( vYaw[EReference_CURRENT] + fPrecalc*fYawDist, rwPI*2.0f );
	if( fYaw > rwPI )	fYaw -= 2.0f*rwPI;*/

	float fPitchDist = m_fPitch[eReference_DESIRED] - m_fPitch[eReference_CURRENT];
	if( fPitchDist < 0.0f )	fPitchDist += F_PI_2;
	// Prendre le sens de rotation le plus court
	if( fPitchDist > F_PI )
		fPitchDist = fPitchDist - F_PI_2;
	m_fPitch[eReference_CURRENT] = fmod( m_fPitch[eReference_CURRENT] + fPrecalc*fPitchDist, F_PI*2.0f );
	if( m_fPitch[eReference_CURRENT] > F_PI )	
		m_fPitch[eReference_CURRENT] -= F_PI_2;

	vec3 v3Dir( cosf( m_fYaw[eReference_CURRENT] )*cosf( m_fPitch[eReference_CURRENT] ), cosf( m_fPitch[eReference_CURRENT] )*sinf( m_fYaw[eReference_CURRENT] ), sinf( m_fPitch[eReference_CURRENT] ) );
	m_Position = /*m_TargetPosition*/ - v3Dir*m_fTargetDist[eReference_CURRENT];

	// Passer en coordonnées world
	m_Position = (m_mTarget[eReference_CURRENT] * vec4( m_Position, 1.0f )).xyz;
	//D3DXVec3TransformCoord( &m_Position, &m_Position, &m_mTarget[eReference_CURRENT] );

	// Mettre à jour les matrices correspondantes
	UpdateMatrix();
}

//-------------------------------------------------------------------------------
/*void Camera::UpdatePosition( eReference _eRef )
{
	//vec3 v3Dir( sinf( m_fYaw[_eRef] )*cosf( m_fPitch[_eRef] ), sinf( m_fPitch[_eRef] ), cosf( m_fYaw[_eRef] )*cosf( m_fPitch[_eRef] ) );
	vec3 v3Dir( cosf( m_fYaw[_eRef] )*cosf( m_fPitch[_eRef] ), cosf( m_fPitch[_eRef] )*sinf( m_fYaw[_eRef] ), sinf( m_fPitch[_eRef] ) );
	m_Position[_eRef] = m_TargetPosition[_eRef] - v3Dir*m_fTargetDist[_eRef];
}

//-------------------------------------------------------------------------------
void Camera::UpdateYawPitchTargetDist( eReference _eRef )
{
	vec3 v3Dir( m_TargetPosition[_eRef] - m_Position[_eRef] );
	m_fTargetDist[_eRef] = D3DXVec3Length( &v3Dir );
	D3DXVec3Normalize( &v3Dir, &v3Dir );
	vec3 v3DirProj( v3Dir.x, 0.0f, v3Dir.z );
	D3DXVec3Normalize( &v3DirProj, &v3DirProj );
	m_fYaw[_eRef] = atan2f( v3DirProj.x, v3DirProj.z );
	m_fPitch[_eRef] = atan2f( v3Dir.y, D3DXVec3Dot( &v3Dir, &v3DirProj ) );
	if( m_fYaw[_eRef] < 0.0f )		m_fYaw[_eRef] = 2.0f*F_PI + m_fYaw[_eRef];
	if( m_fPitch[_eRef] < 0.0f )	m_fPitch[_eRef] = 2.0f*F_PI + m_fPitch[_eRef];
}*/

//## Operation: UpdateRwFrame%3F65ABA6027B
/*void cgCamera::UpdateRwFrame () const
{
	//## begin cgCamera::UpdateRwFrame%3F65ABA6027B.body preserve=yes

	// à l'aide des vecteurs Position & TargetPosition, calcul la matrice de la caméra (lookat)

	if ( NULL != m_pRwFrame)
	{
		RwMatrix *pMatrix = RwFrameGetMatrix( m_pRwFrame);
		RwV3d *pEye = &m_Position[ EReference_CURRENT];
		RwV3d *pAt = &m_TargetPosition[ EReference_CURRENT];
		RwV3d *pUp = &m_UpVector;

		*RwMatrixGetPos( pMatrix) = *pEye;

		RwV3dSub( RwMatrixGetAt( pMatrix), pAt, RwMatrixGetPos( pMatrix));
		RwV3dNormalize( RwMatrixGetAt( pMatrix), RwMatrixGetAt( pMatrix));

		RwV3dCrossProduct( RwMatrixGetRight( pMatrix), RwMatrixGetAt( pMatrix), pUp);
		RwV3dNormalize( RwMatrixGetRight( pMatrix), RwMatrixGetRight( pMatrix));

		RwV3dCrossProduct( RwMatrixGetUp( pMatrix), RwMatrixGetAt( pMatrix), RwMatrixGetRight( pMatrix));
		RwV3dNormalize( RwMatrixGetUp( pMatrix ), RwMatrixGetUp( pMatrix));

		RwMatrixUpdate( pMatrix);
		RwMatrixOrthoNormalize( pMatrix, pMatrix);
		RwFrameUpdateObjects( m_pRwFrame);
	}

	// maj. des propriétés de la caméra RenderWare en fonction de celles-ci

	RwCamera *pRwCamera = dtgRwGraphics::GetCamera();

	RwCameraSetNearClipPlane( pRwCamera, m_fParameters[ EParm_NEARPLANE]);
	RwCameraSetFarClipPlane( pRwCamera, m_fParameters[ EParm_FARPLANE]);

	// [RenderWare] Note that the view-window is fixed at unit distance from the camera and cannot be moved.
	// In this situation changes to the aspect ratio and angular field-of-view of the rendered image can be achieved by defining new values for the width and height of the view-window

	RwV2d temp;
	temp.y = m_fParameters[EParm_FOV];
	temp.x = m_fParameters[EParm_FOV] * m_fParameters[EParm_ASPECTRATIO];

	RwCameraSetViewWindow(pRwCamera, &temp);

	//## end cgCamera::UpdateRwFrame%3F65ABA6027B.body
}*/

//-------------------------------------------------------------------------------
void Camera::UpdateMatrix()
{
	m_mProj.perspective( m_fParameters[eParam_FOV] / m_fParameters[eParam_ASPECTRATIO], m_fParameters[eParam_ASPECTRATIO], m_fParameters[eParam_NEARPLANE], m_fParameters[eParam_FARPLANE] );
	//D3DXMatrixPerspectiveFovRH( &m_mProj, m_fParameters[eParam_FOV] / m_fParameters[eParam_ASPECTRATIO], m_fParameters[eParam_ASPECTRATIO], m_fParameters[eParam_NEARPLANE], m_fParameters[eParam_FARPLANE] );
	//vec3 UpVector( m_mTarget[eReference_CURRENT]._31, m_mTarget[eReference_CURRENT]._32, m_mTarget[eReference_CURRENT]._33 );
	//D3DXMatrixLookAtRH( &m_mView, &m_Position, &m_TargetPosition/*v3LookAt*/, &UpVector /*&m_UpVector[eReference_CURRENT]*/ );
	vec3 UpVector( m_mTarget[eReference_CURRENT][2].xyz );
	m_mView.lookat( m_Position, m_TargetPosition, UpVector );
}

//-------------------------------------------------------------------------------
void Camera::GetViewMatrixVectors( vec3& _Right, vec3& _Up, vec3& _Front )
{
	mat4 ViewT( transpose( m_mView ) );
	_Right = ViewT[0].xyz;
	_Up = ViewT[1].xyz;
	_Front = ViewT[2].xyz;

	//_Right.x = m_mView._11;		_Right.y = m_mView._21;		_Right.z = m_mView._31;
	//_Up.x = m_mView._12;		_Up.y = m_mView._22;		_Up.z = m_mView._32;
	//_Front.x = -m_mView._13;	_Front.y = -m_mView._23;	_Front.z = -m_mView._33;

	//_Pos.x = m_mView._41;		_Pos.y = m_mView._42;		_Pos.z = m_mView._43;
}


void Camera::Create( EntityPattern* Pattern, class tinyxml2::XMLDocument* Proto )
{
	Super::Create( Pattern, Proto );
}
void Camera::Destroy()
{
	Super::Destroy();
}
void Camera::AddToWorld()
{
	Super::AddToWorld();
}
void Camera::RemoveFromWorld()
{
	Super::RemoveFromWorld();
}

} /* namespace bigball */