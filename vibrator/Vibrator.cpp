/*
 * Copyright (C) 2019 The Android Open Source Project
 * Copyright (C) 2021 StatiX
 * SPDX-License-Identifer: Apache-2.0
 */

#define LOG_TAG "vibrator.cannon"

#include "vibrator-impl/Vibrator.h"

#include <android-base/logging.h>
#include <fstream>
#include <thread>

namespace aidl {
namespace android {
namespace hardware {
namespace vibrator {

static constexpr char activate_node[] = "/sys/devices/platform/haptic_pwm/activate";
static constexpr char duration_node[] = "/sys/devices/platform/haptic_pwm/duration";
static constexpr char index_node[] = "/sys/devices/platform/haptic_pwm/index";
static constexpr char control_node[] = "/sys/devices/platform/haptic_pwm/load_wavefile_ctrl";

// Define durations for waveforms
static constexpr uint32_t WAVEFORM_TICK_EFFECT_MS = 14;
static constexpr uint32_t WAVEFORM_TEXTURE_TICK_EFFECT_MS = 10;
static constexpr uint32_t WAVEFORM_CLICK_EFFECT_MS = 12;
static constexpr uint32_t WAVEFORM_HEAVY_CLICK_EFFECT_MS = 12;
static constexpr uint32_t WAVEFORM_DOUBLE_CLICK_EFFECT_MS = 130;
static constexpr uint32_t WAVEFORM_THUD_EFFECT_MS = 15;
static constexpr uint32_t WAVEFORM_POP_EFFECT_MS = 9;

// Select waveform index from firmware through index list
static constexpr uint32_t WAVEFORM_TICK_EFFECT_INDEX = 2;
static constexpr uint32_t WAVEFORM_CLICK_EFFECT_INDEX = 1;
static constexpr uint32_t WAVEFORM_HEAVY_CLICK_EFFECT_INDEX = 3;
static constexpr uint32_t WAVEFORM_DOUBLE_CLICK_EFFECT_INDEX = 7;
static constexpr uint32_t WAVEFORM_THUD_EFFECT_INDEX = 9;

// Waveform control
static constexpr uint32_t LOW_WAVEFORM_OFFSET = 0;
static constexpr uint32_t MID_WAVEFORM_OFFSET = 1;
static constexpr uint32_t HIGH_WAVEFORM_OFFSET = 2;

template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

ndk::ScopedAStatus Vibrator::getCapabilities(int32_t* _aidl_return) {
    LOG(INFO) << "Vibrator reporting capabilities";
    *_aidl_return = IVibrator::CAP_ON_CALLBACK;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::off() {
    LOG(INFO) << "Vibrator off";
    set(activate_node, 0);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::on(int32_t timeoutMs,
                                const std::shared_ptr<IVibratorCallback>& callback) {
    LOG(INFO) << "Vibrator on for timeoutMs: " << timeoutMs;
    set(activate_node, 1);
    usleep(10);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::perform(Effect effect, EffectStrength strength,
                                     const std::shared_ptr<IVibratorCallback>& callback,
                                     int32_t* _aidl_return) {
    ndk::ScopedAStatus status;
    uint32_t timeMs;
    uint32_t index;

    LOG(INFO) << "Vibrator perform";

    switch (effect) {
        case Effect::TICK:
            LOG(INFO) << "Vibrator effect set to TICK";
            set(index_node, WAVEFORM_TICK_EFFECT_INDEX);
            set(control_node, LOW_WAVEFORM_OFFSET);
            set(duration, WAVEFORM_TICK_EFFECT_MS);
            break;
        case Effect::TEXTURE_TICK:
            LOG(INFO) << "Vibrator effect set to TEXTURE_TICK";
            set(index_node, WAVEFORM_CLICK_EFFECT_INDEX);
            set(control_node, LOW_WAVEFORM_OFFSET);
            set(duration, WAVEFORM_TEXTURE_TICK_EFFECT_MS);
            break;
        case Effect::CLICK:
            LOG(INFO) << "Vibrator effect set to CLICK";
            set(index_node, WAVEFORM_CLICK_EFFECT_INDEX);
            set(control_node, LOW_WAVEFORM_OFFSET);
            set(duration, WAVEFORM_CLICK_EFFECT_MS);
            break;
        case Effect::HEAVY_CLICK:
            LOG(INFO) << "Vibrator effect set to HEAVY_CLICK";
            set(index_node, WAVEFORM_HEAVY_CLICK_EFFECT_INDEX);
            set(control_node, MID_WAVEFORM_OFFSET);
            set(duration, WAVEFORM_HEAVY_CLICK_EFFECT_MS);
            break;
        case Effect::DOUBLE_CLICK:
            LOG(INFO) << "Vibrator effect set to DOUBLE_CLICK";
            set(index_node, WAVEFORM_DOUBLE_CLICK_EFFECT_INDEX);
            set(control_node, MID_WAVEFORM_OFFSET);
            set(duration, WAVEFORM_DOUBLE_CLICK_EFFECT_MS);
            break;
        case Effect::THUD:
            LOG(INFO) << "Vibrator effect set to THUD";
            set(index_node, WAVEFORM_THUD_EFFECT_INDEX);
            set(control_node, HIGH_WAVEFORM_OFFSET);
            set(duration, WAVEFORM_THUD_EFFECT_MS);
            break;
        case Effect::POP:
            LOG(INFO) << "Vibrator effect set to POP";
            set(index_node, WAVEFORM_TICK_EFFECT_INDEX);
            set(control_node, LOW_WAVEFORM_OFFSET);
            set(duration, WAVEFORM_POP_EFFECT_MS);
            break;
        default:
            return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }

    status = on(timeMs, nullptr);
    if (!status.isOk()) {
        return status;
    } else {
        *_aidl_return = timeMs;
        return ndk::ScopedAStatus::ok();
    }
}

ndk::ScopedAStatus Vibrator::getSupportedEffects(std::vector<Effect> *_aidl_return) {

    *_aidl_return = {
        Effect::TICK,
        Effect::TEXTURE_TICK,
        Effect::CLICK,
        Effect::HEAVY_CLICK,
        Effect::DOUBLE_CLICK,
        Effect::THUD,
        Effect::POP
    };

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::setAmplitude(float amplitude) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::setExternalControl(bool enabled) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getCompositionDelayMax(int32_t* maxDelayMs) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getCompositionSizeMax(int32_t* maxSize) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getSupportedPrimitives(std::vector<CompositePrimitive>* supported) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getPrimitiveDuration(CompositePrimitive primitive,
                                                  int32_t* durationMs) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::compose(const std::vector<CompositeEffect>& composite,
                                     const std::shared_ptr<IVibratorCallback>& callback) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getSupportedAlwaysOnEffects(std::vector<Effect>* _aidl_return) {
    return getSupportedEffects(_aidl_return);
}

ndk::ScopedAStatus Vibrator::alwaysOnEnable(int32_t id, Effect effect, EffectStrength strength) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::alwaysOnDisable(int32_t id) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getResonantFrequency(float *resonantFreqHz) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getQFactor(float *qFactor) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getFrequencyResolution(float *freqResolutionHz) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getFrequencyMinimum(float *freqMinimumHz) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getBandwidthAmplitudeMap(std::vector<float> *_aidl_return) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getPwlePrimitiveDurationMax(int32_t *durationMs) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getPwleCompositionSizeMax(int32_t *maxSize) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getSupportedBraking(std::vector<Braking> *supported) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::composePwle(const std::vector<PrimitivePwle> &composite,
                                         const std::shared_ptr<IVibratorCallback> &callback) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

}  // namespace vibrator
}  // namespace hardware
}  // namespace android
}  // namespace aidl
