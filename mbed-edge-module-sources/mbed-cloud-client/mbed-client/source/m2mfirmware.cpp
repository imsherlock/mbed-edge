/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed-client/m2mfirmware.h"
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
#include "include/nsdlaccesshelper.h"

#define BUFFER_SIZE 21
#define TRACE_GROUP "mClt"

M2MFirmware* M2MFirmware::_instance = NULL;

M2MFirmware* M2MFirmware::get_instance()
{
    if(_instance == NULL) {
        _instance = new M2MFirmware();
    }
    return _instance;
}

void M2MFirmware::delete_instance()
{
    delete _instance;
    _instance = NULL;
}

M2MFirmware::M2MFirmware()
: M2MObject(M2M_FIRMWARE_ID, stringdup(M2M_FIRMWARE_ID))
{
    M2MBase::set_register_uri(false);
    M2MBase::set_operation(M2MBase::GET_PUT_ALLOWED);
    _firmware_instance = M2MObject::create_object_instance();
    if(_firmware_instance) {
        _firmware_instance->set_operation(M2MBase::GET_PUT_ALLOWED);
        create_mandatory_resources();
    }
}

M2MFirmware::~M2MFirmware()
{
}

// Conditionally put the static part of parameter struct into flash.
// Unfortunately this can't be done yet by default as there is old API which
// may be used to modify the values in sn_nsdl_static_resource_parameters_s.
#ifdef MEMORY_OPTIMIZED_API
#define STATIC_PARAM_TYPE const
#else
#define STATIC_PARAM_TYPE
#endif

#define PACKAGE_PATH FIRMWARE_PATH_PREFIX FIRMWARE_PACKAGE

#ifdef RESOURCE_ATTRIBUTES_LIST
sn_nsdl_attribute_item_s default_attributes[2] = {
        {ATTR_RESOURCE_TYPE, OMA_RESOURCE_TYPE},
        {ATTR_END, 0}
};
#endif

STATIC_PARAM_TYPE
static sn_nsdl_static_resource_parameters_s firmware_package_params_static = {
#ifndef RESOURCE_ATTRIBUTES_LIST
#ifndef DISABLE_RESOURCE_TYPE
    (char*)OMA_RESOURCE_TYPE,      // resource_type_ptr
#endif
#ifndef DISABLE_INTERFACE_DESCRIPTION
    (char*)"",                     // interface_description_ptr
#endif
#else
    default_attributes,
#endif
    (char*)PACKAGE_PATH,    // path
//    (uint8_t*)"",           // resource
//    0,                      // resourcelen
    false,                  // external_memory_block
    SN_GRS_DYNAMIC,         // mode
    false                   // free_on_delete
};

#define PACKAGE_URI_PATH FIRMWARE_PATH_PREFIX FIRMWARE_PACKAGE_URI

STATIC_PARAM_TYPE
static sn_nsdl_static_resource_parameters_s firmware_package_uri_params_static = {
#ifndef RESOURCE_ATTRIBUTES_LIST
#ifndef DISABLE_RESOURCE_TYPE
    (char*)OMA_RESOURCE_TYPE,      // resource_type_ptr
#endif
#ifndef DISABLE_INTERFACE_DESCRIPTION
    (char*)"",                     // interface_description_ptr
#endif
#else
    default_attributes,
#endif
    (char*)PACKAGE_URI_PATH, // path
//    (uint8_t*)"",           // resource
//    0,                      // resourcelen
    false,                  // external_memory_block
    SN_GRS_DYNAMIC,         // mode
    false                   // free_on_delete
};

#define UPDATE_PATH FIRMWARE_PATH_PREFIX FIRMWARE_UPDATE

STATIC_PARAM_TYPE
static sn_nsdl_static_resource_parameters_s firmware_update_params_static = {
#ifndef RESOURCE_ATTRIBUTES_LIST
#ifndef DISABLE_RESOURCE_TYPE
    (char*)OMA_RESOURCE_TYPE,      // resource_type_ptr
#endif
#ifndef DISABLE_INTERFACE_DESCRIPTION
    (char*)"",                     // interface_description_ptr
#endif
#else
    default_attributes,
#endif
    (char*)UPDATE_PATH,     // path
//    (uint8_t*)"",           // resource
//    0,                      // resourcelen
    false,                  // external_memory_block
    SN_GRS_DYNAMIC,         // mode
    false                   // free_on_delete
};

#define STATE_URI_PATH FIRMWARE_PATH_PREFIX FIRMWARE_STATE

STATIC_PARAM_TYPE
static sn_nsdl_static_resource_parameters_s firmware_state_params_static = {
#ifndef RESOURCE_ATTRIBUTES_LIST
#ifndef DISABLE_RESOURCE_TYPE
    (char*)OMA_RESOURCE_TYPE,      // resource_type_ptr
#endif
#ifndef DISABLE_INTERFACE_DESCRIPTION
    (char*)"",                     // interface_description_ptr
#endif
#else
    default_attributes,
#endif
    (char*)STATE_URI_PATH,   // path
    false,                  // external_memory_block
    SN_GRS_DYNAMIC,         // mode
    false                   // free_on_delete
};

#define UPDATE_RESULT_PATH FIRMWARE_PATH_PREFIX FIRMWARE_UPDATE_RESULT

STATIC_PARAM_TYPE
static sn_nsdl_static_resource_parameters_s firmware_update_result_params_static = {
#ifndef RESOURCE_ATTRIBUTES_LIST
#ifndef DISABLE_RESOURCE_TYPE
    (char*)OMA_RESOURCE_TYPE,      // resource_type_ptr
#endif
#ifndef DISABLE_INTERFACE_DESCRIPTION
    (char*)"",                     // interface_description_ptr
#endif
#else
    default_attributes,
#endif
    (char*)UPDATE_RESULT_PATH, // path
    false,                  // external_memory_block
    SN_GRS_DYNAMIC,         // mode
    false                   // free_on_delete
};

static sn_nsdl_dynamic_resource_parameters_s firmware_package_params_dynamic = {
    __nsdl_c_callback,
    &firmware_package_params_static,
    NULL,
    {NULL, NULL},                     // link
    0,
    COAP_CONTENT_OMA_PLAIN_TEXT_TYPE, // coap_content_type
    0, // msg_id
    M2MBase::PUT_ALLOWED,   // access
    0,                      // registered
    false,                  // publish_uri
    false,                  // free_on_delete
    true,                   // observable
    false,                  // auto-observable
    NOTIFICATION_STATUS_INIT
};

static sn_nsdl_dynamic_resource_parameters_s firmware_package_uri_params_dynamic = {
    __nsdl_c_callback,
    &firmware_package_uri_params_static,
    NULL,
    {NULL, NULL},                     // link
    0,
    COAP_CONTENT_OMA_PLAIN_TEXT_TYPE, // coap_content_type
    0, // msg_id
    M2MBase::PUT_ALLOWED,   // access
    0,                      // registered
    false,                  // publish_uri
    false,                  // free_on_delete
    true,                   // observable
    false,                  // auto-observable
    NOTIFICATION_STATUS_INIT
};

static sn_nsdl_dynamic_resource_parameters_s firmware_update_params_dynamic = {
    __nsdl_c_callback,
    &firmware_update_params_static,
    NULL,
    {NULL, NULL},                     // link
    0,
    COAP_CONTENT_OMA_PLAIN_TEXT_TYPE, // coap_content_type
    0, // msg_id
    M2MBase::NOT_ALLOWED,   // access
    0,                      // registered
    false,                  // publish_uri
    false,                  // free_on_delete
    true,                   // observable
    false,                  // auto-observable
    NOTIFICATION_STATUS_INIT
};

static sn_nsdl_dynamic_resource_parameters_s firmware_state_params_dynamic = {
    __nsdl_c_callback,
    &firmware_state_params_static,
    NULL,                   // resource
    {NULL, NULL},           // link
    0,                      // resourcelen
    COAP_CONTENT_OMA_PLAIN_TEXT_TYPE, // coap_content_type
    0, // msg_id
    M2MBase::GET_ALLOWED,   // access
    0,                      // registered
    false,                  // publish_uri
    false,                  // free_on_delete
    true,                   // observable
    false,                  // auto-observable
    NOTIFICATION_STATUS_INIT
};

static sn_nsdl_dynamic_resource_parameters_s firmware_update_result_params_dynamic = {
    __nsdl_c_callback,
    &firmware_update_result_params_static,
    NULL,                   // resource
    {NULL, NULL},           // link
    0,                      // resourcelen
    COAP_CONTENT_OMA_PLAIN_TEXT_TYPE, // coap_content_type
    0, // msg_id
    M2MBase::GET_ALLOWED,   // access
    0,                      // registered
    false,                  // publish_uri
    false,                  // free_on_delete
    true,                   // observable
    false,                  // auto-observable
    NOTIFICATION_STATUS_INIT
};

const static M2MBase::lwm2m_parameters firmware_package_params = {
    0, // max_age
    (char*)FIRMWARE_PACKAGE,
    &firmware_package_params_dynamic,
    M2MBase::Resource, // base_type
    M2MBase::OPAQUE,
    false,
    false, // free_on_delete
    false  // identifier_int_type
};

const static M2MBase::lwm2m_parameters firmware_package_uri_params = {
    0, // max_age
    (char*)FIRMWARE_PACKAGE_URI,
    &firmware_package_uri_params_dynamic,
    M2MBase::Resource, // base_type
    M2MBase::STRING,
    false,
    false, // free_on_delete
    false  // identifier_int_type
};

const static M2MBase::lwm2m_parameters firmware_update_params = {
    0, // max_age
    (char*)FIRMWARE_UPDATE,
    &firmware_update_params_dynamic,
    M2MBase::Resource, // base_type
    M2MBase::OPAQUE,
    false,
    false, // free_on_delete
    false  // identifier_int_type
};

const static M2MBase::lwm2m_parameters firmware_state_params = {
    0, // max_age
    (char*)FIRMWARE_STATE,
    &firmware_state_params_dynamic,
    M2MBase::Resource, // base_type
    M2MBase::INTEGER,
    false,
    false, // free_on_delete
    false  // identifier_int_type
};

const static M2MBase::lwm2m_parameters firmware_update_result_params = {
    0, // max_age
    (char*)FIRMWARE_UPDATE_RESULT,
    &firmware_update_result_params_dynamic,
    M2MBase::Resource, // base_type
    M2MBase::INTEGER,
    false,
    false, // free_on_delete
    false  // identifier_int_type
};

void M2MFirmware::create_mandatory_resources()
{
    _firmware_instance->set_coap_content_type(COAP_CONTENT_OMA_TLV_TYPE);

    M2MResource* res;

    // todo:
    // perhaps we should have a API for batch creation of objects by using a array
    // of lwm2m_parameters.
    res = _firmware_instance->create_dynamic_resource(&firmware_package_params,
                                                        M2MResourceInstance::OPAQUE,
                                                        false);

    res = _firmware_instance->create_dynamic_resource(&firmware_package_uri_params,
                                                    M2MResourceInstance::STRING,
                                                    false);

    res = _firmware_instance->create_dynamic_resource(&firmware_update_params,
                                                    M2MResourceInstance::OPAQUE,
                                                    false);

    res = _firmware_instance->create_dynamic_resource(&firmware_state_params,
                                                    M2MResourceInstance::INTEGER,
                                                    true);

    // XXX: some of the tests expect to have some value available here
    if (res) {
        res->set_value(0);
    }

    res = _firmware_instance->create_dynamic_resource(&firmware_update_result_params,
                                                    M2MResourceInstance::INTEGER,
                                                    true);

    // XXX: some of the tests expect to have some value available here
    if (res) {
        res->set_value(0);
    }
}

M2MResource* M2MFirmware::create_resource(FirmwareResource resource, const String &value)
{
    M2MResource* res = NULL;
    const char* firmware_id_ptr = "";
    M2MBase::Operation operation = M2MBase::GET_ALLOWED;
    if(!is_resource_present(resource)) {
        switch(resource) {
            case PackageName:
                firmware_id_ptr = FIRMWARE_PACKAGE_NAME;
                break;
            case PackageVersion:
                firmware_id_ptr = FIRMWARE_PACKAGE_VERSION;
                break;
            default:
                break;
        }
    }
    String firmware_id(firmware_id_ptr);

    if(!firmware_id.empty() && value.size() < 256) {
        if(_firmware_instance) {
            res = _firmware_instance->create_dynamic_resource(firmware_id,
                                                            OMA_RESOURCE_TYPE,
                                                            M2MResourceInstance::STRING,
                                                            false);

            if(res) {
                res->set_register_uri(false);
                res->set_operation(operation);
                if(value.empty()) {
                    res->clear_value();
                } else {
                    res->set_value((const uint8_t*)value.c_str(),
                                   (uint32_t)value.length());
                }
            }
        }
    }
    return res;
}

M2MResource* M2MFirmware::create_resource(FirmwareResource resource, int64_t value)
{
    M2MResource* res = NULL;
    const char* firmware_id_ptr = "";
    M2MBase::Operation operation = M2MBase::GET_ALLOWED;
    if(!is_resource_present(resource)) {
        switch(resource) {
        case UpdateSupportedObjects:
            if(check_value_range(resource, value)) {
                firmware_id_ptr = FIRMWARE_UPDATE_SUPPORTED_OBJECTS;
                operation = M2MBase::GET_PUT_ALLOWED;
            }
            break;
        default:
            break;
        }
    }

    const String firmware_id(firmware_id_ptr);

    if(!firmware_id.empty()) {
        if(_firmware_instance) {
            res = _firmware_instance->create_dynamic_resource(firmware_id,
                                                            OMA_RESOURCE_TYPE,
                                                            M2MResourceInstance::INTEGER,
                                                            false);

            if(res) {
                res->set_register_uri(false);

                res->set_operation(operation);
                res->set_value(value);
            }
        }
    }
    return res;
}

bool M2MFirmware::set_resource_value(FirmwareResource resource,
                                   const String &value)
{
    bool success = false;
    M2MResource* res = get_resource(resource);
    if(res) {
        if(M2MFirmware::PackageUri == resource  ||
           M2MFirmware::PackageName == resource ||
           M2MFirmware::PackageVersion == resource) {
            if (value.size() < 256) {
                if(value.empty()) {
                    res->clear_value();
                    success = true;
                } else {
                    success = res->set_value((const uint8_t*)value.c_str(),(uint32_t)value.length());
                }
            }
        }
    }
    return success;
}

bool M2MFirmware::set_resource_value(FirmwareResource resource,
                                       int64_t value)
{
    bool success = false;
    M2MResource* res = get_resource(resource);
    if(res) {
        if(M2MFirmware::State == resource          ||
           M2MFirmware::UpdateSupportedObjects == resource ||
           M2MFirmware::UpdateResult == resource) {
            // If it is any of the above resource
            // set the value of the resource.
            if (check_value_range(resource, value)) {

                success = res->set_value(value);
            }
        }
    }
    return success;
}

bool M2MFirmware::set_resource_value(FirmwareResource resource,
                                     const uint8_t *value,
                                     const uint32_t length)
{
    bool success = false;
    M2MResource* res = get_resource(resource);
    if(res) {
        if(M2MFirmware::Package == resource) {
            success = res->set_value(value,length);
        }
    }
    return success;
}

bool M2MFirmware::is_resource_present(FirmwareResource resource) const
{
    bool success = false;
    M2MResource* res = get_resource(resource);
    if(res) {
        success = true;
    }
    return success;
}

const char* M2MFirmware::resource_name(FirmwareResource resource)
{
    const char* res_name = "";
    switch(resource) {
        case Package:
            res_name = FIRMWARE_PACKAGE;
            break;
        case PackageUri:
            res_name = FIRMWARE_PACKAGE_URI;
            break;
        case Update:
            res_name = FIRMWARE_UPDATE;
            break;
        case State:
            res_name = FIRMWARE_STATE;
            break;
        case UpdateSupportedObjects:
            res_name = FIRMWARE_UPDATE_SUPPORTED_OBJECTS;
            break;
        case UpdateResult:
            res_name = FIRMWARE_UPDATE_RESULT;
            break;
        case PackageName:
            res_name = FIRMWARE_PACKAGE_NAME;
            break;
        case PackageVersion:
            res_name = FIRMWARE_PACKAGE_VERSION;
            break;
    }
    return res_name;
}

uint16_t M2MFirmware::per_resource_count(FirmwareResource res) const
{
    uint16_t count = 0;
    if(_firmware_instance) {
        count = _firmware_instance->resource_count(resource_name(res));
    }
    return count;
}

uint16_t M2MFirmware::total_resource_count() const
{
    uint16_t count = 0;
    if(_firmware_instance) {
        count = _firmware_instance->resources().size();
    }
    return count;
}

uint32_t M2MFirmware::resource_value_buffer(FirmwareResource resource,
                               uint8_t *&data) const
{
    uint32_t size = 0;
    M2MResource* res = get_resource(resource);
    if(res) {
        if(M2MFirmware::Package == resource) {
            res->get_value(data,size);
        }
    }
    return size;
}

M2MResource* M2MFirmware::get_resource(FirmwareResource res) const
{
    M2MResource* res_object = NULL;
    if(_firmware_instance) {
        const char* res_name_ptr = "";
        switch(res) {
            case Package:
                res_name_ptr = FIRMWARE_PACKAGE;
                break;
            case PackageUri:
                res_name_ptr = FIRMWARE_PACKAGE_URI;
                break;
            case Update:
                res_name_ptr = FIRMWARE_UPDATE;
                break;
            case State:
                res_name_ptr = FIRMWARE_STATE;
                break;
            case UpdateSupportedObjects:
                res_name_ptr = FIRMWARE_UPDATE_SUPPORTED_OBJECTS;
                break;
            case UpdateResult:
                res_name_ptr = FIRMWARE_UPDATE_RESULT;
                break;
            case PackageName:
                res_name_ptr = FIRMWARE_PACKAGE_NAME;
                break;
            case PackageVersion:
                res_name_ptr = FIRMWARE_PACKAGE_VERSION;
                break;
        }

        res_object = _firmware_instance->resource(res_name_ptr);
    }
    return res_object;
}

bool M2MFirmware::delete_resource(FirmwareResource resource)
{
    bool success = false;
    if(M2MFirmware::UpdateSupportedObjects == resource ||
       M2MFirmware::PackageName == resource            ||
       M2MFirmware::PackageVersion == resource) {
        if(_firmware_instance) {
            success = _firmware_instance->remove_resource(resource_name(resource));
        }
    }
    return success;
}

int64_t M2MFirmware::resource_value_int(FirmwareResource resource) const
{
    int64_t value = -1;
    M2MResource* res = get_resource(resource);
    if(res) {
        if(M2MFirmware::State == resource          ||
           M2MFirmware::UpdateSupportedObjects == resource         ||
           M2MFirmware::UpdateResult == resource) {

            value = res->get_value_int();
        }
    }
    return value;
}

String M2MFirmware::resource_value_string(FirmwareResource resource) const
{
    String value = "";
    M2MResource* res = get_resource(resource);
    if(res) {
        if(M2MFirmware::PackageUri == resource          ||
           M2MFirmware::PackageName == resource           ||
           M2MFirmware::PackageVersion == resource) {

            value = res->get_value_string();
        }
    }
    return value;
}

bool M2MFirmware::check_value_range(FirmwareResource resource, int64_t value) const
{
    bool success = false;
    switch (resource) {
        case UpdateSupportedObjects:
            if(value == 0 || value == 1) {
                success = true;
            }
            break;
        case State:
            if (value >= 0 && value <= 3) {
                success = true;
                M2MResource* updateRes = get_resource(M2MFirmware::Update);
                if (updateRes){
                    if (value == M2MFirmware::Downloaded) {
                        updateRes->set_operation(M2MBase::POST_ALLOWED);
                    }
                    else {
                        updateRes->set_operation(M2MBase::NOT_ALLOWED);
                    }
                }
            }
            break;
        case UpdateResult:
            if (value >= 0 && value <= 7) {
                success = true;
            }
            break;
    default:
        break;
    }
    return success;
}

bool M2MFirmware::set_update_execute_callback(execute_callback callback)
{
    M2MResource* m2mresource;

    m2mresource = get_resource(Update);

    if(m2mresource) {
        m2mresource->set_execute_function(callback);
        return true;
    }

    return false;
}

bool M2MFirmware::set_resource_value_update_callback(FirmwareResource resource,
                                        value_updated_callback callback)
{
    M2MResource* m2mresource;

    m2mresource = get_resource(resource);

    if(m2mresource) {
        m2mresource->set_value_updated_function(callback);
        return true;
    }

    return false;
}

bool M2MFirmware::set_resource_notification_sent_callback(FirmwareResource resource,
                                             notification_sent_callback_2 callback)
{
    M2MResource* m2mresource;

    m2mresource = get_resource(resource);

    if(m2mresource) {
        m2mresource->set_notification_sent_callback(callback);
        return true;
    }

    return false;
}

bool M2MFirmware::set_resource_notification_sent_callback(FirmwareResource resource,
                                             notification_delivery_status_cb callback)
{
    M2MResource* m2mresource;

    m2mresource = get_resource(resource);

    if(m2mresource) {
        m2mresource->set_notification_delivery_status_cb(callback, 0);
        return true;
    }

    return false;
}

#ifndef DISABLE_BLOCK_MESSAGE
bool M2MFirmware::set_package_block_message_callback(incoming_block_message_callback callback)
{
    M2MResource* m2mresource;

    m2mresource = get_resource(Package);

    if(m2mresource) {
        m2mresource->set_incoming_block_message_callback(callback);
        return true;
    }

    return false;
}
#endif // DISABLE_BLOCK_MESSAGE
