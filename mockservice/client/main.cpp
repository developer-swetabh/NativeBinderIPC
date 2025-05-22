#include "IpcClient.cpp"
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <utils/Log.h>

using namespace android;

int main() {
    ALOGI("Starting IPC Client");

    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();

    IpcClient::run();

    IPCThreadState::self()->joinThreadPool();

    return 0;
}
