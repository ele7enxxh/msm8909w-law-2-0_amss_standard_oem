/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPMSERVER_H
#define RPMSERVER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "rpm.h"
#include "kvp.h"
#include "comdef.h"

struct ResourceData;

/* 
 * RPM Resource API
 * 
 * This interface is used to register for control of a resource.  Its goal is
 * to abstract away any functionality that is common across all resources.  As
 * such, it implements the following functionality on behalf of the driver:
 * 
 *      - it provides storage for request data in an efficient manner, so that
 *        the pointer math for buffer lookup is not duplicated
 * 
 *      - it provides a separate translation callback, to allow drivers to
 *        decode incoming requests and compute new request states in whatever
 *        internal representation they see fit (rather than waiting until
 *        application time)
 * 
 *      - it provides an application callback, which will automatically be
 *        invoked to advise the driver of a new client request (hiding the
 *        somewhat complex details about when such applications need to occur
 *        from the driver itself)
 */

/* The following structure comprises the information that a driver may need to
 * decode an incoming request.
 *
 * WARNING: do not rely on the offset of any data member or the overall size of
 * this structure.  Future revisisons may add / reorder fields which should not
 * affect drivers written with this compatibility warning in mind.
 */
typedef struct
{
    void    *cb_data;
    unsigned id;
    unsigned client;
    unsigned client_type;
    void    *dest_buffer;
    kvp_t   *new_kvps;
} rpm_translation_info;

/* 
 * This type defines the format the the RPM request service expects of a
 * translation function.  The purpose of the translation function is to apply
 * the requested updates delivered from another client (new_kvps) on top of the
 * request being modified (dest_buffer).
 * 
 * For example, when a new "immediate" resource request is received, the
 * request service takes the following actions:
 *   1) Determine which resource handler should receive the message.
 *   2) Look up the current request for that (resource, id, client) and copy it
 *      into a temporary buffer (dest_buffer).
 *   3) Call the driver's translation function with the dest_buffer and the
 *      incoming key-value pairs of the new request.
 *   4) Call the driver's apply function with the dest_buffer as the
 *      new_request, and the pre-modification buffer as the old_request.
 * 
 * It is important to note that the format of non-KVP buffers (i.e.,
 * dest_buffer, new_request, old_request) is owned by the resource's driver.
 * 
 * Three requirements are placed upon internal formats:
 *   1) The size of the internal format is constant (known at init time).
 *   2) The size does not vary between resources (i.e., if clock 0 and clock 15
 *      have differently sized internal representations, the maximum size will
 *      be used for all clocks).
 *   3) The driver must advise the RPM resource service of the size of the
 *      driver's internal representation as part of the registration of that
 *      driver.
 */
typedef void (*rpm_translate_cb)(rpm_translation_info *info);

/* The following structure comprises the information that a driver may need to
 * respond to a change in a client's request.
 *
 * WARNING: do not rely on the offset of any data member or the overall size of
 * this structure.  Future revisisons may add / reorder fields which should not
 * affect drivers written with this compatibility warning in mind.
 */
typedef struct
{
    void    *cb_data;
    unsigned id;
    unsigned client;
    unsigned client_type;
    void    *old_state;
    void    *new_state;
    void    *current_aggregation;
    uint64_t settling_time;
    const struct ResourceData* resource_handle;
} rpm_application_info;

/*
 * This type defines the format the the RPM request service expects of an
 * application function.  The purpose of the application function is to update
 * the system state to compensate for the changing requirements of its client.
 * The client's previous request is passed along with its new request; if the
 * requests of other clients are required in order to properly re-aggregate
 * the system state, rpm_get_current_request() can be called.
 *
 * An important item of note is that the identity of the reqesting entity is
 * revealed to the driver only through the opaque enumeration "client."  Only
 * *shared* resources exist on the RPM, and therefore the RPM gives equal
 * opportunity to any processor in the system to make requests against a
 * resource.  However, it is guaranteed that 'client' will vary strictly
 * from 0 to num_clients-1.  num_clients can be retrieved via the
 * rpm_get_num_clients API call.
 */
typedef void (*rpm_apply_cb)(rpm_application_info *info);

/** Register a resource driver with the RPM request service.
 *
 * This API allows a driver to register for a class of resource.
 *
 * The specific number of resources that is supported for this platform is
 * passed as the second argument.  For example, if a driver is responsible for
 * bus slaves and there are 15 bus slaves present on this platform, the driver
 * should call rpm_register_resource(RPM_BUS_SLAVE_REQ, 15, ...).
 *
 * Using this API, the resources will automatically be created as having ID
 * values 0..n-1, where n is the num_supported number that is passed.  If you
 * have a more sparse / particular ID representation scheme that is required,
 * use rpm_register_named_resource which accepts a list of resource ID's.
 *
 * inrep_size is the size of the drivers internal representation for this
 * resource.  The internal representation is never directly handled by the RPM
 * request service, but the size does need to be known in order to efficiently
 * allocate memory (request storage, temporary buffers, etc.) on behalf of
 * drivers.
 *
 * xlate_cb and apply_cb are the translation and application functions for this
 * class of resource, as described above.
 */
void rpm_register_resource
(
    rpm_resource_type resource,
    unsigned          num_supported,
    unsigned          inrep_size,
    rpm_translate_cb  xlate_cb,
    rpm_apply_cb      apply_cb,
    void             *cb_data
);

/** Register a resource driver with particular resource ID's.
 *
 * This API is the exact same as rpm_register_resource, except it allows the
 * direct specification of a list of resource ID's rather than automatically
 * inferring that resources will be enumerated 0..n-1.
 */
void rpm_register_named_resource
(
    rpm_resource_type resource,
    unsigned          num_supported,
    unsigned const   *ids,
    unsigned          inrep_size,
    rpm_translate_cb  xlate_cb,
    rpm_apply_cb      apply_cb,
    void             *cb_data
);

/** Returns the number of "external" clients (other EE's) in the system. */
unsigned rpm_get_num_ees(void);

/** Returns the total number of clients voting upon the named resource. */
unsigned rpm_get_num_clients(const struct ResourceData* resource_handle);

/** Retrive the buffer representing the current request of a client.
 * 
 * This API allows a driver to inquire as to the current setting of a client,
 * which can be useful when aggregating a new state of a resource.  For
 * example, if your driver has 3 clients making request {client_0 : 500,
 * client_1 : 250, client_2 : 127} in a 'maximum' aggregation scheme and the
 * application function is called specifying that client_0 has changed his
 * request to 400, it is necessary to re-examine the client_1 and client_2
 * requests to ensure that they are not > 400.
 * 
 * If you call this API from inside your apply function, the request
 * information new_state for the client being updated will match that returned
 * by this function.
 *
 */
void rpm_get_current_request(const struct ResourceData* resource_handle, unsigned client, unsigned *client_type, const void **request);

/** Retrieve the buffer where the current aggregated state of a resource is stored. */
void rpm_get_aggregated_request_buffer(rpm_resource_type resource, unsigned id, const void **request);

/** Create an "internal" RPM client.
 *
 * This function is used to create a client of an RPM-served resource that can
 * be manipulated by internal entities.  For example, if one of your resources
 * has a dependency upon another resource in the system, it can use this
 * interface to get a "client" of the other resource and thereby issue
 * requests.
 *
 * Internal clients are not translated through the KVP system / translate
 * callback.  Instead, they're expected to issue their requests in the
 * appropriate internal representation for that resource.
 *
 * client_type identifies the type of client being created.  RPM
 * framwork-created clients always have a client_type value of 0 and an
 * inrep_size equal to that passed in rpm_register_resource; some resources may
 * support other kinds of clients.
 *
 * This function returns a valid client id if it was successful, or 0xFFFFFFFF
 * if it failed (either because the resource does not exist or rejected the
 * custom client type).
 */
unsigned rpm_create_client(rpm_resource_type resource, unsigned id, unsigned client_type);

/** Issue a request on an RPM client.
 *
 * This function allows you to issue a request on a client.  It is typically
 * used with an internal client, to express a dependency upon another RPM
 * resource.
 */
void rpm_issue_request(rpm_resource_type resource, unsigned id, unsigned client, unsigned size, void *data);

/** Allow a new type of internal client to be registered.
 *
 * Calling this function allows the driver for a particular resource to
 * register handlers for the creation of new internal (nonstandard) client
 * types.  If all internal requests have the same format as external requests,
 * this function is not required.  However, for "special" client types, the RPM
 * framework needs to be told how large the inrep is for this representation.
 * 
 * An opportunity is provided to receive special creation / destruction
 * callbacks; they are not mandatory and will not be called if they are NULL.
 */
void rpm_register_internal_client_type
(
    rpm_resource_type      resource,
    unsigned               id,
    unsigned               client_type,
    unsigned               inrep_size
);


/*
 * RPM Resource <--> NPA Adapter API
 * 
 * This interface strives to make registering an NPA resource with the RPM
 * resource API a more straightforward process.  NPA already abstracts
 * substantial amounts of work from the driver interface, so it is beneficial
 * if the RPM resource API can be bypassed in favor of the (more broadly used)
 * NPA abstractions.
 *
 * By creating an RPM/NPA adapter for your resource class you relieve yourself
 * from the need to write your own translate and application functions.
 * Subsequently you describe to the adapter which resource id's relate to which
 * resources, which allows the adapter to make requests to your resources when
 * requests are received from clients.
 */

/** Defines a handle to an instance of an adapter provided to a driver */
typedef struct rpm_npa_adapter_s *rpm_npa_adapter;

/** Create an RPM <--> NPA adapter.
 *
 * This returns the handle to an RPM <--> NPA adapter for your driver type.
 * Initially the adapter does not know about any of your NPA resources, so
 * ensure that you link resource ID's to actual resource names using
 * rpm_associate_npa_resource.
 */
rpm_npa_adapter rpm_create_npa_adapter(rpm_resource_type resource, unsigned num_npa_resources);
rpm_npa_adapter rpm_create_npa_settling_adapter(rpm_resource_type resource, unsigned num_npa_resources);

/** Instruct the adapter that a resource ID corresponds to a particular NPA resource */
void rpm_associate_npa_resource(rpm_npa_adapter adapter, unsigned resource_id, const char *resource_name);

typedef enum
{
  SPM_AWAKE = 0,
  SPM_GOING_TO_SLEEP,
  SPM_SLEEPING,
  SPM_WAKING_UP
} rpm_spm_status_type;

typedef void (*spm_cb_fcn)(unsigned ee, rpm_spm_status_type status);

/** Register for notification of a processor's entry or exit from sleep. */
void rpm_register_spm_callback(unsigned ee, spm_cb_fcn cb);

void rpm_shadow_resource
(
    rpm_resource_type resource,
    unsigned          id,
    unsigned          inrep_size,
    rpm_translate_cb  xlate_cb,
    rpm_apply_cb      apply_cb,
    void             *cb_data
);

void rpmserver_init(void);

void rpm_set_changer_common_init(void);

void rpm_send_init_proxy_vote(rpm_resource_type resource, unsigned id, unsigned ee, kvp_t *data);

/*
 * Get a handle to a particular resource.
 * This handle can be used for the rpm_get_current_request or rpm_get_num_clients APIs
 * if a handle isn't already available.
 * The resource must exist or this API will throw an abort.
 * This is a relatively slow function, so don't use if you have other access to the handle.
 */
const struct ResourceData* rpm_get_resource_handle(rpm_resource_type resource, unsigned id);

void rpm_send_init_proxy_vote_sleep(rpm_resource_type resource, unsigned id, unsigned ee, kvp_t *data);

/*
 * Set which resource the RPM should transition last. Will typically be used for Mx such that
 * if we are going to vdd-min we can avoid transitioning Mx as part of the set transition.
 *
 * This API can only be called once, and after this no further resources can be added to the RPM.
 * The resource passed in to the function must already exist.
 * Violating any of these constraints will cause the RPM to throw an abort.
 *
 * It's possible that in the future we might want to extend this API to allow multiple resources to
 * be transitioned last.
 */
void rpm_set_last_transitioning_resource(rpm_resource_type resource, unsigned id);


#ifdef __cplusplus
};
#endif // __cplusplus

#endif // RPMSERVER_H

