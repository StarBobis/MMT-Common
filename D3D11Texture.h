#pragma once
#include <string>
#include <vector>
#include <unordered_map>


class TextureType {
public:
	std::unordered_map<std::string, std::string> PixelSlot_TextureType_Map;

	//���˻��ƣ�����DiffuseMap�ĸ�ʽ������
	std::wstring DiffuseFormat = L"BC7_UNORM";
	
	//���˻��ƣ���Щ��Ϸ����Ҫjpg��׼ȷ����ZZZ����Щ��Ϸ����ʹ��jpg��׼ȷ����WW
	//����Ĭ��Ϊtrue��������Ҫ������Ϊfalse���ٴ���jpg��ʽ��
	bool DetectJpg = true;

	TextureType();

	void SetDiffuse_BC7_UNORM_SRGB();
};

