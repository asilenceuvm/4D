#pragma once

#include <iostream>
#include <chrono>
#include <windows.h> 

namespace Logger {

	bool showSuccess = true;
	bool showWarning = true;
	bool showError = true;
	bool showInfo = true;
	unsigned __int64 sectionStartTime;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	inline unsigned __int64 getSysTime() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	inline void logSuccess(const char* category, const char* message) {
		if (showSuccess) {
			SetConsoleTextAttribute(hConsole, 10);
			std::cout << "[" << category << "] " << message << " finished sucessfully. Took " << getSysTime() - sectionStartTime << " ms." << std::endl;
			SetConsoleTextAttribute(hConsole, 15);
		}
	}

	inline void logWarning(const char* category, const char* message, bool timed, const char* warning = "") {
		if (showWarning) {
			SetConsoleTextAttribute(hConsole, 14);
			if (timed) {
				std::cout << "[" << category << "] " << message << " finished. Took " << getSysTime() - sectionStartTime << " ms." << std::endl;
				std::cout <<  "Warning: " << warning << std::endl;
			}
			else {
				std::cout << "[" << category << "] " << message << std::endl;
			}
			SetConsoleTextAttribute(hConsole, 15);
		}
	}

	inline void logMessage(const char* category, const char* message) {
		SetConsoleTextAttribute(hConsole, 15);
		sectionStartTime = getSysTime();
		std::cout << "[" << category << "] " << message << std::endl;
	}

	inline void logInfo(const char* category, const char* message) {
		if (showInfo) {
			SetConsoleTextAttribute(hConsole, 9);
			sectionStartTime = getSysTime();
			std::cout << "[" << category << "] " << message << std::endl;
		}
	}
	
	inline void logError(const char* category, const char* message) {
		if (showError) {
			SetConsoleTextAttribute(hConsole, 12);
			std::cout << "[" << category << "] " << message << std::endl;
			SetConsoleTextAttribute(hConsole, 15);
		}
	}
};

