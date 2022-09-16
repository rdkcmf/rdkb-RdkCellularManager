/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2022 Sky
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef _CELLULAR_HAL_H_
#define _CELLULAR_HAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <gio/gio.h>
#include <glib-unix.h>

#include <libqmi-glib.h>

/**********************************************************************
                STRUCTURE AND CONSTANT DEFINITIONS
**********************************************************************/

//For now we are supporting QMI library 
#define QMI_SUPPORT

#define RETURN_ERROR        (-1)
#define RETURN_OK           (0)

#ifndef TRUE
#define TRUE                (1)
#endif

#ifndef FALSE
#define FALSE               (0)
#endif

#ifndef BUFLEN_8
#define BUFLEN_8            (8)
#endif

#ifndef BUFLEN_32
#define BUFLEN_32           (32)
#endif

#ifndef BUFLEN_64
#define BUFLEN_64           (64)
#endif

#ifndef BUFLEN_128
#define BUFLEN_128          (128)
#endif

#ifndef BUFLEN_256
#define BUFLEN_256          (256)
#endif

#define CELLULAR_PROFILE_ID_UNKNOWN               (-1)
#define CELLULAR_SLOT_ID_UNKNOWN                  (-1)
#define CELLULAR_PDP_CONTEXT_UNKNOWN              (-1)
#define CELLULAR_PACKET_DATA_INVALID_HANDLE       (0xFFFFFFFF)

/*
*  This struct is for cellular object.
*/

/** Status of the cellular interface */
typedef enum _CellularInterfaceStatus_t {
    IF_UP = 1,
    IF_DOWN,
    IF_UNKNOWN,
    IF_DORMANT,
    IF_NOTPRESENT,
    IF_LOWERLAYERDOWN,
    IF_ERROR
}CellularInterfaceStatus_t;

/** IP Family Preference */
typedef enum _CellularIpFamilyPref_t {
    IP_FAMILY_UNKNOWN = 1,
    IP_FAMILY_IPV4,
    IP_FAMILY_IPV6,
    IP_FAMILY_IPV4_IPV6

}CellularIpFamilyPref_t;

/** IP Family Preference */
typedef enum _CellularPrefAccessTechnology_t {
    PREF_GPRS          = 1,                     //GSM with GPRS
    PREF_EDGE,                                  //GSM with EDGE
    PREF_UMTS,                                  //UMTS
    PREF_UMTSHSPA,                              //3GPP-HSPA
    PREF_CDMA2000OneX,                          //CDMA2000OneX
    PREF_CDMA2000HRPD,                          //CDMA2000HRPD
    PREF_LTE,                                   //LTE
    PREF_NR                                     //5G New Radio

}CellularPrefAccessTechnology_t;

typedef enum _CellularPDPType_t
{ 
    CELLULAR_PDP_TYPE_IPV4         = 0,
    CELLULAR_PDP_TYPE_PPP, 
    CELLULAR_PDP_TYPE_IPV6,
    CELLULAR_PDP_TYPE_IPV4_OR_IPV6

}CellularPDPType_t;

typedef enum _CellularPDPAuthentication_t
{ 
    CELLULAR_PDP_AUTHENTICATION_NONE    = 0,
    CELLULAR_PDP_AUTHENTICATION_PAP , 
    CELLULAR_PDP_AUTHENTICATION_CHAP,

}CellularPDPAuthentication_t;

typedef enum _CellularProfileType_t 
{ 
    CELLULAR_PROFILE_TYPE_3GPP    = 0, 
    CELLULAR_PROFILE_TYPE_3GPP2

} CellularProfileType_t;

typedef enum _CellularPDPNetworkConfig_t
{
   CELLULAR_PDP_NETWORK_CONFIG_NAS = 1,
   CELLULAR_PDP_NETWORK_CONFIG_DHCP
} 
CellularPDPNetworkConfig_t;

typedef enum _CellularModemOperatingConfiguration_t
{ 
    CELLULAR_MODEM_SET_ONLINE     = 1,
    CELLULAR_MODEM_SET_OFFLINE,
    CELLULAR_MODEM_SET_LOW_POWER_MODE,
    CELLULAR_MODEM_SET_RESET,
    CELLULAR_MODEM_SET_FACTORY_RESET

} CellularModemOperatingConfiguration_t;

typedef enum _CellularModemRegisteredServiceType_t
{
   CELLULAR_MODEM_REGISTERED_SERVICE_NONE = 0,
   CELLULAR_MODEM_REGISTERED_SERVICE_PS,
   CELLULAR_MODEM_REGISTERED_SERVICE_CS,
   CELLULAR_MODEM_REGISTERED_SERVICE_CS_PS
} 
CellularModemRegisteredServiceType_t;

typedef enum _CellularRegistrationStatus_t
{
   DEVICE_REGISTERED = 1,
   DEVICE_NOT_REGISTERED,

}CellularRegistrationStatus_t;

/** Interface Input */
typedef  struct                                     
{
    int                             ProfileID;
    CellularProfileType_t           ProfileType;
    int                             PDPContextNumber;
    CellularPDPType_t               PDPType;
    CellularPDPAuthentication_t     PDPAuthentication;
    CellularPDPNetworkConfig_t      PDPNetworkConfig;
    char                            ProfileName[64];
    char                            APN[64];
    char                            Username[256];
    char                            Password[256];
    char                            Proxy[45];
    unsigned int                    ProxyPort;
    unsigned char                   bIsNoRoaming;
    unsigned char                   bIsAPNDisabled;
    unsigned char                   bIsThisDefaultProfile;

} CellularProfileStruct;

typedef  struct                                     
{
    CellularIpFamilyPref_t              enIPFamilyPreference;       //Ipv4 or Ipv6 or dual stack
    CellularProfileStruct               stIfInput;                  //Interface input like APN, UserName, Password etc...
    CellularPrefAccessTechnology_t      enPreferenceTechnology;     //Preference technology like LTE, 3GPP etc...

} CellularContextInitInputStruct;

/** IPv4/Ipv6 IP details */
typedef enum _CellularNetworkIPType_t
{ 
    CELLULAR_NETWORK_IP_FAMILY_UNKNOWN     = 0,
    CELLULAR_NETWORK_IP_FAMILY_IPV4,
    CELLULAR_NETWORK_IP_FAMILY_IPV6,
    CELLULAR_NETWORK_IP_FAMILY_UNSPECIFIED

} CellularNetworkIPType_t;

typedef  struct                                     
{
    char                        WANIFName[16];
    char                        IPAddress[128];
    CellularNetworkIPType_t     IPType;
    char                        SubnetMask[128];
    char                        DefaultGateWay[128];
    char                        DNSServer1[128];
    char                        DNSServer2[128];
    char                        Domains[256];
    unsigned int                MTUSize;

} CellularIPStruct;

typedef  struct                                     
{
    unsigned long               BytesSent;
    unsigned long               BytesReceived;
    unsigned long               PacketsSent;
    unsigned long               PacketsReceived;
    unsigned long               PacketsSentDrop;
    unsigned long               PacketsReceivedDrop;
    unsigned long               UpStreamMaxBitRate;
    unsigned long               DownStreamMaxBitRate;

} CellularPacketStatsStruct;

/* UICC/eUICC */
typedef enum _CellularUICCFormFactor_t
{ 
    CELLULAR_UICC_FORM_FACTOR_1FF     = 0,
    CELLULAR_UICC_FORM_FACTOR_2FF,
    CELLULAR_UICC_FORM_FACTOR_3FF,
    CELLULAR_UICC_FORM_FACTOR_4FF

} CellularUICCFormFactor_t;

typedef enum _CellularUICCStatus_t
{ 
    CELLULAR_UICC_STATUS_VALID     = 0,
    CELLULAR_UICC_STATUS_BLOCKED,
    CELLULAR_UICC_STATUS_ERROR,
    CELLULAR_UICC_STATUS_EMPTY

} CellularUICCStatus_t;

typedef enum _CellularUICCApplication_t
{ 
    CELLULAR_UICC_APPLICATION_USIM     = 0,
    CELLULAR_UICC_APPLICATION_ISIM,
    CELLULAR_UICC_APPLICATION_ESIM

} CellularUICCApplication_t;

typedef  struct                                     
{
    unsigned char               SlotEnable;
    unsigned char               IsCardPresent;
    unsigned char               CardEnable;
    CellularUICCFormFactor_t    FormFactor;
    CellularUICCApplication_t   Application;
    CellularUICCStatus_t        Status;
    char                        MnoName[32];
    char                        iccid[20];
    char                        msisdn[20];

} CellularUICCSlotInfoStruct;

/** NAS */
typedef  struct                                     
{
    int                         RSSI;
    int                         RSRQ;
    int                         RSRP;
    int                         SNR;
    int                         TXPower;

} CellularSignalInfoStruct;

typedef  struct                                     
{
    char                                    plmn_name[32];
    unsigned int                            MCC;
    unsigned int                            MNC;
    CellularRegistrationStatus_t            registration_status;
    CellularModemRegisteredServiceType_t    registered_service;
    unsigned char                           roaming_enabled;
    unsigned int                            area_code;
    unsigned long                           cell_id;

} CellularCurrentPlmnInfoStruct;

typedef  struct                                     
{
    char                                    network_name[32];
    unsigned int                            MCC;
    unsigned int                            MNC;
    unsigned char                           network_allowed_flag;

} CellularNetworkScanResultInfoStruct;

/* Cellular Device Status Events and Callbacks */
typedef enum _CellularDeviceDetectionStatus_t
{
   DEVICE_DETECTED = 1,
   DEVICE_REMOVED,

}CellularDeviceDetectionStatus_t;

typedef enum _CellularDeviceOpenStatus_t
{
   DEVICE_OPEN_STATUS_NOT_READY = 1,
   DEVICE_OPEN_STATUS_INPROGRESS,
   DEVICE_OPEN_STATUS_READY,

}CellularDeviceOpenStatus_t;

typedef enum _CellularDeviceSlotStatus_t
{
   DEVICE_SLOT_STATUS_NOT_READY = 1,
   DEVICE_SLOT_STATUS_SELECTING,
   DEVICE_SLOT_STATUS_READY,

} CellularDeviceSlotStatus_t;

typedef enum _CellularDeviceNASStatus_t
{
   DEVICE_NAS_STATUS_NOT_REGISTERED = 1,
   DEVICE_NAS_STATUS_REGISTERING,
   DEVICE_NAS_STATUS_REGISTERED,

} CellularDeviceNASStatus_t;

typedef enum _CellularDeviceNASRoamingStatus_t
{
   DEVICE_NAS_STATUS_ROAMING_OFF = 1,
   DEVICE_NAS_STATUS_ROAMING_ON,

} CellularDeviceNASRoamingStatus_t;

typedef enum _CellularContextProfileStatus_t
{
   PROFILE_STATUS_INACTIVE = 1,
   PROFILE_STATUS_ACTIVE,

} CellularContextProfileStatus_t;

typedef enum _CellularDeviceProfileSelectionStatus_t
{
   DEVICE_PROFILE_STATUS_NOT_READY = 1,
   DEVICE_PROFILE_STATUS_CONFIGURING,
   DEVICE_PROFILE_STATUS_READY,
   DEVICE_PROFILE_STATUS_DELETED

} CellularDeviceProfileSelectionStatus_t;

typedef enum _CellularDeviceIPReadyStatus_t
{
   DEVICE_NETWORK_IP_NOT_READY = 1,
   DEVICE_NETWORK_IP_READY,

} CellularDeviceIPReadyStatus_t;

typedef enum _CellularNetworkPacketStatus_t
{
   DEVICE_NETWORK_STATUS_DISCONNECTED = 1,
   DEVICE_NETWORK_STATUS_CONNECTED,

} CellularNetworkPacketStatus_t;

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/

/* cellular_hal_IsModemDevicePresent() function */
/**
* @description - Returns Modem Device Available Status
*
* @return The status of the operation
* @retval TRUE if modem device presents
* @retval FALSE if modem device not presents
*
*/
unsigned int
cellular_hal_IsModemDevicePresent
    (
        void
    );

/* cellular_hal_init() function */
/**
* @description - Initialise the Cellular HAL
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int
cellular_hal_init
    (
        CellularContextInitInputStruct *pstCtxInputStruct
    );

/* cellular_device_open_status_api_callback function */
/**
* @description - This callback sends to upper layer when after successfully open cellular device context
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
typedef int (*cellular_device_open_status_api_callback)( char *device_name, char *wan_ifname, CellularDeviceOpenStatus_t device_open_status );

/* cellular_device_removed_status_api_callback function */
/**
* @description - This callback sends to upper layer when after successfully removed modem from device
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
typedef int (*cellular_device_removed_status_api_callback)( char *device_name, CellularDeviceDetectionStatus_t device_detection_status );

typedef  struct                                     
{
    cellular_device_open_status_api_callback        device_open_status_cb;
    cellular_device_removed_status_api_callback     device_remove_status_cb;

} CellularDeviceContextCBStruct;

/* cellular_hal_open_device function */
/**
* @description - This API inform lower layer to create/open device. 
*
* @param[in] pstDeviceCtxCB - The strcture receives function pointers for device open/remove status response from driver.
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_open_device(CellularDeviceContextCBStruct *pstDeviceCtxCB);

/* cellular_hal_IsModemControlInterfaceOpened() function */
/**
* @description - Returns Modem Control Interface Opened or Not
*
* @return The status of the operation
* @retval TRUE if modem device opened
* @retval FALSE if modem device not opened
*
*/
unsigned char cellular_hal_IsModemControlInterfaceOpened( void );

/* cellular_device_slot_status_api_callback function */
/**
* @description - This callback sends to upper layer when after successfully select cellular device slot
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
typedef int (*cellular_device_slot_status_api_callback)( char *slot_name, char *slot_type, int slot_num, CellularDeviceSlotStatus_t device_slot_status );

/* cellular_hal_select_device_slot function */
/**
* @description - This API inform lower layer to select slot for opened device. 
*
* @param[in] cellular_device_slot_status_api_callback - The function pointer which receives device slot status response from driver.
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_select_device_slot(cellular_device_slot_status_api_callback device_slot_status_cb);

/* cellular_hal_sim_power_enable function */
/**
* @description - This API perform to enable/disable SIM power from particular slot
*                    
* @param[in] slot_id - This param will intimate to lower layer to slot id to be enable/disable
* @param[in] enable - This param will intimate to lower layer to enable/disable UICC power
*                    
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_sim_power_enable(unsigned int slot_id, unsigned char enable);

/* cellular_hal_get_total_uicc_slots function */
/**
* @description - This API get UICC total slots count from modem 
*
* @param[out] total_count - This variable receives the total count of UICC slot.
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_total_no_of_uicc_slots(unsigned int *total_count);

/* cellular_hal_get_uicc_slot_info function */
/**
* @description - This API get UICC slot information from modem 
*
* @param[in] slot_index   - Index of UICC slot.
* @param[out] pstSlotInfo - The strcture filled UICC slot information by lower layer.
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_uicc_slot_info(unsigned int slot_index, CellularUICCSlotInfoStruct *pstSlotInfo);

/* cellular_hal_get_active_card_status function */
/**
* @description - This API get current active card status information from modem 
*
* @param[out] CellularUICCStatus_t - The enum filled with current SIM status by lower layer.
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_active_card_status(CellularUICCStatus_t *card_status);

/* cellular_device_registration_status_callback function */
/**
* @description - This callback sends to upper layer when after successfully registered modem with network
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
typedef int (*cellular_device_registration_status_callback)( CellularDeviceNASStatus_t device_registration_status ,
                                                             CellularDeviceNASRoamingStatus_t roaming_status,
                                                             CellularModemRegisteredServiceType_t registered_service );

/* cellular_hal_monitor_device_registration function */
/**
* @description - This API inform lower layer to monitor device registration 
*
* @param[in] cellular_device_registration_status_callback - The function pointer which receives device registration status response from lower layer.
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_monitor_device_registration(cellular_device_registration_status_callback device_registration_status_cb);

/* cellular_device_profile_status_api_callback function */
/**
* @description - This callback sends to upper layer when after successfully create/modify/select
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
typedef int (*cellular_device_profile_status_api_callback)( char *profile_id, CellularPDPType_t  PDPType, CellularDeviceProfileSelectionStatus_t device_profile_status );

/* cellular_hal_profile_create function */
/**
* @description - This API inform lower layer to create profile based on valid pstProfileInput. If NULL then select default profile. 
*
* @param[in] pstProfileInput - Profile structure needs to pass when creating a profile
* @param[in] device_profile_status_cb - The function pointer which receives device profile create status response from driver.
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_profile_create(CellularProfileStruct *pstProfileInput, cellular_device_profile_status_api_callback device_profile_status_cb);

/* cellular_hal_profile_delete function */
/**
* @description - This API inform lower layer to delete profile based on valid pstProfileInput.
*
* @param[in] pstProfileInput - Profile structure needs to pass when deleting a profile
* @param[in] device_profile_status_cb - The function pointer which receives device profile delete status response from driver.
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_profile_delete(CellularProfileStruct *pstProfileInput, cellular_device_profile_status_api_callback device_profile_status_cb);

/* cellular_hal_profile_modify function */
/**
* @description - This API inform lower layer to modify profile based on valid pstProfileInput. If NULL then return error. 
*
* @param[in] pstProfileInput - Profile structure needs to pass when creating a profile
* @param[in] device_profile_status_cb - The function pointer which receives device profile modify status response from driver.
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_profile_modify(CellularProfileStruct *pstProfileInput, cellular_device_profile_status_api_callback device_profile_status_cb);

/* cellular_hal_get_profile_list function */
/**
* @description - This API get list of profiles from Modem
*
* @param[out] ppstProfileOutput - List of profiles needs to be return
* @param[out] profile_count - Total profile count needs to be return
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_profile_list(CellularProfileStruct **ppstProfileOutput, int *profile_count);

/* cellular_network_packet_service_status_api_callback function */
/**
* @description - This callback sends to upper layer when after getting packet service status after start network
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
typedef int (*cellular_network_packet_service_status_api_callback)( char *device_name, CellularNetworkIPType_t ip_type, CellularNetworkPacketStatus_t packet_service_status );

/* cellular_device_network_ip_ready_api_callback function */
/**
* @description - This callback sends IP information to upper layer when after successfully getting ip configuration from driver
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
typedef int (*cellular_device_network_ip_ready_api_callback)( CellularIPStruct *pstIPStruct, CellularDeviceIPReadyStatus_t ip_ready_status );

typedef  struct                                     
{
    cellular_device_network_ip_ready_api_callback           device_network_ip_ready_cb;
    cellular_network_packet_service_status_api_callback     packet_service_status_cb;

} CellularNetworkCBStruct;

/* cellular_hal_start_network function */
/**
* @description - This API inform lower layer to start network based on IP Type and Passed profile input. If NULL then start based on default profile. 
*
* @param[in] ip_request_type - The function pointer which receives IP configuration for started network from driver.
* @param[in] pstProfileInput - Here needs to pass profile to start network. If NULL then it should take it default profile otherwise start based on input
* @param[in] pstCBStruct - Here needs to fill CB function pointer for packet and ip status
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_start_network( CellularNetworkIPType_t ip_request_type, CellularProfileStruct *pstProfileInput, CellularNetworkCBStruct *pstCBStruct );

/* cellular_hal_stop_network function */
/**
* @description - This API inform lower layer to stop network based on valid ip request type.
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_stop_network(CellularNetworkIPType_t ip_request_type);

/* cellular_hal_get_signal_info function */
/**
* @description - This API get current signal information from Modem
*            
* @param[in] signal_info - Needs to parse CellularSignalInfoStruct structure to get signal information.
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_signal_info(CellularSignalInfoStruct *signal_info);

/* cellular_hal_set_modem_operating_configuration function */
/**
* @description - This API inform lower layer to configure modem operating mode.
*              
* @param[in] modem_operating_config - Needs to pass CellularModemOperatingConfiguration_t to configure modem state.
*                               
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_set_modem_operating_configuration(CellularModemOperatingConfiguration_t modem_operating_config);

/* cellular_hal_get_device_imei() function */
/**
* @description - Returns Modem Device IMEI information
*
* @param[out] imei - Needs to return Modem IMEI value on this input.
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_device_imei ( char *imei );

/* cellular_hal_get_device_imei_sv() function */
/**
* @description - Returns Modem Device IMEI Software Version
*
* @param[out] imei_sv - Needs to return Modem IMEI Software Version value on this input.
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_device_imei_sv ( char *imei_sv );

/* cellular_hal_get_modem_current_iccid() function */
/**
* @description - Returns Modem Device Current ICCID Information
*
* @param[out] iccid - Needs to return currently choosed ICCID value on this input.
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_modem_current_iccid ( char *iccid );

/* cellular_hal_get_modem_current_msisdn() function */
/**
* @description - Returns Modem Device Current MSISDN Information
*
* @param[out] msisdn - Needs to return currently choosed MSISDN value on this input.
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_modem_current_msisdn ( char *msisdn );

/* cellular_hal_get_packet_statistics function */
/**
* @description - This API get current network packet statistics from modem
*            
* @param[in] network_packet_stats - Needs to parse CellularPacketStatsStruct structure to get packet statistics information.
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_packet_statistics( CellularPacketStatsStruct *network_packet_stats );

/* cellular_hal_get_current_modem_interface_status function */
/**
* @description - This API get current modem registration status
*            
* @param[in] status - Needs to assign modem current registration status.
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_current_modem_interface_status( CellularInterfaceStatus_t *status );

/* cellular_hal_set_modem_network_attach function */
/**
* @description - This API to attach modem with network registration
*           
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_set_modem_network_attach( void );

/* cellular_hal_set_modem_network_detach function */
/**
* @description - This API to detach modem with network registration
*           
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_set_modem_network_detach( void );

/* cellular_hal_get_modem_firmware_version function */
/**
* @description - This API get current firmware version of modem
*
* @param[out] firmware_version - This string contains firmware version of modem
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_modem_firmware_version(char *firmware_version);

/* cellular_hal_get_current_plmn_information function */
/**
* @description - This API get current plmn information from modem 
*
* @param[in] plmn_info - The strcture receives function pointers for current plmn network information response from modem.
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_current_plmn_information(CellularCurrentPlmnInfoStruct *plmn_info);

/* cellular_hal_get_available_networks_information function */
/**
* @description - This API get current active card status information from modem 
*
* @param[out] CellularNetworkScanResultInfoStruct - The structure filled with available networks information from Modem.
* @param[out] total_network_count - This variable filled with total no of available networks
*                                              
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_available_networks_information(CellularNetworkScanResultInfoStruct **network_info, unsigned int *total_network_count);

/* cellular_hal_get_modem_preferred_radio_technology() function */
/**
* @description - Returns Modem preferred Radio Technologies
*
* @param[out] param_rat - Contains preferred technology.
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_modem_preferred_radio_technology( char *preferred_rat );

/* cellular_hal_set_modem_preferred_radio_technology() function */
/**
* @description - sets Modem preferred Radio Technologies
*
* @param[in] param_rat - Contains preferred technology.Should be part of  supported RAT otherwise AUTO will be set
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_set_modem_preferred_radio_technology( char *preferred_rat );


/* cellular_hal_get_modem_current_radio_technology() function */
/**
* @description - Returns Modem current Radio Technologies
*
* @param[out] param_rat - Contains current technology used for data.
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/
int cellular_hal_get_modem_current_radio_technology( char *current_rat );

/* cellular_hal_get_modem_supported_radio_technology() function */
/**
* @description - Returns Modem supported Radio access Technologies 
*
* @param[out] param_value - Contains information about supported RAT. 
*
* @return The status of the operation
* @retval RETURN_OK if successful
* @retval RETURN_ERR if any error is detected
*
*/

int cellular_hal_get_modem_supported_radio_technology ( char *supported_rat );

#endif //_CELLULAR_HAL_H_
