
#include "../bigball.h"


namespace bigball
{


// "Next Largest Power of 2
// Given a binary integer value x, the next largest power of 2 can be computed by a SWAR algorithm
// that recursively "folds" the upper bits into the lower bits. This process yields a bit vector with
// the same most significant 1 as x, but all 1's below it. Adding 1 to that value yields the next
// largest power of 2. For a 32-bit value:"
uint32 NextPowerOfTwo( uint32 x )
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x+1;
}

bool IsPowerOfTwo( uint32 x )
{
	return (x != 0) && ((x & (x - 1)) == 0);
}

} /* namespace bigball */

