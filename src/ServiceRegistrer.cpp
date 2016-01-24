#include "ServiceRegistrer.h"

void bluetooth::ServiceRegistrer::registerService(const ServiceRecord &serviceRecord) {

    uuid_struct uuidKey = toUUIDStruct(serviceRecord.uuid);

    ServiceRegistrer &sr = getInstance();

    if(sr.serviceMap.find(uuidKey) != sr.serviceMap.end()) {
        throw std::runtime_error("cannot register service. already registred.");
    }

    sr.serviceMap[uuidKey] = sr.registerRecord(serviceRecord);

}

void bluetooth::ServiceRegistrer::unregisterService(const ServiceRecord &serviceRecord) {
    ServiceRegistrer &sr = getInstance();
    if(sr.session == 0) {
        throw std::runtime_error("cannot unregister service. session not open.");
    }
    uuid_struct uuidKey = toUUIDStruct(serviceRecord.uuid);
    std::map<uuid_struct, sdp_record_t*>::iterator it = sr.serviceMap.find(uuidKey);
    if(it == sr.serviceMap.end()) {
        throw std::runtime_error("cannot unregister service. service doesn't exist.");
    }
    sdp_record_unregister(sr.session, it->second);
    /// I assume that the record is freed by the
    /// previous call because the next one gives me
    /// seg fault.
//        sdp_record_free(it->second);
    sr.serviceMap.erase(it);
}

sdp_record_t * bluetooth::ServiceRegistrer::registerRecord(const ServiceRecord &serviceRecord) {

    uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid;
    sdp_list_t *l2cap_list = 0,
               *rfcomm_list = 0,
               *root_list = 0,
               *proto_list = 0,
               *access_proto_list = 0;
    sdp_data_t *channel = 0;

    sdp_record_t *record = sdp_record_alloc();

    // this happened to work.
    unsigned int uuidbl[4] = {
        htonl(serviceRecord.uuid[0]),
        htonl(serviceRecord.uuid[1]),
        htonl(serviceRecord.uuid[2]),
        htonl(serviceRecord.uuid[3])
    };

    // set the general service ID
    sdp_uuid128_create( &svc_uuid, &uuidbl );
    sdp_set_service_id( record, svc_uuid );

    // make the service record publicly browsable
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups( record, root_list );

    // set l2cap information
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_list = sdp_list_append( 0, &l2cap_uuid );
    proto_list = sdp_list_append( 0, l2cap_list );

    // set rfcomm information
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &serviceRecord.rfcommChannel);
    rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
    sdp_list_append( rfcomm_list, channel );
    sdp_list_append( proto_list, rfcomm_list );

    // attach protocol information to service record
    access_proto_list = sdp_list_append( 0, proto_list );
    sdp_set_access_protos( record, access_proto_list );

    // set the name, provider, and description
    sdp_set_info_attr(record, serviceRecord.serviceName.c_str(),
                      serviceRecord.serviceProv.c_str(),
                      serviceRecord.serviceDesc.c_str());

    // connect to the local SDP server, register the service record, and
    // disconnect
    bdaddr_t any = {{0,0,0,0,0,0}};
    bdaddr_t local = {{0,0,0, 0xff,0xff, 0xff}};
    if(session == 0) {
        session = sdp_connect( &any, &local, SDP_RETRY_IF_BUSY );

        if(session == 0) {
            // cleanup
            sdp_data_free( channel );
            sdp_list_free( l2cap_list, 0 );
            sdp_list_free( rfcomm_list, 0 );
            sdp_list_free( root_list, 0 );
            sdp_list_free( access_proto_list, 0 );
            throw std::runtime_error("cannot sdp_connect");
        }
    }
    if(sdp_record_register(session, record, 0) < 0) {
        // cleanup
        sdp_data_free( channel );
        sdp_list_free( l2cap_list, 0 );
        sdp_list_free( rfcomm_list, 0 );
        sdp_list_free( root_list, 0 );
        sdp_list_free( access_proto_list, 0 );
        sdp_close( session );
        throw std::runtime_error("cannot sdp_record_register");
    }

    // cleanup
    sdp_data_free( channel );
    sdp_list_free( l2cap_list, 0 );
    sdp_list_free( rfcomm_list, 0 );
    sdp_list_free( root_list, 0 );
    sdp_list_free( access_proto_list, 0 );

    return record;
    // allocated : sdp_record_t *record = sdp_record_alloc();
}

bluetooth::ServiceRegistrer::~ServiceRegistrer() {
    std::map<uuid_struct, sdp_record_t*>::iterator it;
    for(it = serviceMap.begin() ; it != serviceMap.end() ; it++) {
        sdp_record_unregister(session, it->second);
    }
    sdp_close( session );
}
