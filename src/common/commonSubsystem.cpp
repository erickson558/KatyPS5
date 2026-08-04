#include "common/commonSubsystem.h"

#include "common/byteBuffer.h" // IWYU pragma: associated
#include "common/common.h"     // IWYU pragma: associated
#include "common/debug.h"      // IWYU pragma: associated
#include "common/hash.h"       // IWYU pragma: associated
#include "common/magicEnum.h"  // IWYU pragma: associated
#include "common/singleton.h"  // IWYU pragma: associated
#include "common/virtualMemory.h"

namespace Common {

KATY_SUBSYSTEM_INIT(Common) {
	VirtualMemory::Init();
}

KATY_SUBSYSTEM_UNEXPECTED_SHUTDOWN(Common) {}

KATY_SUBSYSTEM_DESTROY(Common) {}

} // namespace Common
