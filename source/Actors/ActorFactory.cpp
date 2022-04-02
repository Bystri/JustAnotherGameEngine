#include "ActorFactory.h"
#include "../3rdParty/TinyXML2/tinyxml2.h"

#include <iostream>

ActorFactory::ActorFactory() {
	m_lastActorId = -1;
}

std::shared_ptr<Actor> ActorFactory::CreateActor(const char* actorResource) {
	// Grab the root XML node
	tinyxml2::XMLDocument xmlDoc; 
	xmlDoc.LoadFile(actorResource);
	tinyxml2::XMLElement* pRoot = xmlDoc.RootElement();
	if (!pRoot) {
		std::cerr << "Failed to create actor from resource : " + std::string(actorResource) << std::endl;
		return std::shared_ptr<Actor>();
	}

	// create the actor instance
	std::shared_ptr<Actor> pActor(new Actor(GetNextActorId()));
	if (!pActor->Init(pRoot)) {
		std::cerr << "Failed to initialize actor : " + std::string(actorResource) << std::endl;
		return std::shared_ptr<Actor>();
	}

	// Loop through each child element and load the component
	for (tinyxml2::XMLElement* pNode = pRoot->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()) {
		std::shared_ptr<ActorComponent> pComponent(CreateComponent(pNode));
		if (pComponent) {
			pActor->AddComponent(pComponent);
			pComponent->SetOwner(pActor);
		} else {
			return std::shared_ptr<Actor>();
		}
	}
		// Now that the actor has been fully created, run the post init phase
	pActor->PostInit();
	return pActor;
}

std::shared_ptr<ActorComponent> ActorFactory::CreateComponent(tinyxml2::XMLElement* pData)
{
	std::string name(pData->Value());
	std::shared_ptr<ActorComponent> pComponent;
	auto findIt = m_actorComponentCreators.find(name);
	if (findIt != m_actorComponentCreators.end()) {
		ActorComponentCreator creator = findIt->second;
		pComponent.reset(creator());
	} else {
		std::cerr << "Couldn’t find ActorComponent named " + name << std::endl;
		return std::shared_ptr<ActorComponent>(); // fail
	}
	// initialize the component if we found one
	if (pComponent) {
		if (!pComponent->VInit(pData)) {
			std::cerr << "Component failed to initialize : " + name << std::endl;
			return std::shared_ptr<ActorComponent>();
		}
	}
	// pComponent will be NULL if the component wasn’t found. This isn’t
	// necessarily an error since you might have a custom CreateComponent()
	// function in a subclass.
	return pComponent;
} 