#include <algorithm>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "Object.hpp"

namespace eg
{
	Object::Object(Object* newParent) : transform(this)
	{
		if(newParent != nullptr)
			newParent->Add(this);
	};

	Object::~Object()
	{
		for(auto c : children)
			delete c;
		for(auto comp : components)
			delete comp.second;
	}

	void Object::Parent(Object *newParent)
	{
		if(newParent != nullptr)
			newParent->Add(this);
		else
		{
			if(parent != nullptr)
				parent->Remove(this);
			parent = nullptr;
		}
	}

	void Object::Add(Object* child)
	{
		if(child->parent != nullptr)
			child->parent->Remove(child);
		child->parent = this;
		children.push_back(child);
	}

	void Object::Remove(Object *child)
	{
		children.remove(child);
		child->parent = nullptr;
	}

	eg::Object* Object::FindFirstWithName(std::string name)
	{
		if(this->name == name)
			return this;
		for(auto c : children)
		{
			eg::Object* obj = c->FindFirstWithName(name);
			if(obj != nullptr)
				return obj;
		}

		return nullptr;
	}

	vector<eg::Object*> Object::FindWithName(std::string name)
	{
		vector<eg::Object*> objects;
		FindWithName(name, &objects);
		return objects;
	}

	void Object::FindWithName(std::string name, vector<eg::Object*>* store)
	{
		if(this->name == name)
			store->push_back(this);

		for(auto c : children)
		{
			c->FindWithName(name, store);
		}
	}

	void Object::Render(eg::Renderer* renderer) const
	{
		for(auto c : children)
			c->Render(renderer);
		for(auto comp : components)
			comp.second->Render(renderer);
	}

	int Object::RenderOrder() const
	{
		int order = 0;
		for(auto comp : components)
		{
			if(comp.second->HasRenderOrder())
			{
				order ^= comp.second->RenderOrder();
			}
		}

		return order;
	}

	void Object::Update(float seconds)
	{
		transform.Update(seconds);

		for(auto c : children)
			c->Update(seconds);
		for(auto comp : components)
			comp.second->Update(seconds);
	}

	glm::mat4 Object::WorldMatrix() const
	{
		if(parent != nullptr)
			return parent->WorldMatrix() * transform.Matrix();
		return transform.Matrix();
	}

	Transform Object::CumulativeTransform() const
	{
		if(parent == nullptr)
			return transform;
		return Transform::Add(transform, parent->CumulativeTransform());
	}
}