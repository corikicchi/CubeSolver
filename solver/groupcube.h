#ifndef	_GROUPCUBE_H_
#define	_GROUPCUBE_H_

#include "cube.h"

#include <string>

// Faceの番号(U=1,D=2, ...)からCornerのuniqueな番号を計算する
#define FacesToCornerNumber(face1, face2, face3) (((face1 * 6) + face2) * 6 + face3)
// Faceの番号(U=1,D=2, ...)からEdgeのuniqueな番号を計算する
#define FacesToEdgeNumber(face1, face2) (face1 * 6 + face2)

// Cubeが群になるかを判定するためのクラス
// 各Permutation,Orientationを求めて，Parityを計算する
class CGroupCube
{
public:
	CGroupCube();
	~CGroupCube();

	enum Face 
	{
		U, D, L, R, F, B
	};

	enum CubeError 
	{
		VALID,
		INVALID_COLOR,
		INVALID_COLORS_COUNT,
		DUPLICATE_CENTER_COLOR,
		INVALID_CORNER_COLORS,
		INVALID_CORNER_PARITY,
		INVALID_EDGE_COLORS,
		INVALID_EDGE_PARITY,
		INVALID_TOTAL_PARITY,
		NumberOfErrors
	};

	// face面の色(9個:0 - 8)をcubeColorsに格納する
	void SetFaceColors(const int p_face, const std::string p_colors);

	// Cubeクラスのデータの配置が正しいかをcheckする
	// 正しければcubeに状態を設定する
	CubeError SetCubeState(CCube& cube);

	// エラーコードをテキスト変換する
    static std::string GetErrorText(const CubeError p_error);

	// Cubeの状態を出力する
	void PrintCubeState() const;

private:
	// uniqueなCorner番号
	enum CornerNumber
	{
		URF = FacesToCornerNumber(U, R, F),
		RFU = FacesToCornerNumber(R, F, U),
		FUR = FacesToCornerNumber(F, U, R),

		UFL = FacesToCornerNumber(U, F, L),
		FLU = FacesToCornerNumber(F, L, U),
		LUF = FacesToCornerNumber(L, U, F),

		ULB = FacesToCornerNumber(U, L, B),
		LBU = FacesToCornerNumber(L, B, U),
		BUL = FacesToCornerNumber(B, U, L),

		UBR = FacesToCornerNumber(U, B, R),
		BRU = FacesToCornerNumber(B, R, U),
		RUB = FacesToCornerNumber(R, U, B),

		DFR = FacesToCornerNumber(D, F, R),
		FRD = FacesToCornerNumber(F, R, D),
		RDF = FacesToCornerNumber(R, D, F),

		DLF = FacesToCornerNumber(D, L, F),
		LFD = FacesToCornerNumber(L, F, D),
		FDL = FacesToCornerNumber(F, D, L),

		DBL = FacesToCornerNumber(D, B, L),
		BLD = FacesToCornerNumber(B, L, D),
		LDB = FacesToCornerNumber(L, D, B),

		DRB = FacesToCornerNumber(D, R, B),
		RBD = FacesToCornerNumber(R, B, D),
		BDR = FacesToCornerNumber(B, D, R)
	};

	// uniqueなEdge番号
	enum EdgeNumber
	{
		UF = FacesToEdgeNumber(U, F),
		FU = FacesToEdgeNumber(F, U),

		UL = FacesToEdgeNumber(U, L),
		LU = FacesToEdgeNumber(L, U),

		UB = FacesToEdgeNumber(U, B),
		BU = FacesToEdgeNumber(B, U),

		UR = FacesToEdgeNumber(U, R),
		RU = FacesToEdgeNumber(R, U),

		DF = FacesToEdgeNumber(D, F),
		FD = FacesToEdgeNumber(F, D),

		DL = FacesToEdgeNumber(D, L),
		LD = FacesToEdgeNumber(L, D),

		DB = FacesToEdgeNumber(D, B),
		BD = FacesToEdgeNumber(B, D),

		DR = FacesToEdgeNumber(D, R),
		RD = FacesToEdgeNumber(R, D),

		RF = FacesToEdgeNumber(R, F),
		FR = FacesToEdgeNumber(F, R),

		LF = FacesToEdgeNumber(L, F),
		FL = FacesToEdgeNumber(F, L),

		LB = FacesToEdgeNumber(L, B),
		BL = FacesToEdgeNumber(B, L),

		RB = FacesToEdgeNumber(R, B),
		BR = FacesToEdgeNumber(B, R)
	};

	// 各面の中央が正しい配置かをcheck
	CubeError CheckCenters();
	// 6 colors x 9 blocksがそろっているかcheck
	CubeError CheckBlocks();
	// CornerのOrientation, Permutationを設定してcheck
	CubeError CheckCorners();
	// EdgeのOrientation, Permutationを設定してcheck
	CubeError CheckEdges();

	// CornerのPermutation Parityを計算する
	// (true:奇置換，false:偶置換)
	bool GetCornerPermutationParity() const;
	// EdgeのPermutation Parityを計算する
	// (true:奇置換，false:偶置換)
	bool GetEdgePermutationParity() const;
	// 偶置換か奇置換かを取得する
	// (true:奇置換，false:偶置換)
	bool GetPermutationParity(const int* p_permutation, const int p_numberOfCubies) const;
	int BlockToFaceName(const int p_offset);

	// Cube全体における各BlockのColor
	// U[9]+D[9]+L[9]+R[9]+F[9]+B[9]
	char cubeColors[6 * 9];

	// それぞれのFaceのCenterのColor
	// 文字の配列・文字列の両方として扱うのでlegacyな配列を用いる
	char centerColor[6 + 1];

	// CubeのPermutationとOrientation
	// メモリ上に分散しないように，constructorで領域確保を行う
	int cornerBlocksPermutation[CCube::NumberOfCornerSets];
	int cornerBlocksOrientations[CCube::NumberOfCornerSets];
	int edgeBlocksPermutation[CCube::NumberOfEdgeSets];
	int edgeBlocksOrientations[CCube::NumberOfEdgeSets];

	/*

	// Permutation,Orientationを求める際に使用する
	// 位置情報の組み合わせ	: cornerBlocksPos, edgeBlocksPos
	// 回転・反転状態		: cornerStateMap, edgeStateMap

	          Up
	     Left Front Right Back
	          Down

	          00 01 02
			  03 04 05
	          06 07 08

	18 19 20  36 37 38  27 28 29  45 46 47
	21 22 23  39 40 41  30 31 32  48 49 50
	24 25 26  42 43 44  33 34 35  51 52 53

	          09 10 11
	          12 13 14
	          15 16 17
	*/

	// Cube全体におけるCorner Blockの位置の番号の組み合わせ
	// 状態はNoTwist
	static const int cornerBlocksPos[CCube::NumberOfCornerSets][3];

	// CornerのTwistを判定するためのTable
	static const CornerNumber cornerStateMap[CCube::NumberOfCornerSets * 3];

	// Cube全体におけるEdge Blockの位置の番号の組み合わせ
	// 状態はNot fripped
	static const int edgeBlocksPos[CCube::NumberOfEdgeSets][2];

	// EdgeのFlipを判定するためのTable
	static const EdgeNumber edgeStateMap[CCube::NumberOfEdgeSets * 2];

	// エラーテキスト
	static const std::string errorText[NumberOfErrors];
};

#endif // _GROUPCUBE_H_
