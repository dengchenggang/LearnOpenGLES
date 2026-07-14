#ifndef C_VIDEO_PIPELINE_CAMERA_H
#define C_VIDEO_PIPELINE_CAMERA_H
#include "VideoPipelineBase.h"

class VideoPipelineCamera : public VideoPipelineBase {
public:
    VideoPipelineCamera();
    ~VideoPipelineCamera() override {}
    VideoPipelineCamera(const VideoPipelineCamera&) = delete;
    VideoPipelineCamera& operator = (const VideoPipelineCamera&) = delete;
public:
};
#endif
