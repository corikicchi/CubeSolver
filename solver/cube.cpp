#include "cube.h"
#include "printvector.h"

#include <cstring>	// string.hのC++ラッパー

CCube::CCube()
{
	SetCleanCube();
}

CCube::~CCube()
{
}

// 同じ状態かを判別する演算子
bool CCube::operator==(const CCube& cube) const
{
	return(
		!std::memcmp(CornerSetPermutation, cube.CornerSetPermutation, CornerSet::NumberOfCornerSets * sizeof(int)) &&
		!std::memcmp(CornerSetOrientations, cube.CornerSetOrientations, CornerSet::NumberOfCornerSets * sizeof(int)) &&
		!std::memcmp(EdgeSetPermutation, cube.EdgeSetPermutation, EdgeSet::NumberOfEdgeSets * sizeof(int)) &&
		!std::memcmp(EdgeSetOrientations, cube.EdgeSetOrientations, EdgeSet::NumberOfEdgeSets * sizeof(int))
		);
}
bool CCube::operator!=(const CCube& cube) const
{
	return !(*this == cube);
}

// Clean Cubeに戻す
void CCube::SetCleanCube()
{
	for (int edge = FirstEdgeSet; edge <= LastEdgeSet; edge++){
		EdgeSetPermutation[edge] = edge;
		EdgeSetOrientations[edge] = NotFlipped;
	}
	for (int corner = FirstCornerSet; corner <= LastCornerSet; corner++){
		CornerSetPermutation[corner] = corner;
		CornerSetOrientations[corner] = NoTwist;
	}
}

// PermutationとOrientationを設定する関数
void CCube::SetState(
	const int* p_cornerPermutation, const int* p_cornerOrientation,
	const int* p_edgePermutation, const int* p_edgeOrientation)
{
	for (int edge = FirstEdgeSet; edge <= LastEdgeSet; edge++){
		EdgeSetPermutation[edge] = p_edgePermutation[edge];
		EdgeSetOrientations[edge] = p_edgeOrientation[edge];
	}
	for (int corner = FirstCornerSet; corner <= LastCornerSet; corner++){
		CornerSetPermutation[corner] = p_cornerPermutation[corner];
		CornerSetOrientations[corner] = p_cornerOrientation[corner];
	}
}

void CCube::ApplyMove(const int p_move)
{
	switch (p_move){
	case CCube::U:	Move_U();	break;
	case CCube::D:	Move_D();	break;
	case CCube::L:	Move_L();	break;
	case CCube::R:	Move_R();	break;
	case CCube::F:	Move_F();	break;
	case CCube::B:	Move_B();	break;

	case CCube::Ui:	Move_Ui();	break;
	case CCube::Di:	Move_Di();	break;
	case CCube::Li:	Move_Li();	break;
	case CCube::Ri:	Move_Ri();	break;
	case CCube::Fi:	Move_Fi();	break;
	case CCube::Bi:	Move_Bi();	break;

	case CCube::U2:	Move_U2();	break;
	case CCube::D2:	Move_D2();	break;
	case CCube::L2:	Move_L2();	break;
	case CCube::R2:	Move_R2();	break;
	case CCube::F2:	Move_F2();	break;
	case CCube::B2:	Move_B2();	break;
	}
}

// 移動記号の文字列から移動記号を取得する
// 未使用
/*
int Cube::MoveNameToMove(const std::string p_moveName, int& move)
{
	int found = 0;

	for (int moveIndex = 0; moveIndex < NumberOfMoves; moveIndex++){
		if (!strcmp(p_moveName.c_str(), MoveNames[moveIndex].c_str())){
			move = moveIndex;
			found = 1;
			break;
		}
	}
	return found;
}
*/

// Cubeの状態を出力する
void CCube::PrintCubeState() const
{
	PrintVector(CornerSetPermutation, NumberOfCornerSets);
	PrintVector(CornerSetOrientations, NumberOfCornerSets);
	PrintVector(EdgeSetPermutation, NumberOfEdgeSets);
	PrintVector(EdgeSetOrientations, NumberOfEdgeSets);
}

void CCube::Move_U()
{
	ShiftCorners(ULB, UBR, URF, UFL);
	ShiftEdges(UB, UR, UF, UL);
}
void CCube::Move_D()
{
	ShiftCorners(DLF, DFR, DRB, DBL);
	ShiftEdges(DF, DR, DB, DL);
}
void CCube::Move_L()
{
	ShiftCorners(ULB, UFL, DLF, DBL);

	TwistCornerCw(ULB);
	TwistCornerCcw(UFL);
	TwistCornerCw(DLF);
	TwistCornerCcw(DBL);

	ShiftEdges(UL, FL, DL, LB);

	FlipEdge(UL);	
	FlipEdge(FL);	
	FlipEdge(DL);	
	FlipEdge(LB);
}
void CCube::Move_R()
{
	ShiftCorners(URF, UBR, DRB, DFR);

	TwistCornerCw(URF);
	TwistCornerCcw(UBR);
	TwistCornerCw(DRB);
	TwistCornerCcw(DFR);

	ShiftEdges(UR, BR, DR, RF);

	FlipEdge(UR);	
	FlipEdge(BR);	
	FlipEdge(DR);	
	FlipEdge(RF);
}
void CCube::Move_F()
{
	ShiftCorners(UFL, URF, DFR, DLF);

	TwistCornerCw(UFL);
	TwistCornerCcw(URF);
	TwistCornerCw(DFR);
	TwistCornerCcw(DLF);

	ShiftEdges(UF, RF, DF, FL);
}
void CCube::Move_B()
{
	ShiftCorners(UBR, ULB, DBL, DRB);

	TwistCornerCw(UBR);
	TwistCornerCcw(ULB);
	TwistCornerCw(DBL);
	TwistCornerCcw(DRB);

	ShiftEdges(UB, LB, DB, BR);
}

void CCube::Move_Ui()
{
	ShiftCorners(UBR, ULB, UFL, URF);
	ShiftEdges(UB, UL, UF, UR);
}
void CCube::Move_Di()
{
	ShiftCorners(DFR, DLF, DBL, DRB);
	ShiftEdges(DF, DL, DB, DR);
}
void CCube::Move_Li()
{
	ShiftCorners(UFL, ULB, DBL, DLF);

	TwistCornerCcw(UFL);
	TwistCornerCw(ULB);
	TwistCornerCcw(DBL);
	TwistCornerCw(DLF);

	ShiftEdges(UL, LB, DL, FL);

	FlipEdge(UL);	
	FlipEdge(LB);	
	FlipEdge(DL);	
	FlipEdge(FL);
}
void CCube::Move_Ri()
{
	ShiftCorners(UBR, URF, DFR, DRB);

	TwistCornerCcw(UBR);
	TwistCornerCw(URF);
	TwistCornerCcw(DFR);
	TwistCornerCw(DRB);

	ShiftEdges(UR, RF, DR, BR);

	FlipEdge(UR);	
	FlipEdge(RF);	
	FlipEdge(DR);	
	FlipEdge(BR);
}
void CCube::Move_Fi()
{
	ShiftCorners(URF, UFL, DLF, DFR);

	TwistCornerCcw(URF);
	TwistCornerCw(UFL);
	TwistCornerCcw(DLF);
	TwistCornerCw(DFR);

	ShiftEdges(UF, FL, DF, RF);
}
void CCube::Move_Bi()
{
	ShiftCorners(ULB, UBR, DRB, DBL);

	TwistCornerCcw(ULB);
	TwistCornerCw(UBR);
	TwistCornerCcw(DRB);
	TwistCornerCw(DBL);

	ShiftEdges(UB, BR, DB, LB);
}

void CCube::Move_U2()
{
	Move_U(); Move_U();
}
void CCube::Move_D2()
{
	Move_D(); Move_D();
}
void CCube::Move_L2()
{
	Move_L(); Move_L();
}
void CCube::Move_R2()
{
	Move_R(); Move_R();
}
void CCube::Move_F2()
{
	Move_F(); Move_F();
}
void CCube::Move_B2()
{
	Move_B(); Move_B();
}

// 4か所のEdgeをシフトするようにPermutation,Orientationsを操作
void CCube::ShiftEdges(const EdgeSet p_1, const EdgeSet p_2, const EdgeSet p_3, const EdgeSet p_4)
{
	ShiftVector(EdgeSetPermutation, p_1, p_2, p_3, p_4);
	ShiftVector(EdgeSetOrientations, p_1, p_2, p_3, p_4);
}
// 4か所のCornerをシフトするようにPermutation,Orientationsを操作
void CCube::ShiftCorners(const CornerSet p_1, const CornerSet p_2, const CornerSet p_3, const CornerSet p_4)
{
	ShiftVector(CornerSetPermutation, p_1, p_2, p_3, p_4);
	ShiftVector(CornerSetOrientations, p_1, p_2, p_3, p_4);
}
// 4か所のEdge,Cornerを1つずつシフトする
void CCube::ShiftVector(int* vector, const int p_1, const int p_2, const int p_3, const int p_4)
{
	int temp = vector[p_4];
	vector[p_4] = vector[p_3];
	vector[p_3] = vector[p_2];
	vector[p_2] = vector[p_1];
	vector[p_1] = temp;
}

// EdgeSetの位置のEdgeを反転する
void CCube::FlipEdge(const EdgeSet p_edge)
{
	// EX-ORを計算することで反転する
	EdgeSetOrientations[p_edge] ^= 1;
}

// CornerSetの位置のCornerを時計回りに回す
void CCube::TwistCornerCw(const CornerSet p_corner)
{
	CornerSetOrientations[p_corner] = ClockwiseTwists[CornerSetOrientations[p_corner]];
}
// CornerSetの位置のCornerを反時計回りに回す
void CCube::TwistCornerCcw(const CornerSet p_corner)
{
	CornerSetOrientations[p_corner] = CounterClockwiseTwists[CornerSetOrientations[p_corner]];
}

// 反対の動き(ex:R -> Ri, B2 -> B2)
const CCube::Move CCube::InverseMoves[NumberOfMoves] =
{
	Ui, Di, Li, Ri, Fi, Bi,
	U, D, L, R, F, B,
	U2, D2, L2, R2, F2, B2,
};

// 反対側のFace
const CCube::Move CCube::OppositeFaces[NumberOfMoves / 3] =
{
	D, U, R, L, B, F
};

// 移動記号の文字列
const std::string CCube::MoveNames[NumberOfMoves] =
{
	"U", "D", "L", "R", "F", "B",
	"U'", "D'", "L'", "R'", "F'", "B'",
	"U2", "D2", "L2", "R2", "F2", "B2"
};

// Cornerを時計回りに回すTable
const int CCube::ClockwiseTwists[NumberOfTwists] =
{
	Clockwise, CounterClockwise, NoTwist
};

// Cornerを反時計回りに回すTable
const int CCube::CounterClockwiseTwists[NumberOfTwists] =
{
	CounterClockwise, NoTwist, Clockwise
};
