

#ifndef BB_TEMPLATES_H
#define BB_TEMPLATES_H

namespace bigball
{

#define UNUSED(...)
#define ASSERT(...)

#define BB_FREE(p)			{ if(p) Memory::Free(p); (p)=nullptr; }
#define BB_DELETE(p)		{ if(p) delete(p); (p)=nullptr; }
#define BB_DELETE_ARRAY(p)	{ if(p) delete[](p); (p)=nullptr; }

#undef BB_MATH_CONSTANT

} /* namespace bigball */

#endif // BB_TEMPLATES_H

