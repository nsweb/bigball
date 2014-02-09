


#ifndef BB_TICKCONTEXT_H
#define BB_TICKCONTEXT_H


namespace bigball
{


struct BIGBALL_API TickContext
{
	float		m_DeltaSeconds;
	uint32		m_FrameIdx;
};

} /* namespace bigball */

#endif // BB_TICKCONTEXT_H