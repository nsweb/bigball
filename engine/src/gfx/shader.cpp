

#include "../bigball.h"
#include "shader.h"
#include "../system/file.h"




namespace bigball
{


Shader::Shader() :
	m_ProgramID(0)
{
	Memory::Memzero( m_ShaderIDs, sizeof(m_ShaderIDs) );
}

Shader::~Shader()
{
	DeleteShaders();
}

bool Shader::Create( String const& ShaderName )
{
	m_ProgramID = glCreateProgram();

	GLenum ShaderTypes[Shader::MAX] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_COMPUTE_SHADER };
	char const* ShaderExts[Shader::MAX] = { "vs", "fs", "tcs", "tes", "gs", "cs" };
	File ShaderFile;
	String ShaderFileName, ShaderSrc;

	for( int32 i = 0; i < Shader::MAX; ++i )
	{
		ShaderFileName = String::Printf( "../data/shader/%s.%s.glsl", ShaderName.c_str(), ShaderExts[i] );
		if( !ShaderFile.Open( ShaderFileName.c_str(), false /*bWrite*/) )
			continue;

		ShaderFile.SerializeString( ShaderSrc );

		GLuint ShaderID = glCreateShader( ShaderTypes[i] );

		m_ShaderIDs[i] = ShaderID;

		const char* SrcStr = ShaderSrc.c_str();
		glShaderSource( ShaderID, 1, &SrcStr, nullptr );
		glCompileShader( ShaderID );

		GLint CompiledStatus;
		glGetShaderiv( ShaderID, GL_COMPILE_STATUS, &CompiledStatus );
		if ( !CompiledStatus ) 
		{
#if BB_BUILD_DEBUG
			GLsizei Len;
			glGetShaderiv( ShaderID, GL_INFO_LOG_LENGTH, &Len );

			String LogStr;
			LogStr.resize( Len );
			glGetShaderInfoLog( ShaderID, Len, &Len, LogStr.c_str() );
			BB_LOG( Shader, Log, "Shader compilation failed: %s", LogStr.c_str() );
#endif /* BB_BUILD_DEBUG */

			DeleteShaders();
			return false;
		}

		glAttachShader( m_ProgramID, ShaderID );
	}

#ifdef GL_VERSION_4_1
	if ( GLEW_VERSION_4_1 ) 
	{
		// glProgramParameteri( program, GL_PROGRAM_SEPARABLE, GL_TRUE );
	}
#endif /* GL_VERSION_4_1 */

	glLinkProgram( m_ProgramID );

	GLint LinkedStatus;
	glGetProgramiv( m_ProgramID, GL_LINK_STATUS, &LinkedStatus );
	if ( !LinkedStatus ) 
	{
#if BB_BUILD_DEBUG
		GLsizei Len;
		glGetProgramiv( m_ProgramID, GL_INFO_LOG_LENGTH, &Len );

		String LogStr;
		LogStr.resize( Len );
		glGetProgramInfoLog( m_ProgramID, Len, &Len, LogStr.c_str() );
		BB_LOG( Shader, Log, "Shader linking failed: %s", LogStr.c_str() );
#endif /* BB_BUILD_DEBUG */

		DeleteShaders();
		return false;
	}

	return true;
}

void Shader::DeleteShaders()
{
	for( int32 i = 0; i < Shader::MAX; ++i )
	{
		if( m_ShaderIDs[i] != 0 )
			glDeleteShader( m_ShaderIDs[i] );
		m_ShaderIDs[i] = 0;
	}
	if( m_ProgramID != 0 )
		glDeleteProgram( m_ProgramID );
	m_ProgramID = 0;
}

ShaderUniform Shader::GetUniformLocation( char const* UniformName ) const
{
    ShaderUniform ret;

    ret.m_Index = glGetUniformLocation( m_ProgramID, UniformName);
    
	/*GLsizei UniformCount;
	glGetProgramiv( ProgramID, GL_ACTIVE_UNIFORMS, &UniformCount );
	for( int32 i = 0; i < UniformCount; ++i )
	{
	GLsizei bufSize = 256;
	GLsizei length = 0;
	GLint size = 0;
	GLenum type;
	GLchar name[256];

	glGetActiveUniform (ProgramID, i, bufSize, &length, &size, &type, name );
	GLint loc = glGetUniformLocation( ProgramID, name );
	int32 Break=0;
	}*/

    return ret;
}

/*
 * Uniform setters for scalars
 */

void Shader::SetUniform( ShaderUniform const &uni, int i )
{
    glUniform1i( uni.m_Index, i );
}

void Shader::SetUniform( ShaderUniform const &uni, ivec2 const &v )
{
    glUniform2i( uni.m_Index, v.x, v.y);
}

void Shader::SetUniform( ShaderUniform const &uni, ivec3 const &v )
{
    glUniform3i( uni.m_Index, v.x, v.y, v.z);
}

void Shader::SetUniform( ShaderUniform const &uni, ivec4 const &v )
{
    glUniform4i( uni.m_Index, v.x, v.y, v.z, v.w);
}

void Shader::SetUniform( ShaderUniform const &uni, float f )
{
    glUniform1f( uni.m_Index, f);
}

void Shader::SetUniform( ShaderUniform const &uni, vec2 const &v )
{
    glUniform2fv( uni.m_Index, 1, &v[0] );
}

void Shader::SetUniform( ShaderUniform const &uni, vec3 const &v )
{
    glUniform3fv( uni.m_Index, 1, &v[0] );
}

void Shader::SetUniform( ShaderUniform const &uni, vec4 const &v )
{
    glUniform4fv( uni.m_Index, 1, &v[0] );
}

void Shader::SetUniform( ShaderUniform const &uni, mat2 const &m )
{
    glUniformMatrix2fv( uni.m_Index, 1, GL_FALSE, &m[0][0] );
}

void Shader::SetUniform( ShaderUniform const &uni, mat3 const &m )
{
    glUniformMatrix3fv( uni.m_Index, 1, GL_FALSE, &m[0][0] );
}

void Shader::SetUniform( ShaderUniform const &uni, mat4 const &m )
{
    glUniformMatrix4fv( uni.m_Index, 1, GL_FALSE, &m[0][0] );
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
    glUniform1fv( uni.m_Index, v.size(), &v[0] );
}

void Shader::SetUniform( ShaderUniform const &uni, Array<vec2> const &v )
{
    glUniform2fv( uni.m_Index, v.size(), &v[0][0] );
}

void Shader::SetUniform( ShaderUniform const &uni, Array<vec3> const &v )
{
    glUniform3fv( uni.m_Index, v.size(), &v[0][0] );
}

void Shader::SetUniform( ShaderUniform const &uni, Array<vec4> const &v )
{
    glUniform4fv( uni.m_Index, v.size(), &v[0][0] );
}

void Shader::Bind() const
{
    glUseProgram( m_ProgramID );
}

void Shader::Unbind() const
{
    glUseProgram(0);
}



} /*namespace bigball*/