// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

#pragma comment(lib, "ws2_32.lib")

#include <range/v3/all.hpp>
#include <vector>
#include <stack>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <algorithm>
#include <string>
#include <vector>
#include <numeric>
#include <unordered_map>
#include <map>
#include <queue>
#include <functional>
#include <memory> 
#include <Ws2tcpip.h> 
#include <winsock2.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "PROP.h"
#include "ThreadSafeQueue.h"
#include "Vec2.h"
#include "YCTime.h"
#include "YCUtil.h"
#include "IBehavior.h"
#include "YC_LOG.h"
#include "Functional.h"
#include "YCStack.h"


using std::unordered_map;
using std::map;
using std::list;
using std::queue;
using std::vector;
using std::function;
using std::thread;
using std::atomic;
using std::mutex;
using std::shared_ptr;
using std::stack;
using std::string;

using namespace std::chrono;
using byte = unsigned char;



#endif //PCH_H
