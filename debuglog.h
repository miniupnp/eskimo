// (c) 2018 Thomas BERNARD
#ifndef DEBUGLOG_H__
#define DEBUGLOG_H__

#ifndef WIN32
#include <stdio.h>

#define OutputDebugString(s) fputs(s, stdout)

#endif

#endif
