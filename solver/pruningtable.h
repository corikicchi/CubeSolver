#ifndef	_PRUNINGTABLE_H_
#define	_PRUNINGTABLE_H_

#include "movetable.h"

#include <iostream>
#include <fstream>
#include <string>

// パターンデータベースのクラス
// ある状態遷移を行った時の2種類のMoveTableの序数をIndexとして初期状態からのコスト(Depth)を格納する
class CPruningTable
{
public:
	// 組み合わせる2つのMoveTableとcleanCubeにおける各序数を格納する
	CPruningTable(
		CMoveTable& moveTable1, CMoveTable& moveTable2,
		const int p_homeOrdinal1, const int p_homeOrdinal2
		);

	~CPruningTable();

	// 幅優先探索のためのPruningTableを作成
	void Initialize(const std::string p_fileName);

	// PruningTableのIndexからMoveTableの序数を取得
	void PruningTableIndexToMoveTableIndices(const int p_index, int& ordinal1, int& ordinal2) const;

	// MoveTableの序数からPruningTableのIndexを取得
	int MoveTableIndicesToPruningTableIndex(const int p_ordinal1, const int p_ordinal2) const;

	// PruningTableのDepthを取得
	unsigned int GetValue(const int p_index) const;

	// PruningTableにDepthを格納
	void SetValue(int p_index, unsigned int p_value) const;

	// PruningTableのサイズを取得
	int GetSize() const { return m_tableSize; }

	// PruningTableを標準出力で表示
	void PrintPruningTable() const;

private:
	// PruningTableのindex位置が空の時のdepth
	enum { Empty = 0x0F };

	// PruningTableを作成する
	void GeneratePruningTable();

	// MoveTableへのReference
	CMoveTable& m_moveTableRef1;
	CMoveTable& m_moveTableRef2;
	// PruningTable作成の初期位置
	int m_homeOrdinal1;
	int m_homeOrdinal2;
	// 各MoveTableのサイズ
	int m_moveTable1Size;
	int m_moveTable2Size;

	// PruningTableのサイズ
	int m_tableSize;
	// メモリ上のPruningTableのサイズ(byte)
	int m_allocationSize;
	// PrunignTable
	unsigned char *m_table;
};

#endif	// _PRUNINGTABLE_H_
