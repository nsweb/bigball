


#ifndef BB_TICKCONTEXT_H
#define BB_TICKCONTEXT_H


namespace bigball
{


struct BIGBALL_API TickContext
{
	TickContext( float InDeltaSeconds = 0.f, uint32 InFrameIdx = 0 ) :
			m_DeltaSeconds(InDeltaSeconds),
			m_FrameIdx(InFrameIdx)
			{

			}

	float		m_DeltaSeconds;
	uint32		m_FrameIdx;
};

} /* namespace bigball */

#endif // BB_TICKCONTEXT_H