#include "cubeparser.h"

#include <cstring>
#include <sstream>

CCubeParser::CCubeParser()
{
}

CCubeParser::~CCubeParser()
{
}

// 入力データをparseしてGroupCubeクラスに格納する
// 6面それぞれの9色を設定する
CCubeParser::InputError CCubeParser::ParseInput(const std::string p_inputData, CGroupCube& groupCube)
{
	std::vector<int> facesNum(6, 0);	// UDLRFBそれぞれの面の数(1つずつあればOK)
	InputError status;

	// 文字列を' 'で分割する
	std::vector<std::string> inputDatas;
	std::stringstream ss(p_inputData);
	std::string buffer;
	while (std::getline(ss, buffer, ' ')) {
		inputDatas.push_back(buffer);
	}

	// 6面のデータがそろっていない時は文法ミス
	if (inputDatas.size() != 6) return SYNTAX_ERROR;

	// それぞれの文字列をparseする
	for (int i = 0; i < 6; i++) {
		// Faceの名前(U=0,D=1, ...)を取得 -> faceへ格納
		int face;
		if ((status = checkInput(inputDatas.at(i), face)) != VALID) {
			return status;
		}

		// face面のcolor x 9を格納する
		groupCube.SetFaceColors(face, inputDatas.at(i));

		facesNum[face]++;
	}

	// 面は必ず1つずつ存在する
	for (int i = 0; i < 6; i++){
		if (facesNum[i] != 1) {
			return INCOMPLETE_INPUT;
		}
	}

	return VALID;
}

// エラーコードをテキスト変換する
std::string CCubeParser::GetErrorText(const InputError p_error)
{
	if (p_error >= NumberOfErrors) {
		return errorText[0];
	}
	return errorText[p_error];
}

// コマンド文法のエラーをチェック
// 1面分のコマンド"f:cccccccc"を渡す(faceString)
// faceにはどこのFaceか(U=0,D=1, ...)が格納される
CCubeParser::InputError CCubeParser::checkInput(const std::string p_faceString, int& face)
{
	// コマンドの長さを確認
	// f:cccccccc で11文字
	if (p_faceString.length() != 1 + 1 + 9) {
		return SYNTAX_ERROR;
	}

	// 面の名前を確認
	std::string faceNames = "UDLRFB";
	char *name = NULL;
	// faceStringの0文字目は面の名前(UDLRFB)
	// faceNamesの何文字目かを検索し，nameにアドレスを格納
	if (!(name = (char*)std::strchr(faceNames.c_str(), p_faceString.c_str()[0]))) {
		return INVALID_FACENAME;
	}
	// アドレスの差分
	face = (int)(name - faceNames.c_str());

	// ':'を確認
	if (p_faceString.c_str()[1] != ':') {
		return SYNTAX_ERROR;
	}

	// 面を確認
	for (int i = 0; i < 9; i++) {
		// 制御文字はダメ
		if (!isprint(p_faceString.c_str()[2 + i])) {
			return SYNTAX_ERROR;
		}
	}

	return VALID;
}

// エラーテキスト
const std::string CCubeParser::errorText[NumberOfErrors] =
{
	"",
	"Face name (UDLRFB) was invalid",
	"Input was incomplete",
	"Syntax error"
};
