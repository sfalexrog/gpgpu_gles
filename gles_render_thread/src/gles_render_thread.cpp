#include "gles_render_thread.h"
#include "gbm_interface.h"
#include <dlfcn.h>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <dirent.h>
#include <mutex>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <iostream>
#include <string>

#include <GLES2/gl2.h>

#include <memory>
#include <thread>
#include <deque>
#include <vector>

#define LOGI(args) { std::cout << "[INFO] " << args << std::endl; }
#define LOGE(args) { std::cerr << "[ERROR] " << args << std::endl; }

namespace
{

enum GpuCommand
{
    Noop,
    Exec,
    ExecForce,
    Stop
};

struct GpuTask
{
    GpuCommand cmd = GpuCommand::Noop;
    GlesRenderThread::CallbackFunction fn = [](void*){};
    void* callbackParams = nullptr;
    GpuTask() = default;
    GpuTask(GpuCommand cmd, const GlesRenderThread::CallbackFunction &fn, void* callbackParams) : cmd(cmd), fn(fn), callbackParams(callbackParams) {}
};

class GpuThread
{
private:

    /**
     * EGL connection state. Not used if custom initialization is requested.
     */
    struct 
    {
        bool initialized = false;
        EGLDisplay dpy = EGL_NO_DISPLAY;
        struct
        {
            int gbmFd = -1;
            GbmInterface::gbm_device* gbmDevice = nullptr;
        } gbmData;
        struct
        {
            void* windowPtr = nullptr;
        } windowData;
        EGLint major = -1;
        EGLint minor = -1;
        EGLContext context = EGL_NO_CONTEXT;
        EGLSurface surface = EGL_NO_SURFACE;
    } eglState;
    std::mutex eglStateMutex;

    /**
     * Free all resources and reset eglState to its initial state.
     */
    void cleanup()
    {
        // FIXME: take lock?
        if (eglState.dpy != EGL_NO_DISPLAY)
        {
            eglMakeCurrent(eglState.dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        }
        if (eglState.surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(eglState.dpy, eglState.surface);
            eglState.surface = EGL_NO_SURFACE;
        }
        if (eglState.context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(eglState.dpy, eglState.context);
            eglState.context = EGL_NO_CONTEXT;
        }
        if (eglState.gbmData.gbmDevice != nullptr)
        {
            GbmInterface::gbm_device_destroy(eglState.gbmData.gbmDevice);
            eglState.gbmData.gbmDevice = nullptr;
        }
        if (eglState.gbmData.gbmFd != -1)
        {
            close(eglState.gbmData.gbmFd);
            eglState.gbmData.gbmFd = -1;
        }
        // FIXME: Deinitialize native window ptr?
        if (eglState.dpy != EGL_NO_DISPLAY)
        {
            eglTerminate(eglState.dpy);
            eglState.dpy = EGL_NO_DISPLAY;
        }
        eglState.initialized = false;
        eglState.major = -1;
        eglState.minor = -1;
    }

    /**
     * Establish a connection to a GBM-backed display.
     * 
     * @param renderNode A path to a backing rendernode (like "/dev/dri/renderD128")
     * @return A valid EGL display handle, or EGL_NO_DISPLAY on failure
     * @note This function will change the eglState structure.
     */
    EGLDisplay getGbmDisplay(const char* renderNode)
    {
        if (!GbmInterface::init())
        {
            LOGE("Failed to initialize libgbm.so");
            return EGL_NO_DISPLAY;
        }
        LOGI("Trying to create display connection using " << renderNode);
        eglState.gbmData.gbmFd = open(renderNode, O_RDWR);
        if (eglState.gbmData.gbmFd < 0)
        {
            LOGE("Failed to open " << renderNode << ": error " << errno << " (" << strerror(errno) << ")");
            return EGL_NO_DISPLAY;
        }
        eglState.gbmData.gbmDevice = GbmInterface::gbm_create_device(eglState.gbmData.gbmFd);
        if (nullptr == eglState.gbmData.gbmDevice)
        {
            LOGE("Failed to create gbm device from rendernode");
            cleanup();
            return EGL_NO_DISPLAY;
        }
        eglState.dpy = eglGetPlatformDisplay(EGL_PLATFORM_GBM_MESA, eglState.gbmData.gbmDevice, nullptr);
        if (eglInitialize(eglState.dpy, &eglState.major, &eglState.minor))
        {
            LOGI("Initialized connection to the display");
        }
        else
        {
            LOGE("Failed to initialize connection");
            cleanup();
        }
        return eglState.dpy;
    }

    /**
     * Establish a connection to a "default" display.
     * 
     * @return A valid EGL display handle, or EGL_NO_DISPLAY on failure
     * @note This function will change the eglState structure.
     * @note Not all drivers expose the "default" display, so this function
     *       should be used as a fallback.
     */
    EGLDisplay getDefaultDisplay()
    {
        eglState.dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (eglInitialize(eglState.dpy, &eglState.major, &eglState.minor))
        {
            LOGI("Initialized EGL_DEFAULT_DISPLAY connection");
        }
        else
        {
            LOGE("Failed to initialize EGL_DEFAULT_DISPLAY connection");
            eglState.dpy = EGL_NO_DISPLAY;
        }
        return eglState.dpy;
    }

    /**
     * Create an OpenGL ES context.
     * 
     * @param blacklistedRenderers A vector of strings containing blacklisted renderer names
     * @param backingDev A string containing path to a backing GBM device. Empty string will result
     *                   in a "default display" being used.
     * @return true on success, false if display connection could not be established,
     *         no matching config could be found, or the renderer for the context
     *         is blacklisted.
     * @note This function modifies eglState.
     */
    bool createContext(const std::vector<std::string>& blacklistedRenderers,
                       const std::string& backingDev = "")
    {
        if (backingDev != "")
        {
            eglState.dpy = getGbmDisplay(backingDev.c_str());
        }
        else
        {
            eglState.dpy = getDefaultDisplay();
        }
        if (EGL_NO_DISPLAY == eglState.dpy)
        {
            LOGE("Could not obtain EGL display connection");
            cleanup();
            return false;
        }
        LOGI("Vendor: " << eglQueryString(eglState.dpy, EGL_VENDOR));
        LOGI("Version: " << eglQueryString(eglState.dpy, EGL_VERSION));
        LOGI("Client APIs: " << eglQueryString(eglState.dpy, EGL_CLIENT_APIS));
        LOGI("Display extensions: " << eglQueryString(eglState.dpy, EGL_EXTENSIONS));

        // FIXME: Allow specifying more attributes?
        EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
        };
        EGLConfig availableConfigs[100];
        EGLint numConfigs = 0;
        if(!eglChooseConfig(eglState.dpy, attribs, availableConfigs, 50, &numConfigs))
        {
            LOGE("Could not choose appropriate config: " << eglGetError());
            cleanup();
            return false;
        }
        LOGI("Number of matching configs: " << numConfigs);
        if (0 == numConfigs)
        {
            LOGE("No matching configs avaliable");
            cleanup();
            return false;
        }
        if(!eglBindAPI(EGL_OPENGL_ES_API))
        {
            LOGE("Could not create OpenGL ES API binding: " << eglGetError());
            cleanup();
            return false;
        }
        // FIXME: Allow specifying context version?
        EGLint ctxAttribs[] = {
            EGL_CONTEXT_MAJOR_VERSION, 2,
            EGL_CONTEXT_MINOR_VERSION, 0,
            EGL_NONE
        };
        eglState.context = eglCreateContext(eglState.dpy, availableConfigs[0], EGL_NO_CONTEXT, ctxAttribs);
        if(eglState.context == EGL_NO_CONTEXT)
        {
            LOGE("Failed to create a context: " << eglGetError());
            return false;
        }
        if(!eglMakeCurrent(eglState.dpy, eglState.surface, eglState.surface, eglState.context))
        {
            LOGE("Could not set the context as current: " << eglGetError());
            return false;
        }
        std::string renderer{reinterpret_cast<const char*>(glGetString(GL_RENDERER))};
        for(const auto& blRender: blacklistedRenderers)
        {
            if (renderer.find(blRender) != std::string::npos)
            {
                LOGI("Renderer " << renderer << " is blacklisted");
                cleanup();
                return false;
            }
        }
        return true;
    }

    /**
     * Split a string into a vector of strings, delimeted by delimeter.
     * 
     * @param str Source string
     * @param delimeter Delimeter character
     */
    static std::vector<std::string> split(const std::string& str, const char delimeter = ':')
    {
        if (str.size() == 0)
        {
            return {};
        }
        std::vector<std::string> result;
        size_t start = 0, end = str.find(delimeter);
        while (end != std::string::npos)
        {
            result.push_back(str.substr(start, end - start));
            start = end + 1;
            end = str.find(delimeter, start);
        }
        result.push_back(str.substr(start));
        return result;
    }

    /**
     * Create an OpenGL ES context, using hints provided by the user.
     */
    bool initialize()
    {
        std::vector<std::string> blacklistedRenderersVec = split(blacklistedRenderers);
        std::vector<std::string> backingDevicesVec = split(backingDevice);
        for(const auto& device : backingDevicesVec)
        {
            if (createContext(blacklistedRenderersVec, device))
            {
                return true;
            }
        }
        // Fallback to "default display"
        return createContext(blacklistedRenderersVec);
    }


    // Initialization options

    std::string preloadLibs;
    std::string backingDevice;
    std::string blacklistedRenderers;
    bool skipInit;

    // Thread storage

    std::thread gpuExecutionThread;
    std::deque<GpuTask> commandQueue;
    std::mutex commandQueueMutex;

    bool didInitialize = false;

    /**
     * Get next GPU task - the latest added or the one with ExecForce flag
     * 
     * @note The caller must hold the commandQueueMutex lock
     */
    GpuTask getNextTask()
    {
        GpuTask task;
        // Return empty task for empty queue
        while (commandQueue.size() > 0)
        {
            task = commandQueue.front();
            commandQueue.pop_front();
            // Always return the "forced" task
            if (task.cmd == GpuCommand::ExecForce)
            {
                return task;
            }
        }
        // Return the last task
        return task;
    }

    void gpuExecutor()
    {
        if (!skipInit)
        {
            didInitialize = initialize();
        }
        else
        {
            didInitialize = true; // Assume user does that
        }

        if (!didInitialize) return;
        while(true)
        {
            GpuTask currentTask;
            {
                std::lock_guard<std::mutex> lock(commandQueueMutex);
                currentTask = getNextTask();
            }
            switch (currentTask.cmd)
            {
                case GpuCommand::Exec:
                case GpuCommand::ExecForce:
                    currentTask.fn(currentTask.callbackParams);
                    break;
                case GpuCommand::Stop:
                    LOGI("Stopping execution thread");
                    cleanup();
                    return;
                default:
                    // Do nothing
                    break;
            }
            // Cut back on CPU usage
            // FIXME: Should we allow sleeping in this thread?
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

public:
    GpuThread(const GlesRenderThread::EglHints& hints) :
        preloadLibs(hints.preloadLibs),
        backingDevice(hints.backingDevice),
        blacklistedRenderers(hints.blacklistedRenderers),
        skipInit(hints.skipInit)
    {
        gpuExecutionThread = std::thread(&GpuThread::gpuExecutor, this);
    }
    ~GpuThread()
    {
        {
            std::lock_guard<std::mutex> lock(commandQueueMutex);
            commandQueue.clear();
            commandQueue.emplace_back(GpuCommand::Stop, [](void*){}, nullptr);
        }
        gpuExecutionThread.join();
    }
    bool enqueue(GpuTask&& task)
    {
        std::lock_guard<std::mutex> lock(commandQueueMutex);
        if (!didInitialize) return false;
        commandQueue.emplace_back(task);
        return true;
    }
};

std::unique_ptr<GpuThread> g_GpuThread;

}

namespace GlesRenderThread
{

// bool init(bool useDefaultDisplay)
bool init(const EglHints& hints)
{
    g_GpuThread.reset(new GpuThread(hints));
    // FIXME: Wait for initialization, catch errors here?
    return true;
}

void shutdown()
{
    g_GpuThread.reset();
    return true;
}

bool enqueue(CallbackFunction function, void* args, bool runAlways)
{
    if (!g_GpuThread) return false;
    // FIXME: Check whether the initialization succeeded?
    GpuTask gt{runAlways ? GpuCommand::ExecForce : GpuCommand::Exec, function, args};
    return g_GpuThread->enqueue(std::move(gt));
}

}
