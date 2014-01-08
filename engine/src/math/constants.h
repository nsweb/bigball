
// Various maths constants
// -----------------------
//

#ifndef BB_MATHCONSTANTS_H
#define BB_MATHCONSTANTS_H

namespace bigball
{

#define BB_MATH_CONSTANT(name, value) \
    static double const D_##name = (double)value; \
    static float const F_##name = (float)D_##name;


BB_MATH_CONSTANT(PI,   3.14159265358979323846264338327950288419716939937L)
BB_MATH_CONSTANT(PI_2, 1.57079632679489661923132169163975144209858469968L)
BB_MATH_CONSTANT(PI_3, 1.04719755119659774615421446109316762806572313312L)
BB_MATH_CONSTANT(PI_4, 0.78539816339744830961566084581987572104929234984L)

BB_MATH_CONSTANT(1_PI, 0.31830988618379067153776752674502872406891929148L)
BB_MATH_CONSTANT(2_PI, 0.63661977236758134307553505349005744813783858296L)

BB_MATH_CONSTANT(SQRT_2,   1.41421356237309504880168872420969807856967187537L)
BB_MATH_CONSTANT(SQRT_3,   1.73205080756887729352744634150587236694280525381L)
BB_MATH_CONSTANT(SQRT_1_2, 0.70710678118654752440084436210484903928483593768L)

BB_MATH_CONSTANT(DEG_TO_RAD, (D_PI / 180.0L))
BB_MATH_CONSTANT(RAD_TO_DEG, (180.0L / D_PI))

#undef BB_MATH_CONSTANT

} /* namespace bigball */

#endif // BB_MATHCONSTANTS_H

