#pragma once

#pragma warning(push)
#pragma warning(disable:4267)
#include <rapidjson/writer.h>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#pragma warning(pop)

#include <DasherScreen.h>

class DasherController;

typedef websocketpp::server<websocketpp::config::asio> WebsocketServer;
typedef std::map<websocketpp::connection_hdl, std::shared_ptr<DasherController>, std::owner_less<websocketpp::connection_hdl>> Connection_Map;


typedef struct StringToDraw
{
	Dasher::CDasherScreen::Label* Label;
	Dasher::screenint X;
	Dasher::screenint Y;
	unsigned int iFontSize;
	int iColor; /* Index into color map */

	/*	Serializes into this format:
	 * {
	 *	"L" : {
	 *		"S" : "<Label->m_strText>",
	 *		"W" : <Label->m_iWrapSize>
	 *	},
	 *	"X" : <X>,
	 *	"Y" : <Y>,
	 *	"F" : <iFontSize>,
	 *	"C" : <iColor>,
	 * }
	 */
	static void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& Writer, const struct StringToDraw& S) {
		Writer.StartObject();
			Writer.Key("L");
			Writer.StartObject();
				Writer.Key("S");
				Writer.String(S.Label->m_strText.c_str(), static_cast<rapidjson::SizeType>(S.Label->m_strText.length()));
				Writer.Key("W");
				Writer.Int(S.Label->m_iWrapSize);
			Writer.EndObject();
			Writer.Key("X");
			Writer.Int(S.X);
			Writer.Key("Y");
			Writer.Int(S.Y);
			Writer.Key("F");
			Writer.Uint(S.iFontSize);
			Writer.Key("C");
			Writer.Int(S.iColor);
		Writer.EndObject();
	}

} StringToDraw;

typedef struct RectangleToDraw
{
	Dasher::screenint X1;
	Dasher::screenint Y1;
	Dasher::screenint X2;
	Dasher::screenint Y2;
	int iColor; /* Index into color map */
	int iOutlineColor; /* Index into color map */
	int iThickness;

	/*	Serializes into this format:
	 * {
	 *	"X1" : <X1>,
	 *	"Y1" : <Y1>,
	 *	"X2" : <X2>,
	 *	"Y2" : <Y2>,
	 *	"C" : <iColor>,
	 *	"O" : <iOutlineColor>, //optional
	 *	"T" : <iThickness> //optional
	 * }
	 */
	static void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& Writer, const struct RectangleToDraw& S) {
		Writer.StartObject();
			Writer.Key("X1");
			Writer.Int(S.X1);
			Writer.Key("Y1");
			Writer.Int(S.Y1);
			Writer.Key("X2");
			Writer.Int(S.X2);
			Writer.Key("Y2");
			Writer.Int(S.Y2);
			Writer.Key("C");
			Writer.Int(S.iColor);
			if(S.iThickness > 0 && S.iOutlineColor >= 0)
			{
				Writer.Key("O");
				Writer.Int(S.iOutlineColor);
				Writer.Key("T");
				Writer.Int(S.iThickness);
			}
			
		Writer.EndObject();
	}
} RectangleToDraw;

typedef struct CircleToDraw
{
	Dasher::screenint iCX;
	Dasher::screenint iCY;
	Dasher::screenint iR;
	int iFillColor;
	int iLineColor;
	int iLineWidth;

	/*	Serializes into this format:
	 * {
	 *	"X" : <iCX>,
	 *	"Y" : <iCY>,
	 *	"R" : <iR>,
	 *	"F" : <iFillColor>,
	 *	"L" : <iLineColor>, //optional
	 *	"W" : <iLineWidth> //optional
	 * }
	 */
	static void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& Writer, const struct CircleToDraw& S) {
		Writer.StartObject();
			Writer.Key("X");
			Writer.Int(S.iCX);
			Writer.Key("Y");
			Writer.Int(S.iCY);
			Writer.Key("R");
			Writer.Int(S.iR);
			Writer.Key("F");
			Writer.Int(S.iFillColor);
			if(S.iLineWidth > 0 && S.iLineColor >= 0)
			{
				Writer.Key("L");
				Writer.Int(S.iLineColor);
				Writer.Key("W");
				Writer.Int(S.iLineWidth);
			}
		Writer.EndObject();
	}
} CircleToDraw;

typedef struct PolylineToDraw
{
	std::vector<Dasher::CDasherScreen::point> Points;
	int iWidth;
	int iColor;

	/*	Serializes into this format:
	 * {
	 *	"P" : [[<X_0>,<Y_0>],...,[<X_Number>,<Y_Number>]],
	 *	"W" : <iWidth>,
	 *	"C" : <iColor>
	 * }
	 */
	static void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& Writer, const struct PolylineToDraw& S) {
		Writer.StartObject();
			Writer.Key("P");
			Writer.StartArray();
				for(const Dasher::CDasherScreen::point& Point : S.Points)
				{
					Writer.StartArray();
						Writer.Int(Point.x);
						Writer.Int(Point.y);
					Writer.EndArray();
				}
			Writer.EndArray();
			Writer.Key("W");
			Writer.Int(S.iWidth);
			Writer.Key("C");
			Writer.Int(S.iColor);
		Writer.EndObject();
	}
} PolylineToDraw;

typedef struct PolygonToDraw
{
	std::vector<Dasher::CDasherScreen::point> Points;
	int iFillColor;
	int iOutlineColor;
	int LineWidth;

	/*	Serializes into this format:
	 * {
	 *	"P" : [[<X_0>,<Y_0>],...,[<X_Number>,<Y_Number>]],
	 *	"F" : <iFillColor>,
	 *	"O" : <iOutlineColor>, //optional
	 *	"W" : <LineWidth> //optional
	 * }
	 */
	static void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& Writer, const struct PolygonToDraw& S) {
		Writer.StartObject();
			Writer.Key("P");
			Writer.StartArray();
				for (const Dasher::CDasherScreen::point& Point : S.Points)
				{
					Writer.StartArray();
						Writer.Int(Point.x);
						Writer.Int(Point.y);
					Writer.EndArray();
				}
			Writer.EndArray();
			Writer.Key("F");
			Writer.Int(S.iFillColor);
			if(S.LineWidth > 0 && S.iOutlineColor >= 0)
			{
				Writer.Key("O");
				Writer.Int(S.iOutlineColor);
				Writer.Key("W");
				Writer.Int(S.LineWidth);
			}
		Writer.EndObject();
	}
} PolygonToDraw;
