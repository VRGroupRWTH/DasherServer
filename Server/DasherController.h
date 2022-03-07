#pragma once
#include <DashIntfScreenMsgs.h>

#include "Declarations.h"
#include "ServerInput.h"
#include "ServerScreen.h"


class DasherController : public Dasher::CDashIntfScreenMsgs
{
public:
	DasherController(Dasher::CSettingsStore* pSettingsStore, CFileUtils* fileUtils, std::shared_ptr<WebsocketServer> server, websocketpp::connection_hdl connection);

	void editOutput(const std::string& strText, Dasher::CDasherNode* pNode) override;
	void editDelete(const std::string& strText, Dasher::CDasherNode* pNode) override;
	unsigned ctrlMove(bool bForwards, Dasher::CControlManager::EditDistance dist) override;
	unsigned ctrlDelete(bool bForwards, Dasher::CControlManager::EditDistance dist) override;
	std::string GetContext(unsigned iStart, unsigned iLength) override;
	std::string GetAllContext() override;
	std::string GetTextAroundCursor(Dasher::CControlManager::EditDistance iDist) override;
	int GetAllContextLenght() override;
	void ForceSendBuffer();

	void Initialize();
	void Tick();

	void OnMouseEvent(bool isDown);

private:
	void CreateModules() override;

public:
	std::shared_ptr<ServerInput> GetInputModule() { return InputModule; }
	std::shared_ptr<ServerScreen> GetScreenModule() { return ScreenModule; }
	~DasherController() override;

private:
	//Cursor position in the output buffer
	unsigned int Cursor = 0;
	//Output Buffer
	std::string Buffer;

	//Modules
	std::shared_ptr<ServerInput> InputModule;
	std::shared_ptr<ServerScreen> ScreenModule;

	//Server Stuff
	std::shared_ptr<WebsocketServer> Server;
	websocketpp::connection_hdl Connection;

	//Tick
	std::thread* TickThread;
	bool bRunTickThread = true;
};
