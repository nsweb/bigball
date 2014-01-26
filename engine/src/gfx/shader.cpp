

#include "../bigball.h"
#include "shader.h"
#include "file.h"




namespace bigball
{


Shader::Shader() :
	VertexShaderID(0),
	FragmentShaderID(0),
	ProgramID(0)
{
	
}

Shader::~Shader()
{
	
}

bool Shader::Create( String const& ShaderName )
{
	ProgramID = glCreateProgram();

	GLenum ShaderTypes[eType::MAX] = { GL_VERTEX_SHADER, GL_TESSELLATE_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER };
	

	ShaderInfo* entry = shaders;
	while ( entry->type != GL_NONE ) {
		GLuint shader = glCreateShader( entry->type );

		entry->shader = shader;

		const GLchar* source = ReadShader( entry->filename );
		if ( source == NULL ) {
			for ( entry = shaders; entry->type != GL_NONE; ++entry ) {
				glDeleteShader( entry->shader );
				entry->shader = 0;
			}

			return 0;
		}

		glShaderSource( shader, 1, &source, NULL );
		delete [] source;

		glCompileShader( shader );

		GLint compiled;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled ) {
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
	if ( !linked ) {
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