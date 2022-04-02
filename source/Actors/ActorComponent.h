#pragma once

#include "../3rdParty/TinyXML2/tinyxml2.h"

#include <memory>

typedef unsigned long ComponentId;

class Actor;

class ActorComponent
{
	friend class ActorFactory;

public:
	virtual ~ActorComponent() {}

	virtual bool VInit(tinyxml2::XMLElement* pData) = 0;
	virtual void VPostInit() { }
	virtual void VUpdate(int deltaMs) {}

	virtual ComponentId VGetComponentId() const = 0;

private:
	void SetOwner(std::shared_ptr<Actor> pOwner) { m_pOwner = pOwner; }

protected:
	std::shared_ptr<Actor> m_pOwner;
};