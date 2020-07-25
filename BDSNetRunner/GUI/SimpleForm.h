#pragma once

#include <string>
#include <vector>

// 创建一个简易表单字符串
extern std::string createSimpleFormString(std::string, std::string, Json::Value&);

// 创建一个固定模板表单字符串
extern std::string createModalFormString(std::string, std::string, std::string, std::string);
