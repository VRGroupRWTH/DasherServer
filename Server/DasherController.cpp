#include "DasherController.h"

DasherController::DasherController(Dasher::CSettingsStore* pSettingsStore, std::shared_ptr<WebsocketServer> server, websocketpp::connection_hdl connection): CDashIntfScreenMsgs(pSettingsStore),
                                                                                                                                                                                    Server(server),
                                                                                                                                                                                    Connection(connection)
{
	ScreenModule = make_shared<ServerScreen>(Server, Connection);
	ChangeScreen(ScreenModule.get());
}

void DasherController::editOutput(const std::string& strText, Dasher::CDasherNode* pNode) {

	if (Buffer.capacity() < strText.length() + Buffer.size()) {
		Buffer.reserve((strText.length() + Buffer.size()) * 2);
	}
	Buffer.append(strText);
	Cursor += static_cast<unsigned int>(strText.length());
	ForceSendBuffer();
	CDasherInterfaceBase::editOutput(strText, pNode);
}

void DasherController::editDelete(const std::string& strText, Dasher::CDasherNode* pNode) {
	if(0 == Buffer.compare(Buffer.length() - strText.length(), strText.length(), strText))
	{
		Buffer.erase(Buffer.length() - strText.length(), strText.length());
	}
	ForceSendBuffer();
	CDasherInterfaceBase::editDelete(strText, pNode);
}

unsigned DasherController::ctrlMove(bool bForwards, Dasher::CControlManager::EditDistance dist)
{
	if (dist == Dasher::CControlManager::EditDistance::EDIT_CHAR) {
		if (bForwards) Cursor++;
		else Cursor--;
	}
	return Cursor;
}

unsigned DasherController::ctrlDelete(bool bForwards, Dasher::CControlManager::EditDistance dist)
{
	if(dist == Dasher::CControlManager::EditDistance::EDIT_CHAR) {

		Buffer.erase(Cursor - (bForwards ? 0 : 1), 1);
	}
	if(!bForwards) Cursor--;
	ForceSendBuffer();
	return Cursor;

}

std::string DasherController::GetContext(unsigned iStart, unsigned iLength)
{
	return Buffer.substr(iStart, iLength);
}

std::string DasherController::GetAllContext()
{
	std::string CurrentBuffer = Buffer;
	SetBuffer(0);
	ForceSendBuffer();
	return CurrentBuffer;
}

std::string DasherController::GetTextAroundCursor(Dasher::CControlManager::EditDistance iDist) {
	if (Buffer.length() > Cursor && Buffer.length() >= 2) {
		if (iDist == Dasher::CControlManager::EditDistance::EDIT_CHAR) {
			return Buffer.substr(Cursor - 1, 2);
		}

		std::cerr << "tried to get more than just a char" << std::endl;
		return "";
	}
	std::cerr << "Cursor out of bounds" << std::endl;
	return "";
}


//TODO: Some day fix typo in function name
int DasherController::GetAllContextLenght()
{
	return static_cast<int>(Buffer.length());
}

void DasherController::ForceSendBuffer()
{
	rapidjson::StringBuffer MsgBuffer;
	rapidjson::Writer<rapidjson::StringBuffer> Writer(MsgBuffer);

	/*	Serializes into this format:
	 * {
	 *	"T" : "B",
	 *	"B" : <NewBuffer>
	 * }
	 */

	Writer.StartObject();
	Writer.Key("T");
	Writer.String("B");
	Writer.Key("B");
	Writer.String(Buffer.c_str(), static_cast<rapidjson::SizeType>(Buffer.length()));
	Writer.EndObject();

	//Send "Frame" to Client
	Server->send(Connection, MsgBuffer.GetString(), MsgBuffer.GetSize(), websocketpp::frame::opcode::TEXT);
}

void DasherController::Initialize()
{
	Realize(0);
	
	TickThread = new std::thread(&DasherController::Tick, this);

	ScreenModule->GetFontMeasureService()->ChangeAlphabet(GetActiveAlphabet());
	ScreenModule->GetFontMeasureService()->SendAlphabet(Server, Connection);
}

void DasherController::CreateModules()
{
	CDashIntfScreenMsgs::CreateModules();

	InputModule = make_shared<ServerInput>();

	RegisterModule(InputModule.get());
	SetDefaultInputDevice(InputModule.get());
}

DasherController::~DasherController()
{
	bRunTickThread = false;
	TickThread->join();
}

void DasherController::Tick()
{
	const std::chrono::time_point<chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
	while (bRunTickThread) {
		NewFrame(static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count()), true);
		std::this_thread::sleep_for(2ms);
	}
}

void DasherController::OnMouseEvent(bool isDown)
{
	if(isDown)
	{
		KeyDown(0, 100); //100 is the keycode for LMB
	}
	else
	{
		KeyUp(0, 100); //100 is the keycode for LMB
	}
}
