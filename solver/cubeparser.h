#ifndef	_CUBEPARSER_H_
#define	_CUBEPARSER_H_

#include "groupcube.h"

#include <string>
#include <vector>

// 入力データをparseするクラス
class CCubeParser
{
public:
	CCubeParser();
	~CCubeParser();

	// エラーコード
	enum InputError 
	{
		VALID,
		INVALID_FACENAME,
		INCOMPLETE_INPUT,
		SYNTAX_ERROR,
		NumberOfErrors
	};

	// 入力データをparseしてGroupCubeクラスに格納する
	// 6面それぞれの9色を設定する
	static InputError ParseInput(const std::string p_inputData, CGroupCube& grouptCube);

	// エラーコードをテキスト変換する
	static std::string GetErrorText(const InputError p_error);

private:
	// コマンド文法のエラーをチェック
	// 1面分のコマンド"f:cccccccc"を渡す(faceString)
	// faceにはどこのFaceか(U=0,D=1, ...)が格納される
	static CCubeParser::InputError checkInput(const std::string p_faceString, int& face);

	// エラーテキスト
	static const std::string errorText[NumberOfErrors];
};

#endif // _CUBEPARSER_H_
