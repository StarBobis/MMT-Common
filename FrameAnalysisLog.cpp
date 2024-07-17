#include "FrameAnalysisLog.h"

#include "MMTFileUtils.h"
#include "MMTLogUtils.h"
#include "MMTStringUtils.h"

FrameAnalysisLog::FrameAnalysisLog() {


}


FrameAnalysisLog::FrameAnalysisLog(std::wstring InputWorkFolder) {
	this->WorkFolder = InputWorkFolder;
	std::wstring LogTxtFilePath = InputWorkFolder + L"log.txt";
	if (!std::filesystem::exists(LogTxtFilePath)) {
		LOG.Error(L"�޷��ҵ�FrameAnalysis�ļ����е�log.txt: " + LogTxtFilePath);
	}
	std::vector<std::wstring> AllLineList = MMTFile_ReadAllLinesW(LogTxtFilePath);
	
	std::wstring CurrentIndex = L"";
	DrawCall CurrentDrawCall;
	for (std::wstring Line: AllLineList) {
		std::wstring LowerLine = boost::algorithm::to_lower_copy(Line);
		//���������0��ͷ�ģ�˵������Ҫ�ҵ���
		if (!LowerLine.starts_with(L"0")) {
			continue;
		}

		std::wstring Index = LowerLine.substr(0,6);
		if (Index != CurrentIndex) {
			//˵��������Index��������ǰDrawCall����Map������Ϊ�¶���
			if (CurrentIndex != L"") {
				this->Index_DrawCall_Map[CurrentIndex] = CurrentDrawCall;
			}
			CurrentDrawCall = DrawCall();
			CurrentIndex = Index;
		}

		//���substr 7������3dmigoto��ͷ��˵����dump�У����Էŵ�
		std::wstring FuncLine = LowerLine.substr(7);
		if (FuncLine.starts_with(L"3dmigoto")) {
			CurrentDrawCall.DumpCallStringList.push_back(Line);
		}
		else {
			CurrentDrawCall.APICallStringList.push_back(Line);
		}
	}

}


