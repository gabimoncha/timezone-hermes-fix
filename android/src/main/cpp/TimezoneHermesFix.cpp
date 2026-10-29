#include "TimezoneHermesFix.h"
#include <jsi/jsi.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <fbjni/fbjni.h>
#include <jsi/hermes-interfaces.h>
#include <android/log.h>

using namespace facebook;
using namespace facebook::jni;
using namespace facebook::react;

#define LOG_TAG "TimezoneHermesFix"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

void TimezoneHermesFix::registerNatives() {
    registerHybrid({
        makeNativeMethod("initHybrid", TimezoneHermesFix::initHybrid),
        makeNativeMethod("resetTzHermes", TimezoneHermesFix::resetTzHermes),
    });
}

jni::local_ref<TimezoneHermesFix::jhybriddata> TimezoneHermesFix::initHybrid(
    jni::alias_ref<jhybridobject>) {
    return makeCxxInstance();
}

void TimezoneHermesFix::resetTzHermes(jlong jsRuntimePtr) {
    LOGD("resetTzHermes called with jsRuntimePtr: %ld", jsRuntimePtr);

    auto jsiRuntime = reinterpret_cast<jsi::Runtime*>(jsRuntimePtr);
    if (jsiRuntime == nullptr) {
        LOGE("jsiRuntime is null");
        return;
    }

    // The runtime may be wrapped; castInterface finds Hermes inside it, as on iOS.
    auto hermesRuntime = jsi::castInterface<facebook::hermes::IHermes>(jsiRuntime);
    if (hermesRuntime == nullptr) {
        LOGE("the JS runtime does not implement IHermes");
        return;
    }

    try {
        // Call resetTimezoneCache on Hermes runtime (same as iOS)
        hermesRuntime->resetTimezoneCache();
        LOGD("Successfully called resetTimezoneCache on Hermes runtime");

    } catch (const jsi::JSError& error) {
        LOGE("JSI Error calling resetTimezoneCache: %s", error.getMessage().c_str());
    } catch (const std::exception& error) {
        LOGE("Exception calling resetTimezoneCache: %s", error.what());
    }
}
