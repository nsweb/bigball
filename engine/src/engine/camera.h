#include "../bigball.h"

#ifndef BB_CAMERA_H
#define BB_CAMERA_H


namespace bigball
{

class Camera
{
public:
					Camera();
					~Camera();

	enum eParam
	{
		eParam_FOV = 0,
		eParam_ASPECTRATIO,
		eParam_NEARPLANE,
		eParam_FARPLANE,
		eParam_COUNT
	};

	enum eReference
	{
		eReference_CURRENT = 0,
		eReference_DESIRED,
		eReference_COUNT
	};

	void					SetParam( eParam _eParam, float _fValue );
	float						GetParam( eParam _eParm ) const;
	//void					SetUpVector( const P3f& _UpVector, bool _bImmediate = false );
	//const P3f&				GetUpVector( bool _bCurrent = true ) const;
	//void					SetPosition( const P3f& _Position, bool _bImmediate = false );
	const P3f&				GetPosition() const				{ return m_Position;		}
	void					SetYaw( float _fValue, bool _bImmediate = false );
	float						GetYaw( bool _bCurrent = true ) const;
	void					SetPitch( float _fValue, bool _bImmediate = false );
	float						GetPitch( bool _bCurrent = true ) const;
	void					SetTargetDist( float _fValue, bool _bImmediate = false );
	float						GetTargetDist( bool _bCurrent = true ) const;
	/*void					SetTargetPosition( const P3f& _TargetPosition, bool _bImmediate = false );
	const P3f&				GetTargetPosition( bool _bCurrent = true ) const;*/
	const P3f&				GetTargetPosition() const;
	void					SetTarget( const peMatrix& _mTarget, bool _bImmediate = false );
	//const P3f&				GetTarget( bool _bCurrent = true ) const;

	void					Update( float _fDt );

	const D3DXMATRIX&		GetViewMatrix()					{ return m_mView;			}
	const D3DXMATRIX&		GetProjMatrix()					{ return m_mProj;			}
	void					GetViewMatrixVectors( P3f& _Right, P3f& _Up, P3f& _Front );
	

protected:
	//P3f					m_Position[eReference_COUNT];
	//P3f					m_TargetPosition[eReference_COUNT];
	peMatrix			m_mTarget[eReference_COUNT];
	//P3f					m_UpVector[eReference_COUNT];
	float					m_fYaw[eReference_COUNT];
	float					m_fPitch[eReference_COUNT];
	float					m_fTargetDist[eReference_COUNT];

	float					m_fParameters[eParam_COUNT];

	D3DXMATRIX			m_mView;
	D3DXMATRIX			m_mProj;
	P3f					m_Position;
	P3f					m_TargetPosition;

	void					UpdateMatrix();
	//void					UpdateYawPitchTargetDist( eReference _eRef );
	//void					UpdatePosition( eReference _eRef );
};

#endif // BB_CAMERA_H