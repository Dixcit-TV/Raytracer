#pragma once
#include <unordered_map>
#include "Material.h"

class MaterialManager
{
public:
	static MaterialManager* GetInstance();
	~MaterialManager();

	void AddMaterial(const std::string & key, Material* pMaterial);
	Material* GetMaterial(const std::string & key) const;

private:
	MaterialManager() = default;

	static MaterialManager* m_Instance;
	std::unordered_map<std::string, Material*> m_pMaterials;
};

