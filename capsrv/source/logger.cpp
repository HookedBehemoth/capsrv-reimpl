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
#include "logger.hpp"

#include <cstdarg>

#include "capsrv_types.hpp"
#include "capsrv_util.hpp"

static char sprint_buf[1024];
u64 offset = 0;
FsFileSystem sdmc;

Result LogInit() {
    R_TRY(fsOpenSdCardFileSystem(&sdmc));
    if (R_SUCCEEDED(fsFsCreateFile(&sdmc, "/log.txt", 0, 0)))
        return 0;
    FsFile log_file;
    R_TRY(fsFsOpenFile(&sdmc, "/log.txt", FsOpenMode_Read, &log_file));
    ON_SCOPE_EXIT { fsFileClose(&log_file); };
    s64 tmp;
    R_TRY(fsFileGetSize(&log_file, &tmp));
    if (tmp >= 0)
        offset = static_cast<u64>(tmp);
    return 0;
}

void LogExit() {
    fsFsClose(&sdmc);
}

int raw_fprintf(FsFile *file, const char *fmt, ...) {
    va_list args;
    int n;

    va_start(args, fmt);
    n = vsprintf(sprint_buf, fmt, args);
    va_end(args);
    if (file)
        if (R_FAILED(fsFileWrite(file, offset, sprint_buf, n, FsWriteOption_None)))
            return 0;
    return n;
}

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

    FsFile log_file;
    if (R_FAILED(fsFsOpenFile(&sdmc, "/log.txt", FsOpenMode_Write | FsOpenMode_Append, &log_file)))
        return;
    ON_SCOPE_EXIT { fsFileClose(&log_file); };

    offset += raw_fprintf(&log_file, "[%02d:%02d:%02d] [%s] ", hour, min, sec, type);

    va_list f_args;
    va_start(f_args, fmt);
    int n = vsprintf(sprint_buf, fmt, f_args);
    va_end(f_args);
    if (R_SUCCEEDED(fsFileWrite(&log_file, offset, sprint_buf, n, FsWriteOption_None)))
        offset += n;

    offset += raw_fprintf(&log_file, "\n");

    fsFileFlush(&log_file);
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
