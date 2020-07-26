#pragma once

#include <iostream>
#include <chrono>
#include <windows.h> 

namespace Logger {

	bool showSuccess = true;
	bool showWarning = true;
	bool showError = true;
	unsigned __int64 sectionStartTime;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	inline unsigned __int64 getSysTime() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	inline void logSuccess(const char* message) {
		if (showSuccess) {
			SetConsoleTextAttribute(hConsole, 10);
			std::cout << message << " finished sucessfully. Took " << getSysTime() - sectionStartTime << " ms." << std::endl;
			SetConsoleTextAttribute(hConsole, 15);
		}
	}

	inline void logError(const char* message) {
		if (showError) {
			SetConsoleTextAttribute(hConsole, 12);
			std::cout << message << " failed. Took " << getSysTime() - sectionStartTime << " ms." << std::endl;
			SetConsoleTextAttribute(hConsole, 15);
		}
	}

	inline void logWarning(const char* message, const char* warning) {
		if (showWarning) {
			SetConsoleTextAttribute(hConsole, 14);
			std::cout << message << " finished. Took " << getSysTime() - sectionStartTime << " ms." << std::endl;
			std::cout << "Warning: " << warning << std::endl;
			SetConsoleTextAttribute(hConsole, 15);
		}
	}

	inline void logMessage(const char* category, const char* message) {
		SetConsoleTextAttribute(hConsole, 15);
		sectionStartTime = getSysTime();
		std::cout << "[" << category << "] " << message << std::endl;
	}

	inline void logMessage(const char* category, const GLubyte* message) {
		SetConsoleTextAttribute(hConsole, 15);
		sectionStartTime = getSysTime();
		std::cout << "[" << category << "] " << message << std::endl;
	}

	
	inline void logError(const char* category, const char* message) {
		SetConsoleTextAttribute(hConsole, 12);
		std::cout << "[" << category << "] " << message << std::endl;
		SetConsoleTextAttribute(hConsole, 15);
	}
};

