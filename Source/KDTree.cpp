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

	std::vector<int> allTris(tMesh->GetTriangleCount());
	std::iota(std::begin(allTris), std::end(allTris), 0);
	Subdivide(tMesh, 0, tMesh->GetBound(), allTris, 0);
	m_IsIntialized = true;
}

//http://www.irisa.fr/prive/kadi/Sujets_CTR/kadi/Kadi_sujet2_article_Kdtree.pdf
//KD Tree using Surface Area Heuristic to prioritize large empty space and small crowded space
//Order splitting axis candidates to find incrementally the primitive for each children nodes
void KDTree::Subdivide(const TriangleMesh* tMesh, uint32_t currentNodeIdx, const Bound& nodeBounds, std::vector<int>& tris, uint32_t depth)
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

	//If the lowe3st split cost is better than the no-split cost, create children
	//else make this node a leaf
	if (bestSplit.cost < static_cast<float>(triCount) * 3.f)
	{
		currentNode.axis = axis;
		currentNode.splitValue = splits[bestSplit.splitIdx].position;

		//Store the idx of the first child (the second is right next to it)
		m_Nodes[currentNodeIdx].LChild = static_cast<uint32_t>(m_Nodes.size());
		m_Nodes.push_back(TreeNode());
		m_Nodes.push_back(TreeNode());

		//Retrieve the triangles in the Left node
		std::vector<int> lTris{};
		lTris.reserve(bestSplit.lCount);

		for (uint32_t sIdx{}; sIdx < bestSplit.splitIdx; ++sIdx)
		{
			if (splits[sIdx].type == SplitType::Start)
				lTris.push_back(tris[splits[sIdx].triangleIdx]);
		}
		Subdivide(tMesh, m_Nodes[currentNodeIdx].LChild, bestSplit.lBounds, lTris, depth + 1);

		//Retrieve the triangles in the Right node
		std::vector<int> rTris{};
		rTris.reserve(bestSplit.rCount);

		const uint32_t splitCount{ static_cast<uint32_t>(splits.size()) };
		for (uint32_t sIdx{ bestSplit.splitIdx + 1 }; sIdx < splitCount; ++sIdx)
		{
			if (splits[sIdx].type == SplitType::End)
				rTris.push_back(tris[splits[sIdx].triangleIdx]);
		}
		Subdivide(tMesh, m_Nodes[currentNodeIdx].LChild + 1, bestSplit.rBounbs, rTris, depth + 1);
	}
	else
	{
		currentNode.pCandidates = std::move(tris);
		currentNode.isLeaf = true;
	}
}    

std::vector<Split> KDTree::GetSplitCandidates(const TriangleMesh* tMesh, const std::vector<int>& tris, uint8_t axis) const
{
	const uint32_t triCount{ static_cast<uint32_t>(tris.size()) };
	const uint32_t splitCount{ triCount * 2 };
	std::vector<Split> splits{};
	splits.reserve(splitCount);

	//Create a split candidate for each lower and upper bounds of the triangles
	for (uint32_t idx{}; idx < triCount; ++idx)
	{
		int triIdx = tris[idx] * 3;
		Elite::FPoint3 p0{ tMesh->GetVertex(triIdx) };
		Elite::FPoint3 p1{ tMesh->GetVertex(triIdx + 1) };
		Elite::FPoint3 p2{ tMesh->GetVertex(triIdx + 2) };
		auto pair = std::minmax({ p0[axis], p1[axis], p2[axis] });

		splits.push_back(Split{ pair.first, idx, SplitType::Start });
		splits.push_back(Split{ pair.second, idx, SplitType::End });
	}

	//Order the split candidates by position and prioritize the END split in case of tie
	std::sort(begin(splits), end(splits), [](const Split& s1, const Split& s2)
		{
			if (abs(s1.position - s2.position) < 0.00001f)
				return s1.type > s2.type;

			return s1.position < s2.position;
		});

	return splits;
}

BestSplit KDTree::GetBestSplit(const std::vector<Split>& splits, const Bound& nodeBounds, uint8_t axis, uint32_t triangleCount, uint32_t depth) const
{
	BestSplit bestSplit{}, currentSplit{};
	bestSplit.cost = FLT_MAX;
	currentSplit.lCount = 0;
	currentSplit.rCount = triangleCount;
	const uint32_t splitCount{ static_cast<uint32_t>(splits.size()) };
	uint32_t overlapCount{}, lCount{}, rCount{ triangleCount };
	float totalArea{ 2.f * (nodeBounds.size.x * nodeBounds.size.y + nodeBounds.size.y * nodeBounds.size.z + nodeBounds.size.z * nodeBounds.size.x) };
	float totalAreaInv{ 1 / totalArea };

	const float triIntersectionCost{ 3.f };
	//Incrementally tests the split position to find the lowest cost
	for (uint32_t sIdx = 0; sIdx < splitCount; ++sIdx)
	{
		currentSplit.splitIdx = sIdx;
		
		const Split& split{ splits[sIdx] };
		if (split.type == SplitType::End) //Each END split means a triangle completely changed side 
		{
			++lCount;
			--rCount;
			--overlapCount;
		}
		else
		{
			++overlapCount;
		}

		currentSplit.rCount = rCount;
		currentSplit.lCount = lCount + overlapCount - (split.type == SplitType::Start ? 1 : 0);

		Elite::FPoint3 splitSize{ nodeBounds.size };
		splitSize[axis] = split.position - nodeBounds.botFrontLeft[axis];
		//math::VC3 maxBound{ nodeBounds.getMax() };
		//maxBound.v[axis] = split.position;
		//currentSplit.lBounds = Bound(nodeBounds.getMin(), maxBound);
		//
		//math::VC3 minBound{ nodeBounds.getMin() };
		//minBound.v[axis] = maxBound.v[axis];
		//currentSplit.rBounbs = math::Bounds(minBound, nodeBounds.getMax());

		if (splitSize[axis] <= 0.f || splitSize[axis] >= nodeBounds.size[axis])
			continue;

		float lArea{ 2.f * (splitSize.x * splitSize.y + splitSize.y * splitSize.z + splitSize.z * splitSize.x) };
		Elite::FPoint3 rSize{ nodeBounds.size - Elite::FVector3(splitSize) };
		float rArea{ 2.f * (rSize.x * rSize.y + rSize.y * rSize.z + rSize.z * rSize.x) };

		float lProb = lArea * totalAreaInv;
		float rProb = rArea * totalAreaInv;
		currentSplit.cost = static_cast<float>(depth) + triIntersectionCost * (lProb * static_cast<float>(currentSplit.lCount) + rProb * static_cast<float>(currentSplit.rCount));
		if (currentSplit.cost < bestSplit.cost)
		{
			currentSplit.lBounds = Bound{ nodeBounds.botFrontLeft, splitSize };
			currentSplit.rBounbs = Bound{ nodeBounds.botFrontLeft + Elite::FVector3(splitSize), rSize };
			bestSplit = currentSplit;	
		}
	}

	return bestSplit;
}

bool KDTree::IntersectionTest(const TriangleMesh* tMesh, HitRecord& hRecord, bool isShadowTest) const
{	
	float tMin, tMax;
	return RayBoundsIntersection(hRecord.ray, tMesh->GetBound(), tMin, tMax) ?
		IntersectionNodeTest(tMesh, hRecord, m_Nodes[0], tMin, tMax, isShadowTest)
		: false;
}

//https://graphics.stanford.edu/papers/gpu_kdtree/kdtree.pdf
//Recurisive Tree traversal
//Evaluate "near" & "far" node according to the ray direction
//Evaluate the visit order according to the "step the ray cross the spliting axis
bool KDTree::IntersectionNodeTest(const TriangleMesh* tMesh, HitRecord& hRecord, const TreeNode& currentNode, float tMin, float tMax, bool isShadowTest) const
{
	if (currentNode.isLeaf)
	{
		bool result = false;
		for (int triIdx : currentNode.pCandidates)
		{
			int vIdx{ triIdx * 3 };
			if (Math::RayTriangleInterestionTest(hRecord, tMesh->GetVertex(vIdx), tMesh->GetVertex(vIdx + 1), tMesh->GetVertex(vIdx + 2), tMesh->GetCullMode(), isShadowTest))
			{
				result = true;
				hRecord.pMaterial = tMesh->GetMaterial();
				if (isShadowTest)
					break;
			}
		}

		return result;
	}

	float t{ (currentNode.splitValue - hRecord.ray.origin[currentNode.axis]) / hRecord.ray.direction[currentNode.axis] };
	bool isPositivDir{ hRecord.ray.direction[currentNode.axis] > 0 };
	uint32_t nearNodeIdx{ isPositivDir ? currentNode.LChild : currentNode.LChild + 1 };
	uint32_t farNodeIdx{ isPositivDir ? currentNode.LChild + 1 : currentNode.LChild };

	if (t >= hRecord.ray.maxT)
		return IntersectionNodeTest(tMesh, hRecord, m_Nodes[nearNodeIdx], tMin, tMax, isShadowTest);

	if (t <= hRecord.ray.minT)
		return IntersectionNodeTest(tMesh, hRecord, m_Nodes[farNodeIdx], tMin, tMax, isShadowTest);

	return IntersectionNodeTest(tMesh, hRecord, m_Nodes[nearNodeIdx], tMin, t, isShadowTest)
		|| IntersectionNodeTest(tMesh, hRecord, m_Nodes[farNodeIdx], t, tMax, isShadowTest);
}


//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
bool KDTree::RayBoundsIntersection(const Ray& ray, const Bound& bound, float& tMin, float& tMax) const
{
	const Elite::FPoint3& minBound{ bound.botFrontLeft };
	const Elite::FPoint3& maxBound{ minBound + Elite::FVector3(bound.size) };

	const Elite::FVector3 invRayDir{ 1.f / ray.direction.x, 1.f / ray.direction.y, 1.f / ray.direction.z };
	tMin = invRayDir.x >= 0 ? (minBound.x - ray.origin.x) * invRayDir.x : (maxBound.x - ray.origin.x) * invRayDir.x;
	tMax = invRayDir.x >= 0 ? (maxBound.x - ray.origin.x) * invRayDir.x : (minBound.x - ray.origin.x) * invRayDir.x;
	const float tyMin{ invRayDir.y >= 0 ? (minBound.y - ray.origin.y) * invRayDir.y : (maxBound.y - ray.origin.y) * invRayDir.y };
	const float tyMax{ invRayDir.y >= 0 ? (maxBound.y - ray.origin.y) * invRayDir.y : (minBound.y - ray.origin.y) * invRayDir.y };

	if (tMin > tyMax || tyMin > tMax)
		return false;

	tMin = std::max(tMin, tyMin);
	tMax = std::min(tMax, tyMax);

	const float tzMin{ invRayDir.z >= 0 ? (minBound.z - ray.origin.z) * invRayDir.z : (maxBound.z - ray.origin.z) * invRayDir.z };
	const float tzMax{ invRayDir.z >= 0 ? (maxBound.z - ray.origin.z) * invRayDir.z : (minBound.z - ray.origin.z) * invRayDir.z };

	if (tMin > tzMax || tzMin > tMax)
		return false;

	tMin = std::max(tMin, tzMin);
	tMax = std::min(tMax, tzMax);

	return true;
}