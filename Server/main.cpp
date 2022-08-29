#include <thread>
#include "Declarations.h"

#include "rapidjson/document.h"

#include <XmlSettingsStore.h>

#include "DasherController.h"
#include "FileUtils.h"
#include "ParameterDefinitions.h"

std::shared_ptr<WebsocketServer> DasherServer;
Connection_Map Connections;
static Dasher::CommandlineErrorDisplay ErrorDisplay;

int main()
{
	DasherServer = make_shared<WebsocketServer>();
		
	Dasher::XmlSettingsStore* Settings = new Dasher::XmlSettingsStore("Settings.xml",  &ErrorDisplay); //Gets deleted somewhere else
	Settings->Load();
	Settings->Save();

	DasherServer->set_open_handler([Settings](const websocketpp::connection_hdl& Connection)
	{
		shared_ptr<DasherController> Controller = make_shared<DasherController>(Settings,  DasherServer, Connection);

		Controller->Initialize();

		Connections[Connection] = std::move(Controller);
	});

	DasherServer->set_close_handler([](const websocketpp::connection_hdl& Connection)
	{
		Connections.erase(Connection);
	});

	DasherServer->set_message_handler([](websocketpp::connection_hdl Connection, WebsocketServer::message_ptr Msg)
	{
		rapidjson::Document Doc;
		Doc.Parse(Msg->get_raw_payload().c_str());

		rapidjson::Value& Type = Doc["T"];
		if (Type == "R" && Doc.HasMember("W") && Doc.HasMember("H"))
		{
			Connections[Connection]->GetScreenModule()->Resize(Doc["W"].GetUint(), Doc["H"].GetUint());
			Connections[Connection]->ScreenResized(Connections[Connection]->GetScreenModule().get());
		}else
		if(Type == "C" && Doc.HasMember("X") && Doc.HasMember("Y"))
		{
			Connections[Connection]->GetInputModule()->SetMousePosition(Doc["X"].GetInt(), Doc["Y"].GetInt());
		}else 
		if(Type == "M" && Doc.HasMember("D"))
		{
			Connections[Connection]->OnMouseEvent(Doc["D"].GetBool());
		}else
		if (Type == "S" && Doc.HasMember("C") && Doc.HasMember("H"))
		{
			Connections[Connection]->GetScreenModule()->GetFontMeasureService()->ParseAlphabetSizes(Doc);
		}else
		if (Type == "P" && Doc.HasMember("N") && Doc.HasMember("V"))
		{
			const std::string ParameterName = Doc["N"].GetString();
			if(Dasher::ParameterEnumMap.count(ParameterName) < 1) return;

			const int Parameter = Dasher::ParameterEnumMap[ParameterName];
			if(ParameterName[0] == 'B' && Doc["V"].IsBool())
			{
				Connections[Connection]->SetBoolParameter(Parameter, Doc["V"].GetBool());
			}else if(ParameterName[0] == 'S' && Doc["V"].IsString())
			{
				Connections[Connection]->SetStringParameter(Parameter, Doc["V"].GetString());
			}else if(ParameterName[0] == 'L' && Doc["V"].IsInt64())
			{
				Connections[Connection]->SetLongParameter(Parameter, static_cast<long>(Doc["V"].GetInt64()));
			}
		}
	});

	DasherServer->set_access_channels(websocketpp::log::alevel::none);
	DasherServer->set_error_channels(websocketpp::log::elevel::all);

	DasherServer->init_asio();
	DasherServer->listen(9007);
	DasherServer->start_accept();
	   
	DasherServer->run();

	return EXIT_SUCCESS;
} 
