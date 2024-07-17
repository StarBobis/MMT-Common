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
		LOG.Error(L"无法找到FrameAnalysis文件夹中的log.txt: " + LogTxtFilePath);
	}
	std::vector<std::wstring> AllLineList = MMTFile_ReadAllLinesW(LogTxtFilePath);
	
	std::wstring CurrentIndex = L"";
	DrawCall CurrentDrawCall;
	for (std::wstring Line: AllLineList) {
		std::wstring LowerLine = boost::algorithm::to_lower_copy(Line);
		//如果不是以0开头的，说明不是要找的行
		if (!LowerLine.starts_with(L"0")) {
			continue;
		}

		std::wstring Index = LowerLine.substr(0,6);
		if (Index != CurrentIndex) {
			//说明发生了Index更换，当前DrawCall放入Map并重置为新对象
			if (CurrentIndex != L"") {
				this->Index_DrawCall_Map[CurrentIndex] = CurrentDrawCall;
			}
			CurrentDrawCall = DrawCall();
			CurrentIndex = Index;
		}

		//如果substr 7的是以3dmigoto开头，说明是dump行，可以放到
		std::wstring FuncLine = LowerLine.substr(7);
		if (FuncLine.starts_with(L"3dmigoto")) {
			CurrentDrawCall.DumpCallStringList.push_back(Line);
		}
		else {
			CurrentDrawCall.APICallStringList.push_back(Line);
		}
	}

}


