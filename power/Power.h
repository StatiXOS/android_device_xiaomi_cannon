/*
 * Copyright (C) 2020 The Android Open Source Project
 * Copyright (C) 2022 StatiX
 * SPDX-License-Identifer: Apache-2.0
 */


#pragma once

#include <aidl/android/hardware/power/BnPower.h>

namespace aidl {
namespace android {
namespace hardware {
namespace power {
namespace impl {
namespace cannon {

class Power : public BnPower {
    ndk::ScopedAStatus setMode(Mode type, bool enabled) override;
    ndk::ScopedAStatus isModeSupported(Mode type, bool* _aidl_return) override;
    ndk::ScopedAStatus setBoost(Boost type, int32_t durationMs) override;
    ndk::ScopedAStatus isBoostSupported(Boost type, bool* _aidl_return) override;
    ndk::ScopedAStatus createHintSession(int32_t tgid, int32_t uid,
                                         const std::vector<int32_t>& threadIds,
                                         int64_t durationNanos,
                                         std::shared_ptr<IPowerHintSession>* _aidl_return) override;
    ndk::ScopedAStatus getHintSessionPreferredRate(int64_t* outNanoseconds) override;
};

}  // namespace cannon
}  // namespace impl
}  // namespace power
}  // namespace hardware
}  // namespace android
}  // namespace aidl
