

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
 //   inline BB_CONSTEXPR Transform(Quat<T> const& Rot) : m_Rotation(Rot), m_Translation(0) {}
 //   inline BB_CONSTEXPR Transform(Quat<T> const& Rot, Vec3<T> const& Trans) : m_Rotation(Rot), m_Translation(Trans) {}
	//inline BB_CONSTEXPR Transform(Vec3<T> const& Trans) : m_Rotation(1), m_Translation(Trans) {}

  
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

