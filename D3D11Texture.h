#pragma once
#include <string>
#include <vector>
#include <unordered_map>


class TextureType {
public:
	std::unordered_map<std::string, std::string> PixelSlot_TextureType_Map;

	//过滤机制：根据DiffuseMap的格式来过滤
	std::wstring DiffuseFormat = L"BC7_UNORM";
	
	//过滤机制，有些游戏必须要jpg才准确比如ZZZ，有些游戏不能使用jpg才准确比如WW
	//所以默认为true，如有需要单独设为false后不再处理jpg格式。
	bool DetectJpg = true;

	TextureType();

	void SetDiffuse_BC7_UNORM_SRGB();
};

