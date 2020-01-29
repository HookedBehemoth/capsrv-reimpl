/*
 * MIT License
 * 
 * Copyright (c) 2019 KrankRival
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <time.h>
#include <cstdio>
#include <cstdarg>
#include <string.h>
#include <stdlib.h>

typedef enum {
	LogType_Error = 0,
	LogType_Info = 1,
} LogType;

void WriteLogFile(LogType type, const char *fmt, ...) {
	time_t lTime = time(NULL);
	struct tm* timestruct = localtime((const time_t *) &lTime);
	int hour = timestruct->tm_hour;
	int min = timestruct->tm_min;
	int sec = timestruct->tm_sec;
	char time[9];
	sprintf(time, "%02d:%02d:%02d", hour, min, sec);
        char _type[16];
	switch (type) {
		case LogType_Error: 
			strcpy(_type, "ERROR");
			break;
		case LogType_Info: 
			strcpy(_type, "INFO");
			break;
	}
	FILE* pFile = fopen("sdmc:/log.txt", "a");
	if(pFile) {
		fprintf(pFile, "[%s] [%s] ", time, _type);
		va_list args;
		va_start(args, fmt);
		vfprintf(pFile, fmt, args);
		va_end(args);
		fprintf(pFile, "\n");
		fclose(pFile);
	}
}
