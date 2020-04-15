#ifndef GLESIMAGEPROCESSING_UNDISTORTEDMESH_H
#define GLESIMAGEPROCESSING_UNDISTORTEDMESH_H

#include "Mesh.h"
#include <sensor_msgs/CameraInfo.h>

namespace Gfx
{
Mesh createUndistortedMesh(const Mesh& sourceMesh, const sensor_msgs::CameraInfoConstPtr &ci);

Mesh createDistortedMesh(const Mesh& sourceMesh, const sensor_msgs::CameraInfoConstPtr &ci);
}

#endif //GLESIMAGEPROCESSING_UNDISTORTEDMESH_H
