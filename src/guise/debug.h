#pragma once

#include <cassert>

#ifdef _DEBUG
#define GUISE_ASSERT(ASSERTION) assert(ASSERTION);
#else
#define GUISE_ASSERT(ASSERTION)
#endif
