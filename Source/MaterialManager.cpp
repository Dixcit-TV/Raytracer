#include "MaterialManager.h"

MaterialManager* MaterialManager::m_Instance = nullptr;

MaterialManager* MaterialManager::GetInstance()
{
	return m_Instance ? m_Instance : (m_Instance = new MaterialManager());
}

MaterialManager::~MaterialManager()
{
	for (std::pair<const std::string, Material*>& matPair : m_pMaterials)
		delete matPair.second;

	m_pMaterials.clear();
}

void MaterialManager::AddMaterial(const std::string& key, Material* pMaterial)
{
	m_pMaterials.emplace(key, pMaterial);
}

Material* MaterialManager::GetMaterial(const std::string& key) const
{
	auto cIt = m_pMaterials.find(key);

	if (cIt != m_pMaterials.cend())
		return (*cIt).second;

	return nullptr;
}