#include "GlobalConfigs.h"
#include "FrameAnalysisData.h"


//用这个可以确保全局唯一FAData
FrameAnalysisData GlobalConfigs::GetFrameAnalysisData() {
    //如果为空说明没初始化,不然直接返回已经初始化好的
    if (this->FAData.WorkFolder == L"") {
        FrameAnalysisData tmpFAData(this->WorkFolder);
        this->FAData = tmpFAData;
    }
    return this->FAData;
}


FrameAnalysisLog GlobalConfigs::GetFrameAnalysisLog() {
    if (this->FALog.WorkFolder == L"") {
        FrameAnalysisLog tmpFALog(this->WorkFolder);
        this->FALog = tmpFALog;
    }
    return this->FALog;
}


//需提供程序运行的路径
GlobalConfigs::GlobalConfigs(std::wstring ApplicationLocation) {
    //初始化GIMI格式需要的碎片名称
    this->GIMIPartNameAliasMap[L"1"] = L"Head";
    this->GIMIPartNameAliasMap[L"2"] = L"Body";
    this->GIMIPartNameAliasMap[L"3"] = L"Dress";
    this->GIMIPartNameAliasMap[L"4"] = L"Extra";
    this->GIMIPartNameAliasMap[L"5"] = L"Extra1";
    this->GIMIPartNameAliasMap[L"6"] = L"Extra2";
    this->GIMIPartNameAliasMap[L"7"] = L"Extra3";
    this->GIMIPartNameAliasMap[L"8"] = L"Extra4";
    this->GIMIPartNameAliasMap[L"9"] = L"Extra5";
    this->GIMIPartNameAliasMap[L"10"] = L"Extra6";
    this->GIMIPartNameAliasMap[L"12"] = L"Extra7";
    this->GIMIPartNameAliasMap[L"13"] = L"Extra8";
    this->GIMIPartNameAliasMap[L"14"] = L"Extra9";
    this->GIMIPartNameAliasMap[L"15"] = L"Extra10";
    this->GIMIPartNameAliasMap[L"16"] = L"Extra11";
    this->GIMIPartNameAliasMap[L"17"] = L"Extra12";
    this->GIMIPartNameAliasMap[L"18"] = L"Extra13";

    //初始化运行路径
    this->ApplicationRunningLocation = ApplicationLocation;
    LOG.Info(L"ApplicationLocation: " + ApplicationLocation);
    LOG.Info(L"Start to initialize global config:");

    //初始化配置路径
    this->Path_MainJson = this->ApplicationRunningLocation + L"\\Configs\\Main.json";

    //读取当前工作的游戏名称
    LOG.Info(L"Start to read from Main.json:");
    LOG.Info(L"Main.json Path:" + this->Path_MainJson);
    nlohmann::json MainJsonData = MMTJson_ReadJsonFromFile(this->Path_MainJson);
    this->GameName = MMTString_ToWideString(MainJsonData["GameName"]);
    LOG.Info(L"GameName:" + GameName);
    LOG.NewLine();

    //根据游戏名称，计算出Games目录下的具体游戏目录
    this->Path_Game_Folder = this->ApplicationRunningLocation + L"\\Games\\" + GameName + L"\\";
    this->Path_Game_3DmigotoFolder = Path_Game_Folder + L"3Dmigoto\\";
    this->Path_Game_VSCheckJson = Path_Game_Folder + L"VSCheck.json";
    this->Path_Game_ConfigJson = Path_Game_Folder + L"Config.json";
    this->Path_Game_SettingJson = Path_Game_Folder + L"Setting.json";


    //文件夹相关设置
    this->OutputFolder = this->Path_Game_3DmigotoFolder + L"Mods\\output\\";
    LOG.Info(L"OutputFolder:" + OutputFolder);
    std::filesystem::create_directories(OutputFolder);

    this->LoaderFolder = this->Path_Game_3DmigotoFolder;
    LOG.Info(L"LoaderFolder:" + LoaderFolder);
    
    std::wstring latestFrameAnalyseFolder = MMTFile_FindMaxPrefixedDirectory(LoaderFolder, L"FrameAnalysis-");
    if (latestFrameAnalyseFolder == L"") {
        LOG.Warning(L"Can't find any FrameAnalysisFolder.");
    }
    this->FrameAnalyseFolder = latestFrameAnalyseFolder;
    LOG.Info(L"FrameAnalyseFolder:" + FrameAnalyseFolder);

    this->WorkFolder = LoaderFolder + FrameAnalyseFolder + L"\\";
    LOG.Info(L"WorkFolder:" + WorkFolder);
    LOG.NewLine();

    //读取当前输入的运行命令
    this->Path_RunInputJson = this->ApplicationRunningLocation + L"\\Configs\\RunInput.json";
    nlohmann::json runInputJson = MMTJson_ReadJsonFromFile(this->Path_RunInputJson);
    std::string runCommand = runInputJson["RunCommand"];
    this->RunCommand = MMTString_ToWideString(runCommand);

    //TODO 这里的读取需要修改，实在不行就先屏蔽掉。
    if (std::filesystem::exists(this->Path_Game_ConfigJson)) {
        //根据游戏类型，读取configs目录下对应的配置文件
        nlohmann::json configJsonData = MMTJson_ReadJsonFromFile(this->Path_Game_ConfigJson);
        for (const auto& obj : configJsonData) {
            ExtractConfig config;
            //先把DrawIB读取出来
            config.DrawIB = MMTString_ToWideString(obj["DrawIB"]);
            LOG.Info(L"Start to read config for DrawIB:" + config.DrawIB);


            //Merge的4个主配置
            //config.UseDefaultColor = obj["UseDefaultColor"];
            //LOG.LogOutput(L"UseDefaultColor:" + std::to_wstring(config.UseDefaultColor));
            
            config.GameType = MMTString_ToWideString(obj["GameType"]);
            LOG.Info(L"GameType:" + config.GameType);

            config.TangentAlgorithm = MMTString_ToWideString(obj["TangentAlgorithm"]);
            LOG.Info(L"TangentAlgorithm:" + config.TangentAlgorithm);

            config.ColorAlgorithm = MMTString_ToWideString(obj["ColorAlgorithm"]);
            LOG.Info(L"ColorAlgorithm:" + config.ColorAlgorithm);

            //color map
            config.ColorMap = obj["Color"];
            LOG.Info(L"ColorMap:");
            for (const auto& pair : config.ColorMap) {
                LOG.Info(L"Color: " + MMTString_ToWideString(pair.first) + L"  value: " + MMTString_ToWideString(pair.second));
            }

            //Reverse Normal and Tangent
            config.NormalReverseX = obj["NormalReverseX"];
            config.NormalReverseY = obj["NormalReverseY"];
            config.NormalReverseZ = obj["NormalReverseZ"];

            config.TangentReverseX = obj["TangentReverseX"];
            config.TangentReverseY = obj["TangentReverseY"];
            config.TangentReverseZ = obj["TangentReverseZ"];
            config.TangentReverseW = obj["TangentReverseW"];
            LOG.NewLine();

            //接下来判断是否存在tmpConfig,只有在存在的情况下才会进行读取，无需手工指定
            std::wstring tmpConfigPath = OutputFolder + config.DrawIB + L"\\tmp.json";
            LOG.Info(tmpConfigPath);
            if (std::filesystem::exists(tmpConfigPath)) {
                nlohmann::json tmpJsonData = MMTJson_ReadJsonFromFile(tmpConfigPath);

                //Read the config Merge generated.

                LOG.Info(L"Start to read tmp config.");
                config.VertexLimitVB = tmpJsonData["VertexLimitVB"];
                LOG.Info(L"VertexLimitVB:" + MMTString_ToWideString(config.VertexLimitVB));

                config.WorkGameType = tmpJsonData["WorkGameType"];
                LOG.Info(L"WorkGameType:" + MMTString_ToWideString(config.WorkGameType));

                config.TmpElementList = tmpJsonData["TmpElementList"];
                LOG.Info(L"TmpElementList:");
                for (const auto& pair : config.TmpElementList) {
                    LOG.Info(MMTString_ToWideString(pair));
                }

                config.CategoryHashMap = tmpJsonData["CategoryHash"];
                LOG.Info(L"CategoryHash:");
                for (const auto& pair : config.CategoryHashMap) {
                    LOG.Info(L"Category: " + MMTString_ToWideString(pair.first) + L"  Hash: " + MMTString_ToWideString(pair.second));
                }
                config.MatchFirstIndexList = tmpJsonData["MatchFirstIndex"];
                LOG.Info(L"MatchFirstIndex:");
                for (const auto& pair : config.MatchFirstIndexList) {
                    LOG.Info(MMTString_ToWideString(pair));
                }
                config.PartNameList = tmpJsonData["PartNameList"];
                LOG.Info(L"PartNameList:");
                for (const auto& pair : config.PartNameList) {
                    LOG.Info(MMTString_ToWideString(pair));
                }

                config.PartNameTextureResourceReplaceList = tmpJsonData["PartNameTextureResourceReplaceList"];

                LOG.NewLine();
            }

            DrawIB_ExtractConfig_Map[config.DrawIB] = config;
        }
        LOG.Info(L"Read config for every DrawIB success.");
        LOG.NewLine();


    }
    else {
        LOG.Info(L"Can't find config file: " + this->Path_Game_ConfigJson);
    }

}




//必须提供无参构造，这样才能声明全局变量并在后续进行初始化。
GlobalConfigs::GlobalConfigs() {
    
}





