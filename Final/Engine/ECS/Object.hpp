#pragma once

#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <list>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

#include "../Graphics/Texture.hpp"
#include "../Graphics/Shader.hpp"
#include "../Graphics/Mesh.hpp"

namespace eg
{
	class Object;
};

#include "Component.hpp"
#include "Transform.hpp"
#include "../Renderer.hpp"

namespace eg
{
	class Object
	{
	public:
		/**
		 * Create a new object.
		 * @param The parent of this object. If nullptr, this object is at the root level.
		 */
		Object(Object *parent = nullptr);
		~Object();

		/**
		 * Updates this object, all attached components, and all its children.
		 * @param The number of seconds since last update.
		 */
		void Update(float seconds);

		/**
		 * Renders this object, all attached components, and all its children.
		 * @param renderer The renderer that will be used.
		 */
		void Render(eg::Renderer* renderer) const;

		/**
		 * Returns the render order. This is currently just the hash of the
		 * shader program, so that all objects using the same shader render
		 * in order (so less data needs to be uploaded each frame).
		 * 
		 * CURRENTLY NOT IMPLEMENTED!
		 */
		int RenderOrder() const;

		/**
		 * Sets the parent of this object.
		 * @param This object's new parent. If nullptr, this object will be root.
		 */
		void Parent(Object *newParent);

		/**
		 * Returns the parent of this object.
		 */
		Object *Parent() const { return parent; }

		/**
		 * Adds a new child to this object.
		 * @param The child to add.
		 */
		void Add(Object *child);

		/**
		 * Removes a child from this object.
		 * @param The child to remove.
		 */
		void Remove(Object *child);

		/**
		 * Returns the first object under this one that has
		 * the given name, or nullptr.
		 * @param The name to search for.
		 */
		eg::Object* FindFirstWithName(std::string name);

		vector<eg::Object*> FindWithName(std::string name);
		void FindWithName(std::string name, vector<eg::Object*>* store);

		/**
		 * Adds a new component to this object.
		 * @param A pointer to an instance of a Component.
		 */
		template<class T> T *AddComponent(T *);

		/**
		 * Returns the first component of the given type on this object.
		 */
		template<class T> T *GetComponent();

		/**
		 * Returns an iterator to all components of this type on this object.
		 */
		template<class T> 
		std::pair<std::unordered_multimap<std::type_index, Component *>::iterator,
			std::unordered_multimap<std::type_index, Component *>::iterator>
			GetComponents();

		/**
		 * Removes all components of this type from this object.
		 */
		template<class T> void RemoveComponent();
		
		/**
		 * Returns a matrix that represents this object in world space.
		 */
		glm::mat4 WorldMatrix() const;

		Transform CumulativeTransform() const;

		/**
		 * The transform for this object.
		 */
		Transform transform;

		/**
		 * The name of this object.
		 */
		std::string name = "Object";

		static friend void to_json(json& j, const eg::Object& o)
		{
			json componentArr = json::array();
			for(auto c : o.components)
			{
				componentArr.push_back(*c.second);
			}
			json childArr = json::array();
			for(auto c : o.children)
			{
				childArr.push_back(*c);
			}

			j = json{
				{ "name", o.name },
				{ "transform", o.transform },
				{ "components", componentArr },
				{ "children", childArr }
			};
		}
	private:
		std::unordered_multimap<std::type_index, Component *> components;
		std::list<Object *> children;
		Object *parent = nullptr;
	};

	template<class T>
	inline T *Object::AddComponent(T *component)
	{
		components.insert(std::pair<std::type_index, Component *>(typeid(T), component));
		component->object = this;
		return component;
	}

	template<class T>
	inline std::pair<
		std::unordered_multimap<std::type_index, Component*>::iterator,
		std::unordered_multimap<std::type_index, Component*>::iterator>
		Object::GetComponents()
	{
		return components.equal_range(typeid(T));
	}

	template<class T>
	inline T *Object::GetComponent()
	{
		return ++GetComponents<T>().first;
	}

	template<class T>
	inline void Object::RemoveComponent()
	{
		for(auto c : components.equal_range(typeid(T)))
			delete c.second;
		components.erase(typeid(T));
	}
}