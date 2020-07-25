#include <string>
#include <json\json.h>

std::string createSimpleFormString(std::string title, std::string content, Json::Value & bttxts) {
	Json::Value jv;
	jv["type"] = "form";
	jv["title"] = title;
	jv["content"] = content;
	jv["buttons"] = bttxts;
	return jv.toStyledString();
}

std::string createModalFormString(std::string title, std::string content, std::string button1, std::string button2) {
	Json::Value jv;
	jv["type"] = "modal";
	jv["title"] = title;
	jv["content"] = content;
	jv["button1"] = button1;
	jv["button2"] = button2;
	return jv.toStyledString();
}
