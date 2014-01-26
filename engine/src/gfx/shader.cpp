

#include "../bigball.h"
#include "shader.h"
#include "file.h"




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

	GLenum ShaderTypes[eType::MAX] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_COMPUTE_SHADER };
	char const* ShaderExts[eType::MAX] = { vs, ps, tcs, tes, gs, cs };
	File ShaderFile;
	String ShaderFileName, ShaderSrc;

	for( int32 i = 0; i < eType::MAX; ++i )
	{
		ShaderFileName = String:Printf( "%s.%s.glsl", ShaderName.c_str(), ShaderExts[i] );
		if( !ShaderFile.Open( ShaderFileName.c_str(), File::Read ) )
			continue;

		size_t FileSize = ShaderFile.GetFileSize();
		ShaderFile.SerializeString( ShaderSrc );

		GLuint ShaderID = glCreateShader( ShaderTypes[i] );

		ShaderIDs[i] = ShaderID;

		glShaderSource( ShaderID, 1, &ShaderSrc[0], nullptr );
		glCompileShader( ShaderID );

		GLint compiled;
		glGetShaderiv( ShaderID, GL_COMPILE_STATUS, &compiled );
		if ( !compiled ) 
		{
#ifdef _DEBUG
			GLsizei len;
			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

			GLchar* log = new GLchar[len+1];
			glGetShaderInfoLog( shader, len, &len, log );
			std::cerr << "Shader compilation failed: " << log << std::endl;
			delete [] log;
#endif /* DEBUG */

			return 0;
		}

		glAttachShader( program, shader );

		++entry;
	}

#ifdef GL_VERSION_4_1
	if ( GLEW_VERSION_4_1 ) {
		// glProgramParameteri( program, GL_PROGRAM_SEPARABLE, GL_TRUE );
	}
#endif /* GL_VERSION_4_1 */

	glLinkProgram( program );

	GLint linked;
	glGetProgramiv( program, GL_LINK_STATUS, &linked );
	if ( !linked ) 
	{
#ifdef _DEBUG
		GLsizei len;
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );

		GLchar* log = new GLchar[len+1];
		glGetProgramInfoLog( program, len, &len, log );
		std::cerr << "Shader linking failed: " << log << std::endl;
		delete [] log;
#endif /* DEBUG */

		for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
			glDeleteShader( entry->shader );
			entry->shader = 0;
		}

		return 0;
	}

	return program;
}


} /*namespace bigball*/