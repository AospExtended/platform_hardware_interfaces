/*
 * Copyright 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <android-base/logging.h>
#include <android/hardware/tv/tuner/1.0/IFrontend.h>
#include <android/hardware/tv/tuner/1.0/IFrontendCallback.h>
#include <android/hardware/tv/tuner/1.0/types.h>
#include <android/hardware/tv/tuner/1.1/ITuner.h>
#include <binder/MemoryDealer.h>
#include <gtest/gtest.h>
#include <hidl/GtestPrinter.h>
#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>
#include <hidl/ServiceManagement.h>
#include <hidl/Status.h>
#include <hidlmemory/FrameworkUtils.h>
#include <utils/Condition.h>
#include <utils/Mutex.h>
#include <map>

#include "VtsHalTvTunerV1_1TestConfigurations.h"

#define WAIT_TIMEOUT 3000000000
#define INVALID_ID -1

using android::Condition;
using android::IMemory;
using android::IMemoryHeap;
using android::MemoryDealer;
using android::Mutex;
using android::sp;
using android::hardware::fromHeap;
using android::hardware::hidl_vec;
using android::hardware::Return;
using android::hardware::Void;
using android::hardware::tv::tuner::V1_0::FrontendEventType;
using android::hardware::tv::tuner::V1_0::FrontendId;
using android::hardware::tv::tuner::V1_0::FrontendInfo;
using android::hardware::tv::tuner::V1_0::FrontendScanMessage;
using android::hardware::tv::tuner::V1_0::FrontendScanMessageType;
using android::hardware::tv::tuner::V1_0::IFrontend;
using android::hardware::tv::tuner::V1_0::IFrontendCallback;
using android::hardware::tv::tuner::V1_0::Result;
using android::hardware::tv::tuner::V1_1::ITuner;

using ::testing::AssertionResult;

using namespace std;

#define INVALID_ID -1

class FrontendCallback : public IFrontendCallback {
  public:
    virtual Return<void> onEvent(FrontendEventType frontendEventType) override;
    virtual Return<void> onScanMessage(FrontendScanMessageType type,
                                       const FrontendScanMessage& message) override;
};

class FrontendTests {
  public:
    sp<ITuner> mService;

    void setService(sp<ITuner> tuner) { mService = tuner; }

    AssertionResult getFrontendIds();
    AssertionResult getFrontendInfo(uint32_t frontendId);
    AssertionResult openFrontendById(uint32_t frontendId);
    AssertionResult setFrontendCallback();
    AssertionResult closeFrontend();

    void getFrontendIdByType(FrontendType feType, uint32_t& feId);

  protected:
    static AssertionResult failure() { return ::testing::AssertionFailure(); }
    static AssertionResult success() { return ::testing::AssertionSuccess(); }

    sp<IFrontend> mFrontend;
    FrontendInfo mFrontendInfo;
    sp<FrontendCallback> mFrontendCallback;
    hidl_vec<FrontendId> mFeIds;

    bool mIsSoftwareFe = false;
};