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

#include "capsrv_time.hpp"
#include "capsrv_types.hpp"

constexpr size_t buf_size = 1024;
const char *log_path = "sdmc:/log.txt";
static char sprint_buf[buf_size];
u64 offset = 0;

ams::Result LogInit() {
    R_TRY(ams::fs::MountSdCard("sdmc"));
    if (R_SUCCEEDED(ams::fs::CreateFile(log_path, 0)))
        return 0;
    ams::fs::FileHandle file;
    R_TRY(ams::fs::OpenFile(&file, log_path, ams::fs::OpenMode_Read));
    ON_SCOPE_EXIT { ams::fs::CloseFile(file); };
    s64 tmp;
    R_TRY(ams::fs::GetFileSize(&tmp, file));
    if (tmp >= 0)
        offset = static_cast<u64>(tmp);
    return 0;
}

void LogExit() {
    ams::fs::Unmount("sdmc");
}

int raw_fprintf(ams::fs::FileHandle file, const char *fmt, ...) {
    va_list args;
    int n;

    va_start(args, fmt);
    n = std::vsnprintf(sprint_buf, buf_size, fmt, args);
    va_end(args);
    if (R_FAILED(ams::fs::WriteFile(file, offset, sprint_buf, n, ams::fs::WriteOption::None)))
        return 0;
    return n;
}

void WriteLogFile(const char *type, const char *fmt, ...) {
#ifdef __DEBUG__
    u64 timestamp;
    timeGetCurrentTime(TimeType_Default, &timestamp);
    ams::capsrv::DateTime datetime;
    ams::capsrv::time::TimestampToCalendarTime(&datetime, timestamp);
    int hour = datetime.hour;
    int min = datetime.minute;
    int sec = datetime.second;

#ifdef SYSTEM_MODULE
    ams::fs::FileHandle log_file;
    if (R_FAILED(ams::fs::OpenFile(&log_file, log_path, ams::fs::OpenMode_Write | ams::fs::OpenMode_AllowAppend)))
        return;
    ON_SCOPE_EXIT { ams::fs::CloseFile(log_file); };

    offset += raw_fprintf(log_file, "[%02d:%02d:%02d] [%s] ", hour, min, sec, type);

    va_list f_args;
    va_start(f_args, fmt);
    int n = std::vsnprintf(sprint_buf, buf_size, fmt, f_args);
    va_end(f_args);
    if (R_SUCCEEDED(ams::fs::WriteFile(log_file, offset, sprint_buf, n, ams::fs::WriteOption::None)))
        offset += n;

    offset += raw_fprintf(log_file, "\n");

    ams::fs::FlushFile(log_file);
#elif APPLET_TEST
    printf("[%02d:%02d:%02d] [%s] ", hour, min, sec, type);
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
