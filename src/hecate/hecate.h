#pragma once

// NOTE this should only be included *outside* of the HecateLib project

#include "app/application.h"

#include "core/engine.h"
#include "core/logger.h"
#include "core/mediator.h"
#include "core/system.h"

#include "graphics/device.h"
#include "graphics/surface.h"
#include "graphics/surface_state.h"

#include "util/codec/base_n.h"
#include "util/codec/codepage_1252.h"
#include "util/codec/utf.h"
#include "util/algorithm.h"
#include "util/biguint.h"
#include "util/cast.h"
#include "util/concepts.h"
#include "util/debugger.h"
#include "util/endian.h"
#include "util/filesystem.h"
#include "util/literals.h"
#include "util/memory.h"
#include "util/spin_mutex.h"
#include "util/string_util.h"
#include "util/threads.h"
#include "util/typemap.h"
#include "util/unique_resource.h"