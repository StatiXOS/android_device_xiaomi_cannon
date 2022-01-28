/*
 * Copyright (C) 2022 StatiX
 * SPDX-License-Identifer: Apache-2.0
 */

#include <android-base/file.h>
#include <json/reader.h>
#include <json/value.h>
#include <fstream>

#define POWERHINT_CONFIG "/vendor/etc/power/powerhint.json"

namespace android {
namespace powerhintparser {

using namespace Json::Reader;
using namespace Json::Value;

bool HintParser::isConfigAvailable() {
    ifstream config(POWERHINT_CONFIG);
    return true;
}

bool HintParser::isConfigProper() {
    if (isConfigAvailable) {
        if(!reader.parse(POWERHINT_CONFIG)) {
           LOG(ERROR) << reader.getFormattedErrorMessages();
           return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

int HintParser::parseHint() {
    
}

}
}