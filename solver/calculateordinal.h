// N Choose K を計算する
// http://tsujimotter.hatenablog.com/entry/ruby-binomial-coefficient
int NChooseK(int p_N, int p_K);

// Corner or Edgeの順列を表現する序数を計算する
// http://kociemba.org/math/coordlevel.htm
int PermutationToOrdinal(const int* p_vector, const int p_n);

// 序数から，それに対してuniqueなCorner or Edgeの順列を求める
void OrdinalToPermutation(int p_ordinal, int* vector, const int p_n, const int p_offset);
