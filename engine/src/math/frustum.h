

#ifndef BB_MATHFRUSTUM_H
#define BB_MATHFRUSTUM_H


namespace bigball
{

template <typename T> struct Frustum;
typedef Frustum<float> frustum;
typedef Frustum<double> dfrustum;

/*constexpr*/
#define BB_CONSTEXPR 


/*
 * Frustum
 */

template <typename T> class /*BIGBALL_API*/ Frustum
{
public:
    inline BB_CONSTEXPR Frustum() {}
	inline BB_CONSTEXPR Frustum( CameraView const& View )
	{
		float fov_y = View.m_fParameters[eCP_FOV];
		fov_y *= (F_PI / 180.0f);

		float Hnear = 2 * tan(fov_y * 0.5f) * View.m_fParameters[eCP_NEARPLANE];
		float Wnear = Hnear * View.m_fParameters[eCP_ASPECTRATIO];

		float Hfar = 2 * tan(fov_y * 0.5f) * View.m_fParameters[eCP_FARPLANE];
		float Wfar = Hfar * View.m_fParameters[eCP_ASPECTRATIO];

		dvec3 P = View.m_Transform.GetLocation();
		mat4 CamToWorldMat( View.m_Transform.GetRotation() );
		vec3 Right = CamToWorldMat.v0.xyz;
		vec3 Up = CamToWorldMat.v1.xyz;
		vec3 Front = -CamToWorldMat.v2.xyz;

		float Cnear = P + Front * View.m_fParameters[eCP_NEARPLANE];
		float Cfar = P + Front * View.m_fParameters[eCP_FARPLANE];

		vec3 NTL = Cnear + (Up * Hnear) - (Right * Wnear);
		vec3 NTR = Cnear + (Up * Hnear) + (Right * Wnear);
		vec3 NBL = Cnear - (Up * Hnear) - (Right * Wnear);
		vec3 NBR = Cnear + (Up * Hnear) + (Right * Wnear);
		vec3 FTL = Cfar + (Up * Hfar) - (Right * Wfar);
		vec3 FTR = Cfar + (Up * Hfar) + (Right * Wfar);
		vec3 FBL = Cfar - (Up * Hfar) - (Right * Wfar);
		vec3 FBR = Cfar - (Up * Hfar) + (Right * Wfar);
	}
  
    template<typename U>
    friend std::ostream &operator<<(std::ostream &stream, Transform<U> const &v);

	enum eIntersectionStatus
	{
		FullyInside,
		Intersect,
		Outside,
	};

	eIntersectionStatus AABBIntersection( Vec3<T> const& BoxMin, Vec3<T> const& BoxMax ) const
	{
		// check box outside/inside of frustum
		int SumOut = 0;
		for( int i=0; i<6; i++ )
		{
			int Out = 0;
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMin.x, BoxMin.y, BoxMin.z, (T)1.0) ) < (T)0.0 )?1:0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMax.x, BoxMin.y, BoxMin.z, (T)1.0) ) < (T)0.0 )?1:0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMin.x, BoxMax.y, BoxMin.z, (T)1.0) ) < (T)0.0 )?1:0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMax.x, BoxMax.y, BoxMin.z, (T)1.0) ) < (T)0.0 )?1:0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMin.x, BoxMin.y, BoxMax.z, (T)1.0) ) < (T)0.0 )?1:0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMax.x, BoxMin.y, BoxMax.z, (T)1.0) ) < (T)0.0 )?1:0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMin.x, BoxMax.y, BoxMax.z, (T)1.0) ) < (T)0.0 )?1:0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMax.x, BoxMax.y, BoxMax.z, (T)1.0) ) < (T)0.0 )?1:0);
			if( Out==8 ) 
				return Outside;
			SumOut += Out;
		}

		return SumOut > 0 ? Intersect : FullyInside;
	}

    Vec4<T> m_Planes[6];
};


#undef BB_CONSTEXPR


} /* namespace bigball */

#endif // BB_MATHFRUSTUM_H

