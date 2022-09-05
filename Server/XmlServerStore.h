#pragma once
#include <rapidjson/document.h>

#include "Declarations.h"
#include "XmlSettingsStore.h"

class XmlServerStore : public Dasher::XmlSettingsStore {
public:
	XmlServerStore(const std::string& filename, CMessageDisplay* pDisplay) : Dasher::XmlSettingsStore(filename, pDisplay) {};

	void SendParameterListToClient(const std::shared_ptr<WebsocketServer>& DasherServer, const websocketpp::connection_hdl& Connection);
	void SetParameterBasedOnRequest(const std::string& ParameterName, rapidjson::Value& Value);
};
