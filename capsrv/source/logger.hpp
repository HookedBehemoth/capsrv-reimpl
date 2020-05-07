#pragma once

#ifdef __DEBUG__
#define LOG(fmt, ...) SendLog(__FILE__, __func__, __LINE__, fmt, __VA_ARGS__)
#else
#define LOG(fmt, ...)
#endif
