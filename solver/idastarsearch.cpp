#include "idastarsearch.h"
#include "../solverthread.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

CIDAstarSearch::CIDAstarSearch()
	// Clean Cubeを渡してconstructする
	// Phase 1のMoveTable
	: m_twistMoveTable(m_cube),
	m_flipMoveTable(m_cube),
	m_choiceMoveTable(m_cube),
	// Phase 2のMoveTable
	m_cornerPermutationMoveTable(m_cube),
	m_upDownEdgePermutationMoveTable(m_cube),
	m_middleEdgePermutationMoveTable(m_cube),

	// MoveTable2つを組み合わせて，PruningTable(パターンデータベース)を作成する
	// Phase 1の刈込テーブル
	m_twistAndFlipPruningTable(
		m_twistMoveTable, m_flipMoveTable,
		m_cube.GetTwistFromOrientations(), m_cube.GetFlipFromOrientations()),
	m_twistAndChoicePruningTable(
		m_twistMoveTable, m_choiceMoveTable,
		m_cube.GetTwistFromOrientations(), m_cube.GetChoiceFromEdgePermutation()),
	m_flipAndChoicePruningTable(
		m_flipMoveTable, m_choiceMoveTable,
		m_cube.GetFlipFromOrientations(), m_cube.GetChoiceFromEdgePermutation()),
	// Phase 2の刈込テーブル
	m_cornerAndUpDownPruningTable(
		m_cornerPermutationMoveTable, m_middleEdgePermutationMoveTable,
		m_cube.GetOrdinalFromCornerPermutation(), m_cube.GetOrdinalFromMiddleEdgePermutation()),
	m_upDownAndMiddlePruningTable(
		m_upDownEdgePermutationMoveTable, m_middleEdgePermutationMoveTable,
		m_cube.GetOrdinalFromUpDownEdgePermutation(), m_cube.GetOrdinalFromMiddleEdgePermutation())
{
	m_minSolutionLength = InitialSolutionLength;
	m_solutionStack.clear();
}

CIDAstarSearch::~CIDAstarSearch()
{
}

// MoveTable,PruningTableを初期化する
void CIDAstarSearch::InitializeTables()
{
	// Phase 1のMoveTableを作成する
    //std::cout << "Initializing TwistMoveTable" << std::endl;
    //emit SolverThread::notifySolverMessage("Initializing TwistMoveTable");
	m_twistMoveTable.Initialize("TwistMoveTable.mt");
    //std::cout << "Size = " << m_twistMoveTable.GetSize() << std::endl;
    //emit SolverThread::notifySolverMessage("Size = " + QString::number(m_twistMoveTable.GetSize()));

    //std::cout << "Initializing FlipMoveTable" << std::endl;
    //emit SolverThread::notifySolverMessage("Initializing FlipMoveTable");
	m_flipMoveTable.Initialize("FlipMoveTable.mt");
    //std::cout << "Size = " << m_flipMoveTable.GetSize() << std::endl;
    //emit SolverThread::notifySolverMessage("Size = " + QString::number(m_flipMoveTable.GetSize()));

    //std::cout << "Initializing ChoiceMoveTable" << std::endl;
    //emit SolverThread::notifySolverMessage("Initializing ChoiceMoveTable");
	m_choiceMoveTable.Initialize("ChoiceMoveTable.mt");
    //std::cout << "Size = " << m_choiceMoveTable.GetSize() << std::endl;
    //emit SolverThread::notifySolverMessage("Size = " + QString::number(m_choiceMoveTable.GetSize()));

	// Phase 2のMoveTableを作成する
    //std::cout << "Initializing CornerPermutationMoveTable" << std::endl;
    //emit SolverThread::notifySolverMessage("Initializing CornerPermutationMoveTable");
	m_cornerPermutationMoveTable.Initialize("CornerPermutationMoveTable.mt");
    //std::cout << "Size = " << m_cornerPermutationMoveTable.GetSize() << std::endl;
    //emit SolverThread::notifySolverMessage("Size = " + QString::number(m_cornerPermutationMoveTable.GetSize()));

    //std::cout << "Initializing UpDownEdgePermutationMoveTable" << std::endl;
    //emit SolverThread::notifySolverMessage("Initializing UpDownEdgePermutationMoveTable");
	m_upDownEdgePermutationMoveTable.Initialize("UpDownEdgePermutationMoveTable.mt");
    //std::cout << "Size = " << m_upDownEdgePermutationMoveTable.GetSize() << std::endl;
    //emit SolverThread::notifySolverMessage("Size = " + QString::number(m_upDownEdgePermutationMoveTable.GetSize()));

    //std::cout << "Initializing MiddleEdgePermutationMoveTable" << std::endl;
    //emit SolverThread::notifySolverMessage("Initializing MiddleEdgePermutationMoveTable");
	m_middleEdgePermutationMoveTable.Initialize("MiddleEdgePermutationMoveTable.mt");
    //std::cout << "Size = " << m_middleEdgePermutationMoveTable.GetSize() << std::endl;
    //emit SolverThread::notifySolverMessage("Size = " + QString::number(m_middleEdgePermutationMoveTable.GetSize()));

	// Phase 1のPruningTableを作成する
    //std::cout << "Initializing TwistAndFlipPruningTable" << std::endl;
    //emit SolverThread::notifySolverMessage("Initializing TwistAndFlipPruningTable");
	m_twistAndFlipPruningTable.Initialize("TwistAndFlipPruningTable.pt");
    //std::cout << "Size = " << m_twistAndFlipPruningTable.GetSize() << std::endl;
    //emit SolverThread::notifySolverMessage("Size = " + QString::number(m_twistAndFlipPruningTable.GetSize()));

    //std::cout << "Initializing TwistAndChoicePruningTable" << std::endl;
    //emit SolverThread::notifySolverMessage("Initializing TwistAndChoicePruningTable");
	m_twistAndChoicePruningTable.Initialize("TwistAndChoicePruningTable.pt");
    //std::cout << "Size = " << m_twistAndChoicePruningTable.GetSize() << std::endl;
    //emit SolverThread::notifySolverMessage("Size = " + QString::number(m_twistAndChoicePruningTable.GetSize()));

    //std::cout << "Initializing FlipAndChoicePruningTable" << std::endl;
    //emit SolverThread::notifySolverMessage("Initializing FlipAndChoicePruningTable");
	m_flipAndChoicePruningTable.Initialize("FlipAndChoicePruningTable.pt");
    //std::cout << "Size = " << m_flipAndChoicePruningTable.GetSize() << std::endl;
    //emit SolverThread::notifySolverMessage("Size = " + QString::number(m_flipAndChoicePruningTable.GetSize()));

	// Phase 2のPruningTableを作成する
    //std::cout << "Initializing CornerAndUpDownPruningTable" << std::endl;
    //emit SolverThread::notifySolverMessage("Initializing CornerAndUpDownPruningTable");
	m_cornerAndUpDownPruningTable.Initialize("CornerAndUpDownPruningTable.pt");
    //std::cout << "Size = " << m_cornerAndUpDownPruningTable.GetSize() << std::endl;
    //emit SolverThread::notifySolverMessage("Size = " + QString::number(m_cornerAndUpDownPruningTable.GetSize()));

    //std::cout << "Initializing UpDownAndMiddlePruningTable" << std::endl;
    //emit SolverThread::notifySolverMessage("Initializing UpDownAndMiddlePruningTable");
	m_upDownAndMiddlePruningTable.Initialize("UpDownAndMiddlePruningTable.pt");
    //std::cout << "Size = " << m_upDownAndMiddlePruningTable.GetSize() << std::endl;
    //emit SolverThread::notifySolverMessage("Size = " + QString::number(m_upDownAndMiddlePruningTable.GetSize()));
}

// Two Phase AlgorithmによるIDA*探索を開始する
int CIDAstarSearch::Solve(const COrdinalCube &p_scrambledCube, int64_t p_timeOut)
{
	int iteration = 1;	// 反復回数
	int result = NOT_FOUND;

	m_cube = p_scrambledCube;

	// http://piyajk.com/archives/162
	// とりあえず，今の状態からPhase1の推定コストを計算する
	// コストは大きめの値が計算されるようになっている(ヒューリスティック関数)
	// 小さいほど完成状態に近い
	m_threshold1 = Phase1Cost(
		m_cube.GetTwistFromOrientations(),
		m_cube.GetFlipFromOrientations(),
		m_cube.GetChoiceFromEdgePermutation()
		);

	m_nodes1 = 1;		// ノードの場所
	m_solutionLength1 = 0;

	// タイマーConstruct
    m_timer = new CTimer(p_timeOut);
	// タイマーStart
	m_timer->start();

	do{
        //std::cout << "threshold(" << iteration << ") = " << m_threshold1 << std::endl;
        //emit SolverThread::notifySolverMessage(QString::number(iteration) + " : threshold = " + QString::number(m_threshold1));
		m_nextThreshold1 = InitialSolutionLength;	// コストを最大にする

		// 現在のCubeの状態に対して，深さ0のIDA*探索を開始する
		result = Search1(
			m_cube.GetTwistFromOrientations(),
			m_cube.GetFlipFromOrientations(),
			m_cube.GetChoiceFromEdgePermutation(),
			0
			);

		// 閾値をより浅くして探索する
		m_threshold1 = m_nextThreshold1;

		// 反復回数を増やす
		iteration++;

        //std::cout << "Phase 1 nodes = " << m_nodes1 << std::endl;
        //emit SolverThread::notifySolverMessage("Phase 1 nodes = " + QString::number(m_nodes1));

		// タイマーCheck
		if (m_timer->isTimeOut() || result == TIME_OUT) {
			result = TIME_OUT;
			break;
		}

		// 解法が見つかるまで続ける
	} while (result == NOT_FOUND);

	delete m_timer;
	return result;
}

// m_solutionStackの最新の解をreturnする
std::string CIDAstarSearch::GetSolution() const
{
	if (m_solutionStack.size() >= 1) {
		return m_solutionStack.at(m_solutionStack.size() - 1);
	}
	else {
		return "Solution was not found.";
	}
}

// Phase 1の再帰的IDA*探索関数
// 再起呼び出しはdepth+1で行う
int CIDAstarSearch::Search1(const int p_twist, const int p_flip, const int p_choice, const int p_depth)
{
	int totalCost;

	// 現在のCubeの状態からPhase1完成までのコストを計算
	int cost = Phase1Cost(p_twist, p_flip, p_choice);	

	if (cost == 0){
		// (twist, flip, choice)がPhase1の完成状態
		// 解法が見つかったから探索深さを保存
		m_solutionLength1 = p_depth;

		// Phase2の解法探索を行う前に，CubeをPhase1へ初期化する
		// (Cubeを最初の状態に戻す -> Phase1の解法の移動を行う)
		COrdinalCube phase2Cube = m_cube;

		// Cubeに対してPhase1の操作を加える
		for (int i = 0; i < m_solutionLength1; i++){
			// 各移動の回数分動かします
			for (int power = 0; power < m_solutionPowers1[i]; power++) {
				phase2Cube.ApplyMove(m_solutionMoves1[i]);
			}
		}
		// ここでPhase1の完成状態

		// Phase2探索を始める
		// タイマーCheck
		if (m_timer->isTimeOut() || Solve2(phase2Cube) == TIME_OUT) {
			return TIME_OUT;
		}

		// このCubeに対するPhase2探索終わり

		// 別のPhase1完成状態に対して，Phase2のもっと短い解がある可能性があるのでPhase1探索を続ける
	}

	// IDA*探索のsearch-function
	// https://en.wikipedia.org/wiki/Iterative_deepening_A*
	// IDA*探索とは
	// A*探索のコストが，ある閾値を超えた場合にそのノードの探索を終了する探索方法

	// f(n):合計コスト
	// g(n):depth 最初から今の状態nにするために必要なコスト
	// h(n):cost  今の状態nからPhase1の完成に必要なコスト
	totalCost = p_depth + cost;	// 合計コストを計算 (f = g + h)

	// 閾値判定を行う
	// threshold1：
	// 1回目の探索では，初めの状態よりヒューリスティック関数で計算
	// n+1回目の探索以降では，n回目のtotalCostのうち最小のもの(newThreshold1)を用いる
	if (totalCost <= m_threshold1){
		// ノードの探索を行う
		// minSolutionLength : 今まで見つかった解放のうち一番短いものの長さ
		// 探索の深さがminSolutionLengthを超えてたら探索する意味がないのでreturn
		if (p_depth >= m_minSolutionLength - 1) {
			return PHASE_1_FOUND;
		}

		for (int move = CCube::Move::U; move <= CCube::Move::B; move++){
			// 意味の無い動きは除外
			if (IsNotAllowed(move, m_solutionMoves1, p_depth)) continue;

			// 現在の状態
			int twist2 = p_twist;
			int flip2 = p_flip;
			int choice2 = p_choice;

			// 移動指令を保存
			m_solutionMoves1[p_depth] = move;

			// 状態遷移を行う
			// powerは移動の反復回数
			for (int power = 1; power < 4; power++){
				// 移動指令の反復回数を保存
				m_solutionPowers1[p_depth] = power;
				// 状態遷移
				// ex:twist2の状態に移動moveを行った時の新たな状態を取得する
				twist2 = m_twistMoveTable[twist2][move];
				flip2 = m_flipMoveTable[flip2][move];
				choice2 = m_choiceMoveTable[choice2][move];
				// ノードを増やす
				m_nodes1++;
				
				// 今の状態を起点に，深さを増やして探索
				int result;
				if (result = Search1(twist2, flip2, choice2, p_depth + 1)) {
					return result;	// 探索終了(PHASE_1_FOUND)ならreturn
				}
			}
		}
	}
	else{
		// 今のコストで最小閾値を更新する
		// ただし，threshold1は現在の閾値なので，現在の閾値を基準とした探索が終わるまでは
		// 変更しないようにする -> newThreshold1に最小コストを保存しておく
		if (totalCost < m_nextThreshold1) {
			m_nextThreshold1 = totalCost;
		}
	}
	
	return NOT_FOUND;
}

// Phase 2の解探索を開始する
int CIDAstarSearch::Solve2(COrdinalCube& cube)
{
	int iteration = 1;	// 反復回数
	int result = NOT_FOUND;

	// http://piyajk.com/archives/162
	// とりあえず，今の状態からPhase2の推定コストを計算する
	// コストは大きめの値が計算されるようになっている(ヒューリスティック関数)
	// 小さいほど完成状態に近い
	m_threshold2 = Phase2Cost(
		cube.GetOrdinalFromCornerPermutation(),
		cube.GetOrdinalFromUpDownEdgePermutation(),
		cube.GetOrdinalFromMiddleEdgePermutation()
		);

	m_nodes2 = 1;		// 今のノード
	m_solutionLength2 = 0;

	do{
		m_nextThreshold2 = InitialSolutionLength;	// コストを大きな値にする

		// 現在のCubeの状態に対して，深さ0のIDA*探索を開始する
		result = Search2(
			cube.GetOrdinalFromCornerPermutation(),
			cube.GetOrdinalFromUpDownEdgePermutation(),
			cube.GetOrdinalFromMiddleEdgePermutation(),
			0
			);

		// 閾値をより浅くして探索する
		m_threshold2 = m_nextThreshold2;

		// 反復回数を増やす
		iteration++;

		// タイマーCheck
		if (m_timer->isTimeOut() || result == TIME_OUT) {
			result = TIME_OUT;
			break;
		}

		// 解放が見つかるまで続ける
	} while (result == NOT_FOUND);

	return result;
}

int CIDAstarSearch::Search2(const int p_cornerPermutation, const int p_upDownEdgePermutation, const int p_middleEdgePermutation, const int p_depth)
{
	// 合計コスト
	int totalCost;

	// 完成までのコストを計算
	int cost = Phase2Cost(p_cornerPermutation, p_upDownEdgePermutation, p_middleEdgePermutation);

	if (cost == 0){	// 解法が見つかりました
		// 解法が見つかったから探索深さを保存
		m_solutionLength2 = p_depth;
		// 今までの解法より短かったら一番短い解法の長さ(minSolutionLength)を更新しておく
		if (m_solutionLength1 + m_solutionLength2 < m_minSolutionLength) {
			m_minSolutionLength = m_solutionLength1 + m_solutionLength2;
		}
		// Phase1とPhase2が完成したから表示
		// スタックする
		PrintAndStackSolution();

		return PHASE_2_FOUND;
	}

	// IDA*探索のsearch-function
	// https://en.wikipedia.org/wiki/Iterative_deepening_A*
	// IDA*探索とは
	// A*探索のコストが，ある閾値を超えた場合にそのノードの探索を終了する探索方法

	// f(n):合計コスト
	// g(n):depth 最初から今の状態nにするために必要なコスト
	// h(n):cost  今の状態nからPhase1の完成に必要なコスト
	totalCost = p_depth + cost;	// 合計コストを計算 (f = g + h)
	
	// 閾値判定を行う
	// threshold2：
	// 1回目の探索では，初めの状態よりヒューリスティック関数で計算
	// n+1回目の探索以降では，n回目のtotalCostのうち最小のもの(newThreshold2)を用いる
	if (totalCost <= m_threshold2){
		// ノードの探索を行う
		// minSolutionLength : 今まで見つかった解放のうち一番短いものの長さ
		// 探索の深さがminSolutionLengthを超えてたら探索する意味がないのでreturn
		if (m_solutionLength1 + p_depth >= m_minSolutionLength - 1) {
			return ABORT;
		}

		// 6種類の移動に対して
		for (int move = CCube::Move::U; move <= CCube::Move::B; move++){
			// 意味の無い動きは除外
			if (IsNotAllowed(move, m_solutionMoves2, p_depth)) continue;

			// 現在の状態
			int cornerPermutation2 = p_cornerPermutation;
			int upDownEdgePermutation2 = p_upDownEdgePermutation;
			int middleEdgePermutation2 = p_middleEdgePermutation;
			
			// 移動指令を保存
			m_solutionMoves2[p_depth] = move;

			int powerLimit = 4;	// "FBLR"は反時計，時計周りの移動ができる
			if (move != CCube::Move::U && move != CCube::Move::D) {
				powerLimit = 2;	// "UD"は180[deg]回転しかできない
			}

			// 状態遷移を行う
			// powerは移動の反復回数(FBLRは3回まで，UDは2回だけ)
			for (int power = 1; power < powerLimit; power++){
				// 移動指令の反復回数を保存
				m_solutionPowers2[p_depth] = power;
				// 状態遷移
				// ex:現在の状態(cornerPermutation2, nonMiddleSliceEdgePermutation2, middleSliceEdgePermutation2て表現)に移動moveを作用させるとどのような状態に遷移するかを取得する
				cornerPermutation2 = m_cornerPermutationMoveTable[cornerPermutation2][move];
				upDownEdgePermutation2 = m_upDownEdgePermutationMoveTable[upDownEdgePermutation2][move];
				middleEdgePermutation2 = m_middleEdgePermutationMoveTable[middleEdgePermutation2][move];
				// ノードを増やす
				m_nodes2++;

				// 今の状態を起点に，深さを増やして探索
				// PHASE_2_FOUND,ABORTだったら探索終了
				// NOT_FOUNDだったらさらに探索を続ける
				int result;
				if (result = Search2(cornerPermutation2, upDownEdgePermutation2, middleEdgePermutation2, p_depth + 1)) {
					return result;
				}
			}
		}
	}
	else{
		// 今のコストで最小閾値を更新する
		// ただし，threshold2は現在の閾値なので，現在の閾値を基準とした探索が終わるまでは
		// 変更しないようにする -> newThreshold2に最小コストを保存しておく
		if (totalCost < m_nextThreshold2) {
			m_nextThreshold2 = totalCost;
		}
	}

	return NOT_FOUND;
}

// Phase 1のheuristicコスト関数
int CIDAstarSearch::Phase1Cost(const int p_twist, const int p_flip, const int p_choice) const
{
	// 3つのうち一番大きな値をコスト関数として採用する
	// 最適解を見つけるためにはヒューリスティック関数は「楽観的」でなければならない
	int cost = m_twistAndFlipPruningTable.GetValue(p_twist * m_flipMoveTable.GetSize() + p_flip);
	int cost2 = m_twistAndChoicePruningTable.GetValue(p_twist * m_choiceMoveTable.GetSize() + p_choice);
	if (cost2 > cost) cost = cost2;
	cost2 = m_flipAndChoicePruningTable.GetValue(p_flip * m_choiceMoveTable.GetSize() + p_choice);
	if (cost2 > cost) cost = cost2;
	return cost;
}

// Phase 2のheuristicコスト関数
int CIDAstarSearch::Phase2Cost(const int p_cornerPermutation, const int p_upDownEdgePermutation, const int p_middleEdgePermutation) const
{
	// 2つのうち一番大きな値をコスト関数として採用する
	// 最適解を見つけるためにはヒューリスティック関数は「楽観的」でなければならない
	// middleはあまり使う意味がないので使わない
	int cost = m_cornerAndUpDownPruningTable.GetValue(p_cornerPermutation * m_middleEdgePermutationMoveTable.GetSize() + p_middleEdgePermutation);
	int cost2 = m_upDownAndMiddlePruningTable.GetValue(p_upDownEdgePermutation * m_middleEdgePermutationMoveTable.GetSize() + p_middleEdgePermutation);
	if (cost2 > cost) cost = cost2;
	return cost;
}

// 冗長な移動を除外する
bool CIDAstarSearch::IsNotAllowed(const int p_move, const int* p_solutionMoves, const int p_depth) const
{
	// 1つ前の移動を確認する
	if (p_depth >= 1){
		// 連続した移動はできない (ex. UU は U で power = 3 にしないといけない)
		if (p_solutionMoves[p_depth - 1] == p_move) {
			return true;
		}

		// "F B", "R L", "U D"だけできるようにしておく
		// これと等価な"B F", "L R", "D U"の移動はできない
		if ((p_move == CCube::Move::F) && p_solutionMoves[p_depth - 1] == CCube::Move::B) {
			return true;
		}
		if ((p_move == CCube::Move::R) && p_solutionMoves[p_depth - 1] == CCube::Move::L) {
			return true;
		}
		if ((p_move == CCube::Move::U) && p_solutionMoves[p_depth - 1] == CCube::Move::D) {
			return true;
		}

		// ある面の移動2回で，その反対側の面の移動を挟んだ移動はできない
		// ex."F B F" -> "F2 B", "B F2"と同じ
		//    "D U D" -> "D2 U", "U D2"と同じ
		if ((p_depth >= 2) && p_solutionMoves[p_depth - 2] == p_move && p_solutionMoves[p_depth - 1] == CCube::GetOpposingFace(p_move)) {
			return true;
		}
	}
	return false;	// 移動に問題なし
}

void CIDAstarSearch::PrintAndStackSolution()
{
	std::stringstream ss;
	ss.clear();

	ss << m_solutionLength1 + m_solutionLength2 << " ";

	for (int i = 0; i < m_solutionLength1; i++) {
		// 無駄な移動を簡単にしてから表示する
		//std::cout << CCube::NameOfMove(TranslateMove(solutionMoves1[i], solutionPowers1[i], false)) << " ";
        //emit SolverThread::notifySolverMessage(QString::fromStdString(CCube::GetNameOfMove(TranslateMove(m_solutionMoves1[i], m_solutionPowers1[i], false))) + " ");
		ss << CCube::GetNameOfMove(TranslateMove(m_solutionMoves1[i], m_solutionPowers1[i], false)) << " ";
	}
	//std::cout << ". ";
    //emit SolverThread::notifySolverMessage(". ");
	ss << ". ";
	for (int i = 0; i < m_solutionLength2; i++) {
		// 無駄な移動を簡単にしてから表示する
		//std::cout << CCube::NameOfMove(TranslateMove(solutionMoves2[i], solutionPowers2[i], true)) << " ";
        //emit SolverThread::notifySolverMessage(QString::fromStdString(CCube::GetNameOfMove(TranslateMove(m_solutionMoves2[i], m_solutionPowers2[i], true))) + " ");
		ss << CCube::GetNameOfMove(TranslateMove(m_solutionMoves2[i], m_solutionPowers2[i], true)) << " ";
	}
    //std::cout << "(" << solutionLength1 + solutionLength2 << ")" << std::endl;
    //emit SolverThread::notifySolverMessage("(" + QString::number(m_solutionLength1 + m_solutionLength2) + ")");

	m_solutionStack.push_back(ss.str());
}

int CIDAstarSearch::TranslateMove(const int p_move, int p_power,  const bool p_phase2) const
{
	int translatedMove = p_move;

	// Phase2で，"UD"の移動以外は180[deg]回転しかできない
	if (p_phase2 && p_move != CCube::Move::U && p_move != CCube::Move::D) {
		p_power = 2;
	}

	// 90[deg]回転を2回やるなら180[deg]回転に変換する
	if (p_power == 2) {
		translatedMove = CCube::GetQuarterTurnToHalfTurnMove(p_move);
	}
	// 90[deg]回転を3回やるなら-90[deg]回転に変換する
	else if (p_power == 3) {
		translatedMove = CCube::GetInverseOfMove(p_move);
	}
	else {

	}

	return translatedMove;
}
