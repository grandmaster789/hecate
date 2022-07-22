#pragma once

// header-only parts are included here
#include "core/mediator.h"

#include "util/algorithm.h"
#include "util/flat_map.h"
#include "util/function.h"
#include "util/typemap.h"

// pre-declare classes
namespace hecate {
	class Application;
	class Engine;

	namespace core {	
		class Logger;
		class System;
	}

	class Graphics;
	class Platform;
}