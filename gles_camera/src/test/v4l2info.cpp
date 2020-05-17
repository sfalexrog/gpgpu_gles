#include "../v4l2device.h"
#include "../log.h"

#include <chrono>

/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */

void timespec_diff(struct timespec *start, struct timespec *stop,
                   struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
}

int main(int argc, char** argv)
{
    Logger::Logger::minSeverity() = Logger::Severity::LOG_DEBUG;
    gles_camera::V4L2Device camera("/dev/video0", gles_camera::V4L2Device::CaptureMethod::Mmap);
    camera.init();
    
    const auto& formats = camera.supportedFormats();
    const auto& sizes = camera.supportedSizes();

    uint32_t desiredWidth = 1280;
    uint32_t desiredHeight = 720;
    int chosenFormat = -1;

    // Get first non-compressed format
    for(size_t format = 0; format < formats.size(); ++format)
    {
        if (formats[format].flags & V4L2_FMT_FLAG_COMPRESSED)
        {
            LOG_FUNC(DEBUG) << "Ignoring format #" << format << " (reason: compressed)";
            continue;
        }
        LOG_FUNC(DEBUG) << "Looking at available sizes";
        if (sizes[format].size() == 0)
        {
            LOG_FUNC(WARN) << "Format #" << format << "listed as available but has no sizes";
            continue;
        }
        if (sizes[format][0].type == V4L2_FRMSIZE_TYPE_DISCRETE)
        {
            LOG_FUNC(DEBUG) << "Selecting among discrete formats";
            
            for (const auto& size : sizes[format])
            {
                if (size.discrete.width == desiredWidth && size.discrete.height == desiredHeight)
                {
                    LOG_FUNC(DEBUG) << "Found suitable format";
                    break;
                }
            }
        }
        else
        {
            LOG_FUNC(DEBUG) << "Stepwise/continuous format is fine";
        }
        chosenFormat = format;
        break;
    }
    if (chosenFormat == -1)
    {
        LOG(ERROR) << "Could not find acceptable format";
        return EXIT_FAILURE;
    }
    auto format = camera.setFormat(desiredWidth, desiredHeight, formats[chosenFormat].pixelformat);
    camera.prepareBuffers(40);
    camera.startCapture();

    LOG_FUNC(INFO) << "Capturing 10 frames";
    for(int frame = 0; frame < 10; ++frame)
    {
        auto frame_start = std::chrono::system_clock::now();
        auto cap = camera.readFrame();
        auto monotonic = (cap.metadata().flags & V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC) != 0;
        timespec timestamp;
        timespec curTime;
        timestamp.tv_sec = cap.metadata().timestamp.tv_sec;
        timestamp.tv_nsec = cap.metadata().timestamp.tv_usec * 1000;

        clock_gettime(monotonic ? CLOCK_MONOTONIC : CLOCK_REALTIME, &curTime);

        timespec diff;
        timespec_diff(&timestamp, &curTime, &diff);
        double ddiff = diff.tv_sec + diff.tv_nsec / (1000. * 1000. * 1000.);

        LOG_FUNC(INFO) << "Frame #" << frame << ": timestamp " << cap.metadata().timestamp.tv_sec << "." << cap.metadata().timestamp.tv_usec << ", "
                       << "monotonic: " << monotonic << ", delay: " << ddiff << ", "
                       << "timestamp source: " << (cap.metadata().flags & V4L2_BUF_FLAG_TSTAMP_SRC_MASK) << ", "
                       << "buffer #" << cap.metadata().index;

        auto frame_end = std::chrono::system_clock::now();
        LOG_FUNC(INFO) << "Capturing frame took " << (frame_end - frame_start).count() / (1000. * 1000. * 1000.) << " s";
    }

    return 0;
}
