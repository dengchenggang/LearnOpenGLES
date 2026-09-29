/**
 * @file VideoCapture.h
 * @brief 视频捕获管理器，负责 VideoPipeline 的生命周期与帧数据分发
 *
 * 以单例形式提供多路视频管线的统一管理，支持普通帧与硬件缓冲帧两种回调模式。
 */
#pragma once
#include "common/Singleton.hpp"
#include "video/VideoFrame.h"
#include "video/VideoHardwareBuffer.h"
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <mutex>

namespace framework {

class VideoPipeline;

/**
 * @brief 视频捕获单例管理类
 *
 * 维护一组以 URL 为键的 VideoPipeline，提供配置、连接、断开、重启等操作。
 */
class VideoCapture {
    friend class Singleton<VideoCapture>;
public:
    /** @brief 管线状态/事件通知回调类型 */
    using Notification = std::function<void(const std::string&, VideoPipelineEvent, int32_t, const std::string&)>;
public:
    VideoCapture(const VideoCapture&) = delete;
    VideoCapture& operator=(const VideoCapture&) = delete;
public:
    /**
     * @brief 配置指定 URL 的管线参数
     * @param url      视频源地址
     * @param width    视频宽度
     * @param height   视频高度
     * @param format   像素格式
     * @param fps      帧率
     * @return 是否设置成功
     */
    bool setVideoPipelineInfo(const std::string& url, int32_t width, int32_t height, VideoFormat format, float fps);

    /**
     * @brief 设置全局管线事件通知回调
     * @param notification 事件回调函数
     */
    void setPipelineNotification(Notification notification);

    /**
     * @brief 重启指定 URL 的管线
     * @param url         视频源地址
     * @param hardRestart true 表示硬重启（pause -> stop -> start -> resume），false 表示软重启(pause -> resume)
     * @return 是否重启成功
     */
    bool restart(const std::string& url, bool hardRestart = true);

    /**
     * @brief 注册普通视频帧回调
     * @param url        视频源地址
     * @param moduleName 模块名称，用于标识订阅者
     * @param callback   帧数据回调
     * @return 是否连接成功
     */
    bool connect(const std::string& url, const std::string& moduleName, const VideoFrameCallback& callback);

    /**
     * @brief 注册硬件缓冲帧回调（零拷贝路径）
     * @param url        视频源地址
     * @param moduleName 模块名称，用于标识订阅者
     * @param callback   硬件缓冲帧回调
     * @return 是否连接成功
     */
    bool connect(const std::string& url, const std::string& moduleName, const VideoHardwareBufferCallback& callback);

    /**
     * @brief 断开指定模块的帧回调
     * @param url               视频源地址
     * @param moduleName        模块名称
     * @param releaseIfNoObserver 无观察者时是否释放管线，默认 true
     * @return 是否断开成功
     */
    bool disconnect(const std::string& url, const std::string& moduleName, bool releaseIfNoObserver = true);
private:
    /** @brief 管线静态配置信息 */
    struct VideoPipelineInfo {
        std::string url;        ///< 视频源地址
        int32_t width;          ///< 视频宽度
        int32_t height;         ///< 视频高度
        VideoFormat format;     ///< 像素格式
        float fps;              ///< 帧率
    };
private:
    /**
     * @brief 获取或创建指定 URL 的 VideoPipeline
     * @param url            视频源地址
     * @param useHardwareBuffer 是否使用硬件缓冲区模式
     * @return 对应管线指针，失败返回 nullptr
     */
    VideoPipeline* getOrCreatePipeline(const std::string& url, bool useHardwareBuffer);

    VideoCapture();
    ~VideoCapture();
private:
    Notification mPipelineNotification;                     ///< 管线事件通知回调
    std::mutex mPipelineNotificationMutex;                  ///< 通知回调互斥锁
    std::map<std::string, VideoPipelineInfo> mVideoPipelineInfo; ///< URL -> 管线配置映射
    std::map<std::string, std::unique_ptr<VideoPipeline>> mVideoPipelines; ///< URL -> 管线实例映射
    std::mutex mMutex;                                      ///< 管线容器互斥锁
};

/** @brief 获取 VideoCapture 单例引用 */
VideoCapture& GetVideoCaptureInstance();

} // namespace framework

/** @brief 便捷宏，等价于 framework::GetVideoCaptureInstance() */
#define VideoCapture framework::GetVideoCaptureInstance()