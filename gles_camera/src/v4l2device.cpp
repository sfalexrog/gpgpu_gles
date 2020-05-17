#include "v4l2device.h"

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "log.h"


namespace
{

/**
 * Perform an ioctl request on a file descriptor, ignoring "interrupted system call"
 * errors.
 * 
 * @param fd File descriptor on which to perform ioctl
 * @param request Request ID of the ioctl
 * @param arg A pointer to a struct holding ioctl arguments
 * @return 0 or a non-negative value on success, -1 on error
 */
int xioctl(int fd, int request, void *arg)
{
    int retval;

    do {
        retval = ioctl(fd, request, arg);
    } while (-1 == retval && EINTR == errno);

    return retval;
}

}

namespace gles_camera
{

V4L2Device::V4L2Device(const std::string& devName, const CaptureMethod& preferredMethod, bool lateInit) : fd(-1), deviceName(devName), captureMethod(preferredMethod), isCapturing(false)
{
    LOG_FUNC(DEBUG) << "Creating V4L2 device from " << deviceName;
    if (!lateInit)
    {
        LOG_FUNC(DEBUG) << "Eagerly initializing V4L2 device";
        init();
    }
}

std::pair<bool, v4l2_capability> V4L2Device::queryCap()
{
    v4l2_capability retval;
    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &retval))
    {
        LOG_FUNC(ERROR) << "Could not query V4L2 capabilities of " << deviceName;
        return std::make_pair(false, retval);
    }
    LOG_FUNC(DEBUG) << "Driver: " << retval.driver;
    LOG_FUNC(DEBUG) << "Card: " << retval.card;
    LOG_FUNC(DEBUG) << "Bus Info: " << retval.bus_info;
    LOG_FUNC(DEBUG) << "Version: " << retval.version;
    LOG_FUNC(DEBUG) << "Capabilities: " << retval.capabilities;
    LOG_FUNC(DEBUG) << "Device Caps: " << retval.capabilities;

    return std::make_pair(true, retval);
}

bool V4L2Device::requestBuffers(uint32_t bufferCount)
{
    v4l2_requestbuffers request;
    request.count = bufferCount,
    request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    switch(captureMethod)
    {
        case CaptureMethod::Mmap:
            request.memory = V4L2_MEMORY_MMAP;
            break;
        case CaptureMethod::Userptr:
            request.memory = V4L2_MEMORY_USERPTR;
            break;
        default:
            LOG(ERROR) << "requestBuffers called for read-based captures";
            return false;
    }
    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &request))
    {
        LOG(ERROR) << "requestBuffers called with unsupported parameters";
        return false;
    }
    LOG(DEBUG) << "Asked for " << bufferCount << " buffers, got " << request.count;
    if (request.count < 2)
    {
        LOG(ERROR) << "Not enough buffers to continue";
        return false;
    }
    buffers.resize(request.count);
    if (captureMethod == CaptureMethod::Mmap)
    {
        for(uint32_t i = 0; i < buffers.size(); ++i)
        {
            v4l2_buffer buf;
            memset(&buf, 0, sizeof(buf));
            buf.index = i;
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;

            if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
            {
                LOG(ERROR) << "VIDIOC_QUERYBUF failed";
            }

            LOG(DEBUG) << "MMAPping buffer " << i << " with length: " << buf.length << ", offset: " << buf.m.offset; 

            buffers[i].length = buf.length;
            buffers[i].start = mmap(nullptr,
                buf.length,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd, buf.m.offset);
            if (MAP_FAILED == buffers[i].start)
            {
                LOG(ERROR) << "Memory mapping unsuccessful";
            }
        }
    }
    else
    {
        LOG(ERROR) << "userptr capture method initialization unimplemented";
        return false;
    }

    return true;
}

void printCaps(const v4l2_capability& caps)
{
#define PRINT_CAP(cap) {LOG_FUNC(DEBUG) << "Capability " #cap ": " << ((caps.capabilities & cap) ? 1 : 0); }
    PRINT_CAP(V4L2_CAP_VIDEO_CAPTURE);
    PRINT_CAP(V4L2_CAP_VIDEO_CAPTURE_MPLANE);
    PRINT_CAP(V4L2_CAP_VIDEO_OUTPUT);
    PRINT_CAP(V4L2_CAP_VIDEO_OUTPUT_MPLANE);
    PRINT_CAP(V4L2_CAP_VIDEO_M2M);
    PRINT_CAP(V4L2_CAP_VIDEO_M2M_MPLANE);
    PRINT_CAP(V4L2_CAP_VIDEO_OVERLAY);
    PRINT_CAP(V4L2_CAP_READWRITE);
    PRINT_CAP(V4L2_CAP_ASYNCIO);
    PRINT_CAP(V4L2_CAP_STREAMING);
    PRINT_CAP(V4L2_CAP_META_CAPTURE);
    PRINT_CAP(V4L2_CAP_EXT_PIX_FORMAT);
    PRINT_CAP(V4L2_CAP_TIMEPERFRAME);
#undef PRINT_CAP
}

std::string fourcc(uint32_t format)
{
    char formatstr[5] = {0};
    formatstr[0] = format & 255;
    formatstr[1] = (format >> 8) & 255;
    formatstr[2] = (format >> 16) & 255;
    formatstr[3] = (format >> 24) & 255;
    return formatstr;
}

std::vector<v4l2_fmtdesc> V4L2Device::enumerateFormats()
{
    std::vector<v4l2_fmtdesc> result;
    // FIXME: Check for V4L2_CAP_IO_MC flag?
    uint32_t fmtIdx = 0;
    v4l2_fmtdesc currentDesc;
    while (true)
    {
        memset(&currentDesc, 0, sizeof(currentDesc));
        currentDesc.index = fmtIdx;
        // FIXME: Enumerate other tyeps as well?
        currentDesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        // FIXME: Somehow add mbus_code?
        if (-1 == xioctl(fd, VIDIOC_ENUM_FMT, &currentDesc)) { break; }
        LOG_FUNC(DEBUG) << "Enumerated format " << currentDesc.index << ": "
                        << "flags: " << currentDesc.flags << ", "
                        << "description: " << currentDesc.description << ", "
                        << "pixelformat: " << fourcc(currentDesc.pixelformat);
        result.push_back(currentDesc);
        fmtIdx += 1;
    }
    return result;
}

std::vector<std::vector<v4l2_frmsizeenum>> V4L2Device::enumerateSizes()
{
    std::vector<std::vector<v4l2_frmsizeenum>> result;
    result.resize(formats.size());
    for(uint32_t formatIdx = 0; formatIdx < formats.size(); ++formatIdx)
    {
        uint32_t sizeIdx = 0;
        v4l2_frmsizeenum currentSize;
        LOG_FUNC(DEBUG) << "Enumerating formats for " << formats[formatIdx].description;
        while(true)
        {
            memset(&currentSize, 0, sizeof(currentSize));
            currentSize.index = sizeIdx;
            currentSize.pixel_format = formats[formatIdx].pixelformat;
            if (-1 == xioctl(fd, VIDIOC_ENUM_FRAMESIZES, &currentSize)) {break;}
            if (currentSize.type == V4L2_FRMSIZE_TYPE_CONTINUOUS)
            {
                LOG_FUNC(DEBUG) << "Continuous size, min " << currentSize.stepwise.min_width
                                << "x" << currentSize.stepwise.min_height << ", max "
                                << currentSize.stepwise.max_width << "x" << currentSize.stepwise.max_height;
            }
            else if (currentSize.type == V4L2_FRMSIZE_TYPE_STEPWISE)
            {
                LOG_FUNC(DEBUG) << "Stepwise size, min " << currentSize.stepwise.min_width
                                << "x" << currentSize.stepwise.min_height << ", max "
                                << currentSize.stepwise.max_width << "x" << currentSize.stepwise.max_height << ", step "
                                << currentSize.stepwise.step_width << "," << currentSize.stepwise.step_height;
            }
            else if (currentSize.type == V4L2_FRMSIZE_TYPE_DISCRETE) 
            {
                LOG_FUNC(DEBUG) << "Discrete size: " << currentSize.discrete.width << "x"
                                << currentSize.discrete.height;
            }
            else
            {
                LOG_FUNC(ERROR) << "Somewhow got unexpected data in size enumeration!";
            }

            result[formatIdx].push_back(currentSize);
            sizeIdx += 1;
            if (currentSize.type != V4L2_FRMSIZE_TYPE_DISCRETE) { break; }
        }
    }
    return result;
}

bool V4L2Device::init()
{
    if (-1 != fd)
    {
        LOG_FUNC(DEBUG) << "Already initialized, not doing it twice";
        return true;
    }
    LOG_FUNC(DEBUG) << "Initializing device";
    fd = open(deviceName.c_str(), O_RDWR | O_NONBLOCK, 0);

    if (-1 == fd)
    {
        LOG_FUNC(ERROR) << "Could not open " << deviceName << ": " << strerror(errno) << " (" << errno << ")";
        return false;
    }

    auto queryCapResult = queryCap();
    if (!queryCapResult.first)
    {
        close(fd);
        fd = -1;
        return false;
    }
    capabilities = queryCapResult.second;
    printCaps(capabilities);
    formats = enumerateFormats();
    sizes = enumerateSizes();

    if (!(capabilities.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        LOG_FUNC(ERROR) << "Device " << deviceName << " does not expose video capturing capabilities";
        close(fd);
        fd = -1;
        return false;
    }

    if (!(capabilities.capabilities & V4L2_CAP_STREAMING))
    {
        LOG_FUNC(WARN) << "Device " << deviceName << " does not expose streaming interfaces";
        if (captureMethod != CaptureMethod::Read)
        {
            captureMethod = CaptureMethod::Read;
        }
    }

    return true;
}

std::string colorspaceStr(v4l2_colorspace colorspace)
{
#define HANDLE_COLORSPACE(cs) case v4l2_colorspace::cs : return #cs;
    switch(colorspace)
    {
        HANDLE_COLORSPACE(V4L2_COLORSPACE_DEFAULT);
        HANDLE_COLORSPACE(V4L2_COLORSPACE_SMPTE170M);
        HANDLE_COLORSPACE(V4L2_COLORSPACE_REC709);
        HANDLE_COLORSPACE(V4L2_COLORSPACE_SRGB);
        HANDLE_COLORSPACE(V4L2_COLORSPACE_OPRGB);
        HANDLE_COLORSPACE(V4L2_COLORSPACE_BT2020);
        HANDLE_COLORSPACE(V4L2_COLORSPACE_DCI_P3);
        HANDLE_COLORSPACE(V4L2_COLORSPACE_SMPTE240M);
        HANDLE_COLORSPACE(V4L2_COLORSPACE_470_SYSTEM_M);
        HANDLE_COLORSPACE(V4L2_COLORSPACE_470_SYSTEM_BG);
        HANDLE_COLORSPACE(V4L2_COLORSPACE_JPEG);
        HANDLE_COLORSPACE(V4L2_COLORSPACE_RAW);
        default:
            return std::to_string(colorspace) + " (unknown v4l2 value)";
    }
#undef HANDLE_COLORSPACE
}

std::string xferFuncStr(v4l2_xfer_func xfunc)
{
#define HANDLE_FUNC(xf) case v4l2_xfer_func::xf : return #xf;
    switch(xfunc)
    {
        HANDLE_FUNC(V4L2_XFER_FUNC_DEFAULT);
        HANDLE_FUNC(V4L2_XFER_FUNC_709);
        HANDLE_FUNC(V4L2_XFER_FUNC_SRGB);
        HANDLE_FUNC(V4L2_XFER_FUNC_OPRGB);
        HANDLE_FUNC(V4L2_XFER_FUNC_SMPTE240M);
        HANDLE_FUNC(V4L2_XFER_FUNC_NONE);
        HANDLE_FUNC(V4L2_XFER_FUNC_DCI_P3);
        HANDLE_FUNC(V4L2_XFER_FUNC_SMPTE2084);
        default:
            return std::to_string(xfunc) + " (unknown v4l2 value)";
    }
#undef HANDLE_FUNC
}

std::string ycbcrEncStr(v4l2_ycbcr_encoding encoding)
{
#define HANDLE_ENCODING(enc) case v4l2_ycbcr_encoding::enc : return #enc;
    switch(encoding)
    {
        HANDLE_ENCODING(V4L2_YCBCR_ENC_DEFAULT);
        HANDLE_ENCODING(V4L2_YCBCR_ENC_601);
        HANDLE_ENCODING(V4L2_YCBCR_ENC_709);
        HANDLE_ENCODING(V4L2_YCBCR_ENC_XV601);
        HANDLE_ENCODING(V4L2_YCBCR_ENC_XV709);
        HANDLE_ENCODING(V4L2_YCBCR_ENC_BT2020);
        HANDLE_ENCODING(V4L2_YCBCR_ENC_BT2020_CONST_LUM);
        HANDLE_ENCODING(V4L2_YCBCR_ENC_SMPTE240M);
        default:
            return std::to_string(encoding) + " (unknown v4l2 encoding)";
    }
#undef HANDLE_ENCODING
}

std::string quantStr(v4l2_quantization quantization)
{
#define HANDLE_QUANT(qt) case v4l2_quantization::qt : return #qt;
    switch(quantization)
    {
        HANDLE_QUANT(V4L2_QUANTIZATION_DEFAULT);
        HANDLE_QUANT(V4L2_QUANTIZATION_FULL_RANGE);
        HANDLE_QUANT(V4L2_QUANTIZATION_LIM_RANGE);
        default:
            return std::to_string(quantization) + " (unknown v4l2 quantization)";
    }
#undef HANDLE_QUANT
}

v4l2_pix_format V4L2Device::setFormat(uint32_t width, uint32_t height, uint32_t pixelformat,
                            v4l2_field field, uint32_t stride)
{
    v4l2_format format;
    memset(&format, 0, sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = width;
    format.fmt.pix.height = height;
    format.fmt.pix.pixelformat = pixelformat;
    format.fmt.pix.field = field;
    format.fmt.pix.bytesperline = stride;
    if (-1 == xioctl(fd, VIDIOC_S_FMT, &format))
    {
        LOG_FUNC(ERROR) << "Could not set desired format!";
        return v4l2_pix_format();
    }

    LOG_FUNC(DEBUG) << "Initialized format: " << format.fmt.pix.width << "x" << format.fmt.pix.height << ", "
                    << "pixelformat: " << fourcc(format.fmt.pix.pixelformat) << ", "
                    << "stride: " << format.fmt.pix.bytesperline << ", "
                    << "field order: " << format.fmt.pix.field << ", "
                    << "colorspace: " << colorspaceStr((v4l2_colorspace)format.fmt.pix.colorspace) << ", "
                    << "xfer_func: " << xferFuncStr((v4l2_xfer_func)format.fmt.pix.xfer_func) << ", "
                    << "ycbcr encoding: " << ycbcrEncStr((v4l2_ycbcr_encoding)format.fmt.pix.ycbcr_enc) << ", "
                    << "quantization: " << quantStr((v4l2_quantization)format.fmt.pix.quantization) << ", "
                    << "total image size: " << format.fmt.pix.sizeimage;

    uint32_t interval_idx = 0;
    LOG_FUNC(DEBUG) << "Enumerating intervals for given resolution";
    while(true)
    {
        v4l2_frmivalenum interval;
        memset(&interval, 0, sizeof(interval));
        interval.index = interval_idx;
        interval.pixel_format = pixelformat;
        interval.width = width;
        interval.height = height;
        if (-1 == xioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &interval)) {break;}
        if (interval.type != V4L2_FRMIVAL_TYPE_DISCRETE)
        {
            LOG_FUNC(DEBUG) << "Stepwise/continuous interval, min: " << interval.stepwise.min.numerator << " / " << interval.stepwise.min.denominator << ", "
                            << "max: " << interval.stepwise.max.numerator << " / " << interval.stepwise.max.denominator << ", "
                            << "step: " << interval.stepwise.step.numerator << " / " << interval.stepwise.step.denominator;
            LOG_FUNC(DEBUG) << "Attempting to set min frame interval";
            v4l2_streamparm sparm;
            memset(&sparm, 0, sizeof(sparm));
            sparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            sparm.parm.capture.timeperframe = interval.stepwise.min;
            if (-1 == xioctl(fd, VIDIOC_S_PARM, &sparm))
            {
                LOG_FUNC(DEBUG) << "Oh well, at least we tried";
            }
            LOG_FUNC(DEBUG) << "readbuffers: " << sparm.parm.capture.readbuffers;

            break;
        }
        else
        {
            LOG_FUNC(DEBUG) << "Discrete value: " << interval.discrete.numerator << " / " << interval.discrete.denominator;
        }
        interval_idx += 1;
    }

    return format.fmt.pix;
}


bool V4L2Device::prepareBuffers(uint32_t numBuffers)
{
    if (captureMethod == CaptureMethod::Read)
    {
        return true;
    }
    if (!requestBuffers(numBuffers))
    {
        LOG(ERROR) << "Failed to request buffers";
        return false;
    }
    return true;
}

bool V4L2Device::startCapture()
{

    if (captureMethod == CaptureMethod::Read)
    {
        return true;
    }
    if (captureMethod == CaptureMethod::Userptr)
    {
        LOG_FUNC(ERROR) << "Sorry, unimplemented";
        return false;
    }
    v4l2_buf_type type = v4l2_buf_type::V4L2_BUF_TYPE_VIDEO_CAPTURE;
    for(uint32_t bufIdx = 0; bufIdx < buffers.size(); ++bufIdx)
    {
        v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = bufIdx;
        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
        {
            LOG_FUNC(ERROR) << "Could not enqueue buffer #" << bufIdx;
            return false;
        }
    }
    if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
    {
        LOG_FUNC(ERROR) << "Failed to turn streaming on";
        return false;
    }
    isCapturing = true;
    LOG_FUNC(DEBUG) << "Showtime";
    return true;
}


Frame V4L2Device::readFrame()
{
    v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    while(true)
    {
        fd_set fds;
        timeval tv;

        int retval;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        tv.tv_sec = 2;
        tv.tv_usec = 0;

        retval = select(fd + 1, &fds, NULL, NULL, &tv);

        if (-1 == retval)
        {
            if (EINTR == errno)
            {
                LOG_FUNC(DEBUG) << "Interrupted system call, continuing";
                continue;
            }
            LOG_FUNC(ERROR) << "Failure while performing select() call: " << strerror(errno) << " (" << errno << ")";
            return Frame(*this, buffer(), v4l2_buffer());
        }
        if (0 == retval)
        {
            LOG_FUNC(ERROR) << "Timed out waiting for select to finish";
            return Frame(*this, buffer(), v4l2_buffer());
        }

        buf.type = v4l2_buf_type::V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = v4l2_memory::V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
        {
            switch(errno)
            {
                case EAGAIN:
                    LOG_FUNC(DEBUG) << "Call would block, retrying";
                    continue;
                case EIO:
                    LOG_FUNC(DEBUG) << "Stumbled upon EIO";
                default:
                    LOG_FUNC(ERROR) << "Fault: " << strerror(errno) << " (" << errno << ")";
                    return Frame(*this, buffer(), v4l2_buffer());
            }
        }

        LOG_FUNC(DEBUG) << "Returning buffer #" << buf.index;
        return Frame(*this, buffers[buf.index], buf);
    }
}

bool V4L2Device::releaseBuffer(const v4l2_buffer& buffer)
{
    LOG_FUNC(DEBUG) << "Re-enqueueing buffer #" << buffer.index;
    v4l2_buffer buf = buffer;
    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
    {
        LOG_FUNC(ERROR) << "Failed to return buffer to queue";
        return false;
    }
    return true;
}

V4L2Device::~V4L2Device()
{
    if (isCapturing)
    {
        LOG_FUNC(DEBUG) << "Stopping capture";
        if (captureMethod != CaptureMethod::Read)
        {
            v4l2_buf_type type = v4l2_buf_type::V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
            {
                LOG_FUNC(ERROR) << "Could not stop video capture, but won't do anything about it";
            }
        }
    }
    LOG_FUNC(DEBUG) << "Clearing up resources";
    if (buffers.size() > 0)
    {
        switch(captureMethod)
        {
            case CaptureMethod::Mmap:
                for(auto& buffer : buffers)
                {
                    if (-1 == munmap(buffer.start, buffer.length))
                    {
                        LOG_FUNC(ERROR) << "Something went wrong while unmapping buffer, but won't do anything about it";
                    }
                }
            break;
            default:
            break;
        }
    }
    LOG_FUNC(DEBUG) << "Destroying V4L2 device";
    if (fd != -1)
    {
        close(fd);
    }
}

}
