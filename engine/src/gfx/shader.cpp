

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
	
}

bool Shader::Create( String const& ShaderName )
{
	ProgramID = glCreateProgram();

	GLenum ShaderTypes[Shader::MAX] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_COMPUTE_SHADER };
	char const* ShaderExts[Shader::MAX] = { "vs", "ps", "tcs", "tes", "gs", "cs" };
	File ShaderFile;
	String ShaderFileName, ShaderSrc;

	for( int32 i = 0; i < Shader::MAX; ++i )
	{
		ShaderFileName = String::Printf( "%s.%s.glsl", ShaderName.c_str(), ShaderExts[i] );
		if( !ShaderFile.Open( ShaderFileName.c_str(), File::Read ) )
			continue;

		size_t FileSize = ShaderFile.GetFileSize();
		ShaderFile.SerializeString( ShaderSrc );

		GLuint ShaderID = glCreateShader( ShaderTypes[i] );

		ShaderIDs[i] = ShaderID;

		const char* SrcStr = ShaderSrc.c_str();
		glShaderSource( ShaderID, 1, &SrcStr, nullptr );
		glCompileShader( ShaderID );

		GLint compiled;
		glGetShaderiv( ShaderID, GL_COMPILE_STATUS, &compiled );
		if ( !compiled ) 
		{
#ifdef _DEBUG
			GLsizei len;
			glGetShaderiv( ShaderID, GL_INFO_LOG_LENGTH, &len );

			GLchar* log = new GLchar[len+1];
			glGetShaderInfoLog( ShaderID, len, &len, log );
			BB_LOG( Shader, Log, "Shader compilation failed: %s", log );
			delete [] log;
#endif /* DEBUG */

			return 0;
		}

		glAttachShader( ProgramID, ShaderID );
	}

#ifdef GL_VERSION_4_1
	if ( GLEW_VERSION_4_1 ) {
		// glProgramParameteri( program, GL_PROGRAM_SEPARABLE, GL_TRUE );
	}
#endif /* GL_VERSION_4_1 */

	glLinkProgram( ProgramID );

	GLint linked;
	glGetProgramiv( ProgramID, GL_LINK_STATUS, &linked );
	if ( !linked ) 
	{
#ifdef _DEBUG
		GLsizei len;
		glGetProgramiv( ProgramID, GL_INFO_LOG_LENGTH, &len );

		GLchar* log = new GLchar[len+1];
		glGetProgramInfoLog( ProgramID, len, &len, log );
		BB_LOG( Shader, Log, "Shader linking failed: %s", log );
		delete [] log;
#endif /* DEBUG */

		for( int32 i = 0; i < Shader::MAX; ++i )
		{
			if( ShaderIDs[i] != 0 )
				glDeleteShader( ShaderIDs[i] );
			ShaderIDs[i] = 0;
		}

		return 0;
	}

	return ProgramID != 0;
}


} /*namespace bigball*/