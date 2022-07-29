#include "KDTree.h"
#include <algorithm>

//KDTree::KDTree(uint32_t maxDepth, uint32_t maxTriCount)
//	: bounds{ bounds }
//	, nodes()
//	, maxDepth(maxDepth)
//	, minTriCount(maxTriCount)
//	, isIntialized(false)
//{}
//
//void KDTree::build(const Bound& bound, const std::vector<Triangle>& tris)
//{	
//	bounds = bound;
//	nodes.push_back(TreeNode());
//
//	std::vector<const Triangle*> allTris{};
//	allTris.reserve(tris.size());
//
//	for (const Triangle& t : tris)
//		allTris.push_back(&t);
//
//	subdivide(0, bounds, allTris, 0);
//	isIntialized = true;
//}
//
////http://www.irisa.fr/prive/kadi/Sujets_CTR/kadi/Kadi_sujet2_article_Kdtree.pdf
////KD Tree using Surface Area Heuristic to prioritize large empty space and small crowded space
////Order splitting axis candidates to find incrementally the primitive for each children nodes
//void KDTree::subdivide(uint32_t currentNodeIdx, const Bound& nodeBounds, std::vector<const Triangle*>& tris, uint32_t depth)
//{
//	TreeNode& currentNode{ nodes[currentNodeIdx] };
//	const uint32_t triCount{ static_cast<uint32_t>(tris.size()) };
//
//	//Reached the limits, make this node a leaf
//	if (depth >= maxDepth || triCount <= minTriCount)
//	{
//		currentNode.pCandidates = std::move(tris);
//		currentNode.isLeaf = true;
//		return;
//	}
//
//	//Search for the "best" split axis, aka. the largest axis
//	uint8_t axis = 0;
//	float maxDist = 0;
//	for (uint8_t k{}; k < kCount; ++k)
//	{
//		const float dist{ nodeBounds.size[k] };
//		if (dist > maxDist)
//		{
//			axis = k;
//			maxDist = dist;
//		}
//	}
//
//	std::vector<Split> splits{ getSplitCandidates(tris, axis) };
//	BestSplit bestSplit{ getBestSplit(splits, nodeBounds, axis, triCount, depth) };
//
//	//If the lowe3st split cost is better than the no-split cost, create children
//	//else make this node a leaf
//	if (bestSplit.cost < static_cast<float>(triCount) * 3.f)
//	{
//		currentNode.axis = axis;
//		currentNode.splitValue = splits[bestSplit.splitIdx].position;
//
//		//Store the idx of the first child (the second is right next to it)
//		nodes[currentNodeIdx].LChild = static_cast<uint32_t>(nodes.size());
//		nodes.push_back(TreeNode());
//		nodes.push_back(TreeNode());
//
//		//Retrieve the triangles in the Left node
//		std::vector<const Triangle*> lTris{};
//		lTris.reserve(bestSplit.lCount);
//
//		for (uint32_t sIdx{}; sIdx < bestSplit.splitIdx; ++sIdx)
//		{
//			if (splits[sIdx].type == SplitType::Start)
//				lTris.push_back(tris[splits[sIdx].triangleIdx]);
//		}
//		subdivide(nodes[currentNodeIdx].LChild, bestSplit.lBounds, lTris, depth + 1);
//
//		//Retrieve the triangles in the Right node
//		std::vector<const Triangle*> rTris{};
//		rTris.reserve(bestSplit.rCount);
//
//		const uint32_t splitCount{ static_cast<uint32_t>(splits.size()) };
//		for (uint32_t sIdx{ bestSplit.splitIdx + 1 }; sIdx < splitCount; ++sIdx)
//		{
//			if (splits[sIdx].type == SplitType::End)
//				rTris.push_back(tris[splits[sIdx].triangleIdx]);
//		}
//		subdivide(nodes[currentNodeIdx].LChild + 1, bestSplit.rBounbs, rTris, depth + 1);
//	}
//	else
//	{
//		currentNode.pCandidates = std::move(tris);
//		currentNode.isLeaf = true;
//	}
//}    
//
//std::vector<Split> KDTree::getSplitCandidates(const std::vector<const Triangle*>& tris, uint8_t axis) const
//{
//	const uint32_t triCount{ static_cast<uint32_t>(tris.size()) };
//	const uint32_t splitCount{ triCount * 2 };
//	std::vector<Split> splits{};
//	splits.reserve(splitCount);
//
//	//Create a split candidate for each lower and upper bounds of the triangles
//	for (uint32_t idx{}; idx < triCount; ++idx)
//	{
//		math::VC3 lBound{};
//		math::VC3 hBound{};
//		tris[idx]->getBounds(lBound, hBound);
//
//		splits.push_back(Split{ lBound.v[axis], idx, SplitType::Start });
//		splits.push_back(Split{ hBound.v[axis], idx, SplitType::End });
//	}
//
//	//Order the split candidates by position and prioritize the END split in case of tie
//	std::sort(begin(splits), end(splits), [](const Split& s1, const Split& s2)
//		{
//			if (abs(s1.position - s2.position) < 0.00001f)
//				return s1.type > s2.type;
//
//			return s1.position < s2.position;
//		});
//
//	return splits;
//}
//
//BestSplit KDTree::getBestSplit(const std::vector<Split>& splits, const Bound& nodeBounds, uint8_t axis, uint32_t triangleCount, uint32_t depth) const
//{
//	BestSplit bestSplit{}, currentSplit{};
//	bestSplit.cost = FLT_MAX;
//	currentSplit.lCount = 0;
//	currentSplit.rCount = triangleCount;
//	const uint32_t splitCount{ static_cast<uint32_t>(splits.size()) };
//	uint32_t overlapCount{}, lCount{}, rCount{ triangleCount };
//	float totalArea{ 2.f * (nodeBounds.size.x * nodeBounds.size.y + nodeBounds.size.y * nodeBounds.size.z + nodeBounds.size.z * nodeBounds.size.x) };
//	float totalAreaInv{ 1 / totalArea };
//
//	const float triIntersectionCost{ 3.f };
//	//Incrementally tests the split position to find the lowest cost
//	for (uint32_t sIdx = 0; sIdx < splitCount; ++sIdx)
//	{
//		currentSplit.splitIdx = sIdx;
//		
//		const Split& split{ splits[sIdx] };
//		if (split.type == SplitType::End) //Each END split means a triangle completely changed side 
//		{
//			++lCount;
//			--rCount;
//			--overlapCount;
//		}
//		else
//		{
//			++overlapCount;
//		}
//
//		currentSplit.rCount = rCount;
//		currentSplit.lCount = lCount + overlapCount - (split.type == SplitType::Start ? 1 : 0);
//
//		Elite::FPoint3 splitSize{ nodeBounds.size };
//		splitSize[axis] = split.position - nodeBounds.botFrontLeft[axis];
//		//math::VC3 maxBound{ nodeBounds.getMax() };
//		//maxBound.v[axis] = split.position;
//		//currentSplit.lBounds = Bound(nodeBounds.getMin(), maxBound);
//		//
//		//math::VC3 minBound{ nodeBounds.getMin() };
//		//minBound.v[axis] = maxBound.v[axis];
//		//currentSplit.rBounbs = math::Bounds(minBound, nodeBounds.getMax());
//
//		if (splitSize[axis] <= 0.f || splitSize[axis] <= 0.f >= nodeBounds.size[axis])
//			continue;
//
//		float lArea{ 2.f * (splitSize.x * splitSize.y + splitSize.y * splitSize.z + splitSize.z * splitSize.x) };
//		Elite::FPoint3 rSize{ nodeBounds.size - Elite::FVector3(splitSize) };
//		float rArea{ 2.f * (rSize.x * rSize.y + rSize.y * rSize.z + rSize.z * rSize.x) };
//
//		float lProb = lArea * totalAreaInv;
//		float rProb = rArea * totalAreaInv;
//		currentSplit.cost = static_cast<float>(depth) + triIntersectionCost * (lProb * static_cast<float>(currentSplit.lCount) + rProb * static_cast<float>(currentSplit.rCount));
//		if (currentSplit.cost < bestSplit.cost)
//		{
//			currentSplit.lBounds = Bound{ nodeBounds.botFrontLeft, splitSize };
//			currentSplit.rBounbs = Bound{ nodeBounds.botFrontLeft + Elite::FVector3(splitSize), rSize };
//			bestSplit = currentSplit;	
//		}
//	}
//
//	return bestSplit;
//}
//
//bool KDTree::intersectionTest(Ray& ray) const
//{	
//	float tMin, tMax;
//	return rayBoundsIntersection(ray, bounds, tMin, tMax) ?
//		intersectionNodeTest(ray, nodes[0], tMin, tMax)
//		: false;
//}
//
////https://graphics.stanford.edu/papers/gpu_kdtree/kdtree.pdf
////Recurisive Tree traversal
////Evaluate "near" & "far" node according to the ray direction
////Evaluate the visit order according to the "step the ray cross the spliting axis
//bool KDTree::intersectionNodeTest(Ray& ray, const TreeNode& currentNode, float tMin, float tMax) const
//{
//	if (currentNode.isLeaf)
//	{
//		bool somethingFound = false;
//		for (const Triangle* tri : currentNode.pCandidates)
//		{
//			if (ray.startingPoint != tri && tri->intersects(ray) == 1)
//			{
//				/* We have a hit */
//				somethingFound = true;
//				if (ray.intersectionType == IntersectionTypeAny)
//					break;
//			}
//		}
//
//		/* Maybe we found something, maybe not */
//		return somethingFound;
//	}
//
//	float t{ (currentNode.splitValue - ray.origin[currentNode.axis]) / ray.direction[currentNode.axis] };
//	bool isPositivDir{ ray.direction[currentNode.axis] > 0 };
//	uint32_t nearNodeIdx{ isPositivDir ? currentNode.LChild : currentNode.LChild + 1 };
//	uint32_t farNodeIdx{ isPositivDir ? currentNode.LChild + 1 : currentNode.LChild };
//
//	if (t >= tMax)
//		return intersectionNodeTest(ray, nodes[nearNodeIdx], tMin, tMax);
//
//	if (t <= tMin)
//		return intersectionNodeTest(ray, nodes[farNodeIdx], tMin, tMax);
//
//	return intersectionNodeTest(ray, nodes[nearNodeIdx], tMin, t)
//		|| intersectionNodeTest(ray, nodes[farNodeIdx], t, tMax);
//}
//
//
////https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
//bool KDTree::rayBoundsIntersection(const Ray& ray, const Bound& bound, float& tMin, float& tMax) const
//{
//	const Elite::FPoint3& minBound{ bound.botFrontLeft };
//	const Elite::FPoint3& maxBound{ minBound + Elite::FVector3(bound.size) };
//
//	const Elite::FVector3 invRayDir{ 1.f / ray.direction.x, 1.f / ray.direction.y, 1.f / ray.direction.z };
//	tMin = invRayDir.x >= 0 ? (minBound.x - ray.origin.x) * invRayDir.x : (maxBound.x - ray.origin.x) * invRayDir.x;
//	tMax = invRayDir.x >= 0 ? (maxBound.x - ray.origin.x) * invRayDir.x : (minBound.x - ray.origin.x) * invRayDir.x;
//	const float tyMin{ invRayDir.y >= 0 ? (minBound.y - ray.origin.y) * invRayDir.y : (maxBound.y - ray.origin.y) * invRayDir.y };
//	const float tyMax{ invRayDir.y >= 0 ? (maxBound.y - ray.origin.y) * invRayDir.y : (minBound.y - ray.origin.y) * invRayDir.y };
//
//	if (tMin > tyMax || tyMin > tMax)
//		return false;
//
//	tMin = std::max(tMin, tyMin);
//	tMax = std::min(tMax, tyMax);
//
//	const float tzMin{ invRayDir.z >= 0 ? (minBound.z - ray.origin.z) * invRayDir.z : (maxBound.z - ray.origin.z) * invRayDir.z };
//	const float tzMax{ invRayDir.z >= 0 ? (maxBound.z - ray.origin.z) * invRayDir.z : (minBound.z - ray.origin.z) * invRayDir.z };
//
//	if (tMin > tzMax || tzMin > tMax)
//		return false;
//
//	tMin = std::max(tMin, tzMin, ray.zeroDistanceThreshold);
//	tMax = std::min(tMax, tzMax);
//
//	return true;
//}