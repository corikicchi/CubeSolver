#ifndef	_combinat_h
#define	_combinat_h

#include "calculateordinal.h"
#include "printvector.h"

#include <algorithm>
#include <vector>

// N Choose K を計算する
// http://tsujimotter.hatenablog.com/entry/ruby-binomial-coefficient
int NChooseK(int p_N, int p_K)
{
	if (p_N < p_K) return 0;

	p_K = std::min(p_K, p_N - p_K);

	int val = 0;

	if (p_K == 0) {
		val = 1;
	}
	else {
		val = NChooseK(p_N - 1, p_K - 1) * p_N / p_K;
	}
	return val;
}

// Corner or Edgeの順列を表現する序数を計算する
// OrdinalToPermutationの逆演算
// http://kociemba.org/math/coordlevel.htm
int PermutationToOrdinal(const int* p_vector, const int p_n)
{
	int Ordinal = 0;
	std::vector<int> Vector(p_n);	// Edgeは12, Cornerは8

	// vectorをコピー
	for (int i = 0; i < p_n; i++) {
		Vector[i] = p_vector[i];
	}

	// 置換を互換で表したときの互換の数を計算する
	for (int Limit = p_n - 1; Limit > 0; Limit--) {
		// 各Limit(Edgeは11，Cornerは7)から1つずつ下げる

		// Vectorで最も大きな要素を求める
		int Temp = -1;
		int Coeff_i;
		for (int i = 0; i <= Limit; i++){
			if (Vector[i] > Temp){
				Temp = Vector[i];
				Coeff_i = i;
			}
		}
		// Temp:最も大きな値
		// Coeff_i:最も大きな値の要素番号

		// 積算する(Permutationを，p_n進数から10進数に変換するような処理)
		// Limitは次第に小さくなるので，上位桁から設定する
		// PermutationがCleanから遠いほど大きな値が格納される
		Ordinal = Ordinal * (Limit + 1) + Coeff_i;

		// Limit位置の要素が一番大きくなるように入れ替える
		Temp            = Vector[Limit];
		Vector[Limit]   = Vector[Coeff_i];
		Vector[Coeff_i] = Temp;

		// 次のループでは2番目に大きな要素が最後から2番目に格納される
	}
	return Ordinal;
}

// 序数から，それに対してuniqueなCornerorEdgeの順列を求める
// PermutationToOrdinalの逆演算
void OrdinalToPermutation(int p_ordinal, int* vector, const int p_n, const int p_offset)
{
	int Temp;
 
	// cleanな順列を作成する
	for (int i = 0; i < p_n; i++) {
		vector[i] = i + p_offset;
	}

	for (int i = 1; i < p_n; i++) {
        // 剰余を求める
		int Coeff_i = p_ordinal % (i + 1);

		// 剰余を求めたので(i + 1)で割る
		p_ordinal /= (i + 1);

		// 要素を交換する
		Temp            = vector[i];
		vector[i]       = vector[Coeff_i];
		vector[Coeff_i] = Temp;
	}
}
#endif	// _combinat_h
