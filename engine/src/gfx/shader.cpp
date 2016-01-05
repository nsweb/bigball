

#include "../bigball.h"
#include "shader.h"
#include "../system/file.h"


namespace bigball
{


Shader::Shader() :
	m_program_id(0)
{
	Memory::Memzero( m_shader_ids, sizeof(m_shader_ids) );
}

Shader::~Shader()
{
	DeleteShaders();
}

bool Shader::Create( String const& shader_name )
{
	m_program_id = glCreateProgram();

	GLenum shader_types[Shader::MAX] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_COMPUTE_SHADER };
	char const* shader_exts[Shader::MAX] = { "vs", "fs", "tcs", "tes", "gs", "cs" };
	File shader_file;
	String shader_file_name, shader_src;

	for( int32 i = 0; i < Shader::MAX; ++i )
	{
		shader_file_name = String::Printf( "../data/shader/%s.%s.glsl", shader_name.c_str(), shader_exts[i] );
		if( !shader_file.Open( shader_file_name.c_str(), false /*bWrite*/) )
			continue;

		shader_file.SerializeString( shader_src );
        
        bool shader_success = CreateAndCompileShader( shader_src.c_str(), shader_types[i], m_shader_ids[i] );
        if( !shader_success )
        {
            DeleteShaders();
            return false;
        }

		glAttachShader( m_program_id, m_shader_ids[i] );
	}

#ifdef GL_VERSION_4_1
	if ( GLEW_VERSION_4_1 ) 
	{
		// glProgramParameteri( program, GL_PROGRAM_SEPARABLE, GL_TRUE );
	}
#endif /* GL_VERSION_4_1 */

    if( !LinkProgram() )
    {
        DeleteShaders();
        return false;
    }

	return true;
}
    
bool Shader::CreateFromMemory( const char** src_buffers )
{
    m_program_id = glCreateProgram();
    
    GLenum shader_types[Shader::MAX] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_COMPUTE_SHADER };

    for( int32 i = 0; i < Shader::MAX; ++i )
    {
        if( !src_buffers[i] )
            continue;

        bool shader_success = CreateAndCompileShader( src_buffers[i], shader_types[i], m_shader_ids[i] );
        if( !shader_success )
        {
            DeleteShaders();
            return false;
        }
        
        glAttachShader( m_program_id, m_shader_ids[i] );
    }
    
#ifdef GL_VERSION_4_1
    if ( GLEW_VERSION_4_1 )
    {
        // glProgramParameteri( program, GL_PROGRAM_SEPARABLE, GL_TRUE );
    }
#endif /* GL_VERSION_4_1 */
    
    if( !LinkProgram() )
    {
        DeleteShaders();
        return false;
    }
    
    return true;
}
    
bool Shader::CreateAndCompileShader( char const* shader_src, GLenum shader_type, GLuint& shader_id )
{
    shader_id = glCreateShader( shader_type );
    
    glShaderSource( shader_id, 1, &shader_src, nullptr );
    glCompileShader( shader_id );
    
    GLint compiled_status;
    glGetShaderiv( shader_id, GL_COMPILE_STATUS, &compiled_status );
    if ( !compiled_status )
    {
#if BB_BUILD_DEBUG
        GLsizei len;
        glGetShaderiv( shader_id, GL_INFO_LOG_LENGTH, &len );
        
        String log_str;
        log_str.resize( len );
        glGetShaderInfoLog( shader_id, len, &len, log_str.c_str() );
        BB_LOG( Shader, Log, "Shader compilation failed: %s", log_str.c_str() );
#endif /* BB_BUILD_DEBUG */
        
        return false;
    }
    
    return true;
}

bool Shader::LinkProgram()
{
    glLinkProgram( m_program_id );
    
    GLint linked_status;
    glGetProgramiv( m_program_id, GL_LINK_STATUS, &linked_status );
    if ( !linked_status )
    {
#if BB_BUILD_DEBUG
        GLsizei len;
        glGetProgramiv( m_program_id, GL_INFO_LOG_LENGTH, &len );
        
        String log_str;
        log_str.resize( len );
        glGetProgramInfoLog( m_program_id, len, &len, log_str.c_str() );
        BB_LOG( Shader, Log, "Shader linking failed: %s", log_str.c_str() );
#endif /* BB_BUILD_DEBUG */
        
        return false;
    }
    
    return true;
}

void Shader::DeleteShaders()
{
	for( int32 i = 0; i < Shader::MAX; ++i )
	{
		if( m_shader_ids[i] != 0 )
			glDeleteShader( m_shader_ids[i] );
		m_shader_ids[i] = 0;
	}
	if( m_program_id != 0 )
		glDeleteProgram( m_program_id );
	m_program_id = 0;
}

ShaderUniform Shader::GetUniformLocation( char const* uniform_name ) const
{
    ShaderUniform ret;

    ret.m_index = glGetUniformLocation( m_program_id, uniform_name );

    return ret;
}
    
int Shader::GetActiveUniforms( Array<ShaderUniformDetail>& uniforms ) const
{
    GLsizei uniform_count = 0;
    glGetProgramiv( m_program_id, GL_ACTIVE_UNIFORMS, &uniform_count );
    uniforms.resize(uniform_count);
    
    for( int i = 0; i < uniform_count; ++i )
    {
        GLsizei buf_size = 256;
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = GL_ZERO;
        GLchar name[256];
     
        glGetActiveUniform( m_program_id, i, buf_size, &length, &size, &type, name );
        uniforms[i].m_index = glGetUniformLocation( m_program_id, name );
        uniforms[i].m_name = name;
        uniforms[i].m_type = type;
    }
    
    return uniform_count;
}

/*
 * Uniform setters for scalars
 */

void Shader::SetUniform( ShaderUniform const &uni, int i )
{
    glUniform1i( uni.m_index, i );
}

void Shader::SetUniform( ShaderUniform const &uni, ivec2 const &v )
{
    glUniform2i( uni.m_index, v.x, v.y);
}

void Shader::SetUniform( ShaderUniform const &uni, ivec3 const &v )
{
    glUniform3i( uni.m_index, v.x, v.y, v.z);
}

void Shader::SetUniform( ShaderUniform const &uni, ivec4 const &v )
{
    glUniform4i( uni.m_index, v.x, v.y, v.z, v.w);
}

void Shader::SetUniform( ShaderUniform const &uni, float f )
{
    glUniform1f( uni.m_index, f);
}

void Shader::SetUniform( ShaderUniform const &uni, vec2 const &v )
{
    glUniform2fv( uni.m_index, 1, &v[0] );
}

void Shader::SetUniform( ShaderUniform const &uni, vec3 const &v )
{
    glUniform3fv( uni.m_index, 1, &v[0] );
}

void Shader::SetUniform( ShaderUniform const &uni, vec4 const &v )
{
    glUniform4fv( uni.m_index, 1, &v[0] );
}

void Shader::SetUniform( ShaderUniform const &uni, mat2 const &m )
{
    glUniformMatrix2fv( uni.m_index, 1, GL_FALSE, &m[0][0] );
}

void Shader::SetUniform( ShaderUniform const &uni, mat3 const &m )
{
    glUniformMatrix3fv( uni.m_index, 1, GL_FALSE, &m[0][0] );
}

void Shader::SetUniform( ShaderUniform const &uni, mat4 const &m )
{
    glUniformMatrix4fv( uni.m_index, 1, GL_FALSE, &m[0][0] );
}

//void Shader::SetUniform( ShaderUniform const &uni, ShaderTexture tex, int index)
//{
//    glActiveTexture(GL_TEXTURE0 + index);
//    //glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, (int)tex.m_flags);
//    SetUniform(uni, index);
//}

/*
 * Uniform setters for arrays
 */

void Shader::SetUniform( ShaderUniform const &uni, Array<float> const &v )
{
    glUniform1fv( uni.m_index, v.size(), &v[0] );
}

void Shader::SetUniform( ShaderUniform const &uni, Array<vec2> const &v )
{
    glUniform2fv( uni.m_index, v.size(), &v[0][0] );
}

void Shader::SetUniform( ShaderUniform const &uni, Array<vec3> const &v )
{
    glUniform3fv( uni.m_index, v.size(), &v[0][0] );
}

void Shader::SetUniform( ShaderUniform const &uni, Array<vec4> const &v )
{
    glUniform4fv( uni.m_index, v.size(), &v[0][0] );
}
    
void Shader::GetUniform( ShaderUniform const &uni, int& i) const
{
    glGetUniformiv(	m_program_id, uni.m_index, &i );
}
    
void Shader::GetUniform( ShaderUniform const &uni, ivec2& v) const
{
    glGetUniformiv(	m_program_id, uni.m_index, &v[0] );
}

void Shader::GetUniform( ShaderUniform const &uni, ivec3& v) const
{
    glGetUniformiv(	m_program_id, uni.m_index, &v[0] );
}

void Shader::GetUniform( ShaderUniform const &uni, ivec4& v) const
{
    glGetUniformiv(	m_program_id, uni.m_index, &v[0] );
}
    
void Shader::GetUniform( ShaderUniform const &uni, float& f) const
{
    glGetUniformfv(	m_program_id, uni.m_index, &f );
}

void Shader::GetUniform( ShaderUniform const &uni, vec2& v) const
{
    glGetUniformfv(	m_program_id, uni.m_index, &v[0] );
}

void Shader::GetUniform( ShaderUniform const &uni, vec3& v) const
{
    glGetUniformfv(	m_program_id, uni.m_index, &v[0] );
}

void Shader::GetUniform( ShaderUniform const &uni, vec4& v) const
{
    glGetUniformfv(	m_program_id, uni.m_index, &v[0] );
}

void Shader::Bind() const
{
    glUseProgram( m_program_id );
}

void Shader::Unbind() const
{
    glUseProgram(0);
}



} /*namespace bigball*/