#ifndef	_MOVETABLE_H_
#define	_MOVETABLE_H_

#include "cube.h"

#include <string>
#include <QObject>

// CMoveTable
// (状態Sに対して6種類の移動を行ったときの状態S'を列挙するTableを作成するための基底クラス)
class CMoveTable : public QObject
{
    Q_OBJECT
signals:
    void notifySolverMessage(QString p_message);

public:
	// CubeへのReference,MoveTableのサイズ,Phase2かどうかを設定してメモリを確保
	// CubeはReferenceなのでMoveTableのメンバ関数によって操作される
	CMoveTable(CCube& cube, const int p_tableSize, const bool p_isPhase2);
	// MoveTableを解放
	virtual ~CMoveTable();

	// MoveTableを読み込む
	// MoveTableが無ければ作成する
	virtual void Initialize(const std::string p_fileName);

	// privateなTableに対して，オブジェクトの添え字でアクセスするための演算子
	virtual int* operator[](const int p_index);
	
	// MoveTableを出力する
	virtual void PrintMoveTable() const;

	// Tableのサイズ(int単位)を取得
	virtual int GetSize() const { return m_tableSize; }

protected:
	// 継承したクラスで実体を作成する
	virtual int GetOrdinalFromCubeState() const = 0;
	virtual void SetCubeStateFromOrdinal(const int p_ordinal) = 0;

private:
	// MoveTableを作成する
	void GenerateMoveTable();

	// Cubeへのreference
	// 処理の高速化のためにReferenceを用いる
	CCube &m_cubeRef;

	// MoveTableのサイズ
	int m_tableSize;
	// Phase1:false, Phase2:true
	bool m_isPhase2;

	// 配列 int[6] へのポインタ
	// int m_table[m_tableSize][6]を確保して使用する
	// m_table[ordinal][move]は序数ordinalの状態のときにmoveの状態遷移をした後の序数を表す
	// http://www.nurs.or.jp/~sug/soft/tora/tora10.htm
	// http://d.hatena.ne.jp/Guernsey/20090924/1253775843
	int (*m_table)[CCube::Move::NumberOfClockwiseQuarterTurnMoves];
};

#endif	// _MOVETABLE_H_
