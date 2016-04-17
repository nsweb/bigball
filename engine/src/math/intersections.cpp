
#include "../bigball.h"
#include "intersections.h"

namespace bigball
{

//////////////////////////////////////////////////////////////////////////
// Explicit instantiation
template struct Intersect<double>; 

/* Fast Ray-Box Intersection by Andrew Woo */
template <typename T>
bool Intersect<T>::RayAABB( Vec3<T> const& min_box, Vec3<T> const& max_box, Vec3<T> const& origin, Vec3<T> const& dir, Vec3<T>& coord )
{
	const int32 num_dim = 3;
	const int32 RIGHT = 0;
	const int32 LEFT = 1;
	const int32 MIDDLE = 2;

	bool is_inside = true;
	int32 quadrant[num_dim];
	int32 i, which_plane;
	T maxT[num_dim];
	T candidate_plane[num_dim];

	/* Find candidate planes; this loop can be avoided if
   	rays cast all from the eye(assume perpsective view) */
	for(i=0; i<num_dim; i++)
	{
		if(origin[i] < min_box[i])
		{
			quadrant[i] = LEFT;
			candidate_plane[i] = min_box[i];
			is_inside = false;
		}
		else if (origin[i] > max_box[i]) 
		{
			quadrant[i] = RIGHT;
			candidate_plane[i] = max_box[i];
			is_inside = false;
		}
		else	
		{
			quadrant[i] = MIDDLE;
		}
	}

	/* Ray origin inside bounding box */
	if(is_inside)	
	{
		coord = origin;
		return true;
	}

	/* Calculate T distances to candidate planes */
	for (i = 0; i < num_dim; i++)
	{
		if (quadrant[i] != MIDDLE && dir[i] !=0.)
			maxT[i] = (candidate_plane[i] - origin[i]) / dir[i];
		else
			maxT[i] = -1.;
	}

	/* Get largest of the maxT's for final choice of intersection */
	which_plane = 0;
	for (i = 1; i < num_dim; i++)
	{
		if (maxT[which_plane] < maxT[i])
			which_plane = i;
	}

	/* Check final candidate actually inside box */
	if (maxT[which_plane] < 0.) 
		return false;

	for (i = 0; i < num_dim; i++)
	{
		if (which_plane != i) 
		{
			coord[i] = origin[i] + maxT[which_plane] * dir[i];
			if (coord[i] < min_box[i] || coord[i] > max_box[i])
				return false;
		} 
		else 
		{
			coord[i] = candidate_plane[i];
		}
	}

	return true;		/* ray hits box */
}


} /* namespace bigball */

