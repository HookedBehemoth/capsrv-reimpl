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
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include <time.h>

#include <cstdarg>
#include <cstdio>

#include "capsrv_types.hpp"
#include "capsrv_util.hpp"

void WriteLogFile(const char *type, const char *fmt, ...) {
#ifdef __DEBUG__
#ifdef SYSTEM_MODULE
    u64 timestamp;
    timeGetCurrentTime(TimeType_Default, &timestamp);
    ams::capsrv::DateTime datetime;
    ams::capsrv::util::TimestampToCalendarTime(&datetime, timestamp);
    int hour = datetime.hour;
    int min = datetime.minute;
    int sec = datetime.second;
    FILE *pFile = fopen("sdmc:/log.txt", "a");
    if (pFile) {
        fprintf(pFile, "[%02d:%02d:%02d]_[%s] ", hour, min, sec, type);
        va_list args;
        va_start(args, fmt);
        vfprintf(pFile, fmt, args);
        va_end(args);
        fprintf(pFile, "\n");
        fclose(pFile);
    }
#elif APPLET_TEST
    printf("[%s] ", type);
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
#else
    static_assert(false, "undefined state");
#endif
#endif /* __DEBUG__ */
}
