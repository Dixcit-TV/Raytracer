#pragma once
#include "Object.h"
#include <vector>
#include "Enums.h"
#include <string>

class TriangleMesh final : public Object
{
public:
	explicit TriangleMesh(const std::string& objPath, const Elite::FMatrix4& transform, Material* pMaterial, bool isStatic = false, CullMode cullMode = CullMode::NONE);
	~TriangleMesh() override = default;

	void Update(float) override {}
	bool HitCheck(HitRecord& hitRecord, bool isShadowTest = false) const override;

private:
	std::vector<Elite::FPoint3> m_vBuffer;
	std::vector<int> m_iBuffer;
	CullMode m_CullMode;
	bool m_IsStatic;
};

