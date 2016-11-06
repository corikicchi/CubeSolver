#ifndef	_ORDINALCUBE_H_
#define	_ORDINALCUBE_H_

#include "cube.h"

// 各種Ordinalを求めるためのクラス
class COrdinalCube : public CCube
{
public:
	COrdinalCube();
	~COrdinalCube();

	// いろいろな場合の数
	enum 
	{
		Twists = (3 * 3 * 3 * 3 * 3 * 3 * 3), // 3^7 = 2187
		Flips = (2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2), // 2^11 = 2048
		Choices = 495, // 12 choose 4 = 495
		CornerPermutations = (8 * 7 * 6 * 5 * 4 * 3 * 2 * 1), // 8! = 40320
		UpDownEdgePermutations = (8 * 7 * 6 * 5 * 4 * 3 * 2 * 1), // 8! = 40320
		MiddleEdgePermutations = (4 * 3 * 2 * 1) // 4! = 24
	};

	// Phase 1

	// SetOrientationsFromTwist(int)で設定したCornerSetOrientationsから
	// 12個のCornerの方向の組み合わせを0~2047のuniqueな序数(twist)で表現する
	int GetTwistFromOrientations() const;
	// CornerSetOrientationsを設定
	// Cornerにtwistで設定される回転状態を設定する
	void SetOrientationsFromTwist(int p_twist);

	// SetOrientationsFromFlip(int)で設定したEdgeSetOrientationsから
	// 12個のEdgeの方向の組み合わせを0~479001599のuniqueな序数(flip)で表現する
	int GetFlipFromOrientations() const;
	// EdgeSetOrientationsを設定
	// 12個のEdgeにflipで設定される2種類の反転を設定する
	void SetOrientationsFromFlip(int p_flip);

	// Edgeの順列から，Middle層のEdgeの順列を表現するuniqueな序数を計算する
	int GetChoiceFromEdgePermutation() const;
	// 序数から，それに対してuniqueなMiddle層のEdgeの順列を求める
	// Middle層のEdgeのみ操作される
	void SetChoiceFromEdgePermutation(int p_choice);

	// Phase 2

	// Cornerの順列を表現するuniqueな序数を計算する
	int GetOrdinalFromCornerPermutation() const;
	// 序数から，それに対してuniqueなCornerの順列を求める
	void SetCornerPermutationFromOrdinal(const int p_ordinal);

	// Up層とDown層のEdgeの順列を表現するuniqueな序数を計算する
	int GetOrdinalFromUpDownEdgePermutation() const;
	// 序数から，それに対してuniqueなUp層とDown層のEdgeの順列を求める
	// この関数はclean cubeに対してのみ有効
	void SetUpDownEdgePermutationFromOrdinal(const int p_ordinal);

	// Middle層のEdgeの順列を表現するuniqueな序数を計算する
	int GetOrdinalFromMiddleEdgePermutation() const;
	// 序数から，それに対してuniqueなMiddle層のEdgeの順列を求める
	// この関数はclean cubeに対してのみ有効
	void SetMiddleEdgePermutationFromOrdinal(const int p_ordinal);

private:
	// Middle層のEdgeか判断する
	static int IsMiddleEdgeCubie(const int p_cubie);

	// Middle層のEdgeの順列から，Middle層のEdgeの順列を表現するuniqueな序数を計算する
	static int ChoiceOrdinal(int* p_choicePermutation);

	// 序数から，それに対してuniqueなMidlle層のEdgeの順列を求める
	// 割り当てられないEdge(Up層・Down層)はInvalidSetとして返される
	static void ChoicePermutation(int p_choiceOrdinal, int* choicePermutation);
};

#endif // _ORDINALCUBE_H_
