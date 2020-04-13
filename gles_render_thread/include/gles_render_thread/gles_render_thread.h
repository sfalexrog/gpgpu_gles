#pragma once
#include <functional>
#include <string>

namespace GlesRenderThread
{

struct EglHints
{
    /// Names of libraries to be preloaded, colon-separated
    std::string preloadLibs;
    /// Backing device name, if present
    std::string backingDevice;
    /// Blacklisted renderer strings, colon-separeted
    std::string blacklistedRenderers;
    /// Flag to skip initialization completely
    bool skipInit;
};

// Callback function signature: no return value, single-argument (void*)
using CallbackFunction = std::function<void(void*)>;

/**
 * Initialize rendering thread.
 * 
 * @param hints Context creation hints (see EglHints struct description)
 * @return true on success, false otherwise
 */
bool init(const EglHints& hints);

/**
 * Stop rendering thread.
 */
void shutdown();

/**
 * Add a callback to the render thread queue.
 * 
 * @param function A function that satisfies the CallbackFunction signature.
 * @param functionArgs A pointer that will be passed to the CallbackFunction.
 * @param runAlways Don't allow skipping the function execution.
 * @note There is no guarantee your callback will actually be executed: the
 *       rendering tries to discard as many callbacks as possible, running
 *       only the most recent one. This is done to reduce latency. If you
 *       **need** your callback to run (most likely because it sets up your
 *       rendering state), set runAlways to true. Keep in mind that you should
 *       generally avoid putting "runAlways" callbacks in the queue.
 */
bool enqueue(CallbackFunction function, void* functionArgs, bool runAlways = false);

}
