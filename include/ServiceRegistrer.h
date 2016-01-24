#ifndef SERVICEREGISTRER_H
#define SERVICEREGISTRER_H

#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include <string>
#include <map>
#include <stdexcept>

namespace bluetooth {

struct ServiceRecord {

    unsigned int uuid[4];
    std::string serviceName;
    std::string serviceDesc;
    std::string serviceProv;
    unsigned char rfcommChannel;

#define UUID(a,b,c,d)((const unsigned int[4]){a, b, c, d})

    ServiceRecord(const unsigned int uuid[4], const std::string &serviceName,
                  const std::string &serviceDescription, const std::string &serviceProvider,
                  unsigned char rfcommChannel) :
    serviceName(serviceName), serviceDesc(serviceDescription),
    serviceProv(serviceProvider), rfcommChannel(rfcommChannel) {
        this->uuid[0] = uuid[0];
        this->uuid[1] = uuid[1];
        this->uuid[2] = uuid[2];
        this->uuid[3] = uuid[3];
    }

    ServiceRecord(const ServiceRecord &sr) :
        serviceName(sr.serviceName), serviceDesc(sr.serviceDesc),
        serviceProv(sr.serviceProv), rfcommChannel(sr.rfcommChannel) {
        this->uuid[0] = sr.uuid[0];
        this->uuid[1] = sr.uuid[1];
        this->uuid[2] = sr.uuid[2];
        this->uuid[3] = sr.uuid[3];
    }
    ServiceRecord& operator=(const ServiceRecord &sr) {
        this->uuid[0] = sr.uuid[0];
        this->uuid[1] = sr.uuid[1];
        this->uuid[2] = sr.uuid[2];
        this->uuid[3] = sr.uuid[3];
        serviceName = sr.serviceName;
        serviceDesc = sr.serviceDesc;
        serviceProv = sr.serviceProv;
        rfcommChannel = sr.rfcommChannel;
        return *this;
    }
};


class ServiceRegistrer {

    struct uuid_struct {
        int a, b, c, d;
        friend bool operator<(const uuid_struct &a, const uuid_struct &b) {
            return  a.a < b.a ||
                    a.b < b.b ||
                    a.c < b.c ||
                    a.d < b.d;
        }
    };

    std::map<uuid_struct, sdp_record_t *> serviceMap;

    sdp_session_t * session;

    sdp_record_t * registerRecord(const ServiceRecord &serviceRecord) ;

    static uuid_struct toUUIDStruct(const unsigned int uuid[4]) {
        uuid_struct uuidKey;
        uuidKey.a = uuid[0];
        uuidKey.b = uuid[1];
        uuidKey.c = uuid[2];
        uuidKey.d = uuid[3];
        return uuidKey;
    }

    ServiceRegistrer() : session(0) {

    }

    ~ServiceRegistrer();

    static ServiceRegistrer& getInstance() {
        static ServiceRegistrer sr;

        return sr;
    }

public:

    static void registerService(const ServiceRecord &serviceRecord);

    static void unregisterService(const ServiceRecord &serviceRecord);
};


}

#endif // SERVICEREGISTRER_H
