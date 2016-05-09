

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
	static T SquaredDistancePointSegment(Vec3<T> const& point, Vec3<T> const& seg0, Vec3<T> const& seg1, T& t_on_segment);
	static T RaySphereIntersection(Vec3<T> const& ray_orig, Vec3<T> const& ray_dir, Vec3<T> const& center, T radius);
	static T RayBoxIntersection(Vec3<T> const& ray_orig, Vec3<T> const& ray_dir, Vec3<T> const& box_ratio);
};

} /* namespace bigball */

#endif // BB_MATHINTERSECTIONS_H