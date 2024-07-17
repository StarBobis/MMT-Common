#include "RunningConfig.h"

#include "MMTLogUtils.h"
#include "MMTJsonUtils.h"
#include "MMTFileUtils.h"

//接收outputFolder，并保存到outputfolder下面的DrawIB为名称的文件夹里
void ExtractConfig::saveTmpConfigs(std::wstring outputPath) {
    LOG.Info(L"Start to save tmp config");

    //write to json file.
    nlohmann::json tmpJsonData;
    tmpJsonData["VertexLimitVB"] = VertexLimitVB;
    tmpJsonData["TmpElementList"] = TmpElementList;
    tmpJsonData["CategoryHash"] = CategoryHashMap;
    tmpJsonData["MatchFirstIndex"] = MatchFirstIndexList;
    tmpJsonData["PartNameList"] = PartNameList;
    tmpJsonData["WorkGameType"] = WorkGameType;
    tmpJsonData["PartNameTextureResourceReplaceList"] = PartNameTextureResourceReplaceList;

    std::wstring configCompletePath = outputPath + L"tmp.json";
    LOG.Info(L"configCompletePath: " + configCompletePath);
    // save json config into file.
    // 这里如果文件不存在，则会自动创建新的文件来写入，所以不用担心。
    if (std::filesystem::exists(configCompletePath)) {
        std::filesystem::remove(configCompletePath);
    }
    std::ofstream file(configCompletePath);
    std::string content = tmpJsonData.dump(4); // 使用缩进格式保存到文件，缩进为4个空格
    file << content << std::endl;
    file.close();
    LOG.Info(L"Json config has saved into file.");

    LOG.NewLine();
}
