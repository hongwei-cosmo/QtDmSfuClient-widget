#ifndef DM_VIDEO_OBSERVER_H
#define DM_VIDEO_OBSERVER_H

#include <mutex>

#include "api/mediastreaminterface.h"
#include "api/videosinkinterface.h"

// Definitions of callback functions.
typedef  std::function<void(const uint8_t* buffer,
                            int width, int height)> I420FrameReady_callback;
typedef std::function<void()> LocalDataChannelReady_callback;
typedef std::function<void(const char* msg)> DataFromDataChannelReady_callback;
typedef std::function<void(const char* msg)> Failure_callback;
typedef std::function<void(const char* type,
                           const char* sdp)> LocalSdpReadyToSend_callback;
typedef std::function<void(const char* candidate,
                           const int sdp_mline_index,
                           const char* sdp_mid)> IceCandidateReadyToSend_callback;

typedef std::function<void(const void* audio_data,
                           int bits_per_sample,
                           int sample_rate,
                           int number_of_channels,
                           int number_of_frames)> AudioBusReady_callback;

class VideoObserver : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
 public:
  VideoObserver() {}
  ~VideoObserver() override { delete [] buffer_; }
  void SetVideoCallback(I420FrameReady_callback callback);

 protected:
  // VideoSinkInterface implementation
  void OnFrame(const webrtc::VideoFrame& frame) override;

 private:
  I420FrameReady_callback OnI420FrameReady = nullptr;
  std::mutex mutex;
  uint8_t* buffer_ = nullptr;
  size_t buffer_size_ = 0;
};

#endif  // DM_VIDEO_OBSERVER_H
