#include "IMockService.h"
#include <binder/Parcel.h>

namespace android {

IMPLEMENT_META_INTERFACE(MockService, "vendor.marelli.IMockService");
IMPLEMENT_META_INTERFACE(MockCallback, "vendor.marelli.IMockCallback");

status_t BnMockService::onTransact(uint32_t code, const Parcel& data,
                                   Parcel* reply, uint32_t flags) {
    switch (code) {
        case 1: {
            CHECK_INTERFACE(IMockService, data, reply);
            MyData result = getData();
            result.writeToParcel(reply);
            return NO_ERROR;
        }
        case 2: {
            CHECK_INTERFACE(IMockService, data, reply);
            MyData input;
            input.readFromParcel(&data);
            setData(input);
            return NO_ERROR;
        }
        case 3: {
            CHECK_INTERFACE(IMockService, data, reply);
            sp<IBinder> binder = data.readStrongBinder();
            sp<IMockCallback> callback = interface_cast<IMockCallback>(binder);
            registerCallback(callback);
            return NO_ERROR;
        }
        case 4: {
            CHECK_INTERFACE(IMockService, data, reply);
            MyData input;
            input.readFromParcel(&data);
            MyData result = sendDataAndGetResponse(input);
            result.writeToParcel(reply);
            return NO_ERROR;
        }
        case 5: {
            CHECK_INTERFACE(IMockService, data, reply);
            int fd = dup(data.readFileDescriptor());
            int size = data.readInt32();
            processFrames(fd, size);
            return NO_ERROR;
        }
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

status_t BnMockCallback::onTransact(uint32_t code, const Parcel& data,
                                    Parcel* reply, uint32_t flags) {
    switch (code) {
        case 1: {
            CHECK_INTERFACE(IMockCallback, data, reply);
            MyData incoming;
            incoming.readFromParcel(&data);
            onDataPushed(incoming);
            return NO_ERROR;
        }
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

} // namespace android
