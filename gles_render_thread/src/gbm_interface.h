#pragma once

namespace GbmInterface
{

struct gbm_device;

using FnGbmCreateDevice = gbm_device*(int);
using FnGbmDeviceDestroy = void(gbm_device*);
using FnGbmDeviceGetBackendName = const char*(gbm_device*);

extern FnGbmCreateDevice* gbm_create_device;
extern FnGbmDeviceDestroy* gbm_device_destroy;
extern FnGbmDeviceGetBackendName* gbm_device_get_backend_name;

bool init();

}
