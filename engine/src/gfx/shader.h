


#ifndef BB_GFXSHADER_H
#define BB_GFXSHADER_H


namespace bigball
{

class BIGBALL_API Shader
{
public:

	enum eType
	{
		Vertex = 0,
		Fragment,
		TesselationControl,
		TesselationEvaluation,
		Geometry,
		Compute,
		MAX
	};

						Shader();
	virtual				~Shader();

	bool				Create( String const& ShaderName );

protected:
	GLuint	ShaderIDs[eType::MAX];
	GLuint	ProgramID;
	
};

} /* namespace bigball */

#endif // BB_GFXSHADER_H