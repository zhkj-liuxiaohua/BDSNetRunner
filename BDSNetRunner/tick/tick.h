#pragma once
#include <functional>

// 将危险操作放置于tick内
extern void safeTick(std::function<void()>);