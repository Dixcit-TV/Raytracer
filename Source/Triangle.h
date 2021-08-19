#pragma once
#include "Object.h"
#include "Enums.h"

class Triangle final : public Object
{
public:
	explicit Triangle(const Elite::FPoint3& v0, const Elite::FPoint3& v1, const Elite::FPoint3& v2, const Elite::FMatrix4& transform, Material* pMaterial, CullMode cullMode = CullMode::NONE);
	~Triangle() override = default;

	void Update(float deltaT) override;
	bool HitCheck(HitRecord & hitRecord, bool isShadowTest = false) const override;

private:
	Elite::FPoint3 m_Vertices[3];
	CullMode m_CullMode;
};

