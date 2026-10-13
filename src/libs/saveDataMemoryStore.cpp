#include "libs/saveDataMemoryStore.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <fstream>
#include <mutex>
#include <new>
#include <string>
#include <system_error>
#include <utility>

#if defined(_WIN32)
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

namespace Libs::SaveData {
namespace {

constexpr size_t                 HeaderSize   = 40;
constexpr size_t                 ParamMaxSize = 4096;
constexpr uint64_t               FnvOffset    = 14695981039346656037ULL;
constexpr uint64_t               FnvPrime     = 1099511628211ULL;
constexpr std::array<uint8_t, 8> Magic        = {'K', 'S', 'D', 'M', 'E', 'M', '0', '1'};
std::mutex                       g_store_mutex;
std::atomic<uint64_t>            g_temp_sequence {0};

SaveMemoryStoreResult ValidateDirectory(const std::filesystem::path& directory) {
	std::error_code error;
	const auto      absolute = std::filesystem::absolute(directory, error);
	if (error) {
		return SaveMemoryStoreResult::IoError;
	}
	std::filesystem::path current;
	for (const auto& component: absolute) {
		current /= component;
		const auto status = std::filesystem::symlink_status(current, error);
		if (error == std::errc::no_such_file_or_directory ||
		    (!error && status.type() == std::filesystem::file_type::not_found)) {
			return SaveMemoryStoreResult::Missing;
		}
		if (error || !std::filesystem::is_directory(status)) {
			return SaveMemoryStoreResult::IoError;
		}
	}
	return SaveMemoryStoreResult::Ok;
}

void StoreInteger(uint8_t* destination, uint64_t value, size_t bytes) {
	for (size_t i = 0; i < bytes; i++) {
		destination[i] = static_cast<uint8_t>(value >> (i * 8));
	}
}

uint64_t LoadInteger(const uint8_t* source, size_t bytes) {
	uint64_t value = 0;
	for (size_t i = 0; i < bytes; i++) {
		value |= static_cast<uint64_t>(source[i]) << (i * 8);
	}
	return value;
}

uint64_t Checksum(uint64_t hash, const uint8_t* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		hash = (hash ^ data[i]) * FnvPrime;
	}
	return hash;
}

SaveMemoryStoreResult FileSize(const std::filesystem::path& path, uintmax_t* size) {
	std::error_code error;
	const auto      status = std::filesystem::symlink_status(path, error);
	if (error == std::errc::no_such_file_or_directory ||
	    (!error && status.type() == std::filesystem::file_type::not_found)) {
		return SaveMemoryStoreResult::Missing;
	}
	if (error || !std::filesystem::is_regular_file(status)) {
		return SaveMemoryStoreResult::IoError;
	}
	*size = std::filesystem::file_size(path, error);
	return error ? SaveMemoryStoreResult::IoError : SaveMemoryStoreResult::Ok;
}

bool ReadBytes(std::ifstream& file, uint8_t* data, size_t size) {
	if (size == 0) {
		return true;
	}
	file.read(reinterpret_cast<char*>(data), static_cast<std::streamsize>(size));
	return file.gcount() == static_cast<std::streamsize>(size);
}

bool ValidHeader(const std::array<uint8_t, HeaderSize>& header, uintmax_t file_size) {
	const auto data_size  = LoadInteger(header.data() + 16, 8);
	const auto param_size = LoadInteger(header.data() + 24, 8);
	return std::equal(Magic.begin(), Magic.end(), header.begin()) &&
	       LoadInteger(header.data() + 8, 4) == 1 &&
	       LoadInteger(header.data() + 12, 4) == HeaderSize && data_size != 0 &&
	       data_size <= SaveMemoryMaxSize && param_size <= ParamMaxSize &&
	       file_size == HeaderSize + data_size + param_size;
}

SaveMemoryStoreResult ValidatePrevious(const std::filesystem::path& path, uintmax_t size) {
	std::ifstream                   file(path, std::ios::binary);
	std::array<uint8_t, HeaderSize> header {};
	if (!file || !ReadBytes(file, header.data(), header.size())) {
		return SaveMemoryStoreResult::IoError;
	}
	if (!ValidHeader(header, size)) {
		return SaveMemoryStoreResult::Broken;
	}
	// Verify the previous snapshot without another 32 MiB allocation before protecting it as
	// backup.
	uintmax_t                  remaining = size - HeaderSize;
	std::array<uint8_t, 32768> buffer {};
	auto                       checksum = Checksum(FnvOffset, header.data(), 32);
	while (remaining != 0) {
		const auto bytes = static_cast<size_t>(std::min<uintmax_t>(remaining, buffer.size()));
		if (!ReadBytes(file, buffer.data(), bytes)) {
			return SaveMemoryStoreResult::IoError;
		}
		checksum = Checksum(checksum, buffer.data(), bytes);
		remaining -= bytes;
	}
	return checksum == LoadInteger(header.data() + 32, 8) ? SaveMemoryStoreResult::Ok
	                                                      : SaveMemoryStoreResult::Broken;
}

SaveMemoryStoreResult LoadLegacy(const std::filesystem::path& directory, size_t param_size,
                                 SaveMemorySnapshot* out) {
	const auto data_path = directory / "memory.dat";
	uintmax_t  data_size = 0;
	auto       result    = FileSize(data_path, &data_size);
	if (result != SaveMemoryStoreResult::Ok) {
		return result;
	}
	if (data_size == 0 || data_size > SaveMemoryMaxSize) {
		return SaveMemoryStoreResult::Broken;
	}
	SaveMemorySnapshot snapshot;
	snapshot.data.resize(static_cast<size_t>(data_size));
	std::ifstream data_file(data_path, std::ios::binary);
	if (!data_file || !ReadBytes(data_file, snapshot.data.data(), snapshot.data.size())) {
		return SaveMemoryStoreResult::IoError;
	}
	const auto param_path  = directory / "param.bin";
	uintmax_t  param_bytes = 0;
	result                 = FileSize(param_path, &param_bytes);
	if (result != SaveMemoryStoreResult::Missing) {
		if (result != SaveMemoryStoreResult::Ok) {
			return result;
		}
		if (param_bytes != param_size) {
			return SaveMemoryStoreResult::Broken;
		}
		snapshot.param.resize(param_size);
		std::ifstream param_file(param_path, std::ios::binary);
		if (!param_file || !ReadBytes(param_file, snapshot.param.data(), snapshot.param.size())) {
			return SaveMemoryStoreResult::IoError;
		}
	}
	*out = std::move(snapshot);
	return SaveMemoryStoreResult::Ok;
}

std::filesystem::path TemporaryPath(const std::filesystem::path& directory, const char* purpose) {
#if defined(_WIN32)
	const auto process_id = GetCurrentProcessId();
#else
	const auto process_id = getpid();
#endif
	return directory /
	       (std::string(purpose) + "." + std::to_string(process_id) + "." +
	        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "." +
	        std::to_string(g_temp_sequence.fetch_add(1)) + ".tmp");
}

bool ReplaceFileAtomically(const std::filesystem::path& source,
                           const std::filesystem::path& destination) {
	// Keep the old destination until the same-volume rename commits. No delete/copy fallback.
#if defined(_WIN32)
	return MoveFileExW(source.c_str(), destination.c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
#else
	std::error_code error;
	std::filesystem::rename(source, destination, error);
	return !error;
#endif
}

class TemporarySnapshot {
public:
	explicit TemporarySnapshot(std::filesystem::path path): m_path(std::move(path)) {}
	~TemporarySnapshot() {
		if (m_created) {
			std::error_code error;
			std::filesystem::remove(m_path, error);
		}
	}
	TemporarySnapshot(const TemporarySnapshot&)            = delete;
	TemporarySnapshot& operator=(const TemporarySnapshot&) = delete;

	const std::filesystem::path& Path() const { return m_path; }
	void                         Created() { m_created = true; }

private:
	std::filesystem::path m_path;
	bool                  m_created = false;
};

class TemporaryOutput {
public:
	explicit TemporaryOutput(TemporarySnapshot& temporary) {
		// Exclusive creation prevents stale temporaries or host symlinks from being truncated.
#if defined(_WIN32)
		m_file = CreateFileW(temporary.Path().c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
		                     FILE_ATTRIBUTE_NORMAL, nullptr);
#else
		m_file = open(temporary.Path().c_str(), O_WRONLY | O_CREAT | O_EXCL, 0600);
#endif
		if (Valid()) {
			temporary.Created();
		}
	}
	~TemporaryOutput() { Close(); }
	TemporaryOutput(const TemporaryOutput&)            = delete;
	TemporaryOutput& operator=(const TemporaryOutput&) = delete;

	bool Valid() const {
#if defined(_WIN32)
		return m_file != INVALID_HANDLE_VALUE;
#else
		return m_file >= 0;
#endif
	}

	bool Write(const uint8_t* bytes, size_t size) {
		while (size != 0) {
#if defined(_WIN32)
			DWORD written = 0;
			if (WriteFile(m_file, bytes, static_cast<DWORD>(size), &written, nullptr) == 0 ||
			    written == 0) {
				return false;
			}
#else
			const auto written = ::write(m_file, bytes, size);
			if (written < 0 && errno == EINTR) {
				continue;
			}
			if (written <= 0) {
				return false;
			}
#endif
			bytes += written;
			size -= static_cast<size_t>(written);
		}
		return true;
	}

	bool Close() {
		if (!Valid()) {
			return false;
		}
#if defined(_WIN32)
		const bool closed = CloseHandle(m_file) != 0;
		m_file            = INVALID_HANDLE_VALUE;
#else
		const bool closed = close(m_file) == 0;
		m_file            = -1;
#endif
		return closed;
	}

private:
#if defined(_WIN32)
	HANDLE m_file = INVALID_HANDLE_VALUE;
#else
	int m_file = -1;
#endif
};

bool WriteTemporary(TemporarySnapshot& temporary, const std::array<uint8_t, HeaderSize>& header,
                    const SaveMemorySnapshot& snapshot) {
	TemporaryOutput file(temporary);
	if (!file.Valid()) {
		return false;
	}
	const bool written = file.Write(header.data(), header.size()) &&
	                     file.Write(snapshot.data.data(), snapshot.data.size()) &&
	                     file.Write(snapshot.param.data(), snapshot.param.size());
	const bool closed  = file.Close();
	return written && closed;
}

bool CopyTemporary(const std::filesystem::path& source, uintmax_t size,
                   TemporarySnapshot& temporary) {
	std::ifstream input(source, std::ios::binary);
	if (!input) {
		return false;
	}
	TemporaryOutput output(temporary);
	if (!output.Valid()) {
		return false;
	}
	std::array<uint8_t, 32768> buffer {};
	while (size != 0) {
		const auto bytes = static_cast<size_t>(std::min<uintmax_t>(size, buffer.size()));
		if (!ReadBytes(input, buffer.data(), bytes) || !output.Write(buffer.data(), bytes)) {
			return false;
		}
		size -= bytes;
	}
	return output.Close();
}

} // namespace

SaveMemoryStoreResult LoadSaveMemorySnapshot(const std::filesystem::path& directory,
                                             size_t param_size, SaveMemorySnapshot* out) {
	if (out == nullptr || param_size > ParamMaxSize) {
		return SaveMemoryStoreResult::Broken;
	}
	std::lock_guard lock(g_store_mutex);
	try {
		const auto directory_result = ValidateDirectory(directory);
		if (directory_result != SaveMemoryStoreResult::Ok) {
			return directory_result;
		}
		const auto path      = directory / "memory.bin";
		uintmax_t  file_size = 0;
		const auto result    = FileSize(path, &file_size);
		if (result == SaveMemoryStoreResult::Missing) {
			return LoadLegacy(directory, param_size, out);
		}
		if (result != SaveMemoryStoreResult::Ok) {
			return result;
		}
		if (file_size < HeaderSize || file_size > HeaderSize + SaveMemoryMaxSize + ParamMaxSize) {
			return SaveMemoryStoreResult::Broken;
		}
		std::ifstream                   file(path, std::ios::binary);
		std::array<uint8_t, HeaderSize> header {};
		if (!file || !ReadBytes(file, header.data(), header.size())) {
			return SaveMemoryStoreResult::IoError;
		}
		const auto data_size         = LoadInteger(header.data() + 16, 8);
		const auto stored_param_size = LoadInteger(header.data() + 24, 8);
		if (!ValidHeader(header, file_size) || stored_param_size != param_size) {
			return SaveMemoryStoreResult::Broken;
		}
		SaveMemorySnapshot snapshot;
		snapshot.data.resize(static_cast<size_t>(data_size));
		snapshot.param.resize(param_size);
		if (!ReadBytes(file, snapshot.data.data(), snapshot.data.size()) ||
		    !ReadBytes(file, snapshot.param.data(), snapshot.param.size())) {
			return SaveMemoryStoreResult::IoError;
		}
		auto checksum = Checksum(FnvOffset, header.data(), 32);
		checksum      = Checksum(checksum, snapshot.data.data(), snapshot.data.size());
		checksum      = Checksum(checksum, snapshot.param.data(), snapshot.param.size());
		if (checksum != LoadInteger(header.data() + 32, 8)) {
			return SaveMemoryStoreResult::Broken;
		}
		*out = std::move(snapshot);
		return SaveMemoryStoreResult::Ok;
	} catch (const std::bad_alloc&) {
		return SaveMemoryStoreResult::IoError;
	} catch (const std::filesystem::filesystem_error&) {
		return SaveMemoryStoreResult::IoError;
	}
}

SaveMemoryStoreResult WriteSaveMemorySnapshot(const std::filesystem::path& directory,
                                              const SaveMemorySnapshot&    snapshot) {
	if (snapshot.data.empty() || snapshot.data.size() > SaveMemoryMaxSize ||
	    snapshot.param.size() > ParamMaxSize) {
		return SaveMemoryStoreResult::Broken;
	}
	std::lock_guard lock(g_store_mutex);
	try {
		const auto directory_result = ValidateDirectory(directory);
		if (directory_result != SaveMemoryStoreResult::Ok &&
		    directory_result != SaveMemoryStoreResult::Missing) {
			return directory_result;
		}
		std::error_code error;
		std::filesystem::create_directories(directory, error);
		if (error || ValidateDirectory(directory) != SaveMemoryStoreResult::Ok) {
			return SaveMemoryStoreResult::IoError;
		}
		const auto path          = directory / "memory.bin";
		uintmax_t  previous_size = 0;
		const auto previous      = FileSize(path, &previous_size);
		if (previous != SaveMemoryStoreResult::Ok && previous != SaveMemoryStoreResult::Missing) {
			return previous;
		}
		if (previous == SaveMemoryStoreResult::Ok &&
		    (previous_size < HeaderSize ||
		     previous_size > HeaderSize + SaveMemoryMaxSize + ParamMaxSize)) {
			return SaveMemoryStoreResult::Broken;
		}
		if (previous == SaveMemoryStoreResult::Ok) {
			const auto validation = ValidatePrevious(path, previous_size);
			if (validation != SaveMemoryStoreResult::Ok) {
				return validation;
			}
		}
		std::array<uint8_t, HeaderSize> header {};
		std::copy(Magic.begin(), Magic.end(), header.begin());
		StoreInteger(header.data() + 8, 1, 4);
		StoreInteger(header.data() + 12, HeaderSize, 4);
		StoreInteger(header.data() + 16, snapshot.data.size(), 8);
		StoreInteger(header.data() + 24, snapshot.param.size(), 8);
		auto checksum = Checksum(FnvOffset, header.data(), 32);
		checksum      = Checksum(checksum, snapshot.data.data(), snapshot.data.size());
		checksum      = Checksum(checksum, snapshot.param.data(), snapshot.param.size());
		StoreInteger(header.data() + 32, checksum, 8);
		TemporarySnapshot temporary(TemporaryPath(directory, "memory.bin"));
		if (!WriteTemporary(temporary, header, snapshot)) {
			return SaveMemoryStoreResult::IoError;
		}
		if (previous == SaveMemoryStoreResult::Ok) {
			const auto backup_path   = directory / "memory.bin.bak";
			uintmax_t  backup_size   = 0;
			const auto backup_result = FileSize(backup_path, &backup_size);
			if (backup_result != SaveMemoryStoreResult::Ok &&
			    backup_result != SaveMemoryStoreResult::Missing) {
				return backup_result;
			}
			TemporarySnapshot backup(TemporaryPath(directory, "memory.bin.bak"));
			if (!CopyTemporary(path, previous_size, backup)) {
				return SaveMemoryStoreResult::IoError;
			}
			if (!ReplaceFileAtomically(backup.Path(), backup_path)) {
				return SaveMemoryStoreResult::IoError;
			}
		}
		return ReplaceFileAtomically(temporary.Path(), path) ? SaveMemoryStoreResult::Ok
		                                                     : SaveMemoryStoreResult::IoError;
	} catch (const std::bad_alloc&) {
		return SaveMemoryStoreResult::IoError;
	} catch (const std::filesystem::filesystem_error&) {
		return SaveMemoryStoreResult::IoError;
	}
}

} // namespace Libs::SaveData
