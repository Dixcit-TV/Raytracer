#pragma once
#include "Structs.h"
#include "ERGBColor.h"

class Material;

class Object
{
public:
	virtual ~Object() = default;

	virtual void Update(float deltaT) = 0;
	virtual bool HitCheck(HitRecord& hitRecord, bool isShadowTest = false) const = 0;

protected:
	Object(const Elite::FMatrix4& transform, Material* pMaterial);

	Elite::FMatrix4 m_Transform;
	Material* m_pMaterial;
};

