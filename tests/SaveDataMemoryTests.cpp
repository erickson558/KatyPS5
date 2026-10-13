// Exercise the guest APIs and their real storage implementation in this TU.
// The test target excludes the normal libSaveData.cpp TU to avoid duplicate
// exports.
#include "../src/libs/libSaveData.cpp"
#include "common/commonSubsystem.h"
#include "common/emulatorConfig.h"
#include "common/subsystems.h"

#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <system_error>
#include <vector>

namespace {

using namespace Libs::SaveData;
namespace fs = std::filesystem;
constexpr char Title[] = "CUSA00001";

void Check(bool value, const char *message) {
  if (!value) {
    std::fprintf(stderr, "SaveDataMemoryTests: failed: %s\n", message);
    std::abort();
  }
}

void Result(int actual, int expected, const char *message) {
  if (actual != expected) {
    std::fprintf(stderr, "SaveDataMemoryTests: %s: got %d, expected %d\n",
                 message, actual, expected);
    std::abort();
  }
}

void InitSubsystems() {
  static char arg0[] = "save_data_memory_tests";
  static char *argv[] = {arg0};
  auto *slist = Common::SubsystemsList::Instance();
  auto *core = Common::CommonSubsystem::Instance();
  auto *config = Config::ConfigSubsystem::Instance();
  auto *thread = Common::ThreadsSubsystem::Instance();
  slist->SetArgs(1, argv);
  slist->Add(thread, {});
  slist->Add(core, {});
  slist->Add(config, {core});
  Check(slist->InitAll(false), "initialize base subsystems");
  Config::ConfigOptions options;
  options.printf_direction = Config::OutputDirection::Silent;
  Config::Load(options);
  slist->Add(Log::LogSubsystem::Instance(), {core, config});
  Check(slist->InitAll(false), "initialize logging");
}

void SelectTitle(const char *title) {
  const auto path = fs::current_path() / "param.json";
  {
    std::ofstream stream(path, std::ios::binary | std::ios::trunc);
    stream << "{\"titleId\":\"" << title << "\"}";
    Check(stream.good(), "write test title metadata");
  }
  Loader::SystemContentLoadParamSfo(path);
  std::string actual;
  Check(Loader::SystemContentParamSfoGetString("TITLE_ID", &actual) &&
            actual == title,
        "load title through production content loader");
}

class WorkingDirectory {
public:
  explicit WorkingDirectory(const fs::path &path, bool remove_on_exit)
      : m_previous(fs::current_path()), m_path(path), m_remove(remove_on_exit) {
    fs::create_directories(m_path);
    fs::current_path(m_path);
  }
  ~WorkingDirectory() {
    fs::current_path(m_previous);
    if (m_remove) {
      std::error_code error;
      fs::remove_all(m_path, error);
    }
  }

private:
  fs::path m_previous;
  fs::path m_path;
  bool m_remove;
};

fs::path SlotDirectory(int32_t user, uint32_t slot, const char *title = Title) {
  return fs::path("_SaveData") / title /
         ("sce_sdmemory" + (slot == 0 ? std::string() : std::to_string(slot))) /
         std::to_string(user);
}

int Setup(int32_t user, uint32_t slot, size_t size, uint32_t option = 0,
          const SaveDataParam *param = nullptr,
          SaveDataMemorySetupResult *result = nullptr) {
  SaveDataMemorySetup2 setup{};
  setup.user_id = user;
  setup.slot_id = slot;
  setup.memory_size = size;
  setup.option = option;
  setup.init_param = param;
  return SaveDataSetupSaveDataMemory2(&setup, result);
}

int Put(int32_t user, uint32_t slot, const void *bytes, size_t size,
        int64_t offset = 0, const SaveDataParam *param = nullptr) {
  SaveDataMemoryData data{};
  data.buf = const_cast<void *>(bytes);
  data.buf_size = size;
  data.offset = offset;
  SaveDataMemorySet2 set{};
  set.user_id = user;
  set.slot_id = slot;
  set.data = &data;
  set.param = param;
  // Keep the fork's data_num=0 convention: non-null data represents one
  // descriptor.
  return SaveDataSetSaveDataMemory2(&set);
}

int Get(int32_t user, uint32_t slot, void *bytes, size_t size,
        int64_t offset = 0, SaveDataParam *param = nullptr) {
  SaveDataMemoryData data{};
  data.buf = bytes;
  data.buf_size = size;
  data.offset = offset;
  SaveDataMemoryGet2 get{};
  get.user_id = user;
  get.slot_id = slot;
  get.data = &data;
  get.param = param;
  return SaveDataGetSaveDataMemory2(&get);
}

std::vector<uint8_t> ReadFile(const fs::path &path) {
  std::ifstream stream(path, std::ios::binary);
  Check(stream.good(), "open snapshot for verification");
  return std::vector<uint8_t>(std::istreambuf_iterator<char>(stream),
                              std::istreambuf_iterator<char>());
}

void WriteFile(const fs::path &path, const void *bytes, size_t size) {
  fs::create_directories(path.parent_path());
  std::ofstream stream(path, std::ios::binary | std::ios::trunc);
  stream.write(static_cast<const char *>(bytes),
               static_cast<std::streamsize>(size));
  Check(stream.good(), "write regression file");
}

void ClearMemory() {
  Result(SaveDataTerminate(), OK, "terminate and discard active memory");
}

void TestIsolationAndMetadata() {
  SelectTitle(Title);
  SaveDataParam initial{};
  std::strcpy(initial.title, "Saved progress");
  initial.user_param = 21;
  SaveDataMemorySetupResult result{};
  Result(Setup(10, 0, 32, 1, &initial, &result), OK,
         "setup memory with initial metadata");
  Check(result.existed_memory_size == 0, "new slot reports no prior memory");
  Result(Setup(10, 0, 32), SAVE_DATA_ERROR_BUSY,
         "duplicate setup does not replace state");
  Result(Put(10, 0, "first", 5), OK, "implicit one-descriptor write");
  Result(Setup(11, 0, 32), OK, "different user has own slot");
  Result(Put(11, 0, "user!", 5), OK, "write other user");
  Result(Setup(10, 1, 32), OK, "different slot has own state");
  Result(Put(10, 1, "slot!", 5), OK, "write other slot");
  SelectTitle("CUSA00002");
  Result(Setup(10, 0, 32), OK, "different title has own state");
  Result(Put(10, 0, "title", 5), OK, "write other title");
  ClearMemory();
  SelectTitle(Title);
  Result(Setup(10, 0, 48, 1, nullptr, &result), OK,
         "reload and extend existing snapshot");
  Check(result.existed_memory_size == 32,
        "setup reports saved size before extension");
  std::array<char, 48> bytes{};
  SaveDataParam loaded{};
  Result(Get(10, 0, bytes.data(), bytes.size(), 0, &loaded), OK,
         "get persisted metadata/data");
  Check(std::memcmp(bytes.data(), "first", 5) == 0 && bytes[47] == 0,
        "snapshot prefix survives extension, extension is zero-filled");
  Check(std::strcmp(loaded.title, initial.title) == 0 &&
            loaded.user_param == 21,
        "initial metadata is persisted");
  Result(Setup(11, 0, 32), OK, "reload other user");
  Result(Get(11, 0, bytes.data(), 5), OK, "read other user");
  Check(std::memcmp(bytes.data(), "user!", 5) == 0, "user isolation persists");
  Result(Setup(10, 1, 32), OK, "reload other slot");
  Result(Get(10, 1, bytes.data(), 5), OK, "read other slot");
  Check(std::memcmp(bytes.data(), "slot!", 5) == 0, "slot isolation persists");
  ClearMemory();
  SelectTitle("CUSA00002");
  Result(Setup(10, 0, 32), OK, "reload other title");
  Result(Get(10, 0, bytes.data(), 5), OK, "read other title");
  Check(std::memcmp(bytes.data(), "title", 5) == 0, "title isolation persists");
  ClearMemory();
  SelectTitle(Title);
}

void TestValidationIsAtomic() {
  constexpr int32_t user = 20;
  SaveDataParam original{};
  original.user_param = 12;
  Result(Setup(user, 0, 16, 1, &original), OK, "setup validation slot");
  Result(Put(user, 0, "safe", 4), OK, "seed validation snapshot");
  const auto primary = SlotDirectory(user, 0) / "memory.bin";
  const auto before = ReadFile(primary);
  char replacement[] = "lost";
  SaveDataMemoryData data[2]{};
  data[0].buf = replacement;
  data[0].buf_size = 4;
  data[1].buf = replacement;
  data[1].buf_size = 4;
  data[1].offset = 15;
  SaveDataMemorySet2 set{};
  set.user_id = user;
  set.data = data;
  set.data_num = 2;
  SaveDataParam changed{};
  changed.user_param = 99;
  set.param = &changed;
  Result(SaveDataSetSaveDataMemory2(&set), SAVE_DATA_ERROR_PARAMETER,
         "invalid second descriptor rejects whole batch");
  char bytes[4]{};
  SaveDataParam loaded{};
  Result(Get(user, 0, bytes, sizeof(bytes), 0, &loaded), OK,
         "read after rejected batch");
  Check(std::memcmp(bytes, "safe", 4) == 0 &&
            loaded.user_param == original.user_param &&
            ReadFile(primary) == before,
        "rejected batch changes neither data, metadata nor disk");
  Result(Put(user, 0, replacement, 4, -1), SAVE_DATA_ERROR_PARAMETER,
         "negative write offset");
  Result(Put(user, 0, replacement, std::numeric_limits<size_t>::max(), 1),
         SAVE_DATA_ERROR_PARAMETER, "overflowing write range");
  Result(Put(user, 0, nullptr, 1), SAVE_DATA_ERROR_PARAMETER,
         "null write buffer");
  Result(Get(user, 0, bytes, 4, 15), SAVE_DATA_ERROR_PARAMETER,
         "read past end does not extend memory");
  Result(Get(user, 0, bytes, std::numeric_limits<size_t>::max(), 1),
         SAVE_DATA_ERROR_PARAMETER, "overflowing read range");
  Result(Get(user, 0, bytes, 4, -1), SAVE_DATA_ERROR_PARAMETER,
         "negative read offset");
  Result(Get(user, 0, nullptr, 1), SAVE_DATA_ERROR_PARAMETER,
         "null read buffer");
  Check(std::memcmp(bytes, "safe", 4) == 0,
        "invalid reads leave output unchanged");
  Result(Get(user, 0, bytes, 0, 16), OK,
         "zero-length read exactly at end boundary");
  set.data_num = 6;
  Result(SaveDataSetSaveDataMemory2(&set), SAVE_DATA_ERROR_PARAMETER,
         "too many descriptors rejected before accessing array");
  Result(SaveDataSetSaveDataMemory2(nullptr), SAVE_DATA_ERROR_PARAMETER,
         "null set argument");
  Result(SaveDataGetSaveDataMemory2(nullptr), SAVE_DATA_ERROR_PARAMETER,
         "null get argument");
  Result(SaveDataSetupSaveDataMemory2(nullptr, nullptr),
         SAVE_DATA_ERROR_PARAMETER, "null setup argument");
  Check(ReadFile(primary) == before,
        "all invalid operations preserve snapshot");
  ClearMemory();
  Result(Setup(user, 0, 8), SAVE_DATA_ERROR_PARAMETER,
         "smaller setup preserves saved tail");
  Check(ReadFile(primary) == before,
        "rejected shrink preserves original snapshot");
  Result(Get(user, 0, bytes, 4), SAVE_DATA_ERROR_MEMORY_NOT_READY,
         "failed setup creates no active state");
}

void TestLimits() {
  constexpr size_t mib = 1024 * 1024;
  Result(Setup(-1, 0, 16), SAVE_DATA_ERROR_INVALID_LOGIN_USER,
         "negative user rejected");
  Result(Setup(30, 4, 16), SAVE_DATA_ERROR_PARAMETER, "fifth slot rejected");
  Result(Setup(30, 0, 0), SAVE_DATA_ERROR_PARAMETER, "empty setup rejected");
  Result(Setup(30, 0, 16, 4), SAVE_DATA_ERROR_PARAMETER,
         "unsupported setup options rejected");
  Result(Setup(30, 0, 33 * mib), SAVE_DATA_ERROR_PARAMETER,
         "oversized setup rejected");
  Result(Setup(30, 0, 17 * mib, 2), SAVE_DATA_ERROR_LIMITATION_OVER,
         "double buffer charged against total budget");
  Result(Setup(30, 0, 17 * mib), OK, "single buffer fits budget");
  Result(Setup(30, 1, 17 * mib), SAVE_DATA_ERROR_LIMITATION_OVER,
         "all active buffers share bounded budget");
  ClearMemory();
  Result(Setup(30, 0, 16 * mib, 2), OK, "exact double-buffer budget fits");
  ClearMemory();
  for (int32_t user = 31; user < 35; ++user) {
    Result(Setup(user, 0, 16), OK,
           "four independent small active states permitted");
  }
  Result(Setup(35, 0, 16), SAVE_DATA_ERROR_LIMITATION_OVER,
         "fifth active state rejected despite small buffers");
  ClearMemory();
}

void TestSyncAndMetadataPermission() {
  constexpr int32_t user = 36;
  Result(Setup(user, 0, 16), OK, "setup slot without metadata permission");
  SaveDataParam param{};
  param.user_param = 36;
  Result(Put(user, 0, "skip", 4, 0, &param), SAVE_DATA_ERROR_PARAMETER,
         "set metadata requires setup option one");
  char bytes[4]{};
  Result(Get(user, 0, bytes, 4, 0, &param), SAVE_DATA_ERROR_PARAMETER,
         "get metadata requires setup option one");
  Check(!fs::exists(SlotDirectory(user, 0) / "memory.bin"),
        "rejected metadata update creates no snapshot");
  SaveDataMemorySync sync{};
  sync.user_id = user;
  sync.option = 1;
  Result(SaveDataSyncSaveDataMemory(&sync), OK,
         "direct sync persists setup-only data");
  Check(fs::exists(SlotDirectory(user, 0) / "memory.bin"),
        "direct sync writes snapshot");
  SaveDataEvent event{};
  Result(SaveDataGetEventResult(nullptr, &event), SAVE_DATA_ERROR_NOT_FOUND,
         "direct sync does not queue completion");
  sync.option = 0;
  Result(SaveDataSyncSaveDataMemory(&sync), OK,
         "event-based sync accepts operation");
  Result(SaveDataGetEventResult(nullptr, &event), OK,
         "retrieve sync completion");
  Check(event.type == 3 && event.error_code == OK && event.user_id == user &&
            std::strcmp(event.title_id.data, Title) == 0 &&
            std::strcmp(event.dir_name.data, "sce_sdmemory") == 0,
        "sync completion identifies actual persisted user/title/slot");
  Result(SaveDataSyncSaveDataMemory(nullptr), SAVE_DATA_ERROR_PARAMETER,
         "null sync argument rejected");
  sync.option = 2;
  Result(SaveDataSyncSaveDataMemory(&sync), SAVE_DATA_ERROR_PARAMETER,
         "unsupported sync option rejected");
  ClearMemory();
  sync.option = 1;
  Result(SaveDataSyncSaveDataMemory(&sync), SAVE_DATA_ERROR_MEMORY_NOT_READY,
         "sync requires active memory");
}

void TestLegacyAndBackup() {
  constexpr int32_t user = 40;
  const auto directory = SlotDirectory(user, 0);
  const char legacy[] = "legacy-save";
  SaveDataParam metadata{};
  std::strcpy(metadata.title, "Legacy metadata");
  metadata.user_param = 42;
  WriteFile(directory / "memory.dat", legacy, sizeof(legacy));
  WriteFile(directory / "param.bin", &metadata, sizeof(metadata));
  const auto legacy_before = ReadFile(directory / "memory.dat");
  const auto param_before = ReadFile(directory / "param.bin");
  Result(Setup(user, 0, 32, 1), OK, "import legacy files");
  Check(!fs::exists(directory / "memory.bin"),
        "setup import does not write snapshot");
  char bytes[32]{};
  SaveDataParam loaded{};
  Result(Get(user, 0, bytes, sizeof(legacy), 0, &loaded), OK,
         "read imported data/metadata");
  Check(std::memcmp(bytes, legacy, sizeof(legacy)) == 0 &&
            loaded.user_param == 42,
        "legacy payload and metadata retained");
  Result(Put(user, 0, "first-save", 10), OK, "first snapshot after import");
  const auto primary = directory / "memory.bin";
  const auto first = ReadFile(primary);
  Result(Put(user, 0, "next--save", 10), OK, "update snapshot with backup");
  Check(ReadFile(directory / "memory.bin.bak") == first,
        "backup is previous valid snapshot");
  Check(ReadFile(directory / "memory.dat") == legacy_before &&
            ReadFile(directory / "param.bin") == param_before,
        "import originals never modified");
  ClearMemory();
  // Explicit recovery from the retained backup is a user operation, not
  // automatic repair.
  fs::copy_file(directory / "memory.bin.bak", primary,
                fs::copy_options::overwrite_existing);
  Result(Setup(user, 0, 32, 1), OK, "load explicitly restored backup");
  Result(Get(user, 0, bytes, 10), OK, "read recovered data");
  Check(std::memcmp(bytes, "first-save", 10) == 0,
        "explicit backup restores previous progress");
  ClearMemory();
  const auto backup_before = ReadFile(directory / "memory.bin.bak");
  auto corrupt = ReadFile(primary);
  Check(!corrupt.empty(), "snapshot has content");
  corrupt.back() ^= 0x5a;
  WriteFile(primary, corrupt.data(), corrupt.size());
  Result(Setup(user, 0, 32, 1), SAVE_DATA_ERROR_BROKEN,
         "corrupt snapshot rejected");
  Check(ReadFile(primary) == corrupt &&
            ReadFile(directory / "memory.bin.bak") == backup_before,
        "corruption detection preserves primary and recoverable backup");
  Result(Get(user, 0, bytes, 4), SAVE_DATA_ERROR_MEMORY_NOT_READY,
         "corrupt load creates no active state");
  WriteFile(primary, first.data(), 8);
  const auto truncated = ReadFile(primary);
  Result(Setup(user, 0, 32, 1), SAVE_DATA_ERROR_BROKEN,
         "truncated header rejected");
  Check(ReadFile(primary) == truncated,
        "truncated primary is retained for diagnosis");
  auto oversized = first;
  Check(oversized.size() > 24, "snapshot has versioned size fields");
  std::fill(oversized.begin() + 16, oversized.begin() + 24, uint8_t{0xff});
  WriteFile(primary, oversized.data(), oversized.size());
  Result(Setup(user, 0, 32, 1), SAVE_DATA_ERROR_BROKEN,
         "oversized header rejected");
  Check(ReadFile(primary) == oversized &&
            ReadFile(directory / "memory.bin.bak") == backup_before,
        "oversized header cannot consume memory or overwrite backup");
}

void TestFailedWriteKeepsMemory() {
  constexpr int32_t user = 50;
  const auto directory = SlotDirectory(user, 0);
  Result(Setup(user, 0, 16), OK, "setup write-failure slot");
  Result(Put(user, 0, "safe", 4), OK, "seed write-failure slot");
  const auto before = ReadFile(directory / "memory.bin");
  // A regular file at the slot directory forces failure even under privileged
  // CI users.
  const auto moved = directory.parent_path() / "retained-slot";
  fs::rename(directory, moved);
  WriteFile(directory, "block", 5);
  Result(Put(user, 0, "lost", 4), SAVE_DATA_ERROR_INTERNAL,
         "failed write reports error");
  SaveDataMemorySync sync{};
  sync.user_id = user;
  sync.option = 1;
  Result(SaveDataSyncSaveDataMemory(&sync), SAVE_DATA_ERROR_INTERNAL,
         "direct sync returns persistence error");
  sync.option = 0;
  Result(SaveDataSyncSaveDataMemory(&sync), OK,
         "event sync schedules failed operation");
  SaveDataEvent event{};
  Result(SaveDataGetEventResult(nullptr, &event), OK,
         "failed sync completion is delivered");
  Check(event.type == 3 && event.error_code == SAVE_DATA_ERROR_INTERNAL &&
            event.user_id == user,
        "event sync reports real write error instead of success");
  char bytes[4]{};
  Result(Get(user, 0, bytes, 4), OK, "read after failed persistence");
  Check(std::memcmp(bytes, "safe", 4) == 0 &&
            ReadFile(moved / "memory.bin") == before,
        "failed write commits neither memory nor disk");
  fs::remove(directory);
  fs::rename(moved, directory);
  ClearMemory();
  Result(Setup(user, 0, 16), OK, "reload after filesystem repaired");
  Result(Get(user, 0, bytes, 4), OK, "read retained snapshot after repair");
  Check(std::memcmp(bytes, "safe", 4) == 0,
        "previous progress survives write failure");
  ClearMemory();
}

void TestMalformedLegacy() {
  const auto directory = SlotDirectory(60, 0);
  WriteFile(directory / "memory.dat", "old", 3);
  WriteFile(directory / "param.bin", "short", 5);
  Result(Setup(60, 0, 16, 1), SAVE_DATA_ERROR_BROKEN,
         "truncated legacy metadata rejected");
  Check(ReadFile(directory / "memory.dat") ==
                std::vector<uint8_t>({'o', 'l', 'd'}) &&
            fs::file_size(directory / "param.bin") == 5 &&
            !fs::exists(directory / "memory.bin"),
        "malformed import retains both originals");
  fs::remove(directory / "param.bin");
  fs::resize_file(directory / "memory.dat", 33 * 1024 * 1024);
  Result(Setup(60, 0, 16), SAVE_DATA_ERROR_BROKEN,
         "oversized legacy data rejected before reading into memory");
  Check(fs::file_size(directory / "memory.dat") == 33 * 1024 * 1024 &&
            !fs::exists(directory / "memory.bin"),
        "oversized legacy data remains untouched");
}

void WriteRestartFixture() {
  SelectTitle(Title);
  SaveDataParam metadata{};
  std::strcpy(metadata.title, "Restart checkpoint");
  metadata.user_param = 12345;
  Result(Setup(100, 0, 64, 1, &metadata), OK, "restart writer setup");
  Result(Put(100, 0, "old-progress", 12), OK,
         "restart writer first checkpoint");
  metadata.user_param = 54321;
  Result(Put(100, 0, "new-progress", 12, 7, &metadata), OK,
         "restart writer final checkpoint");
  // Deliberately do not call Sync or Terminate: a successful Set must already
  // persist.
}

void ReadRestartFixture() {
  SelectTitle(Title);
  char bytes[64]{};
  Result(Get(100, 0, bytes, 4), SAVE_DATA_ERROR_MEMORY_NOT_READY,
         "fresh reader process starts with no active memory");
  SaveDataMemorySetupResult result{};
  Result(Setup(100, 0, 64, 1, nullptr, &result), OK,
         "fresh process loads writer snapshot");
  Check(result.existed_memory_size == 64,
        "persisted allocation size survives process restart");
  SaveDataParam metadata{};
  Result(Get(100, 0, bytes, sizeof(bytes), 0, &metadata), OK,
         "fresh process reads checkpoint");
  Check(std::memcmp(bytes + 7, "new-progress", 12) == 0 &&
            metadata.user_param == 54321 &&
            std::strcmp(metadata.title, "Restart checkpoint") == 0,
        "real process restart retains data, offsets and metadata");
  Check(fs::exists(SlotDirectory(100, 0) / "memory.bin.bak"),
        "previous checkpoint remains available after restart");
  ClearMemory();
}

} // namespace

int main(int argc, char **argv) {
  if (argc == 3 && std::strcmp(argv[1], "--cleanup") == 0) {
    const auto path = fs::absolute(argv[2]).lexically_normal();
    Check(path.filename() == "save-memory-regression",
          "cleanup only designated fixture");
    std::error_code error;
    fs::remove_all(path, error);
    Check(!error, "remove restart fixture directory");
    return 0;
  }
  InitSubsystems();
  if (argc == 3) {
    WorkingDirectory directory(fs::absolute(argv[2]), false);
    if (std::strcmp(argv[1], "--write") == 0) {
      Check(fs::current_path().filename() == "save-memory-regression",
            "writer only designated fixture");
      const auto savedata = fs::current_path() / "_SaveData";
      Check(savedata.parent_path() == fs::current_path(),
            "fixture cleanup remains contained");
      fs::remove_all(savedata);
      WriteRestartFixture();
    } else if (std::strcmp(argv[1], "--read") == 0) {
      ReadRestartFixture();
    } else {
      Check(false, "unknown test mode");
    }
  } else {
    Check(argc == 1, "expected no arguments or fixture mode and directory");
    const auto unique =
        std::chrono::steady_clock::now().time_since_epoch().count();
    WorkingDirectory directory(
        fs::temp_directory_path() /
            ("katy_save_memory_" + std::to_string(unique)),
        true);
    TestIsolationAndMetadata();
    TestValidationIsAtomic();
    TestLimits();
    TestSyncAndMetadataPermission();
    TestLegacyAndBackup();
    TestFailedWriteKeepsMemory();
    TestMalformedLegacy();
  }
  std::printf("SaveDataMemoryTests: all cases passed\n");
  return 0;
}
