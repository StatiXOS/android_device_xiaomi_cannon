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

static constexpr int32_t kComposeDelayMaxMs = 1000;
static constexpr int32_t kComposeSizeMax = 256;
static constexpr int32_t kComposePwleSizeMax = 127;

static constexpr float kResonantFrequency = 150.0;
static constexpr float kQFactor = 11.0;
static constexpr int32_t COMPOSE_PWLE_PRIMITIVE_DURATION_MAX_MS = 16383;
static constexpr float PWLE_LEVEL_MIN = 0.0;
static constexpr float PWLE_LEVEL_MAX = 0.98256;
static constexpr float PWLE_FREQUENCY_RESOLUTION_HZ = 1.0;
static constexpr float PWLE_FREQUENCY_MIN_HZ = 140.0;
static constexpr float PWLE_FREQUENCY_MAX_HZ = 160.0;

template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

ndk::ScopedAStatus Vibrator::getCapabilities(int32_t* _aidl_return) {
    LOG(INFO) << "Vibrator reporting capabilities";
    *_aidl_return = IVibrator::CAP_ON_CALLBACK | IVibrator::CAP_PERFORM_CALLBACK |
                    IVibrator::CAP_AMPLITUDE_CONTROL | IVibrator::CAP_EXTERNAL_CONTROL |
                    IVibrator::CAP_EXTERNAL_AMPLITUDE_CONTROL | IVibrator::CAP_COMPOSE_EFFECTS |
                    IVibrator::CAP_ALWAYS_ON_CONTROL | IVibrator::CAP_GET_RESONANT_FREQUENCY |
                    IVibrator::CAP_GET_Q_FACTOR | IVibrator::CAP_FREQUENCY_CONTROL |
                    IVibrator::CAP_COMPOSE_PWLE_EFFECTS;
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
    set(duration_node, timeoutMs);
    set(activate_node, 1);
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
        case Effect::TEXTURE_TICK:
            LOG(INFO) << "Vibrator effect set to TEXTURE_TICK";
            set(index_node, WAVEFORM_CLICK_EFFECT_INDEX);
            set(control_node, LOW_WAVEFORM_OFFSET);
            timeMs = WAVEFORM_TEXTURE_TICK_EFFECT_MS;
            break;
        case Effect::TICK:
            LOG(INFO) << "Vibrator effect set to TICK";
            set(index_node, WAVEFORM_TICK_EFFECT_INDEX);
            set(control_node, LOW_WAVEFORM_OFFSET);
            timeMs = WAVEFORM_TICK_EFFECT_MS;
            break;
        case Effect::CLICK:
            LOG(INFO) << "Vibrator effect set to CLICK";
            set(index_node, WAVEFORM_CLICK_EFFECT_INDEX);
            set(control_node, LOW_WAVEFORM_OFFSET);
            timeMs = WAVEFORM_CLICK_EFFECT_MS;
            break;
        case Effect::HEAVY_CLICK:
            LOG(INFO) << "Vibrator effect set to HEAVY_CLICK";
            set(index_node, WAVEFORM_HEAVY_CLICK_EFFECT_INDEX);
            set(control_node, MID_WAVEFORM_OFFSET);
            timeMs = WAVEFORM_HEAVY_CLICK_EFFECT_MS;
            break;
        case Effect::DOUBLE_CLICK:
            LOG(INFO) << "Vibrator effect set to DOUBLE_CLICK";
            set(index_node, WAVEFORM_DOUBLE_CLICK_EFFECT_INDEX);
            set(control_node, MID_WAVEFORM_OFFSET);
            timeMs = WAVEFORM_DOUBLE_CLICK_EFFECT_MS;
            break;
        case Effect::THUD:
            LOG(INFO) << "Vibrator effect set to THUD";
            set(index_node, WAVEFORM_THUD_EFFECT_INDEX);
            set(control_node, HIGH_WAVEFORM_OFFSET);
            timeMs = WAVEFORM_THUD_EFFECT_MS;
            break;
        case Effect::POP:
            LOG(INFO) << "Vibrator effect set to POP";
            set(index_node, WAVEFORM_TICK_EFFECT_INDEX);
            set(control_node, LOW_WAVEFORM_OFFSET);
            timeMs = WAVEFORM_POP_EFFECT_MS;
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
        Effect::TEXTURE_TICK,
        Effect::TICK,
        Effect::CLICK,
        Effect::HEAVY_CLICK,
        Effect::DOUBLE_CLICK,
        Effect::THUD,
        Effect::POP
    };

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::setAmplitude(float amplitude) {
    LOG(INFO) << "Vibrator set amplitude: " << amplitude;
    if (amplitude <= 0.0f || amplitude > 1.0f) {
        return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_ILLEGAL_ARGUMENT));
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::setExternalControl(bool enabled) {
    LOG(INFO) << "Vibrator set external control: " << enabled;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getCompositionDelayMax(int32_t* maxDelayMs) {
    *maxDelayMs = kComposeDelayMaxMs;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getCompositionSizeMax(int32_t* maxSize) {
    *maxSize = kComposeSizeMax;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getSupportedPrimitives(std::vector<CompositePrimitive>* supported) {
    *supported = {
            CompositePrimitive::NOOP,       CompositePrimitive::CLICK,
            CompositePrimitive::THUD,       CompositePrimitive::SPIN,
            CompositePrimitive::QUICK_RISE, CompositePrimitive::SLOW_RISE,
            CompositePrimitive::QUICK_FALL, CompositePrimitive::LIGHT_TICK,
            CompositePrimitive::LOW_TICK,
    };
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getPrimitiveDuration(CompositePrimitive primitive,
                                                  int32_t* durationMs) {
    std::vector<CompositePrimitive> supported;
    getSupportedPrimitives(&supported);
    if (std::find(supported.begin(), supported.end(), primitive) == supported.end()) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }
    if (primitive != CompositePrimitive::NOOP) {
        *durationMs = 100;
    } else {
        *durationMs = 0;
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::compose(const std::vector<CompositeEffect>& composite,
                                     const std::shared_ptr<IVibratorCallback>& callback) {
    if (composite.size() > kComposeSizeMax) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
    }

    std::vector<CompositePrimitive> supported;
    getSupportedPrimitives(&supported);

    for (auto& e : composite) {
        if (e.delayMs > kComposeDelayMaxMs) {
            return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
        }
        if (e.scale < 0.0f || e.scale > 1.0f) {
            return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
        }
        if (std::find(supported.begin(), supported.end(), e.primitive) == supported.end()) {
            return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
        }
    }

    std::thread([=] {
        LOG(INFO) << "Starting compose on another thread";

        for (auto& e : composite) {
            if (e.delayMs) {
                usleep(e.delayMs * 1000);
            }
            LOG(INFO) << "triggering primitive " << static_cast<int>(e.primitive) << " @ scale "
                      << e.scale;

            int32_t durationMs;
            getPrimitiveDuration(e.primitive, &durationMs);
            usleep(durationMs * 1000);
        }

        if (callback != nullptr) {
            LOG(INFO) << "Notifying perform complete";
            callback->onComplete();
        }
    }).detach();

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getSupportedAlwaysOnEffects(std::vector<Effect>* _aidl_return) {
    return getSupportedEffects(_aidl_return);
}

ndk::ScopedAStatus Vibrator::alwaysOnEnable(int32_t id, Effect effect, EffectStrength strength) {
    std::vector<Effect> effects;
    getSupportedAlwaysOnEffects(&effects);

    if (std::find(effects.begin(), effects.end(), effect) == effects.end()) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    } else {
        LOG(INFO) << "Enabling always-on ID " << id << " with " << toString(effect) << "/"
                  << toString(strength);
        return ndk::ScopedAStatus::ok();
    }
}

ndk::ScopedAStatus Vibrator::alwaysOnDisable(int32_t id) {
    LOG(INFO) << "Disabling always-on ID " << id;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getResonantFrequency(float *resonantFreqHz) {
    *resonantFreqHz = kResonantFrequency;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getQFactor(float *qFactor) {
    *qFactor = kQFactor;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getFrequencyResolution(float *freqResolutionHz) {
    *freqResolutionHz = PWLE_FREQUENCY_RESOLUTION_HZ;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getFrequencyMinimum(float *freqMinimumHz) {
    *freqMinimumHz = PWLE_FREQUENCY_MIN_HZ;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getBandwidthAmplitudeMap(std::vector<float> *_aidl_return) {
    // A valid array should be of size:
    //     (PWLE_FREQUENCY_MAX_HZ - PWLE_FREQUENCY_MIN_HZ) / PWLE_FREQUENCY_RESOLUTION_HZ
    *_aidl_return = {0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10,
                     0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.20};
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getPwlePrimitiveDurationMax(int32_t *durationMs) {
    *durationMs = COMPOSE_PWLE_PRIMITIVE_DURATION_MAX_MS;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getPwleCompositionSizeMax(int32_t *maxSize) {
    *maxSize = kComposePwleSizeMax;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getSupportedBraking(std::vector<Braking> *supported) {
    *supported = {
            Braking::NONE,
            Braking::CLAB,
    };
    return ndk::ScopedAStatus::ok();
}

void resetPreviousEndAmplitudeEndFrequency(float &prevEndAmplitude, float &prevEndFrequency) {
    const float reset = -1.0;
    prevEndAmplitude = reset;
    prevEndFrequency = reset;
}

void incrementIndex(int &index) {
    index += 1;
}

void constructActiveDefaults(std::ostringstream &pwleBuilder, const int &segmentIdx) {
    pwleBuilder << ",C" << segmentIdx << ":1";
    pwleBuilder << ",B" << segmentIdx << ":0";
    pwleBuilder << ",AR" << segmentIdx << ":0";
    pwleBuilder << ",V" << segmentIdx << ":0";
}

void constructActiveSegment(std::ostringstream &pwleBuilder, const int &segmentIdx, int duration,
                            float amplitude, float frequency) {
    pwleBuilder << ",T" << segmentIdx << ":" << duration;
    pwleBuilder << ",L" << segmentIdx << ":" << amplitude;
    pwleBuilder << ",F" << segmentIdx << ":" << frequency;
    constructActiveDefaults(pwleBuilder, segmentIdx);
}

void constructBrakingSegment(std::ostringstream &pwleBuilder, const int &segmentIdx, int duration,
                             Braking brakingType) {
    pwleBuilder << ",T" << segmentIdx << ":" << duration;
    pwleBuilder << ",L" << segmentIdx << ":" << 0;
    pwleBuilder << ",F" << segmentIdx << ":" << 0;
    pwleBuilder << ",C" << segmentIdx << ":0";
    pwleBuilder << ",B" << segmentIdx << ":"
                << static_cast<std::underlying_type<Braking>::type>(brakingType);
    pwleBuilder << ",AR" << segmentIdx << ":0";
    pwleBuilder << ",V" << segmentIdx << ":0";
}

ndk::ScopedAStatus Vibrator::composePwle(const std::vector<PrimitivePwle> &composite,
                                         const std::shared_ptr<IVibratorCallback> &callback) {
    std::ostringstream pwleBuilder;
    std::string pwleQueue;

    int compositionSizeMax;
    getPwleCompositionSizeMax(&compositionSizeMax);
    if (composite.size() <= 0 || composite.size() > compositionSizeMax) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
    }

    float prevEndAmplitude;
    float prevEndFrequency;
    resetPreviousEndAmplitudeEndFrequency(prevEndAmplitude, prevEndFrequency);

    int segmentIdx = 0;
    uint32_t totalDuration = 0;

    pwleBuilder << "S:0,WF:4,RP:0,WT:0";

    for (auto &e : composite) {
        switch (e.getTag()) {
            case PrimitivePwle::active: {
                auto active = e.get<PrimitivePwle::active>();
                if (active.duration < 0 ||
                    active.duration > COMPOSE_PWLE_PRIMITIVE_DURATION_MAX_MS) {
                    return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
                }
                if (active.startAmplitude < PWLE_LEVEL_MIN ||
                    active.startAmplitude > PWLE_LEVEL_MAX ||
                    active.endAmplitude < PWLE_LEVEL_MIN || active.endAmplitude > PWLE_LEVEL_MAX) {
                    return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
                }
                if (active.startFrequency < PWLE_FREQUENCY_MIN_HZ ||
                    active.startFrequency > PWLE_FREQUENCY_MAX_HZ ||
                    active.endFrequency < PWLE_FREQUENCY_MIN_HZ ||
                    active.endFrequency > PWLE_FREQUENCY_MAX_HZ) {
                    return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
                }

                if (!((active.startAmplitude == prevEndAmplitude) &&
                      (active.startFrequency == prevEndFrequency))) {
                    constructActiveSegment(pwleBuilder, segmentIdx, 0, active.startAmplitude,
                                           active.startFrequency);
                    incrementIndex(segmentIdx);
                }

                constructActiveSegment(pwleBuilder, segmentIdx, active.duration,
                                       active.endAmplitude, active.endFrequency);
                incrementIndex(segmentIdx);

                prevEndAmplitude = active.endAmplitude;
                prevEndFrequency = active.endFrequency;
                totalDuration += active.duration;
                break;
            }
            case PrimitivePwle::braking: {
                auto braking = e.get<PrimitivePwle::braking>();
                if (braking.braking > Braking::CLAB) {
                    return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
                }
                if (braking.duration > COMPOSE_PWLE_PRIMITIVE_DURATION_MAX_MS) {
                    return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
                }

                constructBrakingSegment(pwleBuilder, segmentIdx, 0, braking.braking);
                incrementIndex(segmentIdx);

                constructBrakingSegment(pwleBuilder, segmentIdx, braking.duration, braking.braking);
                incrementIndex(segmentIdx);

                resetPreviousEndAmplitudeEndFrequency(prevEndAmplitude, prevEndFrequency);
                totalDuration += braking.duration;
                break;
            }
        }
    }

    std::thread([=] {
        LOG(INFO) << "Starting composePwle on another thread";
        usleep(totalDuration * 1000);
        if (callback != nullptr) {
            LOG(INFO) << "Notifying compose PWLE complete";
            callback->onComplete();
        }
    }).detach();

    return ndk::ScopedAStatus::ok();
}

}  // namespace vibrator
}  // namespace hardware
}  // namespace android
}  // namespace aidl
