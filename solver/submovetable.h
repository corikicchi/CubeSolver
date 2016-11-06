#include "movetable.h"
#include "ordinalcube.h"

class CTwistMoveTable : public CMoveTable
{
public:
	CTwistMoveTable(COrdinalCube& cube)
		: CMoveTable(cube, COrdinalCube::Twists, false),TheCube(cube)
	{

	}
private:
	inline int GetOrdinalFromCubeState() const
		{ return TheCube.GetTwistFromOrientations(); }
	inline void SetCubeStateFromOrdinal(const int p_ordinal)
		{ TheCube.SetOrientationsFromTwist(p_ordinal); }
	COrdinalCube& TheCube;
};

class CFlipMoveTable : public CMoveTable
{
public:
	CFlipMoveTable(COrdinalCube& cube)
		: CMoveTable(cube, COrdinalCube::Flips, false),TheCube(cube)
	{

	}
private:
	inline int GetOrdinalFromCubeState() const
		{ return TheCube.GetFlipFromOrientations(); }
	inline void SetCubeStateFromOrdinal(const int p_ordinal)
		{ TheCube.SetOrientationsFromFlip(p_ordinal); }
	COrdinalCube& TheCube;
};

class CChoiceMoveTable : public CMoveTable
{
public:
	CChoiceMoveTable(COrdinalCube& cube)
		: CMoveTable(cube, COrdinalCube::Choices, false), TheCube(cube)
	{
	
	}
private:
	inline int GetOrdinalFromCubeState() const
		{ return TheCube.GetChoiceFromEdgePermutation(); }
	inline void SetCubeStateFromOrdinal(const int p_ordinal)
		{ TheCube.SetChoiceFromEdgePermutation(p_ordinal); }
	COrdinalCube& TheCube;
};

class CCornerPermutationMoveTable : public CMoveTable
{
public:
	CCornerPermutationMoveTable(COrdinalCube& cube)
		: CMoveTable(cube, COrdinalCube::CornerPermutations, true),TheCube(cube)
	{

	}
private:
	inline int GetOrdinalFromCubeState() const
		{ return TheCube.GetOrdinalFromCornerPermutation(); }
	inline void SetCubeStateFromOrdinal(const int p_ordinal)
		{ TheCube.SetCornerPermutationFromOrdinal(p_ordinal); }
	COrdinalCube& TheCube;
};

class CUpDownEdgePermutationMoveTable : public CMoveTable
{
public:
	CUpDownEdgePermutationMoveTable(COrdinalCube& cube)
		: CMoveTable(cube, COrdinalCube::UpDownEdgePermutations, true),TheCube(cube)
	{

	}
private:
	inline int GetOrdinalFromCubeState() const
		{ return TheCube.GetOrdinalFromUpDownEdgePermutation(); }
	inline void SetCubeStateFromOrdinal(const int p_ordinal)
		{ TheCube.SetUpDownEdgePermutationFromOrdinal(p_ordinal); }
	COrdinalCube& TheCube;
};

class CMiddleEdgePermutationMoveTable : public CMoveTable
{
public:
	CMiddleEdgePermutationMoveTable(COrdinalCube& cube)
		: CMoveTable(cube, COrdinalCube::MiddleEdgePermutations, true),TheCube(cube)
	{
	
	}
private:
	inline int GetOrdinalFromCubeState() const
		{ return TheCube.GetOrdinalFromMiddleEdgePermutation(); }
	inline void SetCubeStateFromOrdinal(const int p_ordinal)
		{ TheCube.SetMiddleEdgePermutationFromOrdinal(p_ordinal); }
	COrdinalCube& TheCube;
};
