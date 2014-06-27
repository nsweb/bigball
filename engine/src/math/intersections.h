

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
};

} /* namespace bigball */

#endif // BB_MATHINTERSECTIONS_H