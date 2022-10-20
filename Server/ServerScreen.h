#pragma once

#include "Declarations.h"
#include <DasherScreen.h>

#include "ServerFontMeasure.h"

class ServerScreen : public Dasher::CDasherScreen{
public:
	ServerScreen(std::shared_ptr<WebsocketServer> Server, websocketpp::connection_hdl Connection)
		: Dasher::CDasherScreen(100,100), ServerInstance(std::move(Server)), Connection(std::move(Connection))
	{
		FontMeasureService = std::make_shared<ServerFontMeasureService>();
	}
	~ServerScreen() override;

	std::pair<Dasher::screenint, Dasher::screenint> TextSize(Label* label, unsigned iFontSize) override;
	void DrawString(Label* label, Dasher::screenint x, Dasher::screenint y, unsigned iFontSize, int iColour) override;
	void DrawRectangle(Dasher::screenint x1, Dasher::screenint y1, Dasher::screenint x2, Dasher::screenint y2, int Colour, int iOutlineColour, int iThickness) override;
	void DrawCircle(Dasher::screenint iCX, Dasher::screenint iCY, Dasher::screenint iR, int iFillColour, int iLineColour, int iLineWidth) override;
	void Polyline(point* Points, int Number, int iWidth, int Colour) override;
	void Polygon(point* Points, int Number, int fillColour, int outlineColour, int lineWidth) override;
	void Display() override;
	void SetColourScheme(const Dasher::CColourIO::ColourInfo* pColourScheme) override;
	bool IsPointVisible(Dasher::screenint x, Dasher::screenint y);
	void SendCurrentColorScheme();

	bool MultiSizeFonts() override { return true; }

	void Resize(unsigned int Width, unsigned int Height);

	std::shared_ptr<ServerFontMeasureService> GetFontMeasureService() const
	{
		return FontMeasureService;
	}

private:
	const Dasher::CColourIO::ColourInfo* pColorScheme;
	std::shared_ptr<ServerFontMeasureService> FontMeasureService;
public:

private:
	std::vector<std::unique_ptr<DasherDrawGeometry>> GeometryToDraw;

	std::shared_ptr<WebsocketServer> ServerInstance;
	websocketpp::connection_hdl Connection;
};
