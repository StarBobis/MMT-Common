#pragma once
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <Windows.h>

#include <clocale>
#include <codecvt> 
#include <chrono>
#include <cmath>
#include <filesystem>
#include <map>
#include <unordered_map>

// algorithm
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/string.hpp>
// date_time
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/local_time/local_time_io.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

// property_tree
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "D3d11GameType.h"

#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "MMTFormatUtils.h"
#include "MMTJsonUtils.h"
#include "MMTLogUtils.h"

#include "FrameAnalysisData.h"
#include "FrameAnalysisLog.h"

#include "D3D11Texture.h"
#include "RunningConfig.h"


//��������ʱ�����е�����Ӧ��������ȫ����ʼ�����
class GlobalConfigs {
private:
	FrameAnalysisData FAData;
	FrameAnalysisLog FALog;

	//�����õ�������·�����������ȷźã�����ֱ����
	std::wstring Path_MainJson;
	std::wstring Path_Game_Folder;
	std::wstring Path_Game_3DmigotoFolder;
	std::wstring Path_Game_VSCheckJson;
	std::wstring Path_Game_ConfigJson;
	std::wstring Path_Game_SettingJson;
public:
	//��ȡ�������������ʹ��
	std::wstring Path_RunInputJson;

	//��ǰ����λ��
	std::wstring ApplicationRunningLocation = L"";

	//1.�ȶ�ȡ��Ϸ����
	std::wstring GameName;

	std::wstring FrameAnalyseFolder;
	std::wstring LoaderFolder;
	std::wstring OutputFolder;

	std::wstring ShaderCheckConfigLocation;

	std::wstring RunCommand;

	//�Ƿ���ҪVertexShaderCheck
	bool VertexShaderCheck = false;
	//��Ҫcheck��VertexShader��λ��һ��Ϊvb1,ib
	//std::vector<std::string> VertexShaderCheckList = { "vb1","ib" };
	std::vector<std::wstring> VertexShaderCheckList;
	std::wstring VertexShaderCheckListString;


	std::unordered_map<std::wstring, std::wstring> GIMIPartNameAliasMap;

	//����ʱ���ú���Ҫ�õ����ļ���
	std::wstring WorkFolder;

	//�������Merge��Split��Ԥ������
	std::unordered_map<std::wstring, ExtractConfig> DrawIB_ExtractConfig_Map;

	//ApplicationLocation,
	GlobalConfigs();

	GlobalConfigs(std::wstring);

	//��ǰ��Ϸ���͵�ElementAttribute�б�����ƥ���Զ�ʶ��GameType
	std::vector<D3D11GameType> CurrentD3d11GameTypeList;
	std::vector<std::string> Ordered_GPU_Object_D3D11GameTypeList;
	std::unordered_map<std::string, D3D11GameType> GameTypeName_D3d11GameType_Map;

	//��ǰ��Ϸ���͵���ͼ�����б�����ƥ���Զ�ʶ����ͼ����Ҫ���ö�Ӧ��Ϸ�ķ�������ʼ����
	std::vector<TextureType> TextureTypeList;

	FrameAnalysisData GetFrameAnalysisData();
	FrameAnalysisLog GetFrameAnalysisLog();
};

//ȫ������
extern GlobalConfigs G;




