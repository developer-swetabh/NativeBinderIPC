#ifndef IMOCKSERVICE_H
#define IMOCKSERVICE_H

#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/String16.h>

namespace android {

struct MyData {
    int id;
    String16 name;

    status_t writeToParcel(Parcel* parcel) const {
        parcel->writeInt32(id);
        parcel->writeString16(name);
        return OK;
    }

    status_t readFromParcel(const Parcel* parcel) {
        parcel->readInt32(&id);
        parcel->readString16(&name);
        return OK;
    }
};

// Interface for Callback
class IMockCallback : public IInterface {
public:
    DECLARE_META_INTERFACE(MockCallback);
    virtual void onDataPushed(const MyData& data) = 0;
};

// Proxy (client-side)
class BpMockCallback : public BpInterface<IMockCallback> {
public:
    explicit BpMockCallback(const sp<IBinder>& impl)
        : BpInterface<IMockCallback>(impl) {}

    void onDataPushed(const MyData& data) override {
        Parcel parcel, reply;
        parcel.writeInterfaceToken(IMockCallback::getInterfaceDescriptor());
        data.writeToParcel(&parcel);
        remote()->transact(1, parcel, &reply);
    }
};

// Stub (server-side)
class BnMockCallback : public BnInterface<IMockCallback> {
public:
    status_t onTransact(uint32_t code, const Parcel& data,
                        Parcel* reply, uint32_t flags = 0) override;
};

// Interface for service
class IMockService : public IInterface {
public:
    DECLARE_META_INTERFACE(MockService);

    virtual MyData getData() = 0;
    virtual void setData(const MyData& data) = 0;
    virtual void registerCallback(const sp<IMockCallback>& callback) = 0;
    virtual MyData sendDataAndGetResponse(const MyData& data) = 0;
    virtual void processFrames(int fd, int size) = 0;
};

// Proxy (client-side)
class BpMockService : public BpInterface<IMockService> {
public:
    explicit BpMockService(const sp<IBinder>& impl)
        : BpInterface<IMockService>(impl) {}

    MyData getData() override {
        Parcel data, reply;
        data.writeInterfaceToken(IMockService::getInterfaceDescriptor());
        remote()->transact(1, data, &reply);
        MyData result;
        result.readFromParcel(&reply);
        return result;
    }

    void setData(const MyData& input) override {
        Parcel data, reply;
        data.writeInterfaceToken(IMockService::getInterfaceDescriptor());
        input.writeToParcel(&data);
        remote()->transact(2, data, &reply);
    }

    void registerCallback(const sp<IMockCallback>& callback) override {
        Parcel data, reply;
        data.writeInterfaceToken(IMockService::getInterfaceDescriptor());
        data.writeStrongBinder(IInterface::asBinder(callback));
        remote()->transact(3, data, &reply);
    }

    MyData sendDataAndGetResponse(const MyData& input) override {
        Parcel data, reply;
        data.writeInterfaceToken(IMockService::getInterfaceDescriptor());
        input.writeToParcel(&data);
        remote()->transact(4, data, &reply);
        MyData result;
        result.readFromParcel(&reply);
        return result;
    }

    void processFrames(int fd, int size) override {
        Parcel data, reply;
        data.writeInterfaceToken(IMockService::getInterfaceDescriptor());
        data.writeFileDescriptor(fd);
        data.writeInt32(size);
        remote()->transact(5, data, &reply);
    }
};

// Stub (server-side)
class BnMockService : public BnInterface<IMockService> {
public:
    status_t onTransact(uint32_t code, const Parcel& data,
                        Parcel* reply, uint32_t flags = 0) override;
};

} // namespace android

#endif // IMOCKSERVICE_H
