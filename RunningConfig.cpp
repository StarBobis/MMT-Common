#include "RunningConfig.h"

#include "MMTLogUtils.h"
#include "MMTJsonUtils.h"
#include "MMTFileUtils.h"

//����outputFolder�������浽outputfolder�����DrawIBΪ���Ƶ��ļ�����
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
    // ��������ļ������ڣ�����Զ������µ��ļ���д�룬���Բ��õ��ġ�
    if (std::filesystem::exists(configCompletePath)) {
        std::filesystem::remove(configCompletePath);
    }
    std::ofstream file(configCompletePath);
    std::string content = tmpJsonData.dump(4); // ʹ��������ʽ���浽�ļ�������Ϊ4���ո�
    file << content << std::endl;
    file.close();
    LOG.Info(L"Json config has saved into file.");

    LOG.NewLine();
}
