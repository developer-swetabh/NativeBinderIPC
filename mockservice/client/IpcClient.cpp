#include "IMockService.h"
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cutils/ashmem.h>
#include <string.h>

using namespace android;

class MockCallback : public BnMockCallback {
public:
    void onDataPushed(const MyData& data) override {
        ALOGI("Callback received: id=%d, name=%s", data.id, String8(data.name).string());
    }
};

class IpcClient {
public:
    static void run() {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder = sm->getService(String16("mockservice"));

        if (binder == nullptr) {
            ALOGE("Could not find mockservice");
            return;
        }

        sp<IMockService> service = interface_cast<IMockService>(binder);
        sp<MockCallback> callback = new MockCallback();

        service->registerCallback(callback);

        MyData data;
        data.id = 100;
        data.name = String16("MarelliClient");
        service->setData(data);

        MyData stored = service->getData();
        ALOGI("getData(): id=%d, name=%s", stored.id, String8(stored.name).string());

        MyData response = service->sendDataAndGetResponse(data);
        ALOGI("sendDataAndGetResponse(): id=%d, name=%s", response.id, String8(response.name).string());

        // Create ashmem region
        const int bufferSize = 34;
        int ashmemFd = ashmem_create_region("camera_frames", bufferSize);
        if (ashmemFd < 0) {
            ALOGE("Failed to create ashmem region");
            return;
        }

        // Map memory
        void* sharedMemory = mmap(nullptr, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, ashmemFd, 0);
        if (sharedMemory == MAP_FAILED) {
            ALOGE("Failed to mmap ashmem region");
            close(ashmemFd);
            return;
        }

        //char* frames = static_cast<char*>(sharedMemory);
        char* frames = "Hello, this is a test frame data!";
        
        memcpy(sharedMemory, frames, bufferSize);

        ALOGI("Frames before sending: %s", frames);

        ALOGI("Sending frames to server...");
        service->processFrames(ashmemFd, bufferSize);

        memcpy(frames, sharedMemory, bufferSize);
        ALOGI("Frames after processing: %s", frames);
        ALOGI("Shared Memory: %s", static_cast<char*>(sharedMemory));
        munmap(sharedMemory, bufferSize);
        close(ashmemFd);
        ALOGI("Unmapped and closed ashmem region");
        
        ALOGI("Client work done...");
        sleep(3);
    }
};
