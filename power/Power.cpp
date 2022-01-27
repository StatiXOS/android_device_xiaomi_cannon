/*
 * Copyright (C) 2020 The Android Open Source Project
 * Copyright (C) 2022 StatiX
 * SPDX-License-Identifer: Apache-2.0
 */

#define LOG_TAG "power.cannon"

#include "Power.h"

#include <android-base/logging.h>

namespace aidl {
namespace android {
namespace hardware {
namespace power {
namespace impl {
namespace cannon {

const std::vector<Boost> BOOST_RANGE{ndk::enum_range<Boost>().begin(),
                                     ndk::enum_range<Boost>().end()};
const std::vector<Mode> MODE_RANGE{ndk::enum_range<Mode>().begin(), ndk::enum_range<Mode>().end()};

ndk::ScopedAStatus Power::setMode(Mode type, bool enabled) {
    LOG(VERBOSE) << "Power setMode: " << static_cast<int32_t>(type) << " to: " << enabled;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::isModeSupported(Mode type, bool* _aidl_return) {
    LOG(INFO) << "Power isModeSupported: " << static_cast<int32_t>(type);
    *_aidl_return = type >= MODE_RANGE.front() && type <= MODE_RANGE.back();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::setBoost(Boost type, int32_t durationMs) {
    LOG(VERBOSE) << "Power setBoost: " << static_cast<int32_t>(type)
                 << ", duration: " << durationMs;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::isBoostSupported(Boost type, bool* _aidl_return) {
    LOG(INFO) << "Power isBoostSupported: " << static_cast<int32_t>(type);
    *_aidl_return = type >= BOOST_RANGE.front() && type <= BOOST_RANGE.back();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::createHintSession(int32_t, int32_t, const std::vector<int32_t>&, int64_t,
                                            std::shared_ptr<IPowerHintSession>* _aidl_return) {
    *_aidl_return = nullptr;
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Power::getHintSessionPreferredRate(int64_t* outNanoseconds) {
    *outNanoseconds = -1;
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

}  // namespace cannon
}  // namespace impl
}  // namespace power
}  // namespace hardware
}  // namespace android
}  // namespace aidl
