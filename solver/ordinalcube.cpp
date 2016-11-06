#include "ordinalcube.h"
#include "calculateordinal.h"
#include "printvector.h"

// Constructor
COrdinalCube::COrdinalCube()
{
}

// Destructor
COrdinalCube::~COrdinalCube()
{
}

// SetOrientationsFromTwist(int)で設定したCornerSetOrientationsから
// 12個のCornerの方向の組み合わせを0~2047のuniqueな序数(twist)で表現する
int COrdinalCube::GetTwistFromOrientations() const
{
	// ex. CornerSetOrientations = 0 0 0 0 0 0 1 2 のとき twist = 5
	//     CornerSetOrientations = 0 0 0 0 0 1 1 1 のとき twist = 13
	//     CornerSetOrientations = 0 0 0 0 0 1 1 2 のとき twist = 14
	//     CornerSetOrientations = 0 0 0 0 0 1 2 0 のとき twist = 15

	int twist = 0;
	for (int corner = FirstCornerSet; corner < LastCornerSet; corner++) {
		twist = twist * 3 + CornerSetOrientations[corner];
	}

	return twist;
}

// CornerSetOrientationsを設定
// Cornerにtwistで設定される回転状態を設定する
void COrdinalCube::SetOrientationsFromTwist(int p_twist)
{
	int paritySum = 0;	// Cornerの回転(なし:0，時計回り:1，反時計回り2)を加算する

	// CornerSetOrientationsを設定
	// 最後以外の7個のCornerにtwistで設定される3種類の回転を設定する
	// 10進数の3進数表記と同等
	// ex. twist = 0 のとき 0 0 0 0 0 0 0
	//     twist = 1 のとき 0 0 0 0 0 0 1
	//     twist = 2 のとき 0 0 0 0 0 0 2
	//     ...
	//     twist = 5 のとき 0 0 0 0 0 1 2
	//     twist = 6 のとき 0 0 0 0 0 2 0
	//     twist = 7 のとき 0 0 0 0 0 2 1
	//     twist = 8 のとき 0 0 0 0 0 2 2
	//     twist = 9 のとき 0 0 0 0 1 0 0
	//     ...
	//     この操作で，twistに対応したuniqueなCornerの状態を作成することが出来る

	for (int corner =  LastCornerSet - 1; corner >= FirstCornerSet; corner--){
		paritySum += (CornerSetOrientations[corner] = p_twist % 3);
		p_twist /= 3;
	}

	// 最後のCornerを設定する
	// CornerSetOrientationsの和は3の倍数でなければならないので
	// そうなるように最後のCornerを設定する
	// (8個の和 % 3 == 0 にならなければならない)

	CornerSetOrientations[LastCornerSet] = (3 - paritySum % 3) % 3;
}

// SetOrientationsFromFlip(int)で設定したEdgeSetOrientationsから
// 12個のEdgeの方向の組み合わせを0~479001599のuniqueな序数(flip)で表現する
int COrdinalCube::GetFlipFromOrientations() const
{
	// ex. EdgeSetOrientations = 0 0 0 0 0 0 0 0 0 0 0 0 のとき flip = 0
	//     EdgeSetOrientations = 0 0 0 0 0 0 0 0 0 0 1 1 のとき flip = 3
	//     EdgeSetOrientations = 0 0 0 0 0 0 0 0 0 1 0 1 のとき flip = 5

	int flip = 0;
	for (int edge = FirstEdgeSet; edge < LastEdgeSet; edge++) {
		flip = flip * 2 + EdgeSetOrientations[edge];
	}
	return flip;
}

// EdgeSetOrientationsを設定
// 12個のEdgeにflipで設定される2種類の反転を設定する
void COrdinalCube::SetOrientationsFromFlip(int p_flip)
{
	int paritySum = 0;	// Edgeの反転(0:なし，1:反転)を加算する

	// EdgeSetOrientationsを設定
	// 最後以外の11個のEdgeにflipで設定される2種類の反転を設定する
	// 10進数の2進数表記と同等
	// ex. flip = 0 のとき 0 0 0 0 0 0 0 0 0 0 0
	//     flip = 1 のとき 0 0 0 0 0 0 0 0 0 0 1
	//     flip = 2 のとき 0 0 0 0 0 0 0 0 0 1 0
	//     ...
	//     この操作で，flipに対応したuniqueなCornerの状態を作成することが出来る

	for (int edge = LastEdgeSet - 1; edge >= FirstEdgeSet; edge--) {
		paritySum += (EdgeSetOrientations[edge] = p_flip % 2);
		p_flip /= 2;
	}

	// 最後のEdgeを設定する
	// EdgeSetOrientationsの和は2の倍数でなければならないので
	// そうなるように最後のEdgeを設定する
	// (12個の和 % 2 == 0 にならなければならない)

	EdgeSetOrientations[LastEdgeSet] = paritySum % 2;
}

// Edgeの順列から，Middle層のEdgeの順列を表現するuniqueな序数を計算する
int COrdinalCube::GetChoiceFromEdgePermutation() const
{
	int choicePermutation[4];	// Middle層の4つのEdgeの順列
	int i = 0;

	// Edgeの順列から，Middle層の4つのEdgeの順列を作成する
	for (int edge = FirstEdgeSet; edge <= LastEdgeSet; edge++) {
		if (IsMiddleEdgeCubie(EdgeSetPermutation[edge])) {
			// edgeの位置にある要素がMiddle層ならedgeの位置を追加する
			choicePermutation[i++] = edge;
		}
	}
	return ChoiceOrdinal(choicePermutation);
}

// 序数から，それに対してuniqueなMiddle層のEdgeの順列を求める
// Middle層のEdgeのみ操作される
void COrdinalCube::SetChoiceFromEdgePermutation(const int p_choice)
{
	ChoicePermutation(p_choice, EdgeSetPermutation);
}

// Cornerの順列を表現するuniqueな序数を計算する
int COrdinalCube::GetOrdinalFromCornerPermutation() const
{
	return PermutationToOrdinal(CornerSetPermutation, NumberOfCornerSets);
}

// 序数から，それに対してuniqueなCornerの順列を求める
void COrdinalCube::SetCornerPermutationFromOrdinal(const int p_ordinal)
{
	OrdinalToPermutation(p_ordinal, CornerSetPermutation, NumberOfCornerSets, FirstCornerSet);
}

// Up層とDown層のEdgeの順列を表現するuniqueな序数を計算する
int COrdinalCube::GetOrdinalFromUpDownEdgePermutation() const
{
	return PermutationToOrdinal(EdgeSetPermutation, 8);
}

// 序数から，それに対してuniqueなUp層とDown層のEdgeの順列を求める
// この関数はclean cubeに対してのみ有効
void COrdinalCube::SetUpDownEdgePermutationFromOrdinal(const int p_ordinal)
{
	OrdinalToPermutation(p_ordinal, EdgeSetPermutation, 8, FirstEdgeSet);
}

// Middle層のEdgeの順列を表現するuniqueな序数を計算する
int COrdinalCube::GetOrdinalFromMiddleEdgePermutation() const
{
	return PermutationToOrdinal(&EdgeSetPermutation[FirstMiddleEdgeSet], 4);
}

// 序数から，それに対してuniqueなMiddle層のEdgeの順列を求める
// この関数はclean cubeに対してのみ有効
void COrdinalCube::SetMiddleEdgePermutationFromOrdinal(const int p_ordinal)
{
	OrdinalToPermutation(p_ordinal, &EdgeSetPermutation[FirstMiddleEdgeSet], 4, FirstMiddleEdgeSet);
}

// Middle層のEdgeかどうかを判別する
int COrdinalCube::IsMiddleEdgeCubie(const int p_cubie)
{
	return p_cubie >= FirstMiddleEdgeSet && p_cubie <= LastMiddleEdgeSet;
}

// Middle位置のEdgeのPermutationからuniqueなoridinal(0 - 494)を求めるために
// ルックアップテーブルではなく特殊なアルゴリズムを用いる

// <参照しているedgeがMiddleEdge位置のとき>
// 未チェックのEdge数を減らす(3個 -> 2個 -> 1個)
//
// <参照しているedgeがMiddleEdge位置ではないとき>
// 一番大きなMiddleEdge以外(3個)のうち，未チェックのもの(k個:3個 -> 2個 -> 1個)を
// 現在参照しているedgeより右側(n要素)への格納する場合の組み合わせの数C(n, k)を加算する
// ex.[2 1 3 5] のとき edgeMarkVector = [0 1 1 1 0 1 ... 0], C(11, 3) + C(7, 0) = 166
//    edge = 0 : 残りのMiddleEdge数が4個のとき，4-1=3個(最後の分を引く)の値を11箇所に当てはめる場合の数はC(11, 3) : 加算する
//    edge = 1 : 残りのMiddleEdge数が3個になる
//    edge = 2 : 残りのMiddleEdge数が2個になる
//    edge = 3 : 残りのMiddleEdge数が1個になる
// 未チェックのEdge数が1個になったら，3番目と4番目のEdgeの要素の間にMiddleEdge以外の要素がいくつあるかを加算する
//    edge = 4 : 残りのMiddleEdge数が1個のとき，1-1=0個の値を7箇所に当てはめる場合の数はC(7, 0) : 加算する
// 以上のアルゴリズムで，(0 - 11)の数字から4つを選んだ時の組み合わせをuniqueな数字で表現することが出来る
// 行っている操作は，C(n, k)の加算なので1つのループで実現することが出来る

// ex.全体で6個のEdgeのうち，3個のEdgeがchoiceされる場合の例

// 0,2,4 のとき edgeMarkVector = [1 0 1 0 1 0]
//		edge = 0 : 残りのChoicedEdge数が2個になる
//		edge = 1 : 2-1=1個の値を4箇所に当てはめる場合の数はC(4, 1) = 4
//		edge = 2 : 残りのChoicedEdge数が1個になる
//		edge = 3 : 1-1=0個の値を2箇所に当てはめる場合の数はC(2, 0) = 1
//		edge = 4 : 残りのChoicedEdge数が0個になる
//		加算すると 5

// 1,2,5 のとき edgeMarkVector = [0 1 1 0 0 1]
//		edge = 0 : 3-1=2個の値を5箇所に当てはめる場合の数はC(5, 2) = 10
//		edge = 1 : 残りのChoicedEdge数が2個になる
//		edge = 2 : 残りのChoicedEdge数が1個になる
//		edge = 3 : 1-1=0個の値を2箇所に当てはめる場合の数はC(2, 0) = 1
//		edge = 4 : 1-1=0個の値を1箇所に当てはめる場合の数はC(1, 0) = 1
//		edge = 5 : 残りのChoicedEdge数が0個になる
//		加算すると 12

// 1,3,4 のとき edgeMarkVector = [0 1 0 1 1 0]
//		edge = 0 : 3-1=2個の値を5箇所に当てはめる場合の数はC(5, 2) = 10
//		edge = 1 : 残りのChoicedEdge数が2個になる
//		edge = 2 : 2-1=1個の値を3箇所に当てはめる場合の数はC(3, 1) = 3
//		edge = 3 : 残りのChoicedEdge数が1個になる
//		edge = 4 : 残りのChoicedEdge数が0個になる
//		加算すると 13

// 245 のとき edgeMarkVector = [0 0 1 0 1 1]
//		edge = 0 : 3-1=2個の値を5箇所に当てはめる場合の数はC(5, 2) = 10
//		edge = 1 : 3-1=2個の値を4箇所に当てはめる場合の数はC(4, 2) = 6
//		edge = 2 : 残りのChoicedEdge数が2個になる
//		edge = 3 : 2-1=1個の値を2箇所に当てはめる場合の数はC(2, 1) = 2
//		edge = 4 : 残りのChoicedEdge数が1個になる
//		edge = 4 : 残りのChoicedEdge数が0個になる
//		加算すると 18

// Middle層のEdgeの順列から，Middle層のEdgeの順列を表現するuniqueな序数を計算する
int COrdinalCube::ChoiceOrdinal(int* p_choicePermutation)
{
	bool edgeMarkVector[NumberOfEdgeSets];	// 基数ソートのbuffer
	int edgesRemaining = 4;	// 未ソートのEdge数
	int ordinal = 0;	// choice permutation ordinal

	// Edgeを基数ソート(edgeMarkVectorがtrueのときの要素番号の順列がソートされた順列になる)
	for (int edge = 0; edge < NumberOfEdgeSets; edge++) {
		edgeMarkVector[edge] = 0;
	}
	for (int edge = 0; edge < 4; edge++) {
		// Middle位置の番号だけ1にする
		edgeMarkVector[p_choicePermutation[edge]] = true;
	}

	// Edgeを参照して序数(choice)を求める
	// ex.[0 1 2 4] のとき edgeMarkVector = [1 1 1 0 1 0 ... 0], C(8, 0) = 1
	// ex.[0 1 2 5] のとき edgeMarkVector = [1 1 1 0 0 1 ... 0], C(8, 0) + C(7, 0) = 2
	// ex.[2 1 3 5] のとき edgeMarkVector = [0 1 1 1 0 1 ... 0], C(11, 3) + C(7, 0) = 166
	// ex.[8 9 10 11] のとき edgeMarkVector = [0 ... 0 1 1 1 1], C(11, 3) + C(10, 3) + ... + C(3, 3) = 495
	int edge = 0;
	while (edgesRemaining > 0){
		// Edgeをincrementしながら辞書順で(trueが左に近いほど先に)Scanする
		if (edgeMarkVector[edge++]) {	// edgeがMiddle層のEdgeである
			edgesRemaining--;	// Edge数をcount
		}
		else {
			ordinal += NChooseK(12 - edge, edgesRemaining - 1);
		}
	}
	return ordinal;
}


// 序数から，それに対してuniqueなMidlle層のEdgeの順列を求める
// 割り当てられないEdge(Up層・Down層)はInvalidSetとして返される
void COrdinalCube::ChoicePermutation(int p_choiceOrdinal, int* choicePermutation)
{
	int digitOfWholePermutation = 0;	// Edge順列全体での参照している桁

	// すべてのEdgeをInvalidSetに初期化する
	for (int edge = 0; edge < NumberOfEdgeSets; edge++){
		choicePermutation[edge] = InvalidSet;
	}

	// 4つのEdgeを探索する
	for (int edgeOfMiddleEdges = 0; edgeOfMiddleEdges < 4; edgeOfMiddleEdges++){
		while(true) {
			// Edge番号のうち小さなものから決定する
			// n:12 - 1 - Edge順列全体における要素番号(0 - 11), k:Choiced数(4) - 1 - ChoicedEdge順列のEdge番号(0 - 3)
			// C(n, k)のkを一定として，nをdecrementしながらcombinationを求める

			// ordinalがcombination以上なら，ordinalからその値を引く
			// Edge順列全体における参照桁を右にずらしてを繰り替えす
			// (ordinalを作る際に,MarkVectorがfalseのときにC(n, k)を加算しているため)

			// ordinalがcombinationより小さければ，Edge順列全体における参照桁の値をEdge番号に設定する

			// 上のex.の逆演算例
			// ordinal = 5 のとき
			// 参照桁:0, C(6-1-0, 3-1-0) = 10 > 5 なので 0桁目がtrueになる
			// 参照桁:1. C(6-1-1, 3-1-1) = 4 <= 5 なので ordinal = 5 - 4 = 1
			// 参照桁:2, C(6-1-2, 3-1-1) = 3 > 1 なので 2桁目がtrueになる
			// 参照桁:3. C(6-1-3, 3-1-2) = 1 <= 1 なので ordinal = 1 - 1 = 0
			// 参照桁:4, C(6-1-4, 3-1-2) = 1 > 0 なので 4桁目がtrueになる
			// よって [1 0 1 0 1 0]

			// 現在のedgeで，参照する桁を順番に下げていく
			// 与えられた数字より組み合わせ数が小さくなったらbreak
			int combination = NChooseK(12 - 1 - digitOfWholePermutation, 4 - 1 - edgeOfMiddleEdges);
			digitOfWholePermutation++;
			if (p_choiceOrdinal >= combination) {
				p_choiceOrdinal -= combination;
			}
			else {
				break;
			}
		}

		// digitはincrementされているので1つ戻す
		choicePermutation[digitOfWholePermutation - 1] = FirstMiddleEdgeSet + edgeOfMiddleEdges;
	}
}
