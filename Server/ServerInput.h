#pragma once

#include "DasherInput.h"

class ServerInput : public Dasher::CScreenCoordInput
{
public:
	ServerInput() : CScreenCoordInput(0, _("Server Input"))
	{
	};

	virtual ~ServerInput() = default;
	bool GetScreenCoords(Dasher::screenint& iX, Dasher::screenint& iY, Dasher::CDasherView* pView) override;
	void SetMousePosition(int X, int Y);

private:
	int X = 0;
	int Y = 0;
};