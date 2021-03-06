#pragma once

#include <vector>
#include <string>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define KO RED "[KO]" RESET
#define OK GREEN "[OK]" RESET

#define LOG_ENABLE true

#if LOG_ENABLE
#   define LOG_MESSAGE(...) Nodable::Log::Push( Nodable::LogType::Message, __VA_ARGS__ )
#   define LOG_DEBUG(...) Nodable::Log::Push( Nodable::LogType::Message, __VA_ARGS__ )
#   define LOG_WARNING(...) Nodable::Log::Push( Nodable::LogType::Warning, __VA_ARGS__ )
#   define LOG_ERROR(...)   Nodable::Log::Push( Nodable::LogType::Error, __VA_ARGS__ )
#else
#   define LOG_MESSAGE(...)
#   define LOG_DEBUG(...)
#   define LOG_WARNING(...)
#   define LOG_ERROR(...)
#endif

namespace Nodable{	

	enum class LogType
    {
		Message,
		Warning,
		Error
	};

	struct Message
    {
		LogType type;
		short unsigned int verbosity;
		std::string text;
	};

	class Log
    {
	private:
        static std::vector<Message> Logs;
        static const short unsigned int DefaultVerbosityLevel = 0u;
        static short unsigned int VerbosityLevel;

	public:
        inline static short unsigned int GetVerbosityLevel(){ return Log::VerbosityLevel; }
        static const Message* GetLastMessage();
	    static void SetVerbosityLevel(short unsigned int _verbosityLevel);
		static void Push(LogType _type, short unsigned int _verbosityLevel, const char* _format, ...);
	};
}