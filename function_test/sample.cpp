// Copyright 2023 Ar-Ray-code.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>

#include <libcamera/camera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/control_ids.h>
#include <libcamera/property_ids.h>
#include <libcamera/transform.h>

using CameraManager = libcamera::CameraManager;
using Camera = libcamera::Camera;

std::unique_ptr<CameraManager> camera_manager_;

bool camera_is_connected()
{
	using namespace libcamera;

    std::unique_ptr<CameraManager> cm = std::make_unique<CameraManager>();
    int ret = cm->start();
    if (ret)
    {
        std::cout << "camera manager failed to start, code " << ret << std::endl;
        return 1;
    }

    std::vector<std::shared_ptr<Camera>> cameras = cm->cameras();
    // Do not show USB webcams as these are not supported in libcamera-apps!
    auto rem = std::remove_if(cameras.begin(), cameras.end(),
                                [](auto &cam) { return cam->id().find("/usb") != std::string::npos; });
    cameras.erase(rem, cameras.end());

    if (cameras.size() != 0)
    {
        unsigned int idx = 0;
        // print number of cameras
        std::cout << "Number of cameras: " << cameras.size() << std::endl;
        for (auto const &cam : cameras)
        {
            cam->acquire();
            std::cout << idx++ << " : " << *cam->properties().get(libcamera::properties::Model);
            auto area = cam->properties().get(properties::PixelArrayActiveAreas);
            if (area)
                std::cout << " [" << (*area)[0].size().toString() << "]";
            std::cout << " (" << cam->id() << ")" << std::endl;

            std::unique_ptr<CameraConfiguration> config = cam->generateConfiguration({libcamera::StreamRole::Raw});
            if (!config)
            {
                std::cout << "failed to generate capture configuration" << std::endl;
                return 1;
            }

            cam->release();
        }
    }
    else
    {
        std::cout << "No cameras available!" << std::endl;
        return 1;
    }

    cameras.clear();
    cm->stop();
    return 0;
}

int main(void)
{
    return camera_is_connected();
}