#include "ServerScreen.h"

#include <rapidjson/document.h>

std::pair<Dasher::screenint, Dasher::screenint> ServerScreen::TextSize(Label* Label, unsigned iFontSize)
{
	return FontMeasureService->Measure(Label->m_strText, iFontSize);
}

ServerScreen::~ServerScreen()
{
	GeometryToDraw.clear();
}

void ServerScreen::DrawString(Label* label, Dasher::screenint x, Dasher::screenint y, unsigned iFontSize, int iColour)
{
	GeometryToDraw.push_back(std::make_unique<StringToDraw>(label,x,y,iFontSize,iColour));
}

void ServerScreen::DrawRectangle(Dasher::screenint x1, Dasher::screenint y1, Dasher::screenint x2, Dasher::screenint y2, int Colour, int iOutlineColour, int iThickness)
{
	GeometryToDraw.push_back(std::make_unique<RectangleToDraw>(x1,y1,x2,y2,Colour,iOutlineColour,iThickness));
}

void ServerScreen::DrawCircle(Dasher::screenint iCX, Dasher::screenint iCY, Dasher::screenint iR, int iFillColour, int iLineColour, int iLineWidth)
{
	GeometryToDraw.push_back(std::make_unique<CircleToDraw>(iCX, iCY, iR, iFillColour, iLineColour, iLineWidth));
}

void ServerScreen::Polyline(point* Points, int Number, int iWidth, int Colour)
{
	std::vector<point> pointsVector;
	pointsVector.insert(pointsVector.end(), Points, Points + Number);
	
	GeometryToDraw.push_back(std::make_unique<PolylineToDraw>(pointsVector, iWidth, Colour));
}

void ServerScreen::Polygon(point* Points, int Number, int fillColour, int outlineColour, int lineWidth)
{
	std::vector<point>  pointsVector;
	pointsVector.insert(pointsVector.end(), Points, Points + Number);

	GeometryToDraw.push_back(std::make_unique<PolygonToDraw>(pointsVector, fillColour, outlineColour, lineWidth));
}

void ServerScreen::Display()
{
	/*	Serializes into this format:
	 * {
	 *	"T" : "F",
	 *	"D" : [{<Geometry_0>},...,],
	 * }
	 */

	rapidjson::StringBuffer Buffer;
	rapidjson::Writer<rapidjson::StringBuffer> Writer(Buffer);

	Writer.StartObject();
		Writer.Key("T");
		Writer.String("F");
		Writer.Key("G");
		Writer.StartArray();
		for(std::unique_ptr<DasherDrawGeometry>& G : GeometryToDraw )
		{
			G->Serialize(Writer);
		}
		Writer.EndArray();
	Writer.EndObject();

	//Send "Frame" to Client
	if(ServerInstance->get_con_from_hdl(Connection)->get_state() == websocketpp::session::state::open)
	{
		ServerInstance->send(Connection, Buffer.GetString(), Buffer.GetSize(), websocketpp::frame::opcode::TEXT);
	}

	//Empty buffers
	GeometryToDraw.clear();
}

void ServerScreen::SetColourScheme(const Dasher::CColourIO::ColourInfo* pColourScheme)
{
	this->pColorScheme = pColourScheme;

	if (!pColorScheme) return;

	/*	Serializes into this format:
	 * {
	 *	"T" : "C",
	 *	"I" : "<ColourID>",
	 *	"C" : [[red_0,green_0,blue_0],...,],
	 * }
	 */
	rapidjson::StringBuffer Buffer;
	rapidjson::Writer<rapidjson::StringBuffer> Writer(Buffer);
	Writer.StartObject();
		Writer.Key("T");
		Writer.String("C");
		Writer.Key("I");
		Writer.String(pColorScheme->ColourID.c_str(), static_cast<rapidjson::SizeType>(pColorScheme->ColourID.length()));
		Writer.Key("C");
		Writer.StartArray();
			for (int i = 0; i < pColorScheme->Colors.size(); i++)
			{
				Writer.StartArray();
					Writer.Int(pColorScheme->Colors[i].Red);
					Writer.Int(pColorScheme->Colors[i].Green);
					Writer.Int(pColorScheme->Colors[i].Blue);
				Writer.EndArray();
			}
		Writer.EndArray();
	Writer.EndObject();

	if(ServerInstance->get_con_from_hdl(Connection)->get_state() == websocketpp::session::state::open){
		ServerInstance->send(Connection, Buffer.GetString(), Buffer.GetSize(), websocketpp::frame::opcode::TEXT);
	}
}

bool ServerScreen::IsPointVisible(Dasher::screenint, Dasher::screenint){
	return true;
}

void ServerScreen::SendCurrentColorScheme()
{
	SetColourScheme(pColorScheme); //Set to current one and thus force to resend it
}

void ServerScreen::Resize(unsigned Width, unsigned Height)
{
	resize(static_cast<int>(Width), static_cast<int>(Height));
}
