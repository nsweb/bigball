
#include "../bigball.h"
#include "intersections.h"

namespace bigball
{

//////////////////////////////////////////////////////////////////////////
// Explicit instantiation
template struct Intersect<double>; 

/* Fast Ray-Box Intersection by Andrew Woo */
template <typename T>
bool Intersect<T>::RayAABB( Vec3<T> const& minB, Vec3<T> const& maxB, Vec3<T> const& origin, Vec3<T> const& dir, Vec3<T>& coord )
{
	const int32 NUMDIM = 3;
	const int32 RIGHT = 0;
	const int32 LEFT = 1;
	const int32 MIDDLE = 2;

	bool bInside = true;
	int32 Quadrant[NUMDIM];
	int32 i, whichPlane;
	double maxT[NUMDIM];
	double candidatePlane[NUMDIM];

	/* Find candidate planes; this loop can be avoided if
   	rays cast all from the eye(assume perpsective view) */
	for(i=0; i<NUMDIM; i++)
	{
		if(origin[i] < minB[i])
		{
			Quadrant[i] = LEFT;
			candidatePlane[i] = minB[i];
			bInside = FALSE;
		}
		else if (origin[i] > maxB[i]) 
		{
			Quadrant[i] = RIGHT;
			candidatePlane[i] = maxB[i];
			bInside = FALSE;
		}
		else	
		{
			Quadrant[i] = MIDDLE;
		}
	}

	/* Ray origin inside bounding box */
	if(bInside)	
	{
		coord = origin;
		return true;
	}


	/* Calculate T distances to candidate planes */
	for (i = 0; i < NUMDIM; i++)
	{
		if (Quadrant[i] != MIDDLE && dir[i] !=0.)
			maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
		else
			maxT[i] = -1.;
	}

	/* Get largest of the maxT's for final choice of intersection */
	whichPlane = 0;
	for (i = 1; i < NUMDIM; i++)
	{
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;
	}

	/* Check final candidate actually inside box */
	if (maxT[whichPlane] < 0.) 
		return false;

	for (i = 0; i < NUMDIM; i++)
	{
		if (whichPlane != i) 
		{
			coord[i] = origin[i] + maxT[whichPlane] *dir[i];
			if (coord[i] < minB[i] || coord[i] > maxB[i])
				return false;
		} 
		else 
		{
			coord[i] = candidatePlane[i];
		}
	}

	return true;				/* ray hits box */
}	

} /* namespace bigball */

