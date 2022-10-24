#include "ServerFontMeasure.h"

#include <AlphInfo.h>
#include <algorithm>
#include <rapidjson/document.h>

void ServerFontMeasureService::SendAlphabet(std::shared_ptr<WebsocketServer> Server, websocketpp::connection_hdl Connection)
{
	rapidjson::StringBuffer MsgBuffer;
	rapidjson::Writer<rapidjson::StringBuffer> Writer(MsgBuffer);

	/*	Serializes into this format:
	 * {
	 *	"T" : "A",
	 *	"C" : ["<DisplayText_0>", ...]
	 *	"S" : [<font_size_0>, ...]
	 * }
	 */

	Writer.StartObject();
	Writer.Key("T");
	Writer.String("A");
	Writer.Key("C");
	Writer.StartArray();
		for(unsigned int i = 0; i < AllChars.size(); i++)
		{
			Writer.String(AllChars[i].DisplayText.c_str(), static_cast<rapidjson::SizeType>(AllChars[i].DisplayText.length()));
		}
	Writer.EndArray();
	Writer.Key("S");
	Writer.StartArray();
	for (unsigned int i = 0; i < FontSizes.size(); i++)
	{
		Writer.Int(FontSizes[i]);
	}
	Writer.EndArray();
	Writer.EndObject();

	//Send "Frame" to Client
	Server->send(Connection, MsgBuffer.GetString(), MsgBuffer.GetSize(), websocketpp::frame::opcode::TEXT);
}

ServerFontMeasureService::ServerFontMeasureService()
{
		
}

void ServerFontMeasureService::ParseAlphabetSizes(const rapidjson::Document& Doc)
{
	/*	Parses from this format (answer for SendAlphabet) :
	 * {
	 *	"T" : "S",
	 *	"C" : [
	 *		{
	 *			"D" : "<DisplayText_0>",
	 *			"W" : [<width_for_font_size_0>, ...]
	 *		},
	 *		...
	 *	],
	 *	"H" : [<line_height_for_font_size_0>,...]
	 * }
	 */

	const rapidjson::GenericArray<true, rapidjson::GenericValue<rapidjson::UTF8<>>>& Alphabet = Doc["C"].GetArray();
	for(auto i = Alphabet.Begin(); i != Alphabet.End(); ++i)
	{
		if(!i->GetObj()["D"].IsString() || !i->GetObj()["W"].IsArray()) continue;

		for(auto j = AllChars.begin(); j != AllChars.end(); ++j)
		{
			if(j->DisplayText == i->GetObj()["D"].GetString())
			{
				const rapidjson::GenericValue<rapidjson::UTF8<>>& Widths = i->GetObj()["W"];
				for(auto f = Widths.Begin(); f != Widths.End(); ++f)
				{
					j->Widths.push_back(f->GetFloat());
				}
				break;
			}
		}
	}

	const rapidjson::GenericValue<rapidjson::UTF8<>>& LH = Doc["H"].GetArray();
	for (auto l = LH.Begin(); l != LH.End(); ++l)
	{
		LineHeights.push_back(l->GetFloat());
	}
}

void ServerFontMeasureService::ChangeAlphabet(const Dasher::CAlphInfo* Alphabet)
{
	AllChars.clear();
	for (int i = Alphabet->iStart; i < Alphabet->iEnd; i++)
	{
		AllChars.push_back({ Alphabet->GetText(i), Alphabet->GetDisplayText(i), {} });
	}

	for(int i = 0; i <= 50; i++)
	{
		FontSizes.push_back(i);
	}
}

std::pair<int, int> ServerFontMeasureService::Measure(std::string Input, const int FontSize)
{
	if (Input.length() == 0 || LineHeights.size() == 0) return {0,0};

	for (auto i = AllChars.begin(); i != AllChars.end(); ++i)
	{
		if(Input.rfind(i->DisplayText, 0) == 0)
		{
			const std::pair<int, int> RestSize = Measure(Input.substr(i->DisplayText.length()), FontSize);
			if(RestSize.first >= 0 && RestSize.second >= 0)
			{
				const int FontSizeIndex = static_cast<int>(std::distance(FontSizes.begin(), std::find(FontSizes.begin(), FontSizes.end(), FontSize)));
				return { static_cast<int>(i->Widths[FontSizeIndex] + static_cast<float>(RestSize.first)), static_cast<int>(LineHeights[FontSizeIndex])};
			}
		}
	}
	return { -1, -1 }; //String cant be build from known chars
}
