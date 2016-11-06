// このアルゴリズムでは，Hervelt KociembaによるTwo Phase Algorithm
// をIDA*探索で実行する。

// Phase 1では移動記号<U,D,R,L,F,B>を演算とする群を探索する。
// Corner,EdgeのOrientations(回転状態:Twist，反転状態:Flip)と，
// Up,Down面以外のEdgeのPermutation(順列:Choice)が正しい位置に揃うような解法を探索する。
// Twistは3進数変換，Flipは2進数変換を行うことにより，コスト関数とする。
// Choiceは順列の置換を互換で表した時の互換の数をコスト関数とする。
// (完成に近いほど状態ほどコストが小さくなる:詳細はKociemba氏のHPへ)
// このとき，Corner,Up,Down面のEdgeのPermutationは考慮しない。
// Phase 1の完成状態は，Phase 2で探索対象になる群に属することになる。

// Phase 2では，Phase 1の完成状態から，移動記号<U,D,R2,L2,F2,B2>を演算とする群を探索する。
// 完成状態では，移動記号<I>(恒等変換)を演算とする群に到達する。
// 演算が<U,D,R2,L2,F2,B2>のみなので，Phase 1のTwist,Flip,Choiceは変化しない。
// Corner,Up,Down面のEdgeの順列の置換を互換で表した時の互換の数をコスト関数とする。

// コスト関数の計算を高速化するため，Pruning Table(パターンデータベース)を用いる。
// A*探索で用いるコスト関数は，「heuristic関数」である。
// これは，現在の状態Sから完成状態S'へのコストを「経験則(heuristic)」によって求める，
// すなわち，常にコストが推定可能であるが，正確ではないコスト関数を用いるというものである。
// ただし，ヒューリスティック関数は実際に要するコストよりも小さい値を出す"optimal"な
// 関数である必要がある。(h* <= h)
// "optimal"な関数を用いることで，最初に見つかった解放がPhase1-2を経由する中で最適解である
// (もっとも短い)ことを保証することができる。

// Two Phase Algorithmでは，Phase1,2を分けているので，短時間で最適解
// が見つかる保証はない。しかし，Phase 1の探索深さを下げていくことで，
// 最適解，もしくは最適解に近い解を見つけることができる。これは，Phase 1
// の演算が<U,D,R,L,F,B>(すべての操作が定義されている)ためである。
// ただし，Phase 1の群の探索空間はPhase 2に比べて遥かに大きいので，
// 探索深さが深くなるほど指数関数的に探索時間が増加する。
// 

// IDA*探索やheuristic関数に関してはこちら
// Fifteen Puzzleの例が載っている
// http://academiccommons.columbia.edu/catalog/ac:141237
// pp.11-12のように，コストが常にEstimate <= Actualになっている。

#ifndef	_IDASTARSEARCH_H_
#define	_IDASTARSEARCH_H_

#include <vector>
#include <string>
#include <QObject>

#include "ordinalcube.h"
#include "submovetable.h"
#include "pruningtable.h"
#include "timer.h"

class CIDAstarSearch : public QObject
{
    Q_OBJECT
public slots:
    void onGetSolverMessage(QString p_message)
    {
        emit notifySolverMessage(p_message);
    }
signals:
    void notifySolverMessage(QString p_message);

public:
	CIDAstarSearch();
	~CIDAstarSearch();

	enum 
	{
		NOT_FOUND = 0,	// 現在のノードで解が見つからない
		PHASE_2_FOUND,	// Phase 2の解が見つかった
		PHASE_1_FOUND,	// Phase 1の解が見つかった
		ABORT,		// 解法が長いので探索中止
		TIME_OUT	// タイムアウト
	};

	// MoveTable,PruningTableを初期化する
	void InitializeTables();

	// Two Phase Algorithmによる解探索を開始する
	int Solve(const COrdinalCube &p_scrambledCube, int64_t p_timeOut);

	// m_solutionStackの最新の解をreturnする
	std::string GetSolution() const;

private:
	enum { InitialSolutionLength = 10000 };	// 解法長さの最大値

	// Phase 1の再帰的IDA*探索関数
	// 再起呼び出しはdepth+1で行う
	int Search1(
		const int p_twist, 
		const int p_flip, 
		const int p_choice,
		const int p_depth
		);

	// Phase 2の解探索を開始する
	int Solve2(COrdinalCube& cube);

	// Phase 2の再帰的IDA*探索関数
	// 再起呼び出しはdepth+1で行う
	int Search2(
		const int p_cornerPermutation, 
		const int p_upDownEdgePermutation, 
		const int p_middleEdgePermutation, 
		const int p_depth
		);

	// Phase 1のheuristicコスト関数
	int Phase1Cost(
		const int p_twist, 
		const int p_flip, 
		const int p_choice
		) const;

	// Phase 2のheuristicコスト関数
	int Phase2Cost(
		const int p_cornerPermutation,
		const int p_upDownEdgePermutation,
		const int p_middleEdgePermutation
		) const;


	// 冗長な移動を除外する
	inline bool IsNotAllowed(
		const int p_move, 
		const int* p_solutionMoves, 
		const int p_depth
		) const;

	// 現在得られている解法(一番短いはず)を出力する
	void PrintAndStackSolution();

	// 移動記号と反復回数(face, power)から回転記号を求める
	// ex:(U, 2) -> U2, (R, 1) -> R, (D, 3) -> D'
	// Phase 2での"LRFB"の移動は，180[deg](power = 2)のみだが，
	// Phase 2のMoveTableでは，"L2,R2,F2,B2"の移動をpower = 1
	// として扱っているので，出力時に補正する必要がある
	int TranslateMove(const int p_move, int p_power, const bool p_phase2) const;
	
	// IDA*探索で使用する変数
	int m_nodes1, m_nodes2;	// 現在のノード数
	int m_threshold1, m_threshold2;	// 合計コストの足きり基準(cutoff)
	int m_nextThreshold1, m_nextThreshold2;	// 次の探索で用いる足きり基準を保存する変数

	int m_solutionMoves1[32], m_solutionMoves2[32];	// 移動記号
	int m_solutionPowers1[32], m_solutionPowers2[32];	// 移動記号の反復回数
	int m_solutionLength1, m_solutionLength2;	// 解法の長さ
	int m_minSolutionLength;	// 今まで見つかった解法のうち一番短いものの長さ
	std::vector<std::string> m_solutionStack;	// Solutionを保存するStack

    CTimer *m_timer;	// タイムアウトを計算するオブジェクト

	// 1.MoveTableの初期化に使用するための変数
	// 2.Solve関数で初期状態を保存するために用いる変数
	COrdinalCube m_cube;

	// Phase 1のMoveTable
	CTwistMoveTable m_twistMoveTable;
	CFlipMoveTable m_flipMoveTable;
	CChoiceMoveTable m_choiceMoveTable;
	// Phase 2のMoveTable
	CCornerPermutationMoveTable m_cornerPermutationMoveTable;
	CUpDownEdgePermutationMoveTable m_upDownEdgePermutationMoveTable;
	CMiddleEdgePermutationMoveTable m_middleEdgePermutationMoveTable;
	
	// Phase 1のPruningTable
	CPruningTable m_twistAndFlipPruningTable;
	CPruningTable m_twistAndChoicePruningTable;
	CPruningTable m_flipAndChoicePruningTable;
	// Phase 2のPruningTable
	CPruningTable m_cornerAndUpDownPruningTable;
	CPruningTable m_upDownAndMiddlePruningTable;
};

#endif	// _IDASTARSEARCH_H_
