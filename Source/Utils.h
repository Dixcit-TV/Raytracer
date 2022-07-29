#pragma once
#include "EMath.h"
#include "Structs.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <regex>
#include <unordered_set>
#include <unordered_map>

namespace Utils
{
	template<typename T>
	inline Elite::Vector<3, T> GetWorldX()
	{
		return { static_cast<T>(1.f), static_cast<T>(0.f), static_cast<T>(0.f) };
	}

	template<typename T>
	inline Elite::Vector<3, T> GetWorldZ()
	{
		return { static_cast<T>(0.f), static_cast<T>(0.f), static_cast<T>(1.f) };
	}

	template<typename T>
	inline Elite::Vector<3, T> GetWorldY()
	{
		return { static_cast<T>(0.f), static_cast<T>(1.f), static_cast<T>(0.f) };
	}

	template<int N, int M, typename T>
	void PrintMatrix(const Elite::Matrix<N, M, T>& matrix)
	{
		for (int m{ 0 }; m < M; ++m)
		{
			std::cout << "|";
			for (int n{ 0 }; n < N; ++n)
			{
				std::cout << std::setw(7) << std::left << std::setprecision(3) << matrix[n][m];
			}
			std::cout << "|\n";
		}

		std::cout << "\n\n";
	}
};

namespace Math
{
	template<typename T>
	bool RayTriangleInterestionTest(HitRecord& hitRecord, const Elite::Point<3, T>& p0, const Elite::Point<3, T>& p1, const Elite::Point<3, T>& p2, CullMode cullMode, bool isShadowTest = false)
	{
		//Möller-Trumblore algorithm: 
		//http://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/raytri_tam.pdf
		//https://www.youtube.com/watch?v=fK1RPmF_zjQ

		Elite::FVector3 edge01, edge02, V0ToRay, rayDir_x_edge02, V0ToRay_x_edge01;
		float det, inv_det, u, v, t;
		const float epsilon{ 0.0001f };

		edge01 = p1 - p0;
		edge02 = p2 - p0;
		rayDir_x_edge02 = Elite::Cross(hitRecord.ray.direction, edge02);
		det = Elite::Dot(rayDir_x_edge02, edge01);

		//Allow Cull mode handling
		switch (cullMode)
		{
		case CullMode::BACKFACE:
			if ((!isShadowTest && det < epsilon) || (isShadowTest && det > -epsilon))
				return false;
			break;
		case CullMode::FRONTFACE:
			if ((isShadowTest && det < epsilon) || (!isShadowTest && det > -epsilon))
				return false;
			break;
		default:
			if (abs(det) < epsilon)
				return false;
			break;
		}

		inv_det = 1 / det;
		V0ToRay = hitRecord.ray.origin - p0;
		u = Elite::Dot(rayDir_x_edge02, V0ToRay) * inv_det;

		//Check validity of first baricentric coordinate, discard triangle if invalid
		if (u < 0.f || u > 1.f)
			return false;

		V0ToRay_x_edge01 = Elite::Cross(V0ToRay, edge01);
		v = Elite::Dot(V0ToRay_x_edge01, hitRecord.ray.direction) * inv_det;

		//Check validity of second baricentric coordinate, discard triangle if invalid
		if (v < 0.f || u + v > 1.f)
			return false;

		t = Elite::Dot(V0ToRay_x_edge01, edge02) * inv_det;

		//Check if the hit point is neither too far or if no closer hit point was already found
		//discard triangle if it's the case
		if (t < hitRecord.ray.minT || t > hitRecord.t)
			return false;

		if (isShadowTest)
			return true;

		hitRecord.t = t;
		hitRecord.normal = Elite::GetNormalized(Elite::Cross(edge01, edge02));
		hitRecord.normal *= (cullMode == CullMode::FRONTFACE) * -1.f + (cullMode != CullMode::FRONTFACE) * 1;

		return true;
	}
}

namespace ObjReader
{
	void LoadModel(const std::string& objPath, std::vector<Elite::FPoint3>& vertexBuffer, std::vector<int>& indexBuffer);
}

