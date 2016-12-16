/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This file is part of NEWorld.
* NEWorld is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEWorld is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/

// This is the NEWorld Common Library
#pragma once
// STL include

#include <cmath>
#include <string>
#include <fstream>
#include <cstring>
#include <utility>
#include <algorithm>
#include <type_traits>
// Math Library
#include "nwmath/nwvector.h"
#include "nwmath/nwmatrix.h"
// Physics Library
#include "nwphysics/nwaabb.h"
// Maintenance Library
#include "maintenance/nwdebug.h"
#include "maintenance/nwlogger.hpp"
#include "maintenance/nwexception.hpp"
// Json Library
#include "nwjson/nwjson.h"
// NEWorld Standard Library
#include "nwstdlib/nwstddef.h"
#include "nwstdlib/nwconcepts.hpp"
#include "nwstdlib/nwconsole.hpp"
#include "nwstdlib/nwfilesys.h"
#include "nwstdlib/nwlibrary.h"
#include "nwstdlib/nwratemeter.h"
#include "nwstdlib/nwstrings.h"
