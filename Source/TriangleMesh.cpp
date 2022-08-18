#include "TriangleMesh.h"
#include "Utils.h"
#include "KDTree.h"

TriangleMesh::TriangleMesh(const std::string& objPath, const Elite::FMatrix4& transform, Material* pMaterial, bool isStatic, CullMode cullMode)
	: Object(transform, pMaterial)
	, m_Bound()
	, m_vBuffer()
	, m_vColor()
	, m_tColor()
	, m_iBuffer()
	, m_pPartitioning(nullptr)
	, m_CullMode(cullMode)
	, m_IsStatic(isStatic)
{
	ObjReader::LoadModel(objPath, m_vBuffer, m_iBuffer);
	m_vColor.resize(m_vBuffer.size(), Elite::RGBColor(1.f, 1.f, 1.f));
	m_tColor.resize(m_iBuffer.size() / 3, Elite::RGBColor());

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
		min.x = std::min(min.x, vertex.x);
		max.x = std::max(max.x, vertex.x);
		min.y = std::min(min.y, vertex.y);
		max.y = std::max(max.y, vertex.y);
		min.z = std::min(min.z, vertex.z);
		max.z = std::max(max.z, vertex.z);
	}

	m_Bound.min = min - Elite::FVector3(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON);
	m_Bound.size = (max + Elite::FVector3(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON)) - min;
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

		// Physically Base Rendering: From Theory to Implementation =======> below equation yields a fairly good estimation of max depth for various scenes
		uint32_t maxDepth{ 8u + static_cast<uint32_t>(1.3f * std::log2(GetTriangleCount())) };
		m_pPartitioning = new KDTree(maxDepth, 1);
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
	for (size_t idx{}; idx < idxCount && (!(isShadowTest & result)); idx += 3)
		result |= TraceTriangle(idx / 3, hitRecord, isShadowTest);

	return result;
}

bool TriangleMesh::TraceTriangle(size_t triIDx, HitRecord& hitRecord, bool isShadowTest) const
{
	Elite::FPoint3 vertices[3]{};
	GetTriangle(triIDx, vertices);

	if (!Math::RayTriangleIntersectionTest(hitRecord, vertices[0], vertices[1], vertices[2], m_CullMode, isShadowTest))
		return false;

	hitRecord.pMaterial = m_pMaterial;
	//hitRecord.color = m_tColor[triIDx];
		
	return true;
}