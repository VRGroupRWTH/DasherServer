#pragma once

#include <AlphInfo.h>
#include <rapidjson/document.h>

#include "Declarations.h"

class ServerFontMeasureService
{
public:
	ServerFontMeasureService();

	void SendAlphabet(std::shared_ptr<WebsocketServer> Server, websocketpp::connection_hdl Connection);
	void ParseAlphabetSizes(const rapidjson::Document& Doc);
	void ChangeAlphabet(const Dasher::CAlphInfo* Alphabet);
	std::pair<float, float> Measure(std::string Input, const int FontSize);

private:
	typedef struct CharacterInfo
	{
		std::string Text;
		std::string DisplayText;
		std::vector<float> Widths; //For all Font Sizes
	} CharacterInfo;

	std::vector<CharacterInfo> AllChars;
	std::vector<float> LineHeights;
	std::vector<int> FontSizes;
};