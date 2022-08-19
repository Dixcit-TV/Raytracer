#pragma once
#include "Object.h"
#include <vector>
#include "Enums.h"
#include <string>

class KDTree;

class TriangleMesh final : public Object
{
public:
	explicit TriangleMesh(const std::string& objPath, const Elite::FMatrix4& transform, Material* pMaterial, bool isStatic = false, CullMode cullMode = CullMode::NONE);
	virtual ~TriangleMesh() override;

	void Update(float) override {}
	bool HitCheck(HitRecord& hitRecord, bool isShadowTest = false) const override;
	bool TraceTriangle(size_t triIDx, HitRecord& hitRecord, bool isShadowTest = false) const;

	void setTriangleColor(size_t triId, const Elite::RGBColor& newColor){ m_tColor[triId] = newColor; }

	void GetTriangle(size_t triId, Elite::FPoint3 vertices[3]) const
	{ 
		Elite::IPoint3 indices{  };
		memcpy(indices.data, &m_iBuffer[triId * 3], 3 * sizeof(int));
		Elite::FPoint3 v[3]{ m_vBuffer[indices.x], m_vBuffer[indices.y], m_vBuffer[indices.z] };

		vertices[0] = m_IsStatic ? v[0] : Elite::FPoint3(m_Transform * Elite::FPoint4(v[0]));
		vertices[1] = m_IsStatic ? v[1] : Elite::FPoint3(m_Transform * Elite::FPoint4(v[1]));
		vertices[2] = m_IsStatic ? v[2] : Elite::FPoint3(m_Transform * Elite::FPoint4(v[2]));
	}

	CullMode GetCullMode() const { return m_CullMode; }
	const Bound& GetBound() const { return m_Bound; }

	size_t GetTriangleCount() const { return m_iBuffer.size() / 3; }

	const std::vector<int>& GetIndices() const { return m_iBuffer; }

	void SetPartition(bool usePartition);

private:
	Bound m_Bound;
	std::vector<Elite::FPoint3> m_vBuffer;
	std::vector<Elite::RGBColor> m_vColor;
	std::vector<Elite::RGBColor> m_tColor;
	std::vector<int> m_iBuffer;
	KDTree* m_pPartitioning;
	CullMode m_CullMode;
	bool m_IsStatic;
};

