#include "groupcube.h"
#include "printvector.h"

#include <cstring>

CGroupCube::CGroupCube()
{
	centerColor[6] = '\0';	// 文字列として扱うためのnull終端
}

CGroupCube::~CGroupCube()
{
}

// face面の色(9個:0 - 8)をcubeColorsに格納する
void CGroupCube::SetFaceColors(const int p_face, const std::string p_colors)
{
	for (int block = 0; block < 9; block++) {
		// face:Face(UDLRFB)の番号(0 - 5)
		// block:各FaceにおけるBlockの番号(0 - 8)
		cubeColors[p_face * 9 + block] = (p_colors.c_str() + 2)[block];	// "f:"を読み飛ばす
	}
}

// Cubeクラスのデータの配置が正しいかをcheckする
// 正しければcubeに状態を設定する
CGroupCube::CubeError CGroupCube::SetCubeState(CCube& cube)
{
	CubeError status;
	
	if ((status = CheckCenters()) != VALID) return status;
	if ((status = CheckBlocks()) != VALID) return status;	
	if ((status = CheckCorners()) != VALID) return status;
	if ((status = CheckEdges()) != VALID) return status;

	// Corner PermutationとEdge PermutationのParityが一致しないときは群(置換群)をなさないので解けない
	if ((GetEdgePermutationParity() ^ GetCornerPermutationParity())) return INVALID_TOTAL_PARITY;

	cube.SetState(cornerBlocksPermutation, cornerBlocksOrientations, edgeBlocksPermutation, edgeBlocksOrientations);

	return status;
}

// エラーコードをテキスト変換する
std::string CGroupCube::GetErrorText(const CubeError p_error)
{
	if (p_error >= NumberOfErrors) {
		return errorText[0];
	}
	return errorText[p_error];
}

// Cubeの状態を表示
void CGroupCube::PrintCubeState() const
{
	PrintVector(cornerBlocksPermutation, CCube::NumberOfCornerSets);
	PrintVector(cornerBlocksOrientations, CCube::NumberOfCornerSets);
	PrintVector(edgeBlocksPermutation, CCube::NumberOfEdgeSets);
	PrintVector(edgeBlocksOrientations, CCube::NumberOfEdgeSets);
}

// 各面の中央が正しい配置かをcheck
CGroupCube::CubeError CGroupCube::CheckCenters()
{
	// 初期化
	for (int face = 0; face < 6; face++) {
		centerColor[face] = '\0';
	}
	
	for (int face = 0; face < 6; face++){
		// 6個のFaceを参照する
		// centerColor[6]にcubeColors[face * 9 + 4](faceの中央)と一致するものがないかcheck
		for (int faceName = 0; faceName < 6; faceName++){
			// faceName: UDLRFB
			// 各Face中央の色は1つずつしか存在しないはず
			// -> これまでcenterColorに格納された色に一致するものがあったらおかしい
			if (centerColor[faceName] == cubeColors[face * 9 + 4]) {
				return DUPLICATE_CENTER_COLOR;
			}
		}
		// centerColorにcubeColors[face * 9 + 4]と一致するものがなければok -> 格納する
		centerColor[face] = cubeColors[face * 9 + 4];
	}
	return VALID;
}

// 6 colors x 9 blocksがそろっているかcheck
CGroupCube::CubeError CGroupCube::CheckBlocks()
{
	int blockNum[6] = { 0 };

	for (int block = 0; block < 6 * 9; block++) {
		// 全Blockを参照する
		int face;
		
		if ((face = BlockToFaceName(block)) < 0) {
			return INVALID_COLOR;
		}

		blockNum[face]++;
	}

	for (int face = 0; face < 6; face++) {
		// 各色が9個ずつあるはず
		if (blockNum[face] != 9) {
			return INVALID_COLORS_COUNT;
		}
	}
	
	return VALID;
}

// CornerのOrientation, Permutationを設定してcheck
CGroupCube::CubeError CGroupCube::CheckCorners()
{
	int cornerParity = 0;

	for (int cornerPos = 0; cornerPos < CCube::NumberOfCornerSets; cornerPos++){
		// 現在格納されているCubeのCornerを1つずつ参照する(URF,UFL, ...)

		// ここの計算は配列にまとめられないか？
		// Corner Blockの位置の番号の組み合わせ
		// -> その位置のCornerにある各Blockが完成状態でどのFace(U=0,D=1, ...)になるかを取得
		// -> 位置情報からuniqueなCorner番号を取得
		int cornerNum = FacesToCornerNumber(
			BlockToFaceName(cornerBlocksPos[cornerPos][0]),
			BlockToFaceName(cornerBlocksPos[cornerPos][1]),
			BlockToFaceName(cornerBlocksPos[cornerPos][2])
			);

		int cornerLocation = -1;
		// cornerMapの中から上で取得したCorner番号と一致するものを探索する
		for (int cornerState = 0; cornerState < CCube::NumberOfCornerSets * 3; cornerState++){
			if (cornerNum == cornerStateMap[cornerState]){
				// 一致するものがあった
				// CornerStateMap上のの位置情報を保存
				cornerLocation = cornerState;

				// cornerStateMap[cornerState]のPermutation情報は，同じ位置の情報が3つずつ格納されている
				// ので，それに合わせてPermutationを取得
				// Clean Cubeでは 0 1 2 ... 7 と並ぶ
				cornerBlocksPermutation[cornerPos] = cornerState / 3;
				// cornerStateMap[cornerState]のOrientation情報は，corner%3=0の時 twist=0, corner%3=1の時 twist=1,
				// corner%3=2の時 twist=2 と定義しているので，それに合わせてOrientationを取得
				// Clean Cubeでは 0 0 0 ... 0 と並ぶ
				cornerBlocksOrientations[cornerPos] = cornerState % 3;

				// Orientation Parityのcheckのために加算する
				cornerParity += cornerBlocksOrientations[cornerPos];
				break;
			}
		}
		// なぜか一致するものがなかった
		if (cornerLocation == -1) {
			return INVALID_CORNER_COLORS;
		}
	}
	
	// CornerのOrientation Parityは3の倍数になる
	// (CornerのOrientationの組み合わせは限られる)
	if (cornerParity % 3 != 0) {
		return INVALID_CORNER_PARITY;
	}
	
	return VALID;
}

// EdgeのOrientation, Permutationを設定してcheck
CGroupCube::CubeError CGroupCube::CheckEdges()
{
	int edgeParity = 0;

	for (int edgePos = 0; edgePos < CCube::NumberOfEdgeSets; edgePos++){
		// 現在格納されているCubeのEdgeを1つずつ参照する(UF,UL, ...)

		// ここの計算は配列にまとめられないか？
		// Edge Blockの位置の番号の組み合わせ
		// -> その位置のEdgeにある各Blockが完成状態でどのFace(U=0,D=1, ...)になるかを取得
		// -> 位置情報からuniqueなEdge番号を取得
		int edgeNum = FacesToEdgeNumber(
			BlockToFaceName(edgeBlocksPos[edgePos][0]),
			BlockToFaceName(edgeBlocksPos[edgePos][1])
			);

		int edgeLocation = -1;
		// edgeMapの中から上で取得したEdge番号と一致するものを探索する
		for (int edgeState = 0; edgeState < CCube::NumberOfEdgeSets * 2; edgeState++){
			if (edgeNum == edgeStateMap[edgeState]) {
				// 一致するものがあった
				// EdgeStateMap上の位置情報を保存
				edgeLocation = edgeState;

				// edgeStateMap[edgeState]のPermutation情報は，同じ位置の情報が2つずつ格納されている
				// ので，それに合わせてPermutationを取得
				// Clean Cubeでは 0 1 2 ... 11 と並ぶ
				edgeBlocksPermutation[edgePos] = edgeState / 2;
				// edgeStateMap[edgeState]のOrientation情報は，edgeが偶数の時 flip=0, 奇数の時 flip=1
				// と定義しているので，それに合わせてOrientationを取得
				// Clean Cubeでは 0 0 0 ... 0 と並ぶ
				edgeBlocksOrientations[edgePos] = edgeState % 2;

				// Orientation Parityのcheckのために加算する
				edgeParity += edgeBlocksOrientations[edgePos];
				break;
			}
		}
		// なぜか一致するものがなかった
		if (edgeLocation == -1) {
			return INVALID_EDGE_COLORS;
		}
	}

	// EdgeのOrientation Parityは偶数になる
	// (1つだけ方向が違う状態はあり得ない)
	if (edgeParity % 2 != 0) {
		return INVALID_EDGE_PARITY;
	}

	return VALID;
}

// CornerのPermutation Parityを計算する
// (true:奇置換，false:偶置換)
bool CGroupCube::GetCornerPermutationParity() const
{
	return GetPermutationParity(cornerBlocksPermutation, CCube::NumberOfCornerSets);
}

// EdgeのPermutation Parityを計算する
//(true:奇置換，false:偶置換)
bool CGroupCube::GetEdgePermutationParity() const
{
	return GetPermutationParity(edgeBlocksPermutation, CCube::NumberOfEdgeSets);
}

// PermutationParityとは：偶置換か奇置換かを表す
// (true:奇置換，false:偶置換)
// Cubieの配置の順列の置換群が，偶置換か奇置換かを計算する
// Cornerの順列(URF,UFL, ... ,DRB)とEdgeの順列(UF,UL, ... ,RB)のPermutation Parityが一致する
// (偶置換どうし，または奇置換どうし)であればCubeの解放を見つけることができる
// なぜなら：奇置換の集合は群をなさない(演算が定義できない)ため
//           偶置換どうし，奇置換どうしの演算は偶置換になる
bool CGroupCube::GetPermutationParity(const int* p_permutation, const int p_numberOfCubies) const
{
	int permutationParity = 0;

	for (int p = 0; p < p_numberOfCubies - 1; p++) {
		for (int q = p + 1; q < p_numberOfCubies; q++) {
			if (p_permutation[p] > p_permutation[q]) {
				permutationParity++;
			}
		}
	}
	return (bool)(permutationParity % 2);
}

// cubeColors[offset]の色が完成状態でどのFace(UDLRFB)にあるかを判定
// offset:Cube全体におけるBlockの番号
// ex:cubeColors[offset] = 'O', centerColor = "YWGORB"
//    returnは3(=R)になる
int CGroupCube::BlockToFaceName(const int p_offset)
{
	char* name = NULL;
	// centerColor:各Face(UDLRFB)の中央の色
	// cubeColors:Cube全体における各Blockの色
	// cubeColors[offset]の色が完成状態でどのFace(UDLRFB)にあるかを判定
	// ex:cubeColors[offset] = 'O', centerColor = "YWGORB"
	//    returnは3(=R)になる
	if (!(name = std::strchr(centerColor, cubeColors[p_offset]))) {
		return -1;
	}
	// アドレスの差分
	return (int)(name - centerColor);
}

// Cube全体におけるCorner Blockの位置の番号の組み合わせ
// 状態はNoTwist
const int CGroupCube::cornerBlocksPos[CCube::NumberOfCornerSets][3] =
{
	{ 8,  27, 38 },		// URF
	{ 6,  36, 20 },		// UFL
	{ 0,  18, 47 },		// ULB
	{ 2,  45, 29 },		// UBR
	{ 11, 44, 33 },		// DFR
	{  9, 26, 42 },		// DLF
	{ 15, 53, 24 },		// DBL
	{ 17, 35, 51 }		// DRB
};

// CornerのTwistを判定するためのTable
const CGroupCube::CornerNumber CGroupCube::cornerStateMap[CCube::NumberOfCornerSets * 3] =
{
//   0    1    2	(Twist)
	URF, RFU, FUR,
	UFL, FLU, LUF,
	ULB, LBU, BUL,
	UBR, BRU, RUB,
	DFR, FRD, RDF,
	DLF, LFD, FDL,
	DBL, BLD, LDB,
	DRB, RBD, BDR 
};

// Cube全体におけるEdge Blockの位置の番号の組み合わせ
// 状態はNot fripped
const int CGroupCube::edgeBlocksPos[CCube::NumberOfEdgeSets][2] =
{
	{  7, 37 },		// UF
	{  3, 19 },		// UL
	{  1, 46 },		// UB
	{  5, 28 },		// UR
	{ 10, 43 },		// DF
	{ 12, 25 },		// DL
	{ 16, 52 },		// DB
	{ 14, 34 },		// DR
	{ 30, 41 },		// RF
	{ 23, 39 },		// LF
	{ 21, 50 },		// LB
	{ 32, 48 }		// RB
};

// EdgeのFlipを判定するためのTable
const CGroupCube::EdgeNumber CGroupCube::edgeStateMap[CCube::NumberOfEdgeSets * 2] =
{ 
//   0   1	(Flip)
	UF, FU,
	UL, LU,
	UB, BU,
	UR, RU,
	DF, FD,
	DL, LD,
	DB, BD,
	DR, RD,
	RF, FR,
	LF, FL,
	LB, BL,
	RB, BR 
};

// エラーテキスト
const std::string CGroupCube::errorText[NumberOfErrors] =
{
	"",
	"Color of block did not match any center color",
	"Not found 9 blocks for each color",
	"Duplicatation on center colors",
	"Corner colors was invalid",
	"Corner orientation parity was invalid",
	"Edge colors was invalid",
	"Edge orientation parity was invalid",
	"Total permutation parity was invalid" 
};
