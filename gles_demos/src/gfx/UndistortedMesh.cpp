#include "UndistortedMesh.h"
#include <opencv2/opencv.hpp>

namespace Gfx
{

Mesh createUndistortedMesh(const Mesh& original, const sensor_msgs::CameraInfoConstPtr &ci)
{
    std::vector<cv::Point2d> sourcePoints;
    for(const auto& vtx : original.vertices)
    {
        sourcePoints.emplace_back(vtx.vpos[0], vtx.vpos[1]);
    }
    std::vector<cv::Point2d> undistPoints(sourcePoints.size());
    cv::Mat cameraMatrix(3, 3, CV_64F);
    cv::Mat distCoeffs(ci->D.size(), 1, CV_64F);
    for(int i = 0; i < 9; ++i)
    {
        cameraMatrix.at<double>(i / 3, i % 3) = ci->K[i];
    }

    for(int i = 0; i < ci->D.size(); ++i)
    {
        distCoeffs.at<double>(i) = ci->D[i];
    }

    cv::undistortPoints(sourcePoints, undistPoints, cameraMatrix, distCoeffs, cv::noArray(), cameraMatrix);

    Mesh result;
    for(int i = 0; i < undistPoints.size(); ++i)
    {
        VertexData vd{};
        vd.vpos[0] = undistPoints[i].x;
        vd.vpos[1] = undistPoints[i].y;
        vd.texcoord[0] = original.vertices[i].texcoord[0];
        vd.texcoord[1] = original.vertices[i].texcoord[1];
        result.vertices.push_back(vd);
    }
    result.indices.reserve(original.indices.size());
    std::copy(original.indices.begin(), original.indices.end(), std::back_inserter(result.indices));

    return result;
}

Mesh createDistortedMesh(const Mesh& sourceMesh, const sensor_msgs::CameraInfoConstPtr &ci)
{
    double k1 = 0.215356885;
    double k2 = -0.117472846;
    double p1 = -0.000306197672;
    double p2 = -0.000109444025;
    double k3 = -0.00453657258;
    double k4 = 0.573090623;
    double k5 = -0.127574577;
    double k6 = -0.0286125589;    

    Mesh result;
    for(int i = 0; i < sourceMesh.vertices.size(); ++i)
    {
        VertexData vd{};
        double x = (sourceMesh.vertices[i].vpos[0] - 160) / 160;
        double y = (sourceMesh.vertices[i].vpos[1] - 120) / 120;
        
        double r2 = x * x + y * y;

        double dx = x * (1 + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2) / (1 + k4 * r2 + k5 * r2 * r2 + k6 * r2 * r2 * r2) + 2 * p1 * x * y + p2 * (r2 + 2 * x * x);
        double dy = y * (1 + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2) / (1 + k4 * r2 + k5 * r2 * r2 + k6 * r2 * r2 * r2) + p1 * (r2 + 2 * y * y) + 2 * p2 * x * y;

        vd.vpos[0] = dx * 160 + 160;
        vd.vpos[1] = dy * 120 + 120;

        vd.texcoord[0] = sourceMesh.vertices[i].texcoord[0];
        vd.texcoord[1] = sourceMesh.vertices[i].texcoord[1];
        result.vertices.push_back(vd);
    }
    result.indices.reserve(sourceMesh.indices.size());
    std::copy(sourceMesh.indices.begin(), sourceMesh.indices.end(), std::back_inserter(result.indices));

    return result;
}

}
