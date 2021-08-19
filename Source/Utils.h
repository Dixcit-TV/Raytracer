#pragma once
#include "EMath.h"
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

namespace ObjReader
{
	void LoadModel(const std::string& objPath, std::vector<Elite::FPoint3>& vertexBuffer, std::vector<int>& indexBuffer);
}

