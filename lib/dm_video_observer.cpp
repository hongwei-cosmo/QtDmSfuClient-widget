/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "dm_video_observer.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"

void VideoObserver::SetVideoCallback(I420FrameReady_callback callback) {
  std::lock_guard<std::mutex> lock(mutex);
  OnI420FrameReady = callback;
}

void VideoObserver::OnFrame(const webrtc::VideoFrame& frame) {
  std::unique_lock<std::mutex> lock(mutex);
  if (!OnI420FrameReady)
    return;

  size_t new_size = frame.width() * frame.height() * 4;
  if (new_size > buffer_size_) {
    buffer_size_ = new_size;
    delete [] buffer_;
    buffer_ = new uint8_t[buffer_size_];
  }

  webrtc::ConvertFromI420(frame, webrtc::VideoType::kARGB, 0, buffer_);
  OnI420FrameReady(buffer_, frame.width(), frame.height());
}
