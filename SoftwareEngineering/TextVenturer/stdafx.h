#pragma once

#include <vld.h>
#include <vector>
#include <queue>
#include <thread>
#include <atomic>

#include <Windows.h>

#include <glew.h>
#include <wglew.h>

#include <algorithm>
#include <string>
#include <fstream>
#include <sstream> 
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <regex>

#include <atlbase.h>
#include <atlconv.h>

#define _USE_MATH_DEFINES
#include <math.h>
#define PI M_PI

typedef std::unordered_map<std::string, std::string> dictionary;
typedef std::unordered_set<std::string> tags;
typedef std::vector<std::string> strings;

#define STRINGIZE_HELPER(x) #x
#define STRINGIZE(x) STRINGIZE_HELPER(x)
#define WARNING(desc) message (__FILE__ "(" STRINGIZE(__LINE__) "): warning: " #desc)

#include "ErrorDialog.h"
#include "AliasList.h"

#include "vec2.h"
#include "Matrix2.h"
#include "Matrix3.h"
#include "Color.h"

#include "lodePNG.h"
#include "GLEnums.h"
#include "Shader.h"
#include "VAO.h"
#include "TextureData.h"
#include "Texture.h"
#include "BMPFont.h"
#include "RBO.h"
#include "FBO.h"
