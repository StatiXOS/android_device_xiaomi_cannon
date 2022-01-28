/*
 * Copyright (C) 2022 StatiX
 * SPDX-License-Identifer: Apache-2.0
 */

#include <android-base/file.h>
#include <json/reader.h>
#include <json/value.h>

#define POWERHINT_CONFIG "/vendor/etc/power/powerhint.json"

namespace android {
namespace powerhintparser {

class HintParser {
  public:
    bool isConfigAvailable();
    bool isConfigProper();
    int parseHint();
}

}
}