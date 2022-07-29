#include "TriangleMesh.h"
#include "Utils.h"
#include "KDTree.h"

TriangleMesh::TriangleMesh(const std::string& objPath, const Elite::FMatrix4& transform, Material* pMaterial, bool isStatic, CullMode cullMode)
	: Object(transform, pMaterial)
	, m_Bound()
	, m_vBuffer()
	, m_iBuffer()
	, m_pPartitioning(nullptr)
	, m_CullMode(cullMode)
	, m_IsStatic(isStatic)
{
	ObjReader::LoadModel(objPath, m_vBuffer, m_iBuffer);

	if (isStatic)
	{
		for (Elite::FPoint3& vertex : m_vBuffer)
		{
			vertex = Elite::FPoint3(m_Transform * Elite::FPoint4(vertex));
		}
	}

	Elite::FPoint3 min{ FLT_MAX, FLT_MAX, FLT_MAX }, max{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (Elite::FPoint3& vertex : m_vBuffer)
	{
		for (uint8_t axis{ 0 }; axis < 3; ++axis)
		{
			min[axis] = std::min(min[axis], vertex[axis]);
			max[axis] = std::max(max[axis], vertex[axis]);
		}
	}

	m_Bound.botFrontLeft = min;
	m_Bound.size = max - Elite::FVector3(min);
}

TriangleMesh::~TriangleMesh()
{
	if (m_pPartitioning)
		delete m_pPartitioning;
}

void TriangleMesh::SetPartition(bool usePartition)
{
	if (usePartition)
	{
		if (m_pPartitioning)
		{
			if (m_pPartitioning->IsInitialized())
				return;
			else
				delete m_pPartitioning;
		}

		m_pPartitioning = new KDTree(60, 4);
		m_pPartitioning->Build(this);
	}
	else
	{
		if (m_pPartitioning)
			delete m_pPartitioning;
	}
}

bool TriangleMesh::HitCheck(HitRecord& hitRecord, bool isShadowTest) const
{
	if (m_pPartitioning && m_pPartitioning->IsInitialized())
		return m_pPartitioning->IntersectionTest(this, hitRecord, isShadowTest);

	
	bool result{ false };
	const size_t idxCount{ m_iBuffer.size() };
	for (size_t idx{}; idx < idxCount; idx += 3)
	{
		//Dynamic meshes: Calculate vertex position each frame
		//Static meshes: Use pre-calculated vertex position
		int vIdx0{ m_iBuffer[idx] };
		int vIdx1{ m_iBuffer[idx + 1] };
		int vIdx2{ m_iBuffer[idx + 2] };
		Elite::FPoint3 worldV0 = m_IsStatic ? m_vBuffer[vIdx0] : Elite::FPoint3(m_Transform * Elite::FPoint4(m_vBuffer[vIdx0]));
		Elite::FPoint3 worldV1 = m_IsStatic ? m_vBuffer[vIdx1] : Elite::FPoint3(m_Transform * Elite::FPoint4(m_vBuffer[vIdx1]));
		Elite::FPoint3 worldV2 = m_IsStatic ? m_vBuffer[vIdx2] : Elite::FPoint3(m_Transform * Elite::FPoint4(m_vBuffer[vIdx2]));

		if (Math::RayTriangleInterestionTest(hitRecord, worldV0, worldV1, worldV2, m_CullMode, isShadowTest))
		{
			result = true;
			hitRecord.pMaterial = m_pMaterial;
			if (isShadowTest)
				break;
		}
	}

	return result;
}