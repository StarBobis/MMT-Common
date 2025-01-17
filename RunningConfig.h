#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class ExtractConfig {
public:

	//2.通用项目
	std::wstring DrawIB;
	std::wstring GameType;

	bool UseDefaultColor = false;

	//4.Split配置
	std::unordered_map <std::string, std::string> ColorMap;
	std::wstring TangentAlgorithm;
	std::wstring ColorAlgorithm;


	bool NormalReverseX = false;
	bool NormalReverseY = false;
	bool NormalReverseZ = false;

	bool TangentReverseX = false;
	bool TangentReverseY = false;
	bool TangentReverseZ = false;
	bool TangentReverseW = false;

	//这玩意Merge的时候保存到tmp.json，然后Split的时候从hash.json中提取
	std::unordered_map <std::string, std::string> CategoryHashMap;
	std::vector<std::string> MatchFirstIndexList;
	std::vector<std::string> PartNameList;
	std::vector<std::string> TmpElementList;
	std::string VertexLimitVB;
	std::string WorkGameType;

	std::unordered_map<std::string, std::vector<std::string>> PartNameTextureResourceReplaceList;

	void saveTmpConfigs(std::wstring outputPath);
};