

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
	float					GetParam( eParam _eParm ) const;
	//void					SetUpVector( const P3f& _UpVector, bool _bImmediate = false );
	//const P3f&				GetUpVector( bool _bCurrent = true ) const;
	//void					SetPosition( const P3f& _Position, bool _bImmediate = false );
	const vec3&				GetPosition() const				{ return m_Position;		}
	void					SetYaw( float _fValue, bool _bImmediate = false );
	float					GetYaw( bool _bCurrent = true ) const;
	void					SetPitch( float _fValue, bool _bImmediate = false );
	float					GetPitch( bool _bCurrent = true ) const;
	void					SetTargetDist( float _fValue, bool _bImmediate = false );
	float					GetTargetDist( bool _bCurrent = true ) const;
	/*void					SetTargetPosition( const P3f& _TargetPosition, bool _bImmediate = false );
	const P3f&				GetTargetPosition( bool _bCurrent = true ) const;*/
	const vec3&				GetTargetPosition() const;
	void					SetTarget( const mat4& _mTarget, bool _bImmediate = false );
	//const P3f&				GetTarget( bool _bCurrent = true ) const;

	void					Update( float _fDt );

	const mat4&				GetViewMatrix()					{ return m_mView;			}
	const mat4&				GetProjMatrix()					{ return m_mProj;			}
	void					GetViewMatrixVectors( vec3& _Right, vec3& _Up, vec3& _Front );
	

protected:
	//P3f					m_Position[eReference_COUNT];
	//P3f					m_TargetPosition[eReference_COUNT];
	mat4					m_mTarget[eReference_COUNT];
	//P3f					m_UpVector[eReference_COUNT];
	float					m_fYaw[eReference_COUNT];
	float					m_fPitch[eReference_COUNT];
	float					m_fTargetDist[eReference_COUNT];

	float					m_fParameters[eParam_COUNT];

	mat4					m_mView;
	mat4					m_mProj;
	vec3					m_Position;
	vec3					m_TargetPosition;

	void					UpdateMatrix();
	//void					UpdateYawPitchTargetDist( eReference _eRef );
	//void					UpdatePosition( eReference _eRef );
};

} /* namespace bigball */

#endif // BB_CAMERA_H