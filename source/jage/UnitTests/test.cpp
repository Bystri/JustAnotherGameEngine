#include "pch.h"

#include "../../Actors/ActorFactory.h"

class FirstTestComponent : public ActorComponent {
public:
	const static ComponentId COMPONENT_ID = 1;

	virtual bool VInit(tinyxml2::XMLElement* pData) override {
		tinyxml2::XMLElement* pNestedParameters = pData->FirstChildElement("NestedParameters");
		if (pNestedParameters) {
			tinyxml2::XMLElement* pIntegers = pNestedParameters->FirstChildElement("Integers");
			
			if (pIntegers) {
				x = std::atoi(pIntegers->Attribute("x"));
				y = std::atoi(pIntegers->Attribute("y"));
				z = std::atoi(pIntegers->Attribute("z"));
			} else {
				return false;
			}

			tinyxml2::XMLElement* pFloats = pNestedParameters->FirstChildElement("Floats");

			if (pFloats) {
				floatNumber = std::atof(pFloats->Attribute("f"));
			}
			else {
				return false;
			}
		} else {
			return false;
		}

		return true;
	}

	virtual void VPostInit() override { }
	virtual void VUpdate(int deltaMs) override {}

	virtual ComponentId VGetComponentId() const override { return COMPONENT_ID; }

	long getX() const { return x; }
	long getY() const { return y; }
	long getZ() const { return z; }

	double getFloat() const { return floatNumber; }

private:
	long x = 0;
	long y = 0;
	long z = 0;

	double floatNumber = 0.0;
};

class SecondTestComponent : public ActorComponent {
public:
	const static ComponentId COMPONENT_ID = 2;

	virtual bool VInit(tinyxml2::XMLElement* pData) override {
		tinyxml2::XMLElement* pParameter = pData->FirstChildElement("Parameter");
		if (pParameter) {
			parameter = std::atoi(pParameter->Attribute("p"));
		}
		else {
			return false;
		}

		return true;
	}

	virtual void VPostInit() override { }
	virtual void VUpdate(int deltaMs) override {}

	virtual ComponentId VGetComponentId() const override { return COMPONENT_ID; }

	long getParameter() const { return parameter; }

private:
	long parameter = 0;
};

ActorComponent* CreateFirstTestComponent() {
	return new FirstTestComponent();
}

ActorComponent* CreateSecondTestComponent() {
	return new SecondTestComponent();
}

TEST(TestCaseName, TestName) {
	ActorFactory actorFactory;
	actorFactory.AddComponentCreator("FirstTestComponent", CreateFirstTestComponent);
	actorFactory.AddComponentCreator("SecondTestComponent", CreateSecondTestComponent);

	std::shared_ptr<Actor> pTestActor = actorFactory.CreateActor("TestAssets/Actors/test_actor.xml");
	EXPECT_TRUE(pTestActor != nullptr);
	EXPECT_EQ(pTestActor->GetId(), 0);

	std::weak_ptr<FirstTestComponent> pFirstComponent = pTestActor->GetComponent<FirstTestComponent>(1);
	std::shared_ptr<FirstTestComponent> pStrongFirstComponent = pFirstComponent.lock();
	EXPECT_TRUE(pStrongFirstComponent != nullptr);
	EXPECT_EQ(pStrongFirstComponent->getX(), 0);
	EXPECT_EQ(pStrongFirstComponent->getY(), 1);
	EXPECT_EQ(pStrongFirstComponent->getZ(), -1);
	EXPECT_TRUE(std::abs(pStrongFirstComponent->getFloat() - 3.14f) <= std::numeric_limits<float>::epsilon());

	std::weak_ptr<SecondTestComponent> pSecondComponent = pTestActor->GetComponent<SecondTestComponent>(2);
	std::shared_ptr<SecondTestComponent> pStrongSecondComponent = pSecondComponent.lock();
	EXPECT_TRUE(pStrongSecondComponent != nullptr);
	EXPECT_EQ(pStrongSecondComponent->getParameter(), 42);
}

#include "../../ResourceCache/ZipFile.h"

TEST(ResourceCache, ZibFile) {
	ZipFile zipFile;

	bool initialized = zipFile.Init(L"TestAssets/Archives/TestZip.zip");
	EXPECT_TRUE(initialized);

	int index = zipFile.Find("1.txt");
	EXPECT_EQ(index, 0);
	
	char* buffer = nullptr;
	buffer = new char[zipFile.GetFileLen(index)];
	
	if (buffer) {
		zipFile.ReadFile(index, buffer);
	}

	for (int i = 1; i <= 5; ++i) {
		EXPECT_EQ(buffer[i - 1] - '0', i);
	}

	zipFile.End();
}

#include "../ResourceCache/ResCache.h"

TEST(ResourceCache, ResCache) {
	ResourceZipFile zipFile(L"TestAssets/Archives/TestZip.zip");
	ResCache resCache(50U, &zipFile);

	bool initialized = resCache.Init();
	EXPECT_TRUE(initialized);

	Resource resource("1.txt");
	std::shared_ptr<ResHandle> textRes = resCache.GetHandle(&resource);
	
	EXPECT_TRUE(textRes);
	char* buffer = (char*)textRes->Buffer();

	for (int i = 1; i <= 5; ++i) {
		EXPECT_EQ(buffer[i - 1] - '0', i);
	}
}