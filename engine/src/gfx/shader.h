


#ifndef BB_GFXSHADER_H
#define BB_GFXSHADER_H


namespace bigball
{

struct BIGBALL_API ShaderUniform
{
	friend class Shader;

	inline ShaderUniform() : m_index(-1), m_flags(0) {}

private:
	GLint	m_index;
	uint32	m_flags;
};

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

	bool				Create( String const& shader_name );
    /** Create shaders from memory, src_buffers should hold at least MAX elements */
    bool				CreateFromMemBuffers( const char** src_buffers );
	ShaderUniform		GetUniformLocation( char const* uniform_name ) const;
	void				SetUniform( ShaderUniform const &uni, int i);
	void				SetUniform( ShaderUniform const &uni, ivec2 const &v);
	void				SetUniform( ShaderUniform const &uni, ivec3 const &v);
	void				SetUniform( ShaderUniform const &uni, ivec4 const &v);
	void				SetUniform( ShaderUniform const &uni, float f);
	void				SetUniform( ShaderUniform const &uni, vec2 const &v);
	void				SetUniform( ShaderUniform const &uni, vec3 const &v);
	void				SetUniform( ShaderUniform const &uni, vec4 const &v);
	void				SetUniform( ShaderUniform const &uni, mat2 const &m);
	void				SetUniform( ShaderUniform const &uni, mat3 const &m);
	void				SetUniform( ShaderUniform const &uni, mat4 const &m);
	//void				SetUniform( ShaderUniform const &uni, ShaderTexture tex, int index);
								    
	void				SetUniform( ShaderUniform const &uni, Array<float> const &v);
	void				SetUniform( ShaderUniform const &uni, Array<vec2> const &v);
	void				SetUniform( ShaderUniform const &uni, Array<vec3> const &v);
	void				SetUniform( ShaderUniform const &uni, Array<vec4> const &v);

	void				Bind() const;
	void				Unbind() const;

protected:
	GLuint	m_shader_ids[MAX];
	GLuint	m_program_id;

	void				DeleteShaders();
    bool                CreateAndCompileShader( char const* shader_src, GLenum shader_type, GLuint& shader_id );
    bool                LinkProgram();
};

} /* namespace bigball */

#endif // BB_GFXSHADER_H