#ifndef	_CUBE_H_
#define	_CUBE_H_

#include <string>

// Cubeの操作を行うクラス
class CCube
{
public:
	CCube();
	virtual ~CCube();

	// 各EdgeのClear Permutationを表す
	enum EdgeSet
	{
		// Up層
		UF = 0, UL = 1, UB = 2, UR = 3,
		FU = 0, LU = 1, BU = 2, RU = 3,
		// Down層
		DF = 4, DL = 5, DB = 6, DR = 7,
		FD = 4, LD = 5, BD = 6, RD = 7,
		// Middle層
		RF = 8, FL = 9, LB = 10, BR = 11,
		FR = 8, LF = 9, BL = 10, RB = 11,

		FirstEdgeSet = UF,
		LastEdgeSet = BR,
		FirstMiddleEdgeSet = RF,
		LastMiddleEdgeSet = BR,
		NumberOfEdgeSets = LastEdgeSet + 1
	};

	// 各CornerのClear Permutationを表す
	enum CornerSet
	{
		// Up層
		URF = 0, UFL = 1, ULB = 2, UBR = 3,
		RFU = 0, FLU = 1, LBU = 2, BRU = 3,
		FUR = 0, LUF = 1, BUL = 2, RUB = 3,
		// Down層
		DFR = 4, DLF = 5, DBL = 6, DRB = 7,
		FRD = 4, LFD = 5, BLD = 6, RBD = 7,
		RDF = 4, FDL = 5, LDB = 6, BDR = 7,

		FirstCornerSet = URF,
		LastCornerSet = DRB,
		NumberOfCornerSets = LastCornerSet + 1
	};

	// エラー値を設定
	enum { InvalidSet = LastEdgeSet + 1 };

	// Cornerの回転状態
	enum Twists 
	{
		NoTwist, Clockwise, CounterClockwise,
		NumberOfTwists = CounterClockwise + 1
	};

	// Edgeの反転状態
	enum Flips 
	{
		NotFlipped, Flipped
	};

	enum Move 
	{
		U, D, L, R, F, B,
		Ui, Di, Li, Ri, Fi, Bi,
		U2, D2, L2, R2, F2, B2,

		//FirstMove = U,
		//LastMove = B2,
		NumberOfClockwiseQuarterTurnMoves = 6,
		NumberOfMoves = 18
	};

	// 同じ状態かを判別する演算子
	bool operator==(const CCube& cube) const;
	bool operator!=(const CCube& cube) const;

	// Clean Cubeに戻す
	virtual void SetCleanCube();

	// PermutationとOrientationを設定する関数
	virtual void CCube::SetState(
		const int* p_cornerPermutation, const int* p_cornerOrientation,
		const int* p_edgePermutation, const int* p_edgeOrientation);

	// cubeの移動を行う
	virtual void ApplyMove(const int p_move);

	// moveの反対方向の移動を取得する
	inline static int const GetInverseOfMove(const int p_move)
	{
		return InverseMoves[p_move];
	}

	// +-90[deg]回転を180[deg]回転に変換する (ex. R と Ri は R2 になる)
    inline static int const ConvertQuarterTurnToHalfTurnMove(const int p_move)
	{
		return (U2 + p_move % (B + 1));
	}

	// 反対のFaceを取得する
	inline static int const GetOpposingFace(const int p_move)
	{
		return OppositeFaces[p_move];
	}

	// 移動記号から移動記号の文字列を取得する
	inline static std::string const GetNameOfMove(const int p_move)
	{
		return MoveNames[p_move];
	}

	// 移動記号の文字列から移動記号を取得する
	// 未使用
	//static int MoveNameToMove(const std::string moveName, int& move);

	// Cubeの状態を出力する
	virtual void PrintCubeState() const;

protected:
	// 移動関数(時計回り90[deg])
	void Move_U();
	void Move_D();
	void Move_L();
	void Move_R();
	void Move_F();
	void Move_B();
	// 移動関数(反時計回り90[deg])
	void Move_Ui();
	void Move_Di();
	void Move_Li();
	void Move_Ri();
	void Move_Fi();
	void Move_Bi();
	// 移動関数(180[deg])
	void Move_U2();
	void Move_D2();
	void Move_L2();
	void Move_R2();
	void Move_F2();
	void Move_B2();

	// 4か所のEdgeをシフトするようにPermutation,Orientationsを操作
	void ShiftEdges(const EdgeSet p_1, const EdgeSet p_2, const EdgeSet p_3, const EdgeSet p_4);
	// 4か所のCornerをシフトするようにPermutation,Orientationsを操作
	void ShiftCorners(const CornerSet p_1, const CornerSet p_2, const CornerSet p_3, const CornerSet p_4);
	// 4か所のEdge,Cornerを1つずつシフトする
	void ShiftVector(int* vector, const int p_1, const int p_2, const int p_3, const int p_4);
	
	// EdgeSetの位置のEdgeを反転する
	void FlipEdge(const EdgeSet p_edge);

	// CornerSetの位置のCornerを時計回りに回す
	void TwistCornerCw(const CornerSet p_corner);
	// CornerSetの位置のCornerを反時計回りに回す
	void TwistCornerCcw(const CornerSet p_corner);

	// Clean Cubeにおけるcornerの位置にあるCornerを格納する
	// http://kociemba.org/math/coordlevel.htm
	int CornerSetPermutation[NumberOfCornerSets];

	// cornerの位置のCornerのOrientation(回転状態)を格納する
	// http://kociemba.org/math/coordlevel.htm
	int CornerSetOrientations[NumberOfCornerSets];

	// Clean Cubeにおけるedgeの位置にあるEdgeを格納する
	// http://kociemba.org/math/coordlevel.htm
	int EdgeSetPermutation[NumberOfEdgeSets];

	// edgeの位置のEdgeのOrientation(反転状態)を格納する
	// http://kociemba.org/math/coordlevel.htm
	int EdgeSetOrientations[NumberOfEdgeSets];

private:
	// 反対の動き(ex:R -> Ri, B2 -> B2)
	static const Move InverseMoves[NumberOfMoves];
	// 反対側のFace (ex:U -> D, F -> B, L -> R)
	static const Move OppositeFaces[NumberOfMoves / 3];
	// 移動記号の文字列
	static const std::string MoveNames[NumberOfMoves];
	// Cornerを時計回りに回すTable
	static const int ClockwiseTwists[NumberOfTwists];
	// Cornerを反時計回りに回すTable
	static const int CounterClockwiseTwists[NumberOfTwists];


};

#endif	// _CUBE_H_
