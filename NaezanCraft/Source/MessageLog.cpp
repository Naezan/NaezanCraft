#include "MessageLog.h"

std::shared_ptr<spdlog::logger> MessageLog::EngineLogger;

void MessageLog::Initialize()
{
	spdlog::set_pattern("%^[%T] %n: %l: %v%$");
	EngineLogger = spdlog::stdout_color_mt("EngineLog");
	EngineLogger->set_level(spdlog::level::trace);
}
