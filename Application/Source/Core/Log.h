#pragma once
#include <memory>
#include <string>
#include <iostream>

class Logger;
/**
 * \brief Global log manager
 */
class Log
{
// TODO: Spacing between the time and the actual message should be a setting (for the developer)
public:
	enum LogPriority
	{
		LogPriorityTrace = 0,
		LogPriorityInfo,
		LogPriorityWarn,
		LogPriorityError,
		LogPriorityFatal
	};

	static void Init();
	static void Exit();

	static std::shared_ptr<Logger> GetCoreLogger();
	static std::shared_ptr<Logger> GetClientLogger();

	Log();
	~Log();


#pragma region TEMPLATE_GLOBAL_LOG_FUNCTIONS

	template<typename... Args>
	static void Trace(Args&&... args)
	{
		log(LogPriorityTrace, std::forward<Args>(args)...);
	}

	
	template<typename... Args>
	static void Info(Args&&... args)
	{
		log(LogPriorityInfo, std::forward<Args>(args)...);
	}
	
	template<typename... Args>
	static void Warn(Args&&... args)
	{
		log(LogPriorityWarn, std::forward<Args>(args)...);
	}

	template<typename... Args>
	static void Error(Args&&... args)
	{
		log(LogPriorityError, std::forward<Args>(args)...);
	}

	template<typename... Args>
	static void Fatal(Args&&... args)
	{
		log(LogPriorityFatal, std::forward<Args>(args)...);
		// TODO: Might need to end the application?
	}

private:

	template<class T>
	static void log(LogPriority level, const T& msg)
	{
		SetTextColourFromLogPriority(level);
		std::cout << "[" << RetrieveCurrentTime() << "]" << " "; // Time
		std::cout << msg << "\n";
	}

	template<typename Arg, typename... Args>
	static void log(LogPriority level, Arg&& arg, Args &&... args)
	{
		SetTextColourFromLogPriority(level);
		std::cout << "[" << RetrieveCurrentTime() << "]" << " "; // Time
		std::cout << std::forward<Arg>(arg);
		using pack_expander = int[];
		static_cast<void>(pack_expander{ 0, (static_cast<void>(std::cout << std::forward<Args>(args)), 0)... });
		std::cout << "\n";
	}
#pragma endregion TEMPLATE_GLOBAL_LOG_FUNCTIONS
	friend class Logger;

	static std::string RetrieveCurrentTime();
	static void SetTextColourFromLogPriority(LogPriority level);

	// TODO: rename these
	static std::shared_ptr<Logger> coreLogger_;
};

#pragma region GLOBAL_LOG_MACROS

#define CC_TRACE(...) ::Log::Trace(__VA_ARGS__)
#define CC_INFO(...) ::Log::Info(__VA_ARGS__)
#define CC_WARN(...) ::Log::Warn(__VA_ARGS__)
#define CC_ERROR(...) ::Log::Error(__VA_ARGS__)
#define CC_FATAL(...) ::Log::Fatal(__VA_ARGS__)

#pragma endregion GLOBAL_LOG_MACROS

