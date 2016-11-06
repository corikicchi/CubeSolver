#include <iostream>
#include "printvector.h"

// Permutation, Orientationsを表示する
void PrintVector(const int* p_vector, const int p_n)
{
	int i;
	for (i = 0; i < p_n; i++) {
		std::cout << p_vector[i] << " ";
	}
	std::cout << std::endl;
}
