

#ifndef BB_CAMERA_H
#define BB_CAMERA_H

#include "entity.h"

namespace bigball
{

enum BIGBALL_API eCameraParam
{
	eCP_FOV = 0,
	eCP_ASPECTRATIO,
	eCP_NEARPLANE,
	eCP_FARPLANE,
	eCP_MAX
};

struct BIGBALL_API CameraView
{
	dvec3	m_Eye;
	quat	m_Rotation;
	float	m_fParameters[eCP_MAX];
};

class BIGBALL_API Camera : public Entity
{
private:
	typedef Entity Super;

public:
					Camera();
					~Camera();

	// Begin : Entity interface
	static Entity*			NewEntity()		{ return new Camera();	}
	virtual void			Create( EntityPattern* Pattern, class tinyxml2::XMLDocument* Proto = nullptr );
	virtual void			Destroy();	
	virtual void			AddToWorld();
	virtual void			RemoveFromWorld();
	virtual void			Tick( float DeltaSeconds );
	// End : Entity interface

protected:
	CameraView			m_View;
};

class BIGBALL_API CameraBehavior_Base
{
public:
	virtual char const*	GetBehaviorName()	{ return "Base";	}
	virtual bool		IsA( Name const& BehName )	{ if( BehName == GetBehaviorName() ) return true; return false; }
};

class BIGBALL_API CameraBehavior_Fly : public CameraBehavior_Base
{
private:
	typedef CameraBehavior_Base Super;
public:
	virtual char const*	GetBehaviorName()	{ return "Fly";		}
	virtual bool		IsA( Name const& BehName )	{ if( BehName == GetBehaviorName() ) return true; return Super::IsA( BehName ); }
};

#if 0
class Camera : public Entity
{
private:
	typedef Entity Super;

public:
					Camera();
					~Camera();

	enum eReference
	{
		eReference_CURRENT = 0,
		eReference_DESIRED,
		eReference_COUNT
	};

	// Begin : Entity interface
	static Entity*			NewEntity()		{ return new Camera();	}
	virtual void			Create( EntityPattern* Pattern, class tinyxml2::XMLDocument* Proto = nullptr );
	virtual void			Destroy();	
	virtual void			AddToWorld();
	virtual void			RemoveFromWorld();
	virtual void			Tick( float DeltaSeconds );
	// End : Entity interface

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

	const mat4&				GetViewMatrix()					{ return m_mView;			}
	const mat4&				GetProjMatrix()					{ return m_mProj;			}
	void					GetViewMatrixVectors( vec3& _Right, vec3& _Up, vec3& _Front );
	

protected:
	dvec3					m_Position;
	quat					m_Rotation;


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
#endif

} /* namespace bigball */

#endif // BB_CAMERA_H