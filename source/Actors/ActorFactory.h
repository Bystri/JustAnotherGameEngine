#pragma once

#include <map>
#include <string>

#include "Actor.h"

typedef ActorComponent* (*ActorComponentCreator)();
typedef std::map<std::string, ActorComponentCreator> ActorComponentCreatorMap;

class ActorFactory
{
public:
	ActorFactory();

	void AddComponentCreator(const std::string& name, ActorComponentCreator pComponentCreator) {
		m_actorComponentCreators.insert({ name, pComponentCreator });
	}

	std::shared_ptr<Actor> CreateActor(const char* actorResource);

protected:
	virtual std::shared_ptr<ActorComponent> CreateComponent(tinyxml2::XMLElement* pData);

private:
	ActorId GetNextActorId() { ++m_lastActorId; return m_lastActorId; }

protected:
	ActorComponentCreatorMap m_actorComponentCreators;

private:
	ActorId m_lastActorId;
};