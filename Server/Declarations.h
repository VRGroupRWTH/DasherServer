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


struct DasherDrawGeometry
{
	virtual ~DasherDrawGeometry() {};
	virtual void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& Writer) = 0;
};

struct StringToDraw : DasherDrawGeometry
{
	Dasher::CDasherScreen::Label* Label;
	Dasher::screenint X;
	Dasher::screenint Y;
	unsigned int iFontSize;
	int iColor; /* Index into color map */

	StringToDraw(Dasher::CDasherScreen::Label* Label, Dasher::screenint X, Dasher::screenint Y, unsigned int iFontSize,	int iColor)
	: Label(Label), X(X), Y(Y), iFontSize(iFontSize), iColor(iColor){}

	/*	Serializes into this format:
	 * {
	 *  "G" : "S",
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
	void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& Writer) {
		Writer.StartObject();
			Writer.Key("G");
			Writer.String("S");
			Writer.Key("L");
			Writer.StartObject();
				Writer.Key("S");
				Writer.String(Label->m_strText.c_str(), static_cast<rapidjson::SizeType>(Label->m_strText.length()));
				Writer.Key("W");
				Writer.Int(Label->m_iWrapSize);
			Writer.EndObject();
			Writer.Key("X");
			Writer.Int(X);
			Writer.Key("Y");
			Writer.Int(Y);
			Writer.Key("F");
			Writer.Uint(iFontSize);
			Writer.Key("C");
			Writer.Int(iColor);
		Writer.EndObject();
	}

};

struct RectangleToDraw : DasherDrawGeometry
{
	Dasher::screenint X1;
	Dasher::screenint Y1;
	Dasher::screenint X2;
	Dasher::screenint Y2;
	int iColor; /* Index into color map */
	int iOutlineColor; /* Index into color map */
	int iThickness;

	RectangleToDraw(Dasher::screenint X1,Dasher::screenint Y1,Dasher::screenint X2,Dasher::screenint Y2,int iColor,int iOutlineColor,int iThickness)
	: X1(X1), Y1(Y1), X2(X2), Y2(Y2), iColor(iColor), iOutlineColor(iOutlineColor), iThickness(iThickness) {}

	/*	Serializes into this format:
	 * {
	 * 	"G" : "R",
	 *	"X1" : <X1>,
	 *	"Y1" : <Y1>,
	 *	"X2" : <X2>,
	 *	"Y2" : <Y2>,
	 *	"C" : <iColor>,
	 *	"O" : <iOutlineColor>, //optional
	 *	"T" : <iThickness> //optional
	 * }
	 */
	void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& Writer) {
		Writer.StartObject();
			Writer.Key("G");
			Writer.String("R");
			Writer.Key("X1");
			Writer.Int(X1);
			Writer.Key("Y1");
			Writer.Int(Y1);
			Writer.Key("X2");
			Writer.Int(X2);
			Writer.Key("Y2");
			Writer.Int(Y2);
			Writer.Key("C");
			Writer.Int(iColor);
			if(iThickness > 0 && iOutlineColor >= 0)
			{
				Writer.Key("O");
				Writer.Int(iOutlineColor);
				Writer.Key("T");
				Writer.Int(iThickness);
			}
			
		Writer.EndObject();
	}
};

struct CircleToDraw : DasherDrawGeometry
{
	Dasher::screenint iCX;
	Dasher::screenint iCY;
	Dasher::screenint iR;
	int iFillColor;
	int iLineColor;
	int iLineWidth;

	CircleToDraw(Dasher::screenint iCX,	Dasher::screenint iCY,	Dasher::screenint iR,	int iFillColor,	int iLineColor,	int iLineWidth)
	: iCX(iCX), iCY(iCY), iR(iR), iFillColor(iFillColor), iLineColor(iLineColor), iLineWidth(iLineWidth) {}

	/*	Serializes into this format:
	 * {
	 * 	"G" : "C",
	 *	"X" : <iCX>,
	 *	"Y" : <iCY>,
	 *	"R" : <iR>,
	 *	"F" : <iFillColor>,
	 *	"L" : <iLineColor>, //optional
	 *	"W" : <iLineWidth> //optional
	 * }
	 */
	void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& Writer) {
		Writer.StartObject();
			Writer.Key("G");
			Writer.String("C");
			Writer.Key("X");
			Writer.Int(iCX);
			Writer.Key("Y");
			Writer.Int(iCY);
			Writer.Key("R");
			Writer.Int(iR);
			Writer.Key("F");
			Writer.Int(iFillColor);
			if(iLineWidth > 0 && iLineColor >= 0)
			{
				Writer.Key("L");
				Writer.Int(iLineColor);
				Writer.Key("W");
				Writer.Int(iLineWidth);
			}
		Writer.EndObject();
	}
};

struct PolylineToDraw : DasherDrawGeometry
{
	std::vector<Dasher::CDasherScreen::point> Points;
	int iWidth;
	int iColor;

	PolylineToDraw(std::vector<Dasher::CDasherScreen::point> Points, int iWidth, int iColor) : Points(Points), iWidth(iWidth), iColor(iColor) {}

	/*	Serializes into this format:
	 * {
	 * 	"G" : "L",
	 *	"P" : [[<X_0>,<Y_0>],...,[<X_Number>,<Y_Number>]],
	 *	"W" : <iWidth>,
	 *	"C" : <iColor>
	 * }
	 */
	void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& Writer) {
		Writer.StartObject();
			Writer.Key("G");
			Writer.String("L");
			Writer.Key("P");
			Writer.StartArray();
				for(const Dasher::CDasherScreen::point& Point : Points)
				{
					Writer.StartArray();
						Writer.Int(Point.x);
						Writer.Int(Point.y);
					Writer.EndArray();
				}
			Writer.EndArray();
			Writer.Key("W");
			Writer.Int(iWidth);
			Writer.Key("C");
			Writer.Int(iColor);
		Writer.EndObject();
	}
};

struct PolygonToDraw : DasherDrawGeometry
{
	std::vector<Dasher::CDasherScreen::point> Points;
	int iFillColor;
	int iOutlineColor;
	int LineWidth;

	PolygonToDraw(std::vector<Dasher::CDasherScreen::point> Points,	int iFillColor,	int iOutlineColor, int LineWidth)
	: Points(Points), iFillColor(iFillColor), iOutlineColor(iOutlineColor), LineWidth(LineWidth) {}

	/*	Serializes into this format:
	 * {
	 * 	"G" : "P",
	 *	"P" : [[<X_0>,<Y_0>],...,[<X_Number>,<Y_Number>]],
	 *	"F" : <iFillColor>,
	 *	"O" : <iOutlineColor>, //optional
	 *	"W" : <LineWidth> //optional
	 * }
	 */
	void Serialize(rapidjson::Writer<rapidjson::StringBuffer>& Writer) {
		Writer.StartObject();
			Writer.Key("G");
			Writer.String("P");
			Writer.Key("P");
			Writer.StartArray();
				for (const Dasher::CDasherScreen::point& Point : Points)
				{
					Writer.StartArray();
						Writer.Int(Point.x);
						Writer.Int(Point.y);
					Writer.EndArray();
				}
			Writer.EndArray();
			Writer.Key("F");
			Writer.Int(iFillColor);
			if(LineWidth > 0 && iOutlineColor >= 0)
			{
				Writer.Key("O");
				Writer.Int(iOutlineColor);
				Writer.Key("W");
				Writer.Int(LineWidth);
			}
		Writer.EndObject();
	}
};
