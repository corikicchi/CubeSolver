#include "movetable.h"
#include "../solverthread.h"

#include <iostream>
#include <fstream>
#include <iomanip>

// CubeへのReference,MoveTableのサイズ,Phase2かどうかを設定してメモリを確保
// CubeはReferenceなのでMoveTableのメンバ関数によって操作される
CMoveTable::CMoveTable(CCube &cube, const int p_tableSize, const bool p_isPhase2)
	:m_cubeRef(cube),
	m_tableSize(p_tableSize),
	m_isPhase2(p_isPhase2)
{
	// MoveTableを確保
	// int Table[m_tableSize][6]を確保する
	// http://d.hatena.ne.jp/Guernsey/20090924/1253775843
	m_table = /*キャスト*/(int (*)[CCube::Move::NumberOfClockwiseQuarterTurnMoves])/*メモリ確保*/(new int[m_tableSize * CCube::Move::NumberOfClockwiseQuarterTurnMoves]);
}

// MoveTableを解放
CMoveTable::~CMoveTable()
{
	// MoveTableを解放
	// http://d.hatena.ne.jp/Guernsey/20090924/1253775843
	delete [] m_table;
}

// MoveTableを読み込む
// MoveTableが無ければ作成する
void CMoveTable::Initialize(const std::string p_fileName)
{
	std::ifstream inputFile(p_fileName, std::ios::in | std::ios::binary);
	if (!inputFile) {
		// ファイルが存在しないとき
		// MoveTableを作成する
        //std::cout << "Generating..." << std::endl;
        emit notifySolverMessage("Generating...");
		GenerateMoveTable();

		// MoveTableは，各状態のCubeに対して6種類の移動を行い，
		// ぞれぞれの状態のState Number(ordinal)を代入しているので，
		// 1つの状態(ordinal)に対するテーブルのサイズ(byte)は 6種類の移動 * sizeof(int)になる
        //std::cout << "Saving..." << std::endl;
        emit notifySolverMessage("Saving...");
		std::ofstream outputFile(p_fileName, std::ios::out | std::ios::binary);
		for (int index = 0; index < m_tableSize; index++) {
			outputFile.write((char*)&m_table[index],
				CCube::Move::NumberOfClockwiseQuarterTurnMoves * sizeof(int));
		}
		outputFile.close();
        //std::cout << "Done" << std::endl;
        emit notifySolverMessage("Done");
	}
	else{
		// ファイルが存在するとき
		// MoveTableを読み込む
        //std::cout << "Loading..." << std::endl;
        //emit notifySolverMessage("Loading...");

		// MoveTableは，各状態のcubeに対して6種類の移動を行い，
		// ぞれぞれの状態のState Number(ordinal)を代入しているので，
		// 1つの状態(ordinal)に対するテーブルのサイズ(byte)は 6種類の移動 * sizeof(int)になる
		for (int index = 0; index < m_tableSize; index++) {
			inputFile.read((char*)&m_table[index],
				CCube::Move::NumberOfClockwiseQuarterTurnMoves * sizeof(int));
		}
		inputFile.close();
        //std::cout << "Done" << std::endl;
        //emit notifySolverMessage("Done");
	}
}

// privateなTableに対して，オブジェクトの添え字でアクセスするための演算子
int* CMoveTable::operator[](const int p_index)
{
	return m_table[p_index];
}

// MoveTableを出力する
void CMoveTable::PrintMoveTable() const
{
	// 各MoveTable[ordinal]に対して表示する
	for (int ordinal = 0; ordinal < m_tableSize; ordinal++) {
		// MoveTableの序数を表示
		std::cout << std::setw(8) << ordinal << ": ";
		// 6種類の移動(UDLRFB)に対して表示する
		for (int move = CCube::Move::U; move <= CCube::Move::B; move++) {
			int move2 = move;
			if (m_isPhase2 && move != CCube::Move::U && move != CCube::Move::D) {
                move2 = CCube::ConvertQuarterTurnToHalfTurnMove(move);
			}
			std::cout << CCube::GetNameOfMove(move2) << ":" << std::setw(8) << m_table[ordinal][move] << " ";
		}
		std::cout << std::endl;
	}
}

// MoveTableを作成する
void CMoveTable::GenerateMoveTable()
{
	// 移動テーブルを作成するためにclear cubeを取得する
	m_cubeRef.SetCleanCube();

	// 移動テーブルを初期化
	for (int ordinal = 0; ordinal < m_tableSize; ordinal++) {
		// ordinalの値に応じたuniqueなcubeの状態を設定する
		SetCubeStateFromOrdinal(ordinal);

		// それぞれの状態に対しての移動を設定する
		for (int move = CCube::Move::U; move <= CCube::Move::B; move++) {
			// Phase 1 では <U,D,R,L,F,B> の移動のみ
			// Phase 2 では <U,D,R2,L2,F2,B2> の移動のみ
			int move2 = move;
			if (m_isPhase2 && move != CCube::Move::U && move != CCube::Move::D) {
                move2 = CCube::ConvertQuarterTurnToHalfTurnMove(move);	// R,L,F,Bを180[deg]回転に変換する
			}

			// cubeを動かす(後で戻す)
			// ここでCornerSetOrientations, EdgeSetOrientations等が変化する
			m_cubeRef.ApplyMove(move2);

			// cubeを動かしたときの状態遷移後の序数を格納する
			// OrdinalToCubeState(ordinal)で設定した
			// CornerSetOrientations, EdgeSetOrientations等に対して
			// それらの回転，方向を表現するuniqueな数字を取得して格納する
			// m_table[ordinal][move]は序数ordinalの状態のときにmoveの状態遷移をした後の序数を表す
			m_table[ordinal][move] = GetOrdinalFromCubeState();

			// cubeを戻しておく
			m_cubeRef.ApplyMove(CCube::GetInverseOfMove(move2));
		}
	}
}
