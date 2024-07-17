#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class ExtractConfig {
public:

	//2.ͨ����Ŀ
	std::wstring DrawIB;
	std::wstring GameType;

	bool UseDefaultColor = false;

	//4.Split����
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

	//������Merge��ʱ�򱣴浽tmp.json��Ȼ��Split��ʱ���hash.json����ȡ
	std::unordered_map <std::string, std::string> CategoryHashMap;
	std::vector<std::string> MatchFirstIndexList;
	std::vector<std::string> PartNameList;
	std::vector<std::string> TmpElementList;
	std::string VertexLimitVB;
	std::string WorkGameType;

	std::unordered_map<std::string, std::vector<std::string>> PartNameTextureResourceReplaceList;

	void saveTmpConfigs(std::wstring outputPath);
};