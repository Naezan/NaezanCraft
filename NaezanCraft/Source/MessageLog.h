#pragma once

#include "Config.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <iostream>
#include <memory>

class ENGINE_API MessageLog
{
public:
	static void Initialize();

	inline static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return EngineLogger; }

private:
	static std::shared_ptr<spdlog::logger> EngineLogger;
};

#define NC_LOG_TRACE(...)	::MessageLog::GetEngineLogger()->trace(__VA_ARGS__)
#define NC_LOG_DEBUG(...)	::MessageLog::GetEngineLogger()->debug(__VA_ARGS__)
#define NC_LOG_INFO(...)	::MessageLog::GetEngineLogger()->info(__VA_ARGS__)
#define NC_LOG_WARN(...)	::MessageLog::GetEngineLogger()->warn(__VA_ARGS__)
#define NC_LOG_ERROR(...)	::MessageLog::GetEngineLogger()->error(__VA_ARGS__)
#define NC_LOG_FATAL(...)	::MessageLog::GetEngineLogger()->critical(__VA_ARGS__)
