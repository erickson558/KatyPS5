#ifndef KATY_LIBS_SAVE_DATA_MEMORY_STORE_H_
#define KATY_LIBS_SAVE_DATA_MEMORY_STORE_H_

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace Libs::SaveData {

constexpr size_t SaveMemoryMaxSize = 32 * 1024 * 1024;

enum class SaveMemoryStoreResult { Ok, Missing, Broken, IoError };

struct SaveMemorySnapshot {
	std::vector<uint8_t> data;
	std::vector<uint8_t> param;
};

// Failed loads leave the caller's state intact; a corrupt primary never selects a backup silently.
SaveMemoryStoreResult LoadSaveMemorySnapshot(const std::filesystem::path& directory,
                                             size_t param_size, SaveMemorySnapshot* out);
SaveMemoryStoreResult WriteSaveMemorySnapshot(const std::filesystem::path& directory,
                                              const SaveMemorySnapshot&    snapshot);

} // namespace Libs::SaveData

#endif /* KATY_LIBS_SAVE_DATA_MEMORY_STORE_H_ */
