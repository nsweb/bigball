

#ifndef BB_MATHFRUSTUM_H
#define BB_MATHFRUSTUM_H


namespace bigball
{

template <typename T> class Frustum;
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
	enum eIntersectionStatus
	{
		FullyInside,
		Intersect,
		Outside,
	};
	enum eFrustumPlane
	{
		eFP_Near,
		eFP_Far,
		eFP_Right,
		eFP_Left,
		eFP_Top,
		eFP_Bottom,
		eFP_MAX,
	};
	enum eFrustumPoint
	{
		eFPT_NTL,
		eFPT_NTR,
		eFPT_NBL,
		eFPT_NBR,
		eFPT_FTL,
		eFPT_FTR,
		eFPT_FBL,
		eFPT_FBR,
		eFPT_MAX,
	};
public:
    inline BB_CONSTEXPR Frustum() {}
	Frustum( CameraView const& View, bool bLocalToView )
	{
		BuildFromView( View, bLocalToView );
	}

	void BuildFromView( CameraView const& View, bool bLocalToView )
	{
		T fov_y = (T)View.m_fParameters[eCP_FOV];
		fov_y *= ((T)D_PI / (T)180.0);

		T cf_y = cos(fov_y * (T)0.5);
		T sf_y = sin(fov_y * (T)0.5);
		T tf_y = sf_y / cf_y;
		T half_fov_x = atan2( tf_y * (T)View.m_fParameters[eCP_ASPECT], (T)1.0 );
		T cf_x = cos(half_fov_x);
		T sf_x = sin(half_fov_x);

		auto ComputeFunc = [&] ( Vec3<T> const& Eye, Vec3<T> const& Right, Vec3<T> const& Up, Vec3<T> const& Front )
		{
			// Compute planes
			m_Planes[eFP_Near].xyz = Front;
			m_Planes[eFP_Far].xyz = -Front;
			m_Planes[eFP_Right].xyz = Right * -cf_x + Front * sf_x;
			m_Planes[eFP_Left].xyz = Right * cf_x + Front * sf_x;
			m_Planes[eFP_Top].xyz = Up * -cf_x + Front * sf_x;
			m_Planes[eFP_Bottom].xyz = Up * cf_x + Front * sf_x;

			Vec3<T> PNear = Eye + Front * (T)View.m_fParameters[eCP_NEAR];
			Vec3<T> PFar = Eye + Front * (T)View.m_fParameters[eCP_FAR];
			m_Planes[eFP_Near].w = -dot( PNear, m_Planes[eFP_Near].xyz );
			m_Planes[eFP_Far].w = -dot( PFar, m_Planes[eFP_Far].xyz );
			for( int32 i = eFP_Right; i < eFP_MAX; ++i )
				m_Planes[i].w = -dot( Eye, m_Planes[i].xyz );

			// Compute points
			T y_near = tf_y * (T)View.m_fParameters[eCP_NEAR];
			T x_near = y_near * (T)View.m_fParameters[eCP_ASPECT];
			T y_far = tf_y * (T)View.m_fParameters[eCP_FAR];
			T x_far = y_far * (T)View.m_fParameters[eCP_ASPECT];
			m_Points[eFPT_NTL] = PNear + Up * y_near - Right * x_near;
			m_Points[eFPT_NTR] = PNear + Up * y_near + Right * x_near;
			m_Points[eFPT_NBL] = PNear - Up * y_near - Right * x_near;
			m_Points[eFPT_NBR] = PNear - Up * y_near + Right * x_near;
			m_Points[eFPT_FTL] = PFar + Up * y_far - Right * x_far;
			m_Points[eFPT_FTR] = PFar + Up * y_far + Right * x_far;
			m_Points[eFPT_FBL] = PFar - Up * y_far - Right * x_far;
			m_Points[eFPT_FBR] = PFar - Up * y_far + Right * x_far;
		};

		if( bLocalToView )
		{
			Vec3<T> Eye(0,0,0);
			Vec3<T> Right(1,0,0);
			Vec3<T> Up(0,1,0);
			Vec3<T> Front(0,0,-1);
			ComputeFunc( Eye, Right, Up, Front );
		}
		else
		{
			Mat4<T> CamToWorldMat( View.m_Transform.GetRotation() );
			Vec3<T> Eye = View.m_Transform.GetTranslation();
			Vec3<T> Right = CamToWorldMat.v0.xyz;
			Vec3<T> Up = CamToWorldMat.v1.xyz;
			Vec3<T> Front = -CamToWorldMat.v2.xyz;
			ComputeFunc( Eye, Right, Up, Front );
		}
	}
  
    template<typename U>
    friend std::ostream &operator<<(std::ostream &stream, Transform<U> const &v);

	eIntersectionStatus AABBIntersection( Vec3<T> const& BoxMin, Vec3<T> const& BoxMax ) const
	{
		// check box outside/inside of frustum
		int SumOut = 0;
		for( int i = 0; i < eFP_MAX; i++ )
		{
			int Out = 0;
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMin.x, BoxMin.y, BoxMin.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMax.x, BoxMin.y, BoxMin.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMin.x, BoxMax.y, BoxMin.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMax.x, BoxMax.y, BoxMin.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMin.x, BoxMin.y, BoxMax.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMax.x, BoxMin.y, BoxMax.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMin.x, BoxMax.y, BoxMax.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			Out += ((dot( m_Planes[i], Vec4<T>(BoxMax.x, BoxMax.y, BoxMax.z, (T)1.0) ) < (T)0.0 ) ? 1 : 0);
			if( Out==8 ) 
				return Outside;
			SumOut += Out;
		}

		// check frustum outside/inside box
		int Out;
		Out=0; for( int i=0; i < eFPT_MAX; i++ ) Out += ((m_Points[i].x > BoxMax.x) ? 1 : 0); if( Out==eFPT_MAX ) return Outside;
		Out=0; for( int i=0; i < eFPT_MAX; i++ ) Out += ((m_Points[i].x < BoxMin.x) ? 1 : 0); if( Out==eFPT_MAX ) return Outside;
		Out=0; for( int i=0; i < eFPT_MAX; i++ ) Out += ((m_Points[i].y > BoxMax.y) ? 1 : 0); if( Out==eFPT_MAX ) return Outside;
		Out=0; for( int i=0; i < eFPT_MAX; i++ ) Out += ((m_Points[i].y < BoxMin.y) ? 1 : 0); if( Out==eFPT_MAX ) return Outside;
		Out=0; for( int i=0; i < eFPT_MAX; i++ ) Out += ((m_Points[i].z > BoxMax.z) ? 1 : 0); if( Out==eFPT_MAX ) return Outside;
		Out=0; for( int i=0; i < eFPT_MAX; i++ ) Out += ((m_Points[i].z < BoxMin.z) ? 1 : 0); if( Out==eFPT_MAX ) return Outside;

		return SumOut > 0 ? Intersect : FullyInside;
	}

	eIntersectionStatus SphereIntersection( Vec3<T> const& SphereCenter, T SphereRadius )
	{
		Vec4<T> VSphere( SphereCenter, (T)1.0 );
		int In = 0;
		float d;
		if( (d = dot( m_Planes[eFP_Near], VSphere )) < -SphereRadius ) return Outside;
		In += d > SphereRadius ? 1 : 0;
		if( (d = dot( m_Planes[eFP_Far], VSphere )) < -SphereRadius ) return Outside;
		In += d > SphereRadius ? 1 : 0;
		if( (d = dot( m_Planes[eFP_Right], VSphere )) < -SphereRadius ) return Outside;
		In += d > SphereRadius ? 1 : 0;
		if( (d = dot( m_Planes[eFP_Left], VSphere )) < -SphereRadius ) return Outside;
		In += d > SphereRadius ? 1 : 0;
		if( (d = dot( m_Planes[eFP_Top], VSphere )) < -SphereRadius ) return Outside;
		In += d > SphereRadius ? 1 : 0;
		if( (d = dot( m_Planes[eFP_Bottom], VSphere )) < -SphereRadius ) return Outside;
		In += d > SphereRadius ? 1 : 0;

		return In < 6 ? Intersect : FullyInside;
	}

    Vec4<T> m_Planes[eFP_MAX];
	Vec3<T> m_Points[eFPT_MAX];
};


#undef BB_CONSTEXPR


} /* namespace bigball */

#endif // BB_MATHFRUSTUM_H

