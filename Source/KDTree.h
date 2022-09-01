#pragma once
#include <vector>
#include "Structs.h"

class TriangleMesh;

enum class SplitType : uint8_t
{
	Start = 0
	, End = 1
};

struct TreeNode
{
	union {
		uint32_t rChild{};
		uint32_t primCount;
		uint8_t flag;
	};

	union {
		float splitValue{};
		uint32_t primOffset;
	};
};

struct Split
{
	float position{};
	uint32_t triangleIdx{};
	SplitType type{};
};

struct BestSplit
{
	Bound lBounds{};
	Bound rBounbs{};
	uint32_t splitIdx{};
	uint32_t lCount{};
	uint32_t rCount{};
	float cost{};
};

class KDTree
{
public:
	explicit KDTree(uint32_t maxDepth = 20, uint32_t maxTriCount = 5);

	void Build(const TriangleMesh* tMesh);
	bool IsInitialized() const { return m_IsIntialized; }
	bool IntersectionTest(const TriangleMesh* tMesh, HitRecord& hRecord, bool isShadowTest) const;

	constexpr static float IntersectCost{ 80.f };
	constexpr static float TraversalCost{ 1.f };

private:
	static const uint8_t kCount = 3;

	std::vector<TreeNode> m_Nodes;
	std::vector<uint32_t> m_Prims;
	uint32_t m_MaxDepth;
	uint32_t m_MinTriCount;
	bool m_IsIntialized;

	void Subdivide(const TriangleMesh* tMesh, uint32_t currentNodeIdx, const Bound& nodeBounds, std::vector<uint32_t>& tris, uint32_t depth);
	std::vector<Split> GetSplitCandidates(const TriangleMesh* tMesh, const std::vector<uint32_t>& tris, uint8_t axis) const;
	BestSplit GetBestSplit(const std::vector<Split>& splits, const Bound& nodeBounds, uint8_t axis, uint32_t triangleCount, uint32_t depth) const;

	bool IntersectionNodeTest(const TriangleMesh* tMesh, HitRecord& hRecord, uint32_t currentNodeIdx, float tMin, float tMax, bool isShadowTest) const;
};

