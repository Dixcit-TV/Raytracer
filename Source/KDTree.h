#pragma once
#include <vector>
#include "Structs.h"

enum class SplitType : uint8_t
{
	Start = 0
	, End = 1
};

struct TreeNode
{
	//uint32_t LChild{};
	//std::vector<const Triangle*> pCandidates{};
	//float splitValue{};
	//uint8_t axis{};
	//bool isLeaf{};
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
//public:
//	explicit KDTree(uint32_t maxDepth = 20, uint32_t maxTriCount = 5);
//
//	void build(const math::VC3& minBound, const math::VC3& maxBound, const std::vector<Triangle>& tris);
//	bool isInitialized() const { return isIntialized; }
//	bool intersectionTest(Ray& ray) const;
//
//private:
//	static const uint8_t kCount = 3;
//
//	Bound bounds;
//	std::vector<TreeNode> nodes;
//	uint32_t maxDepth;
//	uint32_t minTriCount;
//	bool isIntialized;
//
//	void subdivide(uint32_t currentNodeIdx, const Bound& nodeBounds, std::vector<const Triangle*>& tris, uint32_t depth);
//	std::vector<Split> getSplitCandidates(const std::vector<const Triangle*>& tris, uint8_t axis) const;
//	BestSplit getBestSplit(const std::vector<Split>& splits, const Bound& nodeBounds, uint8_t axis, uint32_t triangleCount, uint32_t depth) const;
//
//	bool intersectionNodeTest(Ray& ray, const TreeNode& currentNode, float tMin, float tMax) const;
//
//	bool rayBoundsIntersection(const Ray& ray, const Bound& bound, float& tMin, float& tMax) const;
};

