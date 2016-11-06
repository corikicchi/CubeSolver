#include "pruningtable.h"
#include "movetable.h"
#include "./solverthread.h"

#include <iomanip>

// 組み合わせる2つのMoveTableへのReferenceとCleanCubeにおける各序数を格納する
CPruningTable::CPruningTable(CMoveTable& moveTable1, CMoveTable& moveTable2, const int p_homeOrdinal1, const int p_homeOrdinal2) :
	m_moveTableRef1(moveTable1),
	m_moveTableRef2(moveTable2),
	m_homeOrdinal1(p_homeOrdinal1),
	m_homeOrdinal2(p_homeOrdinal2)
{
	// テーブルのサイズを格納
	m_moveTable1Size = m_moveTableRef1.GetSize();
	m_moveTable2Size = m_moveTableRef2.GetSize();
	m_tableSize = m_moveTable1Size * m_moveTable2Size;

	// テーブルのために確保するメモリのサイズを決定する(単位:byte)
	// (intに丸める処理を行う:Table1要素あたり2[byte])
	// TableSize:1 - 8 のとき AllocationSize = 4
	// TableSize:9 - 16 のとき AllocationSize = 8
	// TableSize:17 - 24 のとき AllocationSize = 12
	// ...
	m_allocationSize = ((m_tableSize + 7) / 8) * 4;
	// テーブルをAllocationSize[byte]確保する
	m_table = new unsigned char[m_allocationSize];
}

void CPruningTable::Initialize(const std::string p_fileName)
{
	std::ifstream input(p_fileName, std::ios::in| std::ios::binary);
	if (!input){
		// ファイルが無いときはファイルを作る
        //std::cout << "Generating..." << std::endl;
        emit notifySolverMessage("Generating...");
		GeneratePruningTable();
        //std::cout << "Saving..." << std::endl;
        emit notifySolverMessage("Saving...");
		std::ofstream output(p_fileName, std::ios::out | std::ios::binary);
		output.write((const char*)m_table, m_allocationSize);
        //std::cout << "Done" << std::endl;
        emit notifySolverMessage("Done");
	}
	else{
		// ファイルが存在したら読み込む
        //std::cout << "Loading..." << std::endl;
        //emit notifySolverMessage("Loading...");
		input.read((char*)m_table, m_allocationSize);
        //std::cout << "Done" << std::endl;
        //emit notifySolverMessage("Done");
	}
}

CPruningTable::~CPruningTable()
{
	delete [] m_table;
}

// 幅優先探索のためのPruningTableを作成
void CPruningTable::GeneratePruningTable()
{
	unsigned int depth = 0;
	int numberOfNodes;

	for (int index = 0; index < m_tableSize; index++) {
		SetValue(index, Empty);	// Empty = 0x0F
	}

	// 探索木のスタート地点を設定する
	SetValue(MoveTableIndicesToPruningTableIndex(m_homeOrdinal1, m_homeOrdinal2), depth);
	numberOfNodes = 1;	// ノードの数

	// テーブルサイズいっぱいまで繰り返す
	while (numberOfNodes < m_tableSize) {
		// PruningTableを全探索する -> 現在のdepthと一致するものを参照する
		for (int index = 0; index < m_tableSize; index++) {
			// 現在のdepthと一致したら，そこから枝を伸ばす
			if (GetValue(index) == depth) {
				// MoveTableを用いて，ある移動に対する状態遷移を取得する
				int ordinal1, ordinal2;
				for (int move = CCube::Move::U; move <= CCube::Move::B; move++) {
					// 最初の状態の序数を取得
					PruningTableIndexToMoveTableIndices(index, ordinal1, ordinal2);
					// 同じ移動を90[deg]回転x3行う
					for (int power = 1; power < 4; power++) {
						// MoveTableを用いて，ある移動に対する状態遷移後の序数を取得する
						ordinal1 = m_moveTableRef1[ordinal1][move];
						ordinal2 = m_moveTableRef2[ordinal2][move];
						// 2つの序数からPruningTableのインデックスを取得
						int index2 = MoveTableIndicesToPruningTableIndex(ordinal1, ordinal2);

						// 既にindex2の場所に状態が設定されていたら飛ばす
						if (GetValue(index2) == Empty) {
							SetValue(index2, depth + 1);
							numberOfNodes++;
						}
					}
				}
			}
		}
		// 探索が終了したらdepthを深くする
		depth++;
        //std::cout << "Completed Depth = " << depth << std::endl;
        emit notifySolverMessage("Completed Depth = " + QString::number(depth));
	}
}

// PruningTableのIndexからMoveTableの序数を取得
void CPruningTable::PruningTableIndexToMoveTableIndices(const int p_index, int& ordinal1, int& ordinal2) const
{
	ordinal1 = p_index / m_moveTable2Size;
	ordinal2 = p_index % m_moveTable2Size;
}

// MoveTableの序数からPruningTableのIndexを取得
int CPruningTable::MoveTableIndicesToPruningTableIndex(const int p_ordinal1, const int p_ordinal2) const
{
	// 序数を組み合わせる
	return p_ordinal1 * m_moveTable2Size + p_ordinal2;
}

// PruningTableのDepthを取得
unsigned int CPruningTable::GetValue(const int p_index) const
{
	// indexが奇数:Table[i]の前半4ビットにdepthを格納
	// indexが偶数:Table[i]の後半4ビットにdepthを格納
	if (p_index % 2) {
		// 奇数
		return (m_table[p_index / 2] & 0xF0) >> 4;
	}
	else {
		// 偶数
		return (m_table[p_index / 2] & 0x0F) >> 0;
	}
}

// PruningTableにDepthを格納
void CPruningTable::SetValue(int p_index, unsigned int p_value) const
{
	int i = p_index / 2;
	// indexが奇数:Table[i]の前半を0でマスクする(Table[i] & ~(0xF0))
	//            :Table[i]の前半4ビットにdepthを格納
	//            :valueを4ビット右シフトする
	// indexが偶数:Table[i]の後半を0でマスクする(Table[i] & ~(0x0F))
	//            :Table[i]の後半4ビットにdepthを格納
	//            :valueをシフトしない
	if (p_index % 2) {
		// 奇数
		m_table[i] = (m_table[i] & 0x0F) | (p_value << 4);
	}
	else {
		// 偶数
		m_table[i] = (m_table[i] & 0xF0) | (p_value << 0);
	}
}

// PruningTableを標準出力で表示
void CPruningTable::PrintPruningTable() const
{
	for (int index = 0; index < m_tableSize; index++){
		std::cout << std::setw(7) << index << ": " << std::setw(2) << GetValue(index) << std::endl;
	}
}
