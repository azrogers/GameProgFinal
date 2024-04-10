#include <memory>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <plog/Log.h>

#include "Shader.hpp"

using glm::vec4; using glm::mat4;

namespace eg {
    template<typename T, typename U>
    string show_log_info(GLuint object, T glGet__iv, U glGet__InfoLog) {
        GLint log_length;
        
        glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
        unique_ptr<char[]> buffer(new char[log_length]());
        glGet__InfoLog(object, log_length, NULL, buffer.get());
        return string(buffer.get());
    }
}

namespace eg {
    template<>
    Name<Shader>::~Name() {
        if (name) glDeleteShader(name);
    }
    
    Shader::Shader(GLenum type, string const& source):
        Name(glCreateShader(type))
    {
        GLint length = static_cast<GLint>(source.length());
        char const* text = source.c_str();
        glShaderSource(name, 1, &text, &length);
        glCompileShader(name);
        GLint shaderStatus = false;
        glGetShaderiv(name, GL_COMPILE_STATUS, &shaderStatus);
		if(!shaderStatus)
		{
			LOG_ERROR << "Failed to compile shader: " << show_log_info(name, glGetShaderiv, glGetShaderInfoLog);
			throw invalid_argument("Failed to compile shader.");
		}

		std::hash<string> hasher;
		hash = hasher(source);
    }

	LightUniform::LightUniform(eg::Program* program, std::string lightName) :
		type(program->Uniform<GLint>(lightName + ".type")),
		position(program->Uniform<glm::vec3>(lightName + ".position")),
		color(program->Uniform<glm::vec3>(lightName + ".color")),
		ambient(program->Uniform<glm::vec3>(lightName + ".ambient")),
		direction(program->Uniform<glm::vec3>(lightName + ".direction")),
		falloff(program->Uniform<float>(lightName + ".falloff")),
		spotAngle(program->Uniform<float>(lightName + ".spotAngle")),
		innerAngle(program->Uniform<float>(lightName + ".innerAngle")),
		radius(program->Uniform<float>(lightName + ".radius"))
	{};
    
    template<>
    Name<Program>::Name():
        name(glCreateProgram())
    {}
    
    template<>
    Name<Program>::~Name() {
        if (name) glDeleteProgram(name);
    }
    
    Program::Program(Shader const& vertex, Shader const& fragment) {
        list<Shader const*> shaders;
        shaders.push_back(&vertex);
        shaders.push_back(&fragment);
        Link(shaders);
    }
    
    Program::Program(Shader const& vertex, Shader const& fragment, std::unordered_map<string, GLint> const& bindings) {
        Bind(bindings);
        list<Shader const*> shaders;
        shaders.push_back(&vertex);
        shaders.push_back(&fragment);
        Link(shaders);
    }
    
    void Program::Activate( void ) const {
        glUseProgram(name);
    }

	int Program::GetShaderHash() const
	{
		return shaderHash;
	}
    
    template<>
    Program::UniformAccessor<GLint>::operator GLint() const {
        GLint value;
        glGetUniformiv(program.name, index, &value);
        return value;
    }
    
    template<>
    Program::UniformAccessor<GLuint>::operator GLuint() const {
        GLuint value;
        glGetUniformuiv(program.name, index, &value);
        return value;
    }

	template<>
	Program::UniformAccessor<float>::operator float() const
	{
		float value;
		glGetUniformfv(program.name, index, &value);
		return value;
	}

	template<>
	Program::UniformAccessor<vec3>::operator vec3() const
	{
		vec3 value;
		glGetUniformfv(program.name, index, glm::value_ptr(value));
		return value;
	}
    
    template<>
    Program::UniformAccessor<vec4>::operator vec4() const {
        vec4 value;
        glGetUniformfv(program.name, index, glm::value_ptr(value));
        return value;
    }
    
    template<>
    Program::UniformAccessor<mat4>::operator mat4() const {
        mat4 value;
        glGetUniformfv(program.name, index, glm::value_ptr(value));
        return value;
    }
    
    template<>
    Program::UniformAccessor<GLint>& Program::UniformAccessor<GLint>::operator=(GLint const& value) {
        glProgramUniform1i(program.name, index, value);
        return *this;
    }
    
    template<>
    Program::UniformAccessor<GLuint>& Program::UniformAccessor<GLuint>::operator=(GLuint const& value) {
        glProgramUniform1ui(program.name, index, value);
        return *this;
    }

	template<>
	Program::UniformAccessor<float>& Program::UniformAccessor<float>::operator=(float const& value)
	{
		glProgramUniform1f(program.name, index, value);
		return *this;
	}

	template<>
	Program::UniformAccessor<vec3>& Program::UniformAccessor<vec3>::operator=(vec3 const& value)
	{
		glProgramUniform3fv(program.name, index, 1, glm::value_ptr(value));
		return *this;
	}
    
    template<>
    Program::UniformAccessor<vec4>& Program::UniformAccessor<vec4>::operator=(vec4 const& value) {
        glProgramUniform4fv(program.name, index, 1, glm::value_ptr(value));
        return *this;
    }
    
    template<>
    Program::UniformAccessor<mat4>& Program::UniformAccessor<mat4>::operator= (mat4 const& value) {
        glProgramUniformMatrix4fv(program.name, index, 1, GL_FALSE, glm::value_ptr(value));
        return *this;
    }

    void Program::Bind(std::unordered_map<string, GLint> const& bindings) {
        for (auto pair: bindings) {
            glBindAttribLocation(name, pair.second, pair.first.c_str());
        }
    }
    
    void Program::Link(std::list<Shader const*> sh) {
		for(Shader const* shader : sh)
		{
			glAttachShader(name, shader->name);
		}

        glLinkProgram(name);
		for(Shader const* shader : sh)
		{
			glDetachShader(name, shader->name);
		}

        GLint linkStatus = false;
        glGetProgramiv(name, GL_LINK_STATUS, &linkStatus);
		if(!linkStatus)
		{
			LOG_ERROR << "Failed to link program: " << show_log_info(name, glGetProgramiv, glGetProgramInfoLog);
			throw invalid_argument("Failed to link program.");
		}

		// compute shader hash - currently it's the hash of the input shaders xored
		shaderHash = sh.front()->GetHash();
		auto it = sh.begin();
		// iterate from second element
		std::advance(it, 1);
		while(it != sh.end())
		{
			auto shader = *it;
			shaderHash = shaderHash ^ shader->GetHash();
			it++;
		}

		// fill light uniform cache
		lightUniformCache = new LightUniform*[SHADER_MAX_LIGHTS];
		for(int i = 0; i < SHADER_MAX_LIGHTS; i++)
		{
			std::string lightName = "lights[" + std::to_string(i) + "]";
			lightUniformCache[i] = new LightUniform(this, lightName);
		}
    }
    
    template<>
    Name<Program::Pipeline>::Name() {
        glGenProgramPipelines(1, &name);
    }
    
    template<>
    Name<Program::Pipeline>::~Name() {
        if (name) glDeleteProgramPipelines(1, &name);
    }
    
    Program::Pipeline::Pipeline(): Name()
    {}
    
    Program::Pipeline::Pipeline(Program const& vertex, Program const& fragment): Name() {
        glUseProgramStages(name, GL_VERTEX_SHADER_BIT, vertex.name);
        glUseProgramStages(name, GL_FRAGMENT_SHADER_BIT, fragment.name);
    }
    
    void Program::Pipeline::Activate( void ) const {
        glUseProgram(0);
        glBindProgramPipeline(name);
    }
    
    void Program::Pipeline::Deactivate( void ) {
        glBindProgramPipeline(0);
    }
}