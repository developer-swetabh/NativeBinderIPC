#include "IMockService.h"
#include "MockService.cpp"
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <utils/String16.h>

using namespace android;

int main() {
    sp<ProcessState> proc(ProcessState::self());
    sp<IServiceManager> sm = defaultServiceManager();

    sp<IBinder> binder = new MockService();
    status_t status = sm->addService(String16("mockservice"), binder);

    if (status != OK) {
        ALOGE("Failed to register mockservice (%d)", status);
        return 1;
    }

    ALOGI("MockService registered successfully");

    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();

    return 0;
}
