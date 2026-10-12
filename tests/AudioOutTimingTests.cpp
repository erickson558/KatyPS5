// Exercise production scheduling without relying on a host sound device or
// wall-clock timing.
// The fake backend has no SDL runtime or SDL_main entry-point dependency on Windows.
#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "common/threads.h"
#include "kernel/semaphore.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include <vector>

namespace {

void Check(bool condition, const char* message) {
	if (!condition) {
		std::fprintf(stderr, "AudioOutTimingTests: %s\n", message);
		std::abort();
	}
}

struct Device {
	SDL_AudioSpec spec {};
	uint64_t      frames  = 0; // Millionths of a frame keep simulated consumption exact.
	uint64_t      updated = 0;
};

uint64_t                                      g_now = 1000000;
std::vector<uint32_t>                         g_sleeps;
std::unordered_map<SDL_AudioDeviceID, Device> g_devices;
SDL_AudioDeviceID                             g_next_device  = 1;
bool                                          g_fail_open    = false;
bool                                          g_stalled      = false;
int                                           g_queued_calls = 0;
int                                           g_clear_calls  = 0;

Device& GetDevice(SDL_AudioDeviceID id) {
	const auto it = g_devices.find(id);
	Check(it != g_devices.end(), "unknown device");
	return it->second;
}

void Drain(Device& device) {
	if (!g_stalled) {
		device.frames -= std::min(device.frames, (g_now - device.updated) * device.spec.freq);
	}
	device.updated = g_now;
}

} // namespace

namespace Fake {

int InitSubSystem(Uint32) {
	return 0;
}
Uint32 WasInit(Uint32 flags) {
	return flags;
}
const char* GetError() {
	return "deterministic test backend";
}
void PauseAudioDevice(SDL_AudioDeviceID, int) {}

SDL_AudioDeviceID OpenAudioDevice(const char*, int, const SDL_AudioSpec* desired,
                                  SDL_AudioSpec* obtained, int) {
	if (g_fail_open) {
		return 0;
	}
	*obtained     = *desired;
	const auto id = g_next_device++;
	g_devices.emplace(id, Device {*desired, 0, g_now});
	return id;
}

void CloseAudioDevice(SDL_AudioDeviceID id) {
	Check(g_devices.erase(id) == 1, "device closed twice");
}

int BuildAudioCVT(SDL_AudioCVT* cvt, SDL_AudioFormat source_format, Uint8 source_channels,
                  int source_rate, SDL_AudioFormat target_format, Uint8 target_channels,
                  int target_rate) {
	Check(source_format == target_format && source_channels == target_channels &&
	          source_rate == target_rate,
	      "test device unexpectedly requires conversion");
	*cvt = {};
	return 0;
}

int ConvertAudio(SDL_AudioCVT*) {
	Check(false, "unexpected audio conversion");
	return -1;
}

Uint32 GetQueuedAudioSize(SDL_AudioDeviceID id) {
	auto& device = GetDevice(id);
	Drain(device);
	return static_cast<Uint32>(device.frames / 1000000 *
	                           (SDL_AUDIO_BITSIZE(device.spec.format) / 8) * device.spec.channels);
}

int QueueAudio(SDL_AudioDeviceID id, const void* data, Uint32 bytes) {
	Check(data != nullptr, "backend received null PCM");
	auto& device = GetDevice(id);
	Drain(device);
	device.frames += static_cast<uint64_t>(bytes) * 1000000 /
	                 ((SDL_AUDIO_BITSIZE(device.spec.format) / 8) * device.spec.channels);
	++g_queued_calls;
	return 0;
}

void ClearQueuedAudio(SDL_AudioDeviceID id) {
	auto& device   = GetDevice(id);
	device.frames  = 0;
	device.updated = g_now;
	++g_clear_calls;
}

} // namespace Fake

namespace Common {

class TimingThread: public Thread {
public:
	using Thread::Thread;
	static void SleepMicro(uint32_t micros) {
		g_sleeps.push_back(micros);
		g_now += micros;
	}
};

} // namespace Common

// Keep the scheduler, null-PCM handling, and queue limits in the production
// implementation. Only clock/sleep and the SDL device boundary are replaced; no
// scheduling condition is copied.
#define Thread                 TimingThread
#define SDL_InitSubSystem      Fake::InitSubSystem
#define SDL_WasInit            Fake::WasInit
#define SDL_GetError           Fake::GetError
#define SDL_OpenAudioDevice    Fake::OpenAudioDevice
#define SDL_PauseAudioDevice   Fake::PauseAudioDevice
#define SDL_CloseAudioDevice   Fake::CloseAudioDevice
#define SDL_BuildAudioCVT      Fake::BuildAudioCVT
#define SDL_ConvertAudio       Fake::ConvertAudio
#define SDL_GetQueuedAudioSize Fake::GetQueuedAudioSize
#define SDL_QueueAudio         Fake::QueueAudio
#define SDL_ClearQueuedAudio   Fake::ClearQueuedAudio
#include "libs/audio.cpp"
#undef Thread
#undef SDL_InitSubSystem
#undef SDL_WasInit
#undef SDL_GetError
#undef SDL_OpenAudioDevice
#undef SDL_PauseAudioDevice
#undef SDL_CloseAudioDevice
#undef SDL_BuildAudioCVT
#undef SDL_ConvertAudio
#undef SDL_GetQueuedAudioSize
#undef SDL_QueueAudio
#undef SDL_ClearQueuedAudio

namespace Libs::LibKernel {

uint64_t KATY_SYSV_ABI KernelGetProcessTime() {
	return g_now;
}

namespace Semaphore {

// Audio3d lives in the same production translation unit but is outside these
// output tests.
int KATY_SYSV_ABI KernelCreateSema(KernelSema*, const char*, uint32_t, int, int, void*) {
	Check(false, "unexpected Audio3d semaphore creation");
	return KERNEL_ERROR_EINVAL;
}
int KATY_SYSV_ABI KernelWaitSema(KernelSema, int, KernelUseconds*) {
	Check(false, "unexpected Audio3d semaphore wait");
	return KERNEL_ERROR_EINVAL;
}
int KATY_SYSV_ABI KernelSignalSema(KernelSema, int) {
	Check(false, "unexpected Audio3d semaphore signal");
	return KERNEL_ERROR_EINVAL;
}

} // namespace Semaphore
} // namespace Libs::LibKernel

namespace {

using Audio = Libs::Audio::Audio;

struct Fixture {
	Audio                   audio;
	std::array<float, 1024> pcm {};

	Fixture() {
		Check(g_devices.empty(), "device leaked between tests");
		g_now       = 1000000;
		g_fail_open = g_stalled = false;
		g_queued_calls = g_clear_calls = 0;
		g_sleeps.clear();
		Libs::Audio::g_audio = &audio;
	}
	~Fixture() { Libs::Audio::g_audio = nullptr; }

	Audio::Id Open(int type = 0) {
		const auto id = audio.AudioOutOpen(type, 256, 48000, Audio::Format::FloatStereo);
		Check(id.IsValid(), "port open failed");
		return id;
	}
};

void TestInactiveDeviceKeepsMixedBatchFallback() {
	Fixture    f;
	const auto main      = f.Open();
	const auto vibration = f.Open(10);
	// The public API permits null PCM and does not use AudioInternal's filtering
	// wrapper.
	Libs::Audio::AudioOut::AudioOutOutputParam batch[] {{main.ToInt(), nullptr},
	                                                    {vibration.ToInt(), f.pcm.data()}};
	Check(Libs::Audio::AudioOut::AudioOutOutputs(batch, 2) == 256, "first mixed output failed");
	const auto start = g_now;
	Check(Libs::Audio::AudioOut::AudioOutOutputs(batch, 2) == 256, "second mixed output failed");
	Check(g_now - start == 5333 && g_sleeps.size() == 1 && g_sleeps[0] == 5333,
	      "a device without PCM disabled fallback pacing");
	Check(g_queued_calls == 0, "inactive device received PCM");

	g_now += 1000000;
	g_sleeps.clear();
	Libs::Audio::AudioOut::AudioOutOutputs(batch, 2);
	Check(g_sleeps.empty(), "fallback waited after a gap longer than the sample period");
}

void TestActiveMixedBatchUsesDeviceQueue() {
	Fixture            f;
	const auto         main      = f.Open();
	const auto         vibration = f.Open(10);
	Audio::OutputParam batch[] {{vibration, f.pcm.data()}, {main, f.pcm.data()}};
	const auto         start = g_now;
	// SDL2 allows the latency cushion to fill before it applies backpressure.
	for (int i = 0; i < 9; ++i) {
		f.audio.AudioOutOutputs(batch, 2);
	}
	Check(g_now == start && g_sleeps.empty() && g_queued_calls == 9,
	      "vibration fallback prevented priming a device-backed batch");
	f.audio.AudioOutOutputs(batch, 2);
	Check(g_now - start == 6000 && g_sleeps.size() == 6 &&
	          std::all_of(g_sleeps.begin(), g_sleeps.end(), [](auto us) { return us == 1000; }),
	      "mixed batch did not use SDL queue backpressure alone");
	g_sleeps.clear();
	f.audio.AudioOutOutputs(batch, 2, false);
	Check(g_sleeps.empty() && g_now - start == 6000 && g_queued_calls == 11,
	      "asynchronous device output waited for queue backpressure");
}

void TestDeviceFreeFallbackAndAsync() {
	for (bool fail_open: {false, true}) {
		Fixture f;
		g_fail_open             = fail_open;
		const auto         port = f.Open(fail_open ? 0 : 10);
		Audio::OutputParam param {port, f.pcm.data()};
		f.audio.AudioOutOutputs(&param, 1);
		const auto start = g_now;
		f.audio.AudioOutOutputs(&param, 1);
		Check(g_now - start == 5333 && g_sleeps.size() == 1,
		      "device-free output did not wait exactly one sample period");
		g_sleeps.clear();
		f.audio.AudioOutOutputs(&param, 1, false);
		Check(g_sleeps.empty() && g_now - start == 5333, "asynchronous fallback slept");
	}
}

void TestStalledDeviceWaitIsBounded() {
	Fixture            f;
	const auto         port = f.Open();
	Audio::OutputParam param {port, f.pcm.data()};
	for (int i = 0; i < 9; ++i) {
		f.audio.AudioOutOutputs(&param, 1);
	}
	g_stalled        = true;
	const auto start = g_now;
	f.audio.AudioOutOutputs(&param, 1);
	Check(g_now - start == 201000 && g_clear_calls == 1 && g_queued_calls == 10,
	      "stalled queue did not recover after the bounded SDL wait");
}

} // namespace

int main() {
	TestInactiveDeviceKeepsMixedBatchFallback();
	TestActiveMixedBatchUsesDeviceQueue();
	TestDeviceFreeFallbackAndAsync();
	TestStalledDeviceWaitIsBounded();
	Check(g_devices.empty(), "output device leaked");
	std::puts("AudioOutTimingTests: all cases passed");
}
