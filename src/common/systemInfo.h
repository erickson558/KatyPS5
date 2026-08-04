#ifndef KATY_COMMON_SYSTEM_INFO_H_
#define KATY_COMMON_SYSTEM_INFO_H_

#include <string>

namespace Common {

struct SystemInfo {
	std::string ProcessorName;
};

[[nodiscard]] SystemInfo GetSystemInfo();

} // namespace Common

#endif /* KATY_COMMON_SYSTEM_INFO_H_ */
