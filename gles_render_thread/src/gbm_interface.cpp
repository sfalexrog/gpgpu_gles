#include "gbm_interface.h"
#include <dlfcn.h>

namespace
{

bool initialized = false;
void* gbmLibHandle = nullptr;

}

namespace GbmInterface
{

FnGbmCreateDevice* gbm_create_device;
FnGbmDeviceDestroy* gbm_device_destroy;
FnGbmDeviceGetBackendName* gbm_device_get_backend_name;

bool init()
{
    if (initialized) return true;
    gbmLibHandle = dlopen("libgbm.so", RTLD_LAZY);
    if (!gbmLibHandle)
    {
        return false;
    }
    gbm_create_device = reinterpret_cast<FnGbmCreateDevice*>(dlsym(gbmLibHandle, "gbm_create_device"));
    gbm_device_destroy = reinterpret_cast<FnGbmDeviceDestroy*>(dlsym(gbmLibHandle, "gbm_device_destroy"));
    gbm_device_get_backend_name = reinterpret_cast<FnGbmDeviceGetBackendName*>(dlsym(gbmLibHandle,
        "gbm_device_get_backend_name"));
    initialized = true;
    return true;
}

}
