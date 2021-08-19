#include "SceneGraph.h"
#include "Object.h"
#include "Light.h"
#include "ProjectSettings.h"

SceneGraph::SceneGraph(SceneGraph&& other) noexcept
	: m_pObjects{ std::move(other.m_pObjects) }
	, m_pLights{ std::move(other.m_pLights) }
{
	other.m_pObjects.clear();
	other.m_pLights.clear();
}

SceneGraph& SceneGraph::operator=(SceneGraph&& other) noexcept
{
	for (Object* pObject : m_pObjects)
		delete pObject;

	m_pObjects.clear();

	m_pObjects = std::move(other.m_pObjects);

	for (Light* pLight : m_pLights)
		delete pLight;

	m_pLights.clear();

	m_pLights = std::move(other.m_pLights);

	return *this;
}

SceneGraph::~SceneGraph()
{
	for (Object* pObject : m_pObjects)
		delete pObject;

	m_pObjects.clear();

	for (Light* pLight : m_pLights)
		delete pLight;

	m_pLights.clear();
}

const std::vector<Object*>& SceneGraph::GetObjects() const
{
	return m_pObjects;
}

const std::vector<Light*>& SceneGraph::GetLights() const
{
	return m_pLights;
}

void SceneGraph::AddObject(Object* pNewObject)
{
	m_pObjects.push_back(pNewObject);
}

void SceneGraph::AddLight(Light* pNewLight)
{
	m_pLights.push_back(pNewLight);
}

void SceneGraph::Update(float deltaT)
{
	for (Object* pObject : m_pObjects)
		pObject->Update(deltaT);

	m_pLights[0]->SetIsOn(ProjectSettings::GetInstance()->IsPointLight1Enabled());
	m_pLights[1]->SetIsOn(ProjectSettings::GetInstance()->IsPointLight2Enabled());
	m_pLights[2]->SetIsOn(ProjectSettings::GetInstance()->IsDirectionalLightEnabled());
}