#include "KDTree.h"
#include <algorithm>
#include <numeric>
#include "Utils.h"
#include "TriangleMesh.h"

KDTree::KDTree(uint32_t maxDepth, uint32_t maxTriCount)
	: m_Nodes()
	, m_MaxDepth(maxDepth)
	, m_MinTriCount(maxTriCount)
	, m_IsIntialized(false)
{}

void KDTree::Build(const TriangleMesh* tMesh)
{	
	m_Nodes.push_back(TreeNode());

	std::vector<uint32_t> allTris(tMesh->GetTriangleCount());
	std::iota(std::begin(allTris), std::end(allTris), 0u);
	Subdivide(tMesh, 0, tMesh->GetBound(), allTris, 0);
	m_IsIntialized = true;
}

//http://www.irisa.fr/prive/kadi/Sujets_CTR/kadi/Kadi_sujet2_article_Kdtree.pdf
//KD Tree using Surface Area Heuristic to prioritize large empty space and small crowded space
//Order splitting axis candidates to find incrementally the primitive for each children nodes
void KDTree::Subdivide(const TriangleMesh* tMesh, uint32_t currentNodeIdx, const Bound& nodeBounds, std::vector<uint32_t>& tris, uint32_t depth)
{
	TreeNode& currentNode{ m_Nodes[currentNodeIdx] };
	const uint32_t triCount{ static_cast<uint32_t>(tris.size()) };

	//Reached the limits, make this node a leaf
	if (depth >= m_MaxDepth || triCount <= m_MinTriCount)
	{
		currentNode.pCandidates = std::move(tris);
		currentNode.isLeaf = true;
		return;
	}

	//Search for the "best" split axis, aka. the largest axis
	uint8_t axis = 0;
	float maxDist = 0;
	for (uint8_t k{}; k < kCount; ++k)
	{
		const float dist{ nodeBounds.size[k] };
		if (dist > maxDist)
		{
			axis = k;
			maxDist = dist;
		}
	}

	std::vector<Split> splits{ GetSplitCandidates(tMesh, tris, axis) };
	BestSplit bestSplit{ GetBestSplit(splits, nodeBounds, axis, triCount, depth) };

	//If the lowest split cost is better than the no-split cost, create children
	//else make this node a leaf
	if (bestSplit.cost < static_cast<float>(triCount) * IntersectCost)
	{
		currentNode.axis = axis;
		currentNode.splitValue = splits[bestSplit.splitIdx].position;

		m_Nodes.push_back(TreeNode());

		//Retrieve the triangles in the Left node
		std::vector<uint32_t> lTris{};
		lTris.reserve(bestSplit.lCount);

		for (uint32_t sIdx{}; sIdx < bestSplit.splitIdx; ++sIdx)
		{
			if (splits[sIdx].type == SplitType::Start)
				lTris.push_back(splits[sIdx].triangleIdx);
		}
		Subdivide(tMesh, currentNodeIdx + 1, bestSplit.lBounds, lTris, depth + 1);

		//Store the idx of the first child (the second is right next to it)
		uint32_t rChildIdx{ static_cast<uint32_t>(m_Nodes.size()) };
		m_Nodes[currentNodeIdx].rChild = rChildIdx;
		m_Nodes.push_back(TreeNode());
		//Retrieve the triangles in the Right node
		std::vector<uint32_t> rTris{};
		rTris.reserve(bestSplit.rCount);

		const uint32_t splitCount{ static_cast<uint32_t>(splits.size()) };
		for (uint32_t sIdx{ bestSplit.splitIdx + 1 }; sIdx < splitCount; ++sIdx)
		{
			if (splits[sIdx].type == SplitType::End)
				rTris.push_back(splits[sIdx].triangleIdx);
		}
		Subdivide(tMesh, rChildIdx, bestSplit.rBounbs, rTris, depth + 1);
	}
	else
	{
		currentNode.pCandidates = std::move(tris);
		currentNode.isLeaf = true;
	}
}    

std::vector<Split> KDTree::GetSplitCandidates(const TriangleMesh* tMesh, const std::vector<uint32_t>& tris, uint8_t axis) const
{
	const uint32_t triCount{ static_cast<uint32_t>(tris.size()) };
	const uint32_t splitCount{ triCount * 2 };
	std::vector<Split> splits{};
	splits.reserve(splitCount);

	//Create a split candidate for each lower and upper bounds of the triangles
	for (uint32_t idx{}; idx < triCount; ++idx)
	{
		uint32_t triIdx{ tris[idx] };
		Elite::FPoint3 vertices[3];
		tMesh->GetTriangle(triIdx, vertices);

		auto minMaxPair{ std::minmax({ vertices[0][axis], vertices[1][axis], vertices[2][axis] }) };
		splits.push_back(Split{ minMaxPair.first, triIdx, SplitType::Start });
		splits.push_back(Split{ minMaxPair.second, triIdx, SplitType::End });
	}

	//Order the split candidates by position and prioritize the END split in case of tie
	std::sort(begin(splits), end(splits), [](const Split& s1, const Split& s2)
		{
			if (abs(s1.position - s2.position) < FLT_EPSILON)
				return s1.type > s2.type;

			return s1.position < s2.position;
		});

	return splits;
}

BestSplit KDTree::GetBestSplit(const std::vector<Split>& splits, const Bound& nodeBounds, uint8_t axis, uint32_t triangleCount, uint32_t depth) const
{
	BestSplit bestSplit{}, currentSplit{};
	currentSplit.lCount = 0;
	currentSplit.rCount = triangleCount;
	bestSplit.cost = FLT_MAX;
	const uint32_t splitCount{ static_cast<uint32_t>(splits.size()) };
	float totalArea{ 2.f * (nodeBounds.size.x * nodeBounds.size.y + nodeBounds.size.y * nodeBounds.size.z + nodeBounds.size.z * nodeBounds.size.x) };
	float totalAreaInv{ 1 / totalArea };

	//Incrementally tests the split position to find the lowest cost
	for (uint32_t sIdx = 0; sIdx < splitCount; ++sIdx)
	{
		currentSplit.splitIdx = sIdx;
		
		const Split& split{ splits[sIdx] };
		if (split.type == SplitType::End) //Each END split means a triangle completely changed side 
			--currentSplit.rCount;

		Elite::FVector3 splitSize{ nodeBounds.size };
		splitSize[axis] = split.position - nodeBounds.min[axis];
		float lProb{ 2.f * (splitSize.x * splitSize.y + splitSize.y * splitSize.z + splitSize.z * splitSize.x) * totalAreaInv };

		Elite::FVector3 rSize{ nodeBounds.size };
		rSize[axis] -= splitSize[axis];
		float rProb{ 2.f * (rSize.x * rSize.y + rSize.y * rSize.z + rSize.z * rSize.x) * totalAreaInv };

		currentSplit.cost = static_cast<float>(depth) * TraversalCost + IntersectCost * (lProb * static_cast<float>(currentSplit.lCount) + rProb * static_cast<float>(currentSplit.rCount));
		if (currentSplit.cost < bestSplit.cost)
		{
			currentSplit.lBounds = Bound{ nodeBounds.min, splitSize };
			Elite::FVector3 offset{ Elite::FVector3::ZeroVector() };
			offset[axis] = split.position;
			currentSplit.rBounbs = Bound{ nodeBounds.min + offset, rSize };
			bestSplit = currentSplit;
		}
		
		if (split.type == SplitType::Start)
			++currentSplit.lCount;
	}

	return bestSplit;
}

bool KDTree::IntersectionTest(const TriangleMesh* tMesh, HitRecord& hRecord, bool isShadowTest) const
{	
	float tMin, tMax;
	return RayBoundsIntersection(hRecord.ray, tMesh->GetBound(), tMin, tMax) ?
		IntersectionNodeTest(tMesh, hRecord, 0, tMin, tMax, isShadowTest)
		: false;

	//float tMin, tMax;
	//if (!RayBoundsIntersection(hRecord.ray, tMesh->GetBound(), tMin, tMax))
	//	return false;

	//std::vector<Elite::FVector3> queue{};
	//queue.push_back(Elite::FVector3(0, tMin, tMax));
	//bool result{ false };
	//while (queue.size() != 0 && !(isShadowTest && result))
	//{
	//	Elite::FVector3 currentNodeInfo{ queue.back() };
	//	uint32_t currentNodeIdx{ uint32_t(currentNodeInfo.x) };
	//	const TreeNode& currentNode{ m_Nodes[currentNodeIdx]};
	//	queue.pop_back();

	//	if (currentNode.isLeaf)
	//	{

	//		const size_t triCount{ currentNode.pCandidates.size() };
	//		for (size_t idx{}; idx < triCount && (!(isShadowTest && result)); ++idx)
	//			result |= tMesh->TraceTriangle(currentNode.pCandidates[idx], hRecord, isShadowTest);
	//	}
	//	else
	//	{
	//		float t{ (currentNode.splitValue - hRecord.ray.origin[currentNode.axis]) / hRecord.ray.direction[currentNode.axis] };
	//		bool belowFirst{ hRecord.ray.direction[currentNode.axis] > 0 };
	//		uint32_t nearNodeIdx{ belowFirst ? currentNodeIdx + 1 : currentNode.rChild };
	//		uint32_t farNodeIdx{ belowFirst ? currentNode.rChild : currentNodeIdx + 1 };

	//		if (t >= currentNodeInfo.z)
	//			queue.push_back(Elite::FVector3(float(nearNodeIdx), currentNodeInfo.y, currentNodeInfo.z));
	//		else if (t <= currentNodeInfo.y)
	//			queue.push_back(Elite::FVector3(float(farNodeIdx), currentNodeInfo.y, currentNodeInfo.z));
	//		else
	//		{
	//			queue.push_back(Elite::FVector3(float(farNodeIdx), t, currentNodeInfo.z));
	//			queue.push_back(Elite::FVector3(float(nearNodeIdx), currentNodeInfo.y, t));
	//		}
	//	}
	//}

	//return result;
}

//https://graphics.stanford.edu/papers/gpu_kdtree/kdtree.pdf
//Recurisive Tree traversal
//Evaluate "near" & "far" node according to the ray direction
//Evaluate the visit order according to the "step the ray cross the spliting axis
bool KDTree::IntersectionNodeTest(const TriangleMesh* tMesh, HitRecord& hRecord, uint32_t currentNodeIdx, float tMin, float tMax, bool isShadowTest) const
{
 	const TreeNode& currentNode{ m_Nodes[currentNodeIdx] };
	if (currentNode.isLeaf)
	{
		bool result{ false };
		const size_t triCount{ currentNode.pCandidates.size() };
		for (size_t idx{}; idx < triCount && (!(isShadowTest & result)); ++idx)
			result |= tMesh->TraceTriangle(currentNode.pCandidates[idx], hRecord, isShadowTest);

		return result;
	}

	float t{ (currentNode.splitValue - hRecord.ray.origin[currentNode.axis]) / hRecord.ray.direction[currentNode.axis] };
	bool belowFirst{ hRecord.ray.direction[currentNode.axis] > 0 };
	uint32_t nearNodeIdx{ belowFirst ? currentNodeIdx + 1 : currentNode.rChild };
	uint32_t farNodeIdx{ belowFirst ? currentNode.rChild : currentNodeIdx + 1 };

	if (t >= tMax)
		return IntersectionNodeTest(tMesh, hRecord, nearNodeIdx, tMin, tMax, isShadowTest);

	if (t <= tMin)
		return IntersectionNodeTest(tMesh, hRecord, farNodeIdx, tMin, tMax, isShadowTest);

	return IntersectionNodeTest(tMesh, hRecord, nearNodeIdx, tMin, t, isShadowTest) 
		| IntersectionNodeTest(tMesh, hRecord, farNodeIdx, t, tMax, isShadowTest);
}


//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
bool KDTree::RayBoundsIntersection(const Ray& ray, const Bound& bound, float& tMin, float& tMax) const
{
	const Elite::FVector3 invRayDir{ 1.f / ray.direction.x, 1.f / ray.direction.y, 1.f / ray.direction.z };

	Elite::FPoint3 tL{ bound.min - ray.origin };
	Elite::FPoint3 tH{ tL + bound.size };
	tL.x *= invRayDir.x;
	tL.y *= invRayDir.y;
	tL.z *= invRayDir.z;

	tH.x *= invRayDir.x;
	tH.y *= invRayDir.y;
	tH.z *= invRayDir.z;

	auto minMaxX{ std::minmax(tL.x, tH.x) };
	auto minMaxY{ std::minmax(tL.y, tH.y) };
	auto minMaxZ{ std::minmax(tL.z, tH.z) };

	tMin = std::max({ minMaxX.first, minMaxY.first, minMaxZ.first, ray.minT });
	tMax = std::min({ minMaxX.second, minMaxY.second, minMaxZ.second, ray.maxT });

	return tMin <= tMax;
}