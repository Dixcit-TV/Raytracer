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

	Elite::FPoint3 GetVertex(int idx) const { return m_IsStatic ? m_vBuffer[m_iBuffer[idx]] : Elite::FPoint3(m_Transform * Elite::FPoint4(m_vBuffer[m_iBuffer[idx]])); }
	CullMode GetCullMode() const { return m_CullMode; }
	const Bound& GetBound() const { return m_Bound; }

	size_t GetTriangleCount() const { return m_iBuffer.size() / 3; }

	const std::vector<int>& GetIndices() const { return m_iBuffer; }

	void SetPartition(bool usePartition);

private:
	Bound m_Bound;
	std::vector<Elite::FPoint3> m_vBuffer;
	std::vector<int> m_iBuffer;
	KDTree* m_pPartitioning;
	CullMode m_CullMode;
	bool m_IsStatic;
};

