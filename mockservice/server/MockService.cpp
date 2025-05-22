#include "IMockService.h"
#include <utils/Log.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace android {

class MockService : public BnMockService {
public:
    MyData currentData;
    sp<IMockCallback> clientCallback;

    MockService() {
        currentData.id = 42;
        currentData.name = String16("Default");
    }

    MyData getData() override {
        ALOGI("MockService::getData()");
        return currentData;
    }

    void setData(const MyData& data) override {
        ALOGI("MockService::setData() id=%d, name=%s", data.id, String8(data.name).string());
        currentData = data;
    }

    void registerCallback(const sp<IMockCallback>& callback) override {
        ALOGI("MockService::registerCallback()");
        clientCallback = callback;
    }

    MyData sendDataAndGetResponse(const MyData& data) override {
        ALOGI("MockService::sendDataAndGetResponse()");
        currentData = data;
        if (clientCallback != nullptr) {
            clientCallback->onDataPushed(currentData);
        } else {
            ALOGI("MockService::No callback registered");
        }
        return currentData;
    }

    void processFrames(int fd, int size) override {
        int dupFd = dup(fd);
        if (dupFd < 0) {
            ALOGE("MockService::failed to duplicate file descriptor");
            return;
        }
        ALOGI("MockService::processFrames() fd=%d, size=%d", fd, size);
        // Simulate processing frames
        void* buffer = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (buffer == MAP_FAILED) {
            ALOGE("MockService::mmap() failed");
            return;
        }

        char* data = static_cast<char*>(buffer);
        ALOGI("MockService::First byte before processing: %s", data);

        // Modify: example invert colors
        for (int i = 0; i < size; ++i) {
            data[i] = 'A'; // Invert colors
        }

        ALOGI("MockService:: Frame buffer modified");
        // Unmap the buffer
        munmap(buffer, size);
        ALOGI("MockService::Buffer unmapped");
        close(fd);
        ALOGI("MockService::File descriptor closed");
    }
};

} // namespace android
