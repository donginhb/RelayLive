#pragma once
#include <string>
#include <stdint.h>

class ILiveSession {
public:
    virtual void AsyncSend() = 0;
};

/**
 * uri中解析出参数
 */
struct RequestParam {
    std::string           strCode;              // 设备编码，必填项
    std::string           strType;              // 媒体类型，默认为flv
    uint32_t              nWidth;               // 视频宽度，默认为0，不缩放视频
    uint32_t              nHeight;              // 视频高度，默认为0，不缩放视频
    uint32_t              nProbSize;            // 探测PS流的大小，默认为25600
    uint32_t              nProbTime;            // 探测PS流的时间，默认为1秒
};

namespace Server {

int Init(int port);

int Cleanup();
};