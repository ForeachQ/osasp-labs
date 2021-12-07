#include <iostream>
#include <Windows.h>
#include "Sorter.hpp"

LPSTR* GetLinesFromFile(LPWSTR filename, LPDWORD linesCount);
DWORD numOfThreadsInput();

int main()
{
	std::string fileName;
    std::cout << "File name:\n";
    std::cin >> fileName;

	int numberOfThreads = numOfThreadsInput();
	DWORD linesCount = 0;
	wchar_t wcFileName[100] = { 0 };
	mbstowcs(wcFileName, fileName.c_str(), fileName.length());
	LPSTR* lines = GetLinesFromFile(wcFileName, &linesCount);
	Sort(lines, linesCount, numberOfThreads);

	for (int i = 0; i < linesCount; i++)
	{
		printf("%s\n", lines[i]);
	}
}

DWORD numOfThreadsInput() {
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION* processorsInfo = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION[1];

	DWORD processorsInfoSize = 0;
	GetLogicalProcessorInformation(processorsInfo, &processorsInfoSize);
	delete[](processorsInfo);

	processorsInfo = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION[processorsInfoSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)];
	GetLogicalProcessorInformation(processorsInfo, &processorsInfoSize);

	int logicalProcessorsCount = 0;
	for (int i = 0; i < processorsInfoSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++) {
		if (processorsInfo[i].Relationship == RelationProcessorCore) {
			ULONG_PTR mask = processorsInfo[i].ProcessorMask;
			while (mask != 0) {
				if (mask & 1) logicalProcessorsCount++;
				mask = mask >> 1;
			}
		}
	}

	int numThreads = 0;
	while (numThreads > logicalProcessorsCount || numThreads < 1) {
		std::cout << "Enter number of threads: " << std::endl;
		std::cin >> numThreads;
		if (numThreads > logicalProcessorsCount) {
			std::cout << "Number of threads should be <= " << logicalProcessorsCount << std::endl;
		}
		if (numThreads < 1) {
			std::cout << "Number of threads should be >= " << 1 << std::endl;
		}
	}
	return numThreads;
}

LPSTR* GetLinesFromFile(LPWSTR filename, LPDWORD linesCount)
{
	HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	DWORD lpFileSizeHigh = GetFileSize(hFile, NULL);
	if (lpFileSizeHigh == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		return NULL;
	}

	LPSTR lpBuffer = (LPSTR)calloc(lpFileSizeHigh + 1, 1);
	if (!ReadFile(hFile, lpBuffer, lpFileSizeHigh, NULL, NULL))
	{
		CloseHandle(hFile);
		free(lpBuffer);
		return NULL;
	}
	CloseHandle(hFile);

	LPCSTR lpsDelimiters = "\r\n";
	LPSTR* lpsResult = (LPSTR*)calloc(0, sizeof(LPSTR)), * lpsBuffer;
	LPSTR lpsContext = 0;
	DWORD dwLinesCount = 0;
	LPSTR lpsToken = strtok_s(lpBuffer, lpsDelimiters, &lpsContext);

	while (lpsToken != NULL)
	{
		do
		{
			lpsBuffer = (LPSTR*)realloc(lpsResult, (dwLinesCount + 1) * sizeof(LPSTR));
		} while (lpsBuffer == NULL);
		lpsResult = lpsBuffer;
		lpsResult[dwLinesCount++] = lpsToken;
		lpsToken = strtok_s(NULL, lpsDelimiters, &lpsContext);
	}

	*linesCount = dwLinesCount;
	return lpsResult;
}
