#pragma once
#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <cassert>
#include <sstream>
#include "Functional.h"
#include "YCSetting.h"
#include "fmt/format.h"


const static std::string currentDateTime() {
    time_t     now = time(0); 
    struct tm  tstruct;
    char       buf[80];

    localtime_s(&tstruct, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

namespace yc
{
    template <typename S, typename ...Args>
    static void log(S format, Args&& ...args)
    {
        if (!USE_LOG_SETTING) return;
        auto s = fmt::format("[{}] {}\n", currentDateTime(), format);
        
        fmt::print(s, args...);
    }
}