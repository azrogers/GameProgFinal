#pragma once

#include <string>
#include <list>
#include <unordered_map>
using namespace std;

#include "../OpenGL.hpp"

// should match define in shader file
#define SHADER_MAX_LIGHTS 16

namespace eg
{
	class LightUniform;

	class Shader : public Name<Shader>
	{
	public:
		Shader(GLenum kind, string const& source);

		int GetHash() const { return hash; }
	protected:
		friend class Program;
		int hash;
	};

	class Program : public Name<Program>
	{
	public:
		Program(Shader const& vertex, Shader const& fragment);
		Program(Shader const& vertex, Shader const& fragment, std::unordered_map<string, GLint> const& bindings);

		template<template <typename> class C>
		Program(C<Shader*> const& sh)
		{
			Link(list<Shader const*>(begin(sh), end(sh)));
		}

		template<template <typename> class C>
		Program(C<Shader*> const& sh, std::unordered_map<string, GLint> const& bindings)
		{
			Bind(bindings);
			Link(list<Shader const*>(begin(sh), end(sh)));
		}

		void Activate(void) const;
		int GetShaderHash() const;

			template <typename V>
		class UniformAccessor
		{
		public:
			UniformAccessor & operator= (V const& value);

			operator V () const;
		private:
			friend class Program;
			UniformAccessor(GLint i, Program const& prog) : index(i), program(prog) {}
			GLint const index;
			Program const& program;
		};

		template <typename V>
		UniformAccessor<V> Uniform(string const& uniformName)
		{
			return UniformAccessor<V>(glGetUniformLocation(name, uniformName.c_str()), *this);
		}

		template <typename V>
		UniformAccessor<V> const Uniform(string const& uniformName) const
		{
			return UniformAccessor<V>(glGetUniformLocation(name, uniformName.c_str()), *this);
		}

		LightUniform** GetLightUniforms() { return lightUniformCache; }

		class Pipeline : public Name<Pipeline>
		{
		public:
			Pipeline();

			Pipeline(Program const& vertex, Program const& fragment);

			template<template <typename, typename> class C>
			Pipeline(C<Program*, GLenum> elements) : Name()
			{
				for(auto assoc : elements) glUseProgramStages(name, assoc.second, assoc.first->name);
			}

			void Activate(void) const;

			static void Deactivate(void);
		};

	private:
		void Bind(std::unordered_map<string, GLint> const& bindings);
		void Link(std::list<Shader const*> sh);

		int shaderHash;
		LightUniform** lightUniformCache;
	};

	class LightUniform
	{
	public:
		Program::UniformAccessor<GLint> type;
		Program::UniformAccessor<glm::vec3> position, color, ambient, direction;
		Program::UniformAccessor<float> falloff, spotAngle, innerAngle, radius;

		LightUniform(eg::Program* program, std::string lightName);
	};
}