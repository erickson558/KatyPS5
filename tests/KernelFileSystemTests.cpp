#include "common/commonSubsystem.h"
#include "common/emulatorConfig.h"
#include "common/logging/log.h"
#include "common/subsystems.h"
#include "common/threads.h"
#include "kernel/fileSystem.h"
#include "libs/errno.h"
#include "libs/network.h"
#include "loader/symbolDatabase.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <system_error>
#include <thread>

namespace Libs {
void InitLibKernel_1_FS(Loader::SymbolDatabase* symbols);
}

namespace {

namespace FileSystem       = Libs::LibKernel::FileSystem;
constexpr char GuestPath[] = "/app0/regression.bin";

void Check(bool value, const char* message) {
	if (!value) {
		std::fprintf(stderr, "KernelFileSystemTests: failed: %s\n", message);
		std::abort();
	}
}

class TempDirectory {
public:
	TempDirectory() {
		const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
		m_path            = std::filesystem::temp_directory_path() /
		                    ("katy_kernel_file_system_" + std::to_string(unique));
		Check(std::filesystem::create_directory(m_path), "create temporary directory");
	}

	~TempDirectory() {
		std::error_code error;
		std::filesystem::remove_all(m_path, error);
	}

	const std::filesystem::path& Path() const { return m_path; }

private:
	std::filesystem::path m_path;
};

void InitSubsystems() {
	static char  arg0[] = "kernel_file_system_tests";
	static char* argv[] = {arg0};
	auto*        slist  = Common::SubsystemsList::Instance();
	auto*        core   = Common::CommonSubsystem::Instance();
	auto*        config = Config::ConfigSubsystem::Instance();
	auto*        thread = Common::ThreadsSubsystem::Instance();
	slist->SetArgs(1, argv);
	slist->Add(thread, {});
	slist->Add(core, {});
	slist->Add(config, {core});
	Check(slist->InitAll(false), "initialize base subsystems");
	Config::ConfigOptions options;
	options.printf_direction = Config::OutputDirection::Silent;
	Config::Load(options);
	slist->Add(Log::LogSubsystem::Instance(), {core, config});
	slist->Add(Libs::Network::NetworkSubsystem::Instance(), {core, thread});
	slist->Add(FileSystem::FileSystemSubsystem::Instance(), {core, thread});
	Check(slist->InitAll(false), "initialize logging, network and filesystem");
}

void TestInvalidSeekReleasesLock() {
	const int fd = FileSystem::KernelOpen(GuestPath, 2, 0);
	Check(fd >= 3, "open file for seek regression");
	Check(FileSystem::KernelLseek(fd, -1, 0) == Libs::LibKernel::KERNEL_ERROR_EINVAL,
	      "reject negative seek");
	// Windows mutexes are recursive: a second call on the same thread hides the leak.
	std::jthread worker([fd] {
		Check(FileSystem::KernelLseek(fd, 0, 0) == 0, "seek from another thread after error");
		char data[8] {};
		Check(FileSystem::KernelRead(fd, data, sizeof(data)) == 8 &&
		          std::memcmp(data, "12345678", 8) == 0,
		      "read after rejected seek");
	});
	worker.join();
	Check(FileSystem::KernelClose(fd) == OK, "close seek regression file");
}

void TestRejectedTruncationReleasesDescriptor() {
	const int first_fd = FileSystem::KernelOpen(GuestPath, 0, 0);
	Check(first_fd >= 3, "open reference descriptor");
	Check(FileSystem::KernelClose(first_fd) == OK, "release reference descriptor");
	for (int attempt = 0; attempt < 64; ++attempt) {
		Check(FileSystem::KernelOpen(GuestPath, 0x0400, 0) == Libs::LibKernel::KERNEL_ERROR_EACCES,
		      "reject read-only truncation");
	}
	const int fd = FileSystem::KernelOpen(GuestPath, 0, 0);
	Check(fd == first_fd, "rejected opens do not consume descriptor slots");
	FileSystem::FileStat stat {};
	Check(FileSystem::KernelFstat(fd, &stat) == OK && stat.st_size == 8,
	      "rejected truncation preserves file data");
	Check(FileSystem::KernelClose(fd) == OK, "close descriptor regression file");
}

void TestNativeFtruncateImport() {
	Loader::SymbolDatabase symbols;
	Libs::InitLibKernel_1_FS(&symbols);
	const auto* symbol =
	    symbols.Find({"VW3TVZiM4-E", "libkernel", 1, "libkernel", 1, 1, Loader::SymbolType::Func});
	Check(symbol != nullptr, "resolve sceKernelFtruncate with its library/module versions");
	Check(symbol->vaddr == reinterpret_cast<uint64_t>(&FileSystem::KernelFtruncate),
	      "ftruncate import uses the native handler");
	const auto truncate = reinterpret_cast<decltype(&FileSystem::KernelFtruncate)>(symbol->vaddr);
	const int  fd       = FileSystem::KernelOpen(GuestPath, 2, 0);
	Check(fd >= 3, "open file for native truncation");
	Check(FileSystem::KernelLseek(fd, 8, 0) == 8, "set position before truncation");
	Check(truncate(fd, -1) == Libs::LibKernel::KERNEL_ERROR_EINVAL,
	      "reject negative truncation length");
	Check(truncate(fd, 4) == OK, "truncate through resolved import");
	Check(FileSystem::KernelLseek(fd, 0, 1) == 8, "truncation preserves file position");
	FileSystem::FileStat stat {};
	Check(FileSystem::KernelFstat(fd, &stat) == OK && stat.st_size == 4,
	      "native truncation updates file size");
	Check(FileSystem::KernelLseek(fd, 0, 0) == 0, "seek to truncated data");
	char data[8] {};
	Check(FileSystem::KernelRead(fd, data, sizeof(data)) == 4 && std::memcmp(data, "1234", 4) == 0,
	      "truncation retains the file prefix");
	Check(FileSystem::KernelClose(fd) == OK, "close truncated file");
	const int read_fd = FileSystem::KernelOpen(GuestPath, 0, 0);
	Check(read_fd >= 3, "open read-only truncation test file");
	Check(truncate(read_fd, 0) == Libs::LibKernel::KERNEL_ERROR_EBADF,
	      "native truncation rejects read-only descriptor");
	Check(FileSystem::KernelClose(read_fd) == OK, "close read-only file");
}

} // namespace

int main() {
	InitSubsystems();
	TempDirectory temp;
	FileSystem::Mount(temp.Path(), "/app0");
	const int fd = FileSystem::KernelOpen(GuestPath, 0x0602, 0600);
	Check(fd >= 3, "create regression file");
	Check(FileSystem::KernelWrite(fd, "12345678", 8) == 8, "write regression data");
	Check(FileSystem::KernelClose(fd) == OK, "close regression data");
	TestInvalidSeekReleasesLock();
	TestRejectedTruncationReleasesDescriptor();
	TestNativeFtruncateImport();
	FileSystem::Umount("/app0");
	std::printf("KernelFileSystemTests: all cases passed\n");
	return 0;
}
