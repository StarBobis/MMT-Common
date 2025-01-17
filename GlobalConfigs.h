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


//工具运行时，所有的配置应该在这里全部初始化完成
class GlobalConfigs {
private:
	FrameAnalysisData FAData;
	FrameAnalysisLog FALog;

	//工具用到的所有路径都在这里先放好，后面直接用
	std::wstring Path_MainJson;
	std::wstring Path_Game_Folder;
	std::wstring Path_Game_3DmigotoFolder;
	std::wstring Path_Game_VSCheckJson;
	std::wstring Path_Game_ConfigJson;
	std::wstring Path_Game_SettingJson;
public:
	//读取程序的输入命令使用
	std::wstring Path_RunInputJson;

	//当前运行位置
	std::wstring ApplicationRunningLocation = L"";

	//1.先读取游戏名称
	std::wstring GameName;

	std::wstring FrameAnalyseFolder;
	std::wstring LoaderFolder;
	std::wstring OutputFolder;

	std::wstring ShaderCheckConfigLocation;

	std::wstring RunCommand;

	//是否需要VertexShaderCheck
	bool VertexShaderCheck = false;
	//需要check的VertexShader槽位，一般为vb1,ib
	//std::vector<std::string> VertexShaderCheckList = { "vb1","ib" };
	std::vector<std::wstring> VertexShaderCheckList;
	std::wstring VertexShaderCheckListString;


	std::unordered_map<std::wstring, std::wstring> GIMIPartNameAliasMap;

	//运行时设置好需要用到的文件夹
	std::wstring WorkFolder;

	//用来存放Merge和Split的预设配置
	std::unordered_map<std::wstring, ExtractConfig> DrawIB_ExtractConfig_Map;

	//ApplicationLocation,
	GlobalConfigs();

	GlobalConfigs(std::wstring);

	//当前游戏类型的ElementAttribute列表，用于匹配自动识别GameType
	std::vector<D3D11GameType> CurrentD3d11GameTypeList;
	std::vector<std::string> Ordered_GPU_Object_D3D11GameTypeList;
	std::unordered_map<std::string, D3D11GameType> GameTypeName_D3d11GameType_Map;

	//当前游戏类型的贴图配置列表，用于匹配自动识别贴图，需要调用对应游戏的方法来初始化。
	std::vector<TextureType> TextureTypeList;

	FrameAnalysisData GetFrameAnalysisData();
	FrameAnalysisLog GetFrameAnalysisLog();
};

//全局配置
extern GlobalConfigs G;




