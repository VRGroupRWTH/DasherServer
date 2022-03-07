#include "ServerInput.h"

bool ServerInput::GetScreenCoords(Dasher::screenint& iX, Dasher::screenint& iY, Dasher::CDasherView* pView)
{
	iX = X;
	iY = Y;
	return true;
}

void ServerInput::SetMousePosition(int X, int Y)
{
	this->X = X;
	this->Y = Y;
}
