//! Thin tagged logging wrapper. Prefixes every line with "[NML]" so NML output is easy to filter in logs.
class NML_Log
{
	protected static const string PREFIX = "[NML] ";

	//------------------------------------------------------------------------------------------------
	static void Info(string message)
	{
		Print(PREFIX + message, LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	static void Warning(string message)
	{
		Print(PREFIX + message, LogLevel.WARNING);
	}

	//------------------------------------------------------------------------------------------------
	static void Error(string message)
	{
		Print(PREFIX + message, LogLevel.ERROR);
	}
}
