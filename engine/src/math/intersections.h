

#ifndef BB_MATHINTERSECTIONS_H
#define BB_MATHINTERSECTIONS_H


namespace bigball
{

template <typename T> struct Intersect;
typedef Intersect<float> intersect;
typedef Intersect<double> dintersect;

template <typename T> struct Intersect
{
	static bool RayAABB( Vec3<T> const& minB, Vec3<T> const& maxB, Vec3<T> const& origin, Vec3<T> const& dir, Vec3<T>& coord );
  
    static T SquaredDistancePointSegment( Vec3<T> const& point, Vec3<T> const& seg0, Vec3<T> const& seg1, T& t_on_segment )
    {
        Vec3<T> V_seg0  = point - seg0;
        Vec3<T> dir_seg = seg1 - seg0;
        T t             = bigball::max( dot( dir_seg, V_seg0 ), (T)0.0 );
        
        T dot_dd        = dot( dir_seg, dir_seg );
        t               = min( t, dot_dd ) / dot_dd;
        V_seg0          += dir_seg*(-t);
        
        t_on_segment    = t;
        return dot( V_seg0, V_seg0 );
    }
};

} /* namespace bigball */

#endif // BB_MATHINTERSECTIONS_H