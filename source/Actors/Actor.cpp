#include "Actor.h"

#include <iostream>
#include <string>
#include <cassert>

Actor::Actor(ActorId id) {
	m_id = id;
}

Actor::~Actor() {
	std::cout << "Actor: " << std::string("Destroying Actor ") + std::to_string(m_id) << "\n";
}

bool Actor::Init(tinyxml2::XMLElement* pData) {
	return true;
}

void Actor::PostInit() {
	for (auto it = m_components.begin(); it != m_components.end(); ++it) {
		it->second->VPostInit();
	}
}

void Actor::Destroy() {
	m_components.clear();
}

void Actor::Update(int deltaMs) {
	for (auto it = m_components.begin(); it != m_components.end(); ++it) {
		it->second->VUpdate(deltaMs);
	}
}

void Actor::AddComponent(std::shared_ptr<ActorComponent> pComponent) {
	std::pair<ActorComponents::iterator, bool> success = m_components.insert(std::make_pair(pComponent->VGetComponentId(), pComponent));
	assert(success.second);
}