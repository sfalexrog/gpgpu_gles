#include "gfx/Framebuffer.h"
#include "gfx/Texture.h"
#include "gfx/Mesh.h"
#include "gfx/Shader.h"
#include "gfx/ShaderSources.h"
#include "gfx/UndistortedMesh.h"

#include <ros/ros.h>
#include <nodelet/nodelet.h>
#include <pluginlib/class_list_macros.h>
#include <image_transport/image_transport.h>

#include <gles_render_thread/gles_render_thread.h>
#include <sensor_msgs/image_encodings.h>

#ifdef OPENGL_DEBUG
  #define _XSTR(s) _STR(s)
  #define _STR(s) #s
  #define OGL_CHECKED(func) {func; GLenum error = glGetError(); if (error != GL_NO_ERROR) std::cerr << "OpenGL error at " << std::dec << __LINE__ << " while running " << _STR(func) << ": 0x" << std::hex << error << std::endl;}
#else
  #define OGL_CHECKED(func) {func;}
#endif

namespace gles_demos
{

class Undistort : public nodelet::Nodelet
{
public:
    virtual ~Undistort() override
    {
        GlesRenderThread::shutdown();
    }
private:

    Gfx::FboData fbo_;
    Gfx::TextureData tex_;
    Gfx::GpuMesh mesh_;
    Gfx::Shader shader_;

    ros::NodeHandle nh_, nh_priv_;

    image_transport::CameraPublisher fbo_pub_;
    image_transport::CameraSubscriber cam_sub_;

    int mesh_subdiv_x_;
    int mesh_subdiv_y_;

    sensor_msgs::CameraInfoPtr camera_info_;

    void onInit() override
    {
        if (!GlesRenderThread::init({"", "/dev/dri/renderD128:/dev/dri/renderD129", "llvmpipe:swrast", false}))
        {
            NODELET_FATAL("Failed to initialize render thread");
        }
        nh_ = getNodeHandle();
        nh_priv_ = getPrivateNodeHandle();
        image_transport::ImageTransport it_(nh_);
        image_transport::ImageTransport it_priv_(nh_priv_);

        mesh_subdiv_x_ = nh_priv_.param("subdiv_x", 40);
        mesh_subdiv_y_ = nh_priv_.param("subdiv_y", 30);

        camera_info_ = boost::make_shared<sensor_msgs::CameraInfo>();

        fbo_pub_ = it_priv_.advertiseCamera("image_rect", 1);

        cam_sub_ = it_.subscribeCamera("image_raw", 1, &Undistort::cameraCallback, this);
    }

    bool did_init_ = false;

    // Attribute locations
    const GLuint POSITION_LOC = 0;
    const GLuint TEXTURE_LOC = 1;

    void initData(const sensor_msgs::CameraInfoConstPtr& cameraInfo)
    {
        NODELET_INFO_STREAM("Camera width, height: " << cameraInfo->width << ", " << cameraInfo->height);
        OGL_CHECKED(fbo_ = Gfx::createFbo(cameraInfo->width, cameraInfo->height));
        OGL_CHECKED(tex_ = Gfx::createTexture(cameraInfo->width, cameraInfo->height, nullptr, GL_RGB));
        Gfx::Mesh origMesh = Gfx::createRegularMesh(mesh_subdiv_x_, mesh_subdiv_y_, cameraInfo->width, cameraInfo->height);
        Gfx::Mesh undistMesh = Gfx::createUndistortedMesh(origMesh, cameraInfo);
        // Gfx::Mesh undistMesh = origMesh;
        for (auto& vtx : undistMesh.vertices)
        {
            vtx.vpos[0] = (2.0f * vtx.vpos[0] / cameraInfo->width) - 1.0f;
            vtx.vpos[1] = (2.0f * vtx.vpos[1] / cameraInfo->height) - 1.0f;
        }
        OGL_CHECKED(mesh_ = Gfx::uploadMesh(undistMesh));
        shader_.init()
            .addStage(GL_VERTEX_SHADER, Gfx::Shaders::undistortVertexShader)
            .addStage(GL_FRAGMENT_SHADER, Gfx::Shaders::undistortFragmentShader)
            .link()
            .setAttrLoc("pos", POSITION_LOC)
            .setAttrLoc("texCoord", TEXTURE_LOC);
    }


    void processImage(const sensor_msgs::ImageConstPtr& src, const sensor_msgs::CameraInfoConstPtr& srcCI)
    {
        sensor_msgs::ImagePtr result = boost::make_shared<sensor_msgs::Image>();
        result->header = src->header;
        result->width = fbo_.width;
        result->height = fbo_.height;
        result->encoding = sensor_msgs::image_encodings::RGBA8;
        result->step = result->width * 4;
        result->data.resize(result->step * result->height);

        OGL_CHECKED(glBindFramebuffer(GL_FRAMEBUFFER, fbo_.framebuffer));
        OGL_CHECKED(glViewport(0, 0, fbo_.width, fbo_.height));
        OGL_CHECKED(glClearColor(1.0f, 0.0f, 0.0f, 1.0f));
        OGL_CHECKED(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        OGL_CHECKED(tex_ = Gfx::updateTexture(tex_, src->width, src->height, (void*)src->data.data(), Gfx::glTexFormat(src->encoding)));

        OGL_CHECKED(shader_.activate());
        OGL_CHECKED(glActiveTexture(GL_TEXTURE0));
        glBindTexture(GL_TEXTURE_2D, tex_.id);

        glUniform1i(shader_.unfLoc("tex"), 0);

        OGL_CHECKED(glBindBuffer(GL_ARRAY_BUFFER, mesh_.vtxBuf));
        OGL_CHECKED(glEnableVertexAttribArray(POSITION_LOC));
        OGL_CHECKED(glVertexAttribPointer(POSITION_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(Gfx::VertexData), (void*)offsetof(Gfx::VertexData, vpos)));
        OGL_CHECKED(glEnableVertexAttribArray(TEXTURE_LOC));
        OGL_CHECKED(glVertexAttribPointer(TEXTURE_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(Gfx::VertexData), (void*)offsetof(Gfx::VertexData, texcoord)));

        OGL_CHECKED(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_.idxBuf));
        OGL_CHECKED(glDrawElements(GL_TRIANGLES, mesh_.idxCount, GL_UNSIGNED_INT, nullptr));

        OGL_CHECKED(glPixelStorei(GL_PACK_ALIGNMENT, 4));
        OGL_CHECKED(glPixelStorei(GL_PACK_ROW_LENGTH, result->width));
        result->header.frame_id = src->header.frame_id;
        OGL_CHECKED(glReadPixels(0, 0, result->width, result->height, GL_RGBA, GL_UNSIGNED_BYTE, result->data.data()));

        sensor_msgs::CameraInfoPtr resultCI = boost::make_shared<sensor_msgs::CameraInfo>();
        resultCI->header = srcCI->header;
        resultCI->width = srcCI->width;
        resultCI->height = srcCI->height;
        // FIXME: Replace with actual K
        resultCI->K = srcCI->K;
        resultCI->D = std::vector<double>(5);
        resultCI->P = srcCI->P;
        resultCI->R = srcCI->R;
        resultCI->distortion_model = srcCI->distortion_model;
        resultCI->binning_x = srcCI->binning_x;
        resultCI->binning_y = srcCI->binning_y;
        resultCI->roi = srcCI->roi;

        fbo_pub_.publish(result, resultCI);
    }

    void cameraCallback(const sensor_msgs::ImageConstPtr& src, const sensor_msgs::CameraInfoConstPtr& cameraInfo)
    {
        if (!did_init_)
        {
            GlesRenderThread::enqueue([this, cameraInfo](void*){
                initData(cameraInfo);
                NODELET_INFO("Initialized undistorting mesh");
            }, nullptr, true);
            did_init_ = true;
        }
        
        GlesRenderThread::enqueue([this, src, cameraInfo](void*){
            processImage(src, cameraInfo);
        }, nullptr, true);
    }

};


}

PLUGINLIB_EXPORT_CLASS(gles_demos::Undistort, nodelet::Nodelet);
