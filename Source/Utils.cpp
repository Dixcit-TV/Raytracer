#include "Utils.h"

Elite::FPoint3 Math::GetRayOriginOffset(const Elite::FPoint3 p, const Elite::FVector3 n)
{
	Elite::IPoint3 of_i(int(int_scale() * n.x), int(int_scale() * n.y), int(int_scale() * n.z));
	Elite::IPoint3 pi(*(int*)&p.x + ((p.x < 0.f) ? -of_i.x : of_i.x)
		, *(int*)&p.y + ((p.y < 0.f) ? -of_i.y : of_i.y)
		, *(int*)&p.z + ((p.z < 0.f) ? -of_i.z : of_i.z));

	return Elite::FPoint3(fabsf(p.x) < origin() ? p.x + float_scale() * n.x : *(float*)&pi.x
		, fabsf(p.y) < origin() ? p.y + float_scale() * n.y : *(float*)&pi.y
		, fabsf(p.z) < origin() ? p.z + float_scale() * n.z : *(float*)&pi.z);
}

Elite::FVector3 Math::GetCosineWeightedPoint(float r1, float r2)
{
	float r{ sqrtf(r1) };
	float a{ 2 * PI * r2 };

	return Elite::FVector3{ cosf(a) * r, sinf(a) * r, sqrtf(1 - r1) };
}

Elite::FMatrix3 Math::GetONB(const Elite::FVector3& lookAt)
{
	Elite::FVector3 right{ Cross(Utils::GetWorldY<float>(), lookAt) };
	Normalize(right);
	Elite::FVector3 up{ Cross(lookAt, right) };

	return Elite::FMatrix3(right.x, up.x, lookAt.x
		, right.y, up.y, lookAt.y
		, right.z, up.z, lookAt.z);
}

//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
bool Math::RayBoundsIntersection(const Ray& ray, const Bound& bound, float& tMin, float& tMax)
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

void ObjReader::LoadModel(const std::string& objPath, std::vector<Elite::FPoint3>& vertexBuffer, std::vector<int>& indexBuffer)
{
	std::ifstream objStream{ objPath, std::ios::in | std::ios::binary };

	if (!objStream.is_open())
		std::cout << "Error: Could not open obj file \"" << objPath << "\".";

	std::string line{};

	while (!objStream.eof())
	{
		//std::regex_search(line, color_match, color_regex)
		std::getline(objStream, line, '\n');

		if (!line.empty())
		{
			float values[3];
     		if (sscanf_s(line.c_str(), "%*s %f %f %f", values, values + 1, values + 2) == 3)
			{
				if (line._Starts_with("v "))
				{
					vertexBuffer.push_back(Elite::FPoint3{ values[0], values[1], values[2] });
				}
				else if (line._Starts_with("f "))
				{
					indexBuffer.push_back(static_cast<int>(values[0]) - 1);
					indexBuffer.push_back(static_cast<int>(values[1]) - 1);
					indexBuffer.push_back(static_cast<int>(values[2]) - 1);
				}
			}
		}
	}

	auto vertexHash = [](const Elite::FPoint3& v)
	{
		return std::hash<std::string>()(std::to_string(v.x) + std::to_string(v.y) + std::to_string(v.z));
	};

	std::unordered_set<Elite::FPoint3, decltype(vertexHash)> s{ vertexBuffer.size(), vertexHash };
	std::unordered_map<Elite::FPoint3, int, decltype(vertexHash)> originals{ vertexBuffer.size(), vertexHash };
	int idx{ 0 };
	auto end = std::copy_if(vertexBuffer.begin(), vertexBuffer.end(), vertexBuffer.begin(),
		[&s, &originals, &idx, &indexBuffer](const Elite::FPoint3 v)
		{
			if (s.insert(v).second)
			{
				originals.emplace(v, idx);
				++idx;
				return true;
			}
			else
			{
				std::replace(indexBuffer.begin(), indexBuffer.end(), idx, originals[v]);
				return false;
			}
		});

	vertexBuffer.erase(end, vertexBuffer.end());
}