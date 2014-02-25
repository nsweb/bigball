

#ifndef BB_MATHTRANSFORM_H
#define BB_MATHTRANSFORM_H


namespace bigball
{

template <typename T> struct Transform;
typedef Transform<float> transform;
typedef Transform<double> dtransform;

/*constexpr*/
#define BB_CONSTEXPR 


/*
 * Transform - rotation, translation, and no scale
 */

template <typename T> struct /*BIGBALL_API*/ Transform
{
    inline BB_CONSTEXPR Transform() {}
    inline BB_CONSTEXPR Transform(Quat<T> const& Rot) : m_Rotation(Rot), m_Translation(0), m_Scale(1) {}
    inline BB_CONSTEXPR Transform(Quat<T> const& Rot, Vec3<T> const& Trans, T Scale) : m_Rotation(Rot), m_Translation(Trans), m_Scale(Scale) {}
	inline BB_CONSTEXPR Transform(Vec3<T> const& Trans) : m_Rotation(1), m_Translation(Trans), m_Scale(1) {}

	inline Quat<T>& GetRotation()											{ return m_Rotation;	}
	inline Quat<T> const & GetRotation() const								{ return m_Rotation;	}
	inline Vec3<T>& GetTranslation()										{ return m_Translation;	}
	inline Vec3<T> const & GetTranslation()	const							{ return m_Translation;	}
	inline T& GetScale()													{ return m_Scale;		}
	inline T  GetScale() const												{ return m_Scale;		}
	inline void	SetRotation( Quat<T> const& Rot )							{ m_Rotation = Rot;		}
	inline void	SetTranslation( Vec3<T> const& Trans )						{ m_Translation = Trans; }
	inline void	SetScale( T Scale )											{ m_Scale = Scale;		}
	inline void Set( Quat<T> const& Rot, Vec3<T> const& Trans )				{ m_Rotation = Rot; m_Translation = Trans; }
	inline void Set( Quat<T> const& Rot, Vec3<T> const& Trans, T Scale )	{ m_Rotation = Rot; m_Translation = Trans; m_Scale = Scale; }
	inline Transform<T> operator * ( Transform<T> const& B ) const
	{
		Transform<T> Ret;
		Multiply( &Ret, this, &B );
		return Ret;
	}
	inline static void Multiply( Transform<T>* OutT, Transform<T> const * A, Transform<T> const * B )
	{
		Transform<T> Ret;
		Ret.m_Rotation = B->m_Rotation * A->m_Rotation;
		Ret.m_Translation = A->m_Rotation * (B->m_Translation * A->m_Scale) + A->m_Translation;
		Ret.m_Scale = B->m_Scale * A->m_Scale;

		*OutT = Ret;
	}
	/**  Get this -> B transform */
	Transform<T> GetTransformTo( Transform<T> const& B ) const
	{
		Transform<T> Ret;

		BB_ASSERT( isNormalized( B.GetRotation() ) );

		Quat<T> BInvRot = ~B.m_Rotation;
		Ret.m_Rotation = BInvRot * m_Rotation;
		Ret.m_Translation = (BInvRot * (m_Translation - B.m_Translation)) / B.m_Scale;
		Ret.m_Scale = m_Scale / B.m_Scale;

		return Ret;
	}

	inline Vec3<T> TransformPositionInverse( Vec3<T> const& V ) const
	{
		return ( ~m_Rotation * (V - m_Translation) ) / m_Scale;
	}
	inline Vec3<T> TransformVectorInverse( Vec3<T> const& V ) const
	{
		return ( ~m_Rotation * V ) / m_Scale;
	}
	inline Vec3<T> TransformPosition( Vec3<T> const& V ) const
	{
		Vec3<T> Ret = m_Rotation*(V * m_Scale);
		Ret += m_Translation;
		return Ret;
	}
	inline Vec3<T> TransformVector( Vec3<T> const& V ) const
	{
		Vec3<T> Ret = m_Rotation*(V * m_Scale);
		return Ret;
	}

    template<typename U>
    friend std::ostream &operator<<(std::ostream &stream, Transform<U> const &v);

    Quat<T> m_Rotation;
	Vec3<T> m_Translation;
	T		m_Scale;
};


#undef BB_CONSTEXPR


} /* namespace bigball */

#endif // BB_MATHTRANSFORM_H
