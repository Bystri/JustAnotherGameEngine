#pragma once

#include <map>
#include <memory>

#include "ActorComponent.h"
#include "../3rdParty/TinyXML2/tinyxml2.h"


class Actor;

typedef unsigned long ActorId;

class Actor
{
	friend class ActorFactory;

	typedef std::map<ComponentId, std::shared_ptr<ActorComponent>> ActorComponents;

public:
	explicit Actor(ActorId id);
	~Actor();

	bool Init(tinyxml2::XMLElement* pData);
	void PostInit();
	void Destroy();

	void Update(int deltaMs);

	ActorId GetId() const { return m_id; }

	// template function for retrieving components
	template <class ComponentType> std::weak_ptr<ComponentType> GetComponent(ComponentId id) {
		ActorComponents::iterator findIt = m_components.find(id);
		if (findIt != m_components.end()) {
			std::shared_ptr<ActorComponent> pBase(findIt->second);
			// cast to subclass version of the pointer
			std::shared_ptr<ComponentType> pSub(std::static_pointer_cast<ComponentType>(pBase));
			std::weak_ptr<ComponentType> pWeakSub(pSub); // convert strong pointer
			// to weak pointer
			return pWeakSub; // return the weak pointer
		} else {
			return std::weak_ptr<ComponentType>();
		}
	}

private:
	// This is called by the ActorFactory; no one else should be
	// adding components.
	void AddComponent(std::shared_ptr<ActorComponent> pComponent);

private:
	ActorId m_id;
	ActorComponents m_components;
};