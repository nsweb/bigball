

#include "../bigball.h"
#include "shader.h"
#include "../system/file.h"




namespace bigball
{


Shader::Shader() :
	ProgramID(0)
{
	Memory::Memzero( ShaderIDs, sizeof(ShaderIDs) );
}

Shader::~Shader()
{
	DeleteShaders();
}

bool Shader::Create( String const& ShaderName )
{
	ProgramID = glCreateProgram();

	GLenum ShaderTypes[Shader::MAX] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_COMPUTE_SHADER };
	char const* ShaderExts[Shader::MAX] = { "vs", "fs", "tcs", "tes", "gs", "cs" };
	File ShaderFile;
	String ShaderFileName, ShaderSrc;

	for( int32 i = 0; i < Shader::MAX; ++i )
	{
		ShaderFileName = String::Printf( "../data/shader/%s.%s.glsl", ShaderName.c_str(), ShaderExts[i] );
		if( !ShaderFile.Open( ShaderFileName.c_str(), File::Read ) )
			continue;

		ShaderFile.SerializeString( ShaderSrc );

		GLuint ShaderID = glCreateShader( ShaderTypes[i] );

		ShaderIDs[i] = ShaderID;

		const char* SrcStr = ShaderSrc.c_str();
		glShaderSource( ShaderID, 1, &SrcStr, nullptr );
		glCompileShader( ShaderID );

		GLint CompiledStatus;
		glGetShaderiv( ShaderID, GL_COMPILE_STATUS, &CompiledStatus );
		if ( !CompiledStatus ) 
		{
#ifdef _DEBUG
			GLsizei Len;
			glGetShaderiv( ShaderID, GL_INFO_LOG_LENGTH, &Len );

			String LogStr;
			LogStr.resize( Len );
			glGetShaderInfoLog( ShaderID, Len, &Len, LogStr.c_str() );
			BB_LOG( Shader, Log, "Shader compilation failed: %s", LogStr.c_str() );
#endif /* DEBUG */

			DeleteShaders();
			return false;
		}

		glAttachShader( ProgramID, ShaderID );
	}

#ifdef GL_VERSION_4_1
	if ( GLEW_VERSION_4_1 ) 
	{
		// glProgramParameteri( program, GL_PROGRAM_SEPARABLE, GL_TRUE );
	}
#endif /* GL_VERSION_4_1 */

	glLinkProgram( ProgramID );

	GLint LinkedStatus;
	glGetProgramiv( ProgramID, GL_LINK_STATUS, &LinkedStatus );
	if ( !LinkedStatus ) 
	{
#ifdef _DEBUG
		GLsizei Len;
		glGetProgramiv( ProgramID, GL_INFO_LOG_LENGTH, &Len );

		String LogStr;
		LogStr.resize( Len );
		glGetProgramInfoLog( ProgramID, Len, &Len, LogStr.c_str() );
		BB_LOG( Shader, Log, "Shader linking failed: %s", LogStr.c_str() );
#endif /* DEBUG */

		DeleteShaders();
		return false;
	}

	return true;
}

void Shader::DeleteShaders()
{
	for( int32 i = 0; i < Shader::MAX; ++i )
	{
		if( ShaderIDs[i] != 0 )
			glDeleteShader( ShaderIDs[i] );
		ShaderIDs[i] = 0;
	}
	if( ProgramID != 0 )
		glDeleteProgram( ProgramID );
	ProgramID = 0;
}

} /*namespace bigball*/