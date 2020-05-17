#pragma once

#include <linux/videodev2.h>
#include <cstdint>
#include <string>
#include <vector>
#include <utility>

namespace gles_camera
{

class Frame;

class V4L2Device
{
public:
    enum class CaptureMethod
    {
        Read,
        Mmap,
        Userptr
    };
    struct buffer {
        void *start;
        size_t length;
    };

private:

    int fd;
    std::string deviceName;
    std::vector<buffer> buffers;
    std::vector<v4l2_fmtdesc> formats;
    std::vector<std::vector<v4l2_frmsizeenum>> sizes;

    CaptureMethod captureMethod;

    v4l2_capability capabilities;

    bool isCapturing;

    std::pair<bool, v4l2_capability> queryCap();
    bool requestBuffers(uint32_t bufferCount);
    int createBuffers(v4l2_create_buffers&);
    std::vector<v4l2_fmtdesc> enumerateFormats();
    std::vector<std::vector<v4l2_frmsizeenum>> enumerateSizes();

public:
    V4L2Device(const std::string& devName, const CaptureMethod& preferredMethod = CaptureMethod::Userptr, bool lateInit=true);
    ~V4L2Device();

    const std::vector<v4l2_fmtdesc>& supportedFormats() { return formats; }
    const std::vector<std::vector<v4l2_frmsizeenum>>& supportedSizes() { return sizes; }

    v4l2_pix_format setFormat(uint32_t width, uint32_t height, uint32_t pixelformat,
                            v4l2_field field = v4l2_field::V4L2_FIELD_ANY, uint32_t stride = 0);


    bool init();
    bool prepareBuffers(uint32_t numBuffers);
    bool startCapture();

    Frame readFrame();

    bool releaseBuffer(const v4l2_buffer& buffer);

};

class Frame
{
friend class V4L2Device;
private:
    V4L2Device& parent;
    V4L2Device::buffer buffer;
    v4l2_buffer v4l2buf;
public:
    virtual ~Frame() {parent.releaseBuffer(v4l2buf);}
    V4L2Device::buffer& data() {return buffer;}
    v4l2_buffer& metadata() {return v4l2buf;}
private:
    Frame(V4L2Device& parent, V4L2Device::buffer buffer, v4l2_buffer v4l2Buf) : parent(parent), buffer(buffer), v4l2buf(v4l2Buf) {}
};

} //gles_camera
