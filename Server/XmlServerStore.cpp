#include "XmlServerStore.h"
#include "Parameters.h"

// Case insensitive equals
static bool iequals(const std::string& a, const std::string& b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}

/*
 *	Serializes to something like:
 *
 *	{
 *		"T": "P",
 *		"Bool" : [
 *			{"Name" : <Param Name>, "Default" : <Param Default>, "Description" : <Param Description>, "Value" : <Current Param Value>},
 *			...
 *		],
 *		"String" : [
 *			{"Name" : <Param Name>, "Default" : <Param Default>, "Description" : <Param Description>, "Value" : <Current Param Value>},
 *			...
 *		],
 *		"Long" : [
 *			{"Name" : <Param Name>, "Default" : <Param Default>, "Description" : <Param Description>, "Value" : <Current Param Value>},
 *			...
 *		]
 *	}
 *
 */

void XmlServerStore::SendParameterListToClient(const std::shared_ptr<WebsocketServer>& DasherServer, const websocketpp::connection_hdl& Connection)
{
	rapidjson::StringBuffer Buffer;
	rapidjson::Writer<rapidjson::StringBuffer> Writer(Buffer);

	Writer.StartObject();
		Writer.Key("T");
		Writer.String("P");

		// Bools
		Writer.Key("Bool");
		Writer.StartArray();
		for(const Dasher::Settings::bp_table& entry : Dasher::Settings::boolparamtable)
		{
			Writer.StartObject();
				Writer.Key("Name");
				Writer.String(entry.regName);
				Writer.Key("Default");
				Writer.Bool(entry.defaultValue);
				Writer.Key("Description");
				Writer.String(entry.humanReadable);
				Writer.Key("Value");
				Writer.Bool(GetBoolParameter(entry.key));
			Writer.EndObject();
		}
		Writer.EndArray();

		// Strings
		Writer.Key("String");
		Writer.StartArray();
		for(const Dasher::Settings::sp_table& entry : Dasher::Settings::stringparamtable)
		{
			Writer.StartObject();
				Writer.Key("Name");
				Writer.String(entry.regName);
				Writer.Key("Default");
				Writer.String(entry.defaultValue);
				Writer.Key("Description");
				Writer.String(entry.humanReadable);
				Writer.Key("Value");
				Writer.String(GetStringParameter(entry.key).c_str());
			Writer.EndObject();
		}
		Writer.EndArray();

		// Longs
		Writer.Key("Long");
		Writer.StartArray();
		for(const Dasher::Settings::lp_table& entry : Dasher::Settings::longparamtable)
		{
			Writer.StartObject();
				Writer.Key("Name");
				Writer.String(entry.regName);
				Writer.Key("Default");
				Writer.Int64(entry.defaultValue);
				Writer.Key("Description");
				Writer.String(entry.humanReadable);
				Writer.Key("Value");
				Writer.Int64(GetLongParameter(entry.key));
			Writer.EndObject();
		}
		Writer.EndArray();
	Writer.EndObject();

	//Send Parameters to Client
	DasherServer->send(Connection, Buffer.GetString(), Buffer.GetSize(), websocketpp::frame::opcode::TEXT);
}

void XmlServerStore::SetParameterBasedOnRequest(const std::string& ParameterName, rapidjson::Value& Value)
{
	for(const Dasher::Settings::bp_table& entry : Dasher::Settings::boolparamtable)
	{
		if(iequals(ParameterName, entry.regName))
		{
			SetBoolParameter(entry.key, Value.GetBool());
			return;
		}
	}

	for(const Dasher::Settings::lp_table& entry : Dasher::Settings::longparamtable)
	{
		if(iequals(ParameterName, entry.regName))
		{
			SetLongParameter(entry.key, static_cast<long>(Value.GetInt64()));
			return;
		}
	}

	for(const Dasher::Settings::sp_table& entry : Dasher::Settings::stringparamtable)
	{
		if(iequals(ParameterName, entry.regName))
		{
			SetStringParameter(entry.key, Value.GetString());
			return;
		}
	}
}
