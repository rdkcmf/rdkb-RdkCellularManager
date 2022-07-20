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

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/

/**************************************************************************

    module: cellularmgr_cellular_internal.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

        *  CellularMgr_CellularCreate
        *  CellularMgr_CellularInitialize
        *  CellularMgr_CellularRemove

**************************************************************************/
#include "cellularmgr_cellular_apis.h"
#include "cellularmgr_cellular_internal.h"
#include "cellularmgr_plugin_main_apis.h"
#include "poam_irepfo_interface.h"
#include "sys_definitions.h"
#include "cellularmgr_sm.h"
#include "dmsb_tr181_psm_definitions.h"

/**********************************************************************
                STRUCTURE AND CONSTANT DEFINITIONS
**********************************************************************/

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/

/**********************************************************************
                FUNCTION DEFINITIION
**********************************************************************/

static int CellularMgr_InitializeContextDefaultProfile( CellularContextInitInputStruct *pstCtxInputStruct )
{
    int retPsmGet = CCSP_SUCCESS;
    char param_value[256];
    char param_name[512];

    if( NULL == pstCtxInputStruct )
    {
        return CCSP_FAILURE;
    }

    _ansc_memset(param_name, 0, sizeof(param_name));
    _ansc_memset(param_value, 0, sizeof(param_value));
    _ansc_sprintf(param_name, PSM_CELLULARMANAGER_DEFAULT_PROFILE_ID);
    retPsmGet = CellularMgr_RdkBus_GetParamValuesFromDB(param_name,param_value,sizeof(param_value));
    if (retPsmGet == CCSP_SUCCESS)
    {
        unsigned int ProfileID = 0;

        _ansc_sscanf(param_value, "%d", &ProfileID);

        if( 0 < ProfileID )
        {
            pstCtxInputStruct->stIfInput.ProfileID = ProfileID;
        }
        else
        {
            pstCtxInputStruct->stIfInput.ProfileID = CELLULAR_PROFILE_ID_UNKNOWN;
        }
    }
    else
    {
        pstCtxInputStruct->stIfInput.ProfileID = CELLULAR_PROFILE_ID_UNKNOWN;
    }

    _ansc_memset(param_name, 0, sizeof(param_name));
    _ansc_memset(param_value, 0, sizeof(param_value));
    _ansc_sprintf(param_name, PSM_CELLULARMANAGER_DEFAULT_PROFILE_TYPE);
    retPsmGet = CellularMgr_RdkBus_GetParamValuesFromDB(param_name,param_value,sizeof(param_value));
    if (retPsmGet == CCSP_SUCCESS)
    {
        if(strcmp(param_value, "3gpp") == 0)
        {
            pstCtxInputStruct->stIfInput.ProfileType = CELLULAR_PROFILE_TYPE_3GPP;
        }
        else if(strcmp(param_value, "3gpp2") == 0)
        {
            pstCtxInputStruct->stIfInput.ProfileType = CELLULAR_PROFILE_TYPE_3GPP2;
        }
        else
        {
            pstCtxInputStruct->stIfInput.ProfileType = CELLULAR_PROFILE_TYPE_3GPP;
        }
    }
    else
    {
        pstCtxInputStruct->stIfInput.ProfileType = CELLULAR_PROFILE_TYPE_3GPP;
    }

    _ansc_memset(param_name, 0, sizeof(param_name));
    _ansc_memset(param_value, 0, sizeof(param_value));
    _ansc_sprintf(param_name, PSM_CELLULARMANAGER_DEFAULT_PROFILE_PDPTYPE);
    retPsmGet = CellularMgr_RdkBus_GetParamValuesFromDB(param_name,param_value,sizeof(param_value));
    if (retPsmGet == CCSP_SUCCESS)
    {
        if(strcmp(param_value, "IPv4") == 0)
        {
            pstCtxInputStruct->stIfInput.PDPType = CELLULAR_PDP_TYPE_IPV4;
        }
        else if(strcmp(param_value, "IPv6") == 0)
        {
            pstCtxInputStruct->stIfInput.PDPType = CELLULAR_PDP_TYPE_IPV6;
        }
        else if(strcmp(param_value, "IPv4IPv6") == 0)
        {
            pstCtxInputStruct->stIfInput.PDPType = CELLULAR_PDP_TYPE_IPV4_OR_IPV6;
        }
        else
        {
            pstCtxInputStruct->stIfInput.PDPType = CELLULAR_PDP_TYPE_IPV4;
        }
    }
    else
    {
        pstCtxInputStruct->stIfInput.PDPType = CELLULAR_PDP_TYPE_IPV4;
    }

    _ansc_memset(param_name, 0, sizeof(param_name));
    _ansc_memset(param_value, 0, sizeof(param_value));
    _ansc_sprintf(param_name, PSM_CELLULARMANAGER_DEFAULT_PROFILE_AUTHENTICATION);
    retPsmGet = CellularMgr_RdkBus_GetParamValuesFromDB(param_name,param_value,sizeof(param_value));
    if (retPsmGet == CCSP_SUCCESS)
    {
        if(strcmp(param_value, "PAP") == 0)
        {
            pstCtxInputStruct->stIfInput.PDPAuthentication = CELLULAR_PDP_AUTHENTICATION_PAP;
        }
        else if(strcmp(param_value, "CHAP") == 0)
        {
            pstCtxInputStruct->stIfInput.PDPAuthentication = CELLULAR_PDP_AUTHENTICATION_PAP;
        }
        else
        {
            pstCtxInputStruct->stIfInput.PDPAuthentication = CELLULAR_PDP_AUTHENTICATION_NONE;
        }
    }
    else
    {
        pstCtxInputStruct->stIfInput.PDPAuthentication = CELLULAR_PDP_AUTHENTICATION_NONE;
    }

    _ansc_memset(param_name, 0, sizeof(param_name));
    _ansc_memset(param_value, 0, sizeof(param_value));
    _ansc_sprintf(param_name, PSM_CELLULARMANAGER_DEFAULT_PROFILE_NAME);
    retPsmGet = CellularMgr_RdkBus_GetParamValuesFromDB(param_name,param_value,sizeof(param_value));
    if (retPsmGet == CCSP_SUCCESS)
    {
        AnscCopyString(pstCtxInputStruct->stIfInput.ProfileName, param_value);
    }
    else
    {
        AnscCopyString(pstCtxInputStruct->stIfInput.ProfileName, "");
    }

    _ansc_memset(param_name, 0, sizeof(param_name));
    _ansc_memset(param_value, 0, sizeof(param_value));
    _ansc_sprintf(param_name, PSM_CELLULARMANAGER_DEFAULT_PROFILE_APN);
    retPsmGet = CellularMgr_RdkBus_GetParamValuesFromDB(param_name,param_value,sizeof(param_value));
    if (retPsmGet == CCSP_SUCCESS)
    {
        AnscCopyString(pstCtxInputStruct->stIfInput.APN, param_value);
    }
    else
    {
        AnscCopyString(pstCtxInputStruct->stIfInput.APN, "");
    }

    _ansc_memset(param_name, 0, sizeof(param_name));
    _ansc_memset(param_value, 0, sizeof(param_value));
    _ansc_sprintf(param_name, PSM_CELLULARMANAGER_DEFAULT_PROFILE_USERNAME);
    retPsmGet = CellularMgr_RdkBus_GetParamValuesFromDB(param_name,param_value,sizeof(param_value));
    if (retPsmGet == CCSP_SUCCESS)
    {
        AnscCopyString(pstCtxInputStruct->stIfInput.Username, param_value);
    }
    else
    {
        AnscCopyString(pstCtxInputStruct->stIfInput.Username, "");
    }

    _ansc_memset(param_name, 0, sizeof(param_name));
    _ansc_memset(param_value, 0, sizeof(param_value));
    _ansc_sprintf(param_name, PSM_CELLULARMANAGER_DEFAULT_PROFILE_PASSWORD);
    retPsmGet = CellularMgr_RdkBus_GetParamValuesFromDB(param_name,param_value,sizeof(param_value));
    if (retPsmGet == CCSP_SUCCESS)
    {
        AnscCopyString(pstCtxInputStruct->stIfInput.Password, param_value);
    }
    else
    {
        AnscCopyString(pstCtxInputStruct->stIfInput.Password, "");
    }

    _ansc_memset(param_name, 0, sizeof(param_name));
    _ansc_memset(param_value, 0, sizeof(param_value));
    _ansc_sprintf(param_name, PSM_CELLULARMANAGER_DEFAULT_PROFILE_IS_NOROAMING);
    retPsmGet = CellularMgr_RdkBus_GetParamValuesFromDB(param_name,param_value,sizeof(param_value));
    if (retPsmGet == CCSP_SUCCESS)
    {
        if(strcmp(param_value, "TRUE") == 0)
        {
            pstCtxInputStruct->stIfInput.bIsNoRoaming = TRUE;
        }
        else 
        {
            pstCtxInputStruct->stIfInput.bIsNoRoaming = FALSE;
        }
    }
    else
    {
        pstCtxInputStruct->stIfInput.bIsNoRoaming = TRUE;
    }

    _ansc_memset(param_name, 0, sizeof(param_name));
    _ansc_memset(param_value, 0, sizeof(param_value));
    _ansc_sprintf(param_name, PSM_CELLULARMANAGER_DEFAULT_PROFILE_IS_APNDISABLED);
    retPsmGet = CellularMgr_RdkBus_GetParamValuesFromDB(param_name,param_value,sizeof(param_value));
    if (retPsmGet == CCSP_SUCCESS)
    {
        if(strcmp(param_value, "TRUE") == 0)
        {
            pstCtxInputStruct->stIfInput.bIsAPNDisabled = TRUE;
        }
        else 
        {
            pstCtxInputStruct->stIfInput.bIsAPNDisabled = FALSE;
        }
    }
    else
    {
        pstCtxInputStruct->stIfInput.bIsAPNDisabled = FALSE;
    }

    pstCtxInputStruct->stIfInput.bIsThisDefaultProfile = TRUE;

    CcspTraceInfo(("************** Default Profile Information *************\n"));
    CcspTraceInfo(("\t ProfileID         : %d\n",pstCtxInputStruct->stIfInput.ProfileID));
    CcspTraceInfo(("\t ProfileType       : %d\n",pstCtxInputStruct->stIfInput.ProfileType));
    CcspTraceInfo(("\t PDPType           : %d\n",pstCtxInputStruct->stIfInput.PDPType));
    CcspTraceInfo(("\t Authentication    : %d\n",pstCtxInputStruct->stIfInput.PDPAuthentication));
    CcspTraceInfo(("\t ProfileName       : %s\n",pstCtxInputStruct->stIfInput.ProfileName));
    CcspTraceInfo(("\t APN               : %s\n",pstCtxInputStruct->stIfInput.APN));
    CcspTraceInfo(("\t Username          : %s\n",pstCtxInputStruct->stIfInput.Username));
    CcspTraceInfo(("\t Password          : %s\n",pstCtxInputStruct->stIfInput.Password));
    CcspTraceInfo(("\t IsNoRoaming       : %d\n",pstCtxInputStruct->stIfInput.bIsNoRoaming));
    CcspTraceInfo(("\t IsAPNDisabled     : %d\n",pstCtxInputStruct->stIfInput.bIsAPNDisabled));
    CcspTraceInfo(("\t IsthisDefault     : %d\n",pstCtxInputStruct->stIfInput.bIsThisDefaultProfile));

    return retPsmGet;
}

ANSC_STATUS DmlCellularInitialize ( ANSC_HANDLE  hDml )
{
    ANSC_STATUS                    returnStatus   =  ANSC_STATUS_SUCCESS;
	PCELLULARMGR_CELLULAR_DATA     pMyObject      =  (PCELLULARMGR_CELLULAR_DATA) hDml;
    PCELLULAR_DML_INFO             pstDmlCellular =  NULL;
    CellularContextInitInputStruct stCtxInputStruct = { 0 };
    CellularMgrSMInputStruct       stStateMachineInput = { 0 };
    INT                            iLoopCount;

    if ( pMyObject == NULL )
    {
        return  ANSC_STATUS_FAILURE;
    }

    /*
     * Initialize Cellular DML
    */
    pstDmlCellular = (PCELLULAR_DML_INFO)AnscAllocateMemory(sizeof(CELLULAR_DML_INFO));
    if ( pstDmlCellular == NULL )
    {
        return  ANSC_STATUS_FAILURE;
    }

    AnscZeroMemory(pstDmlCellular, sizeof(CELLULAR_DML_INFO));

    /*
     * Initialize Interface DML
    */
    pstDmlCellular->X_RDK_Enable                 = TRUE;
    pstDmlCellular->X_RDK_Status                 = RDK_STATUS_DOWN;
    pstDmlCellular->X_RDK_ControlInterfaceStatus = CONTROL_STATUS_CLOSED;
    pstDmlCellular->X_RDK_RadioEnvConditions     = RADIO_ENV_CONDITION_UNAVAILABLE;
    pstDmlCellular->X_RDK_DataInterfaceLink      = DATA_INTERFACE_LINK_RAW_IP;

    pstDmlCellular->ulInterfaceNoEntries        = 1;
    pstDmlCellular->pstInterfaceInfo = (PCELLULAR_INTERFACE_INFO)AnscAllocateMemory(sizeof(CELLULAR_INTERFACE_INFO) * pstDmlCellular->ulInterfaceNoEntries);
    if ( pstDmlCellular->pstInterfaceInfo == NULL )
    {
        return  ANSC_STATUS_FAILURE;
    }

    AnscZeroMemory(pstDmlCellular->pstInterfaceInfo, sizeof(CELLULAR_INTERFACE_INFO) * pstDmlCellular->ulInterfaceNoEntries);

    for( iLoopCount = 0; iLoopCount < pstDmlCellular->ulInterfaceNoEntries; iLoopCount++ )
    {
        PCELLULAR_INTERFACE_INFO   pstInterfaceInfo = &(pstDmlCellular->pstInterfaceInfo[iLoopCount]);
        
        pstInterfaceInfo->Enable              = TRUE;
        pstInterfaceInfo->Status              = IF_DOWN;
        pstInterfaceInfo->X_RDK_RegisteredService = REGISTERED_SERVICE_NONE; 

        pstInterfaceInfo->stPlmnAccessInfo.RoamingStatus  = ROAMING_STATUS_HOME;
        pstInterfaceInfo->stPlmnAccessInfo.ulAvailableNetworkNoOfEntries  = 0;

        //Below information ready once Modem Opened
        pstInterfaceInfo->ulNeighbourNoOfEntries = 0;

        //Static Context
        pstInterfaceInfo->ulContextProfileNoOfEntries = 1;

        pstInterfaceInfo->pstContextProfileInfo = (PCELLULAR_INTERFACE_CONTEXTPROFILE_INFO)AnscAllocateMemory(sizeof(CELLULAR_INTERFACE_CONTEXTPROFILE_INFO) * pstInterfaceInfo->ulContextProfileNoOfEntries);
        if ( pstInterfaceInfo->pstContextProfileInfo == NULL )
        {
            return  ANSC_STATUS_FAILURE;
        }

        memset(pstInterfaceInfo->pstContextProfileInfo, 0, sizeof(CELLULAR_INTERFACE_CONTEXTPROFILE_INFO));
    }

    /*
     * Initialize UICC slot DML and needs to be build once Modem opened
    */
    pstDmlCellular->ulUICCNoOfEntries = 0;

    /*
     * Initialize EUICC slot DML and needs to be build once Modem opened
    */
    pstDmlCellular->stEUICCSlotInfo.ulMNOProfileNoOfEntries = 0;

    /*
     * Initialize AccessPoint DML and needs to be build once Modem opened
    */
    pstDmlCellular->ulAccessPointNoOfEntries = 0;

    /* Assign to master structure */
    pMyObject->pstDmlCellular = pstDmlCellular;

    //Read all defaults from DB
    stCtxInputStruct.stIfInput.PDPContextNumber   = CELLULAR_PDP_CONTEXT_UNKNOWN;
    stCtxInputStruct.stIfInput.ProxyPort          = 0;
    snprintf(stCtxInputStruct.stIfInput.Proxy, sizeof(stCtxInputStruct.stIfInput.Proxy), "%s", "");
    CellularMgr_InitializeContextDefaultProfile( &stCtxInputStruct );

    //HAL Init
    cellular_hal_init( &stCtxInputStruct );

    //Start Cellular Manager SM
    stStateMachineInput.bModemEnable = pstDmlCellular->X_RDK_Enable;
    stStateMachineInput.pCmIfData = &(pMyObject->pstDmlCellular->pstInterfaceInfo[0]);
    memcpy(&(stStateMachineInput.stContextProfile), &(stCtxInputStruct.stIfInput), sizeof(CellularProfileStruct));
    CellularMgr_Start_State_Machine( &stStateMachineInput );

    CcspTraceInfo(("%s %d - Done\n",__FUNCTION__,__LINE__));
    
    return returnStatus;
}

int CellularMgrGetLTEIfaceIndex ( char *wan_ifname )
{
    unsigned int totalNoOfIface = 0;
    char paramName[256];
    char paramValue[256];
    unsigned int index = 0;

    memset ( paramValue, 0, sizeof(paramValue) );

    if ( (CellularMgr_RdkBus_GetParamValue( WAN_COMPONENT_NAME, WAN_DBUS_PATH, DML_NO_OF_INTERFACE_ENTRIES, paramValue ) != RETURN_OK)
            || ( paramValue == NULL) )
    {
        CcspTraceError(("%s %d - Unable to fetch total no of interfaces.\n", __FUNCTION__, __LINE__));
        return -1;
    }

    totalNoOfIface = atoi (paramValue);

    memset ( paramName, 0, sizeof(paramName) );
    memset ( paramValue, 0, sizeof(paramValue) );

    for (index = 0; index < totalNoOfIface; index++)
    {
        snprintf(paramName, sizeof(paramName), DML_IFTABLE_IFACE_NAME, index + 1);
        if ( (CellularMgr_RdkBus_GetParamValue ( WAN_COMPONENT_NAME, WAN_DBUS_PATH, paramName, paramValue) != RETURN_OK)
                || (paramValue == NULL))
        {
            CcspTraceError(("%s %d - Unable to fetch %s.\n", __FUNCTION__, __LINE__, paramName));
            return -1;
        }

        if ( strncmp (paramValue, wan_ifname, strlen(wan_ifname)) == 0 )
        {
            return index + 1;
        }
    }
    CcspTraceError(("%s %d - Unable to fetch %s interface entry in interface table.\n", __FUNCTION__, __LINE__, wan_ifname));
    return -1;
}

int CellularMgrUpdatePhyStatus ( char *wan_ifname, CellularDeviceOpenStatus_t device_open_status )
{
    int index = -1;
    char paramName[256];
    char paramValue[256];
    
    index = CellularMgrGetLTEIfaceIndex(wan_ifname);

    if( -1 == index )
    {
        return RETURN_ERROR;
    }

    memset( paramName, 0, sizeof(paramName) );
    memset( paramValue, 0, sizeof(paramValue) );

    // Send Phy Status for Interface
    if ( device_open_status == DEVICE_OPEN_STATUS_READY )
    {
        // Set Phy.Path
        snprintf( paramName, sizeof(paramName), DML_IFTABLE_IFACE_PHY_PATH, index);
        snprintf( paramValue, sizeof(paramValue), DML_LTE_IFACE_PATH);

        if ( CellularMgr_RdkBus_SetParamValue( WAN_COMPONENT_NAME, WAN_DBUS_PATH, paramName, paramValue, ccsp_string, true) != RETURN_OK )
        {
            CcspTraceError(("%s %d - Unable to set %s value.\n", __FUNCTION__, __LINE__, DML_IFTABLE_IFACE_PHY_PATH));
            return RETURN_ERROR;
        }

        // Set Phy.Status
        memset( paramName, 0, sizeof(paramName) );
        memset( paramValue, 0, sizeof(paramValue) );
        snprintf( paramName, sizeof(paramName), DML_IFTABLE_IFACE_PHY_STATUS, index);
        snprintf( paramValue, sizeof(paramValue), UP_STR);

        if ( CellularMgr_RdkBus_SetParamValue( WAN_COMPONENT_NAME, WAN_DBUS_PATH, paramName, paramValue, ccsp_string, true) != RETURN_OK )
        {
            CcspTraceError(("%s %d - Unable to set %s value.\n", __FUNCTION__, __LINE__, DML_IFTABLE_IFACE_PHY_PATH));
            return RETURN_ERROR;
        }
    }
    else
    {
        // Set Phy.Status
        snprintf( paramName, sizeof(paramName), DML_IFTABLE_IFACE_PHY_STATUS, index);
        snprintf( paramValue, sizeof(paramValue), DOWN_STR);
        if ( CellularMgr_RdkBus_SetParamValue( WAN_COMPONENT_NAME, WAN_DBUS_PATH, paramName, paramValue, ccsp_string, true) != RETURN_OK )
        {
            CcspTraceError(("%s %d - Unable to set %s value.\n", __FUNCTION__, __LINE__, DML_IFTABLE_IFACE_PHY_PATH));
            return RETURN_ERROR;
        }
    }

    return RETURN_OK;
}

int CellularMgrUpdateLinkStatus ( char *wan_ifname, char *status )
{
    int index = -1;
    char paramName[256];
    char paramValue[256];

    index = CellularMgrGetLTEIfaceIndex(wan_ifname);
    
    if( -1 == index )
    {
        return RETURN_ERROR;
    }

    memset( paramName, 0, sizeof(paramName) );
    memset( paramValue, 0, sizeof(paramValue) );

    snprintf( paramName, sizeof(paramName), DML_IFTABLE_IFACE_WAN_NAME, index);
    snprintf( paramValue, sizeof(paramValue) - 1, wan_ifname);

    if ( CellularMgr_RdkBus_SetParamValue( WAN_COMPONENT_NAME, WAN_DBUS_PATH, paramName, paramValue, ccsp_string, true) != RETURN_OK )
    {
        CcspTraceError(("%s %d - Unable to set %s value.\n", __FUNCTION__, __LINE__, DML_IFTABLE_IFACE_WAN_NAME));
        return RETURN_ERROR;
    }
    
    memset( paramName, 0, sizeof(paramName) );
    memset( paramValue, 0, sizeof(paramValue) );

    snprintf( paramName, sizeof(paramName), DML_IFTABLE_IFACE_LINK_STATUS, index);
    snprintf( paramValue, sizeof(paramValue) - 1, status);

    if ( CellularMgr_RdkBus_SetParamValue( WAN_COMPONENT_NAME, WAN_DBUS_PATH, paramName, paramValue, ccsp_string, true) != RETURN_OK )
    {
        CcspTraceError(("%s %d - Unable to set %s value.\n", __FUNCTION__, __LINE__, DML_IFTABLE_IFACE_LINK_STATUS));
        return RETURN_ERROR;
    }

    return RETURN_OK;
}

int CellularMgr_AccessPointGetProfileList( CELLULAR_INTERFACE_ACCESSPOINT_INFO  **ppstAPInfo, int *profile_count )
{
    CellularProfileStruct                *pstProfileOutput = NULL;
    INT                                  iProfileCount = 0;

    if( RETURN_OK == cellular_hal_get_profile_list( &pstProfileOutput, &iProfileCount ) )
    {
        if( 0 < iProfileCount )
        {
            CELLULAR_INTERFACE_ACCESSPOINT_INFO  *pstAPInfo = NULL;

            pstAPInfo = (CELLULAR_INTERFACE_ACCESSPOINT_INFO*)malloc( (sizeof(CELLULAR_INTERFACE_ACCESSPOINT_INFO) * (iProfileCount) ) );
            
            if( NULL != pstAPInfo )
            {
                int i;

                memset( pstAPInfo, 0, ( sizeof(CELLULAR_INTERFACE_ACCESSPOINT_INFO) * (iProfileCount) ) );

                for( i = 0; i < iProfileCount; i++ )
                {
                    pstAPInfo[i].ProfileIndex       = pstProfileOutput[i].ProfileID;
                    pstAPInfo[i].PDPContextNumber   = pstProfileOutput[i].PDPContextNumber;
                    pstAPInfo[i].X_RDK_Default      = pstProfileOutput[i].bIsThisDefaultProfile;
                    pstAPInfo[i].Enable             = ( pstProfileOutput[i].bIsAPNDisabled ) ?  FALSE : TRUE;
                    snprintf(pstAPInfo[i].Alias, sizeof(pstAPInfo[i].Alias), pstProfileOutput[i].ProfileName);
                    snprintf(pstAPInfo[i].APN, sizeof(pstAPInfo[i].APN), pstProfileOutput[i].APN);
                    snprintf(pstAPInfo[i].Username, sizeof(pstAPInfo[i].Username), pstProfileOutput[i].Username);
                    snprintf(pstAPInfo[i].Password, sizeof(pstAPInfo[i].Password), pstProfileOutput[i].Password);
                    pstAPInfo[i].X_RDK_ApnAuthentication  = pstProfileOutput[i].PDPAuthentication;
                    pstAPInfo[i].X_RDK_PdpInterfaceConfig = CELLULAR_PDP_NETWORK_CONFIG_NAS;
                    pstAPInfo[i].X_RDK_Roaming            = ( pstProfileOutput[i].bIsNoRoaming ) ? FALSE : TRUE;

                    if( CELLULAR_PDP_TYPE_IPV4 == pstProfileOutput[i].PDPType )
                    {
                        pstAPInfo[i].X_RDK_IpAddressFamily = INTERFACE_PROFILE_FAMILY_IPV4;
                    }
                    else if( CELLULAR_PDP_TYPE_IPV6 == pstProfileOutput[i].PDPType )
                    {
                        pstAPInfo[i].X_RDK_IpAddressFamily = INTERFACE_PROFILE_FAMILY_IPV6;
                    }
                    else if( CELLULAR_PDP_TYPE_IPV4_OR_IPV6 == pstProfileOutput[i].PDPType )
                    {
                        pstAPInfo[i].X_RDK_IpAddressFamily = INTERFACE_PROFILE_FAMILY_IPV4_IPV6;
                    }

                    CcspTraceDebug(("%s - Profile Name[%s] ProfileID[%d] APN[%s] IP[%d] User[%s] Password[%s] Auth[%d] APN Disabled[%d] Default[%d]\n",
                                                            __FUNCTION__,
                                                            pstAPInfo[i].Alias,
                                                            pstAPInfo[i].ProfileIndex,
                                                            pstAPInfo[i].APN,
                                                            pstAPInfo[i].X_RDK_IpAddressFamily,
                                                            pstAPInfo[i].Username,
                                                            pstAPInfo[i].Password,
                                                            pstAPInfo[i].X_RDK_ApnAuthentication,
                                                            pstAPInfo[i].Enable,
                                                            pstAPInfo[i].X_RDK_Default));
                }
            }

            //Initialize passed variables
            *profile_count = 0;

            if( NULL != *ppstAPInfo )
            {
                free(*ppstAPInfo);
                *ppstAPInfo = NULL;
            }

            //Assign proper values to passed structure
            *ppstAPInfo    = pstAPInfo;
            *profile_count = iProfileCount;

            //Release HAL structure memory
            if( NULL != pstProfileOutput )
            {
                free(pstProfileOutput);
                pstProfileOutput = NULL;
            }
        }
        else
        {
            //Initialize passed variables
            *profile_count = 0;

            if( NULL != *ppstAPInfo )
            {
                free(*ppstAPInfo);
                *ppstAPInfo = NULL;
            }
        }

        return RETURN_OK;
    }

    return RETURN_OK;
}

int CellularMgr_AccessPointCreateProfile( PCELLULAR_INTERFACE_ACCESSPOINT_INFO pstAPInfo )
{
    CellularProfileStruct stProfileInput = { 0 };

    stProfileInput.ProfileID                = pstAPInfo->ProfileIndex;
    stProfileInput.PDPContextNumber         = CELLULAR_PDP_CONTEXT_UNKNOWN;
    stProfileInput.bIsThisDefaultProfile    = pstAPInfo->X_RDK_Default;
    stProfileInput.bIsAPNDisabled           = ( pstAPInfo->Enable ) ? FALSE : TRUE;
    snprintf(stProfileInput.ProfileName, sizeof(stProfileInput.ProfileName), pstAPInfo->Alias);
    snprintf(stProfileInput.APN, sizeof(stProfileInput.APN), pstAPInfo->APN);
    snprintf(stProfileInput.Username, sizeof(stProfileInput.Username), pstAPInfo->Username);
    snprintf(stProfileInput.Password, sizeof(stProfileInput.Password), pstAPInfo->Password);
    stProfileInput.PDPAuthentication        = pstAPInfo->X_RDK_ApnAuthentication;
    stProfileInput.PDPNetworkConfig         = pstAPInfo->X_RDK_PdpInterfaceConfig;
    stProfileInput.bIsNoRoaming             = ( pstAPInfo->X_RDK_Roaming ) ? FALSE : TRUE;

    if( INTERFACE_PROFILE_FAMILY_IPV4 == pstAPInfo->X_RDK_IpAddressFamily )
    {
        stProfileInput.PDPType = CELLULAR_PDP_TYPE_IPV4;
    }
    else if( INTERFACE_PROFILE_FAMILY_IPV6 == pstAPInfo->X_RDK_IpAddressFamily )
    {
        stProfileInput.PDPType = CELLULAR_PDP_TYPE_IPV6;
    }
    else if( INTERFACE_PROFILE_FAMILY_IPV4_IPV6 == pstAPInfo->X_RDK_IpAddressFamily )
    {
        stProfileInput.PDPType = CELLULAR_PDP_TYPE_IPV4_OR_IPV6;
    }
    else
    {
        stProfileInput.PDPType = CELLULAR_PDP_TYPE_IPV4;
    }

    //Create Profile
    if ( RETURN_OK != cellular_hal_profile_create( &stProfileInput, NULL ) )
    {
        CcspTraceError(("%s - Failed to create profile information\n", __FUNCTION__));
        return RETURN_ERROR;
    }

    return RETURN_OK;
}

int CellularMgr_AccessPointDeleteProfile( PCELLULAR_INTERFACE_ACCESSPOINT_INFO pstAPInfo )
{
    CellularProfileStruct stProfileInput = { 0 };

    stProfileInput.ProfileID                = pstAPInfo->ProfileIndex;
    stProfileInput.PDPContextNumber         = pstAPInfo->PDPContextNumber;
    stProfileInput.bIsThisDefaultProfile    = pstAPInfo->X_RDK_Default;
    stProfileInput.bIsAPNDisabled           = ( pstAPInfo->Enable ) ? FALSE : TRUE;
    snprintf(stProfileInput.ProfileName, sizeof(stProfileInput.ProfileName), pstAPInfo->Alias);
    snprintf(stProfileInput.APN, sizeof(stProfileInput.APN), pstAPInfo->APN);
    snprintf(stProfileInput.Username, sizeof(stProfileInput.Username), pstAPInfo->Username);
    snprintf(stProfileInput.Password, sizeof(stProfileInput.Password), pstAPInfo->Password);
    stProfileInput.PDPAuthentication        = pstAPInfo->X_RDK_ApnAuthentication;
    stProfileInput.PDPNetworkConfig         = pstAPInfo->X_RDK_PdpInterfaceConfig;
    stProfileInput.bIsNoRoaming             = ( pstAPInfo->X_RDK_Roaming ) ? FALSE : TRUE;

    if( INTERFACE_PROFILE_FAMILY_IPV4 == pstAPInfo->X_RDK_IpAddressFamily )
    {
        stProfileInput.PDPType = CELLULAR_PDP_TYPE_IPV4;
    }
    else if( INTERFACE_PROFILE_FAMILY_IPV6 == pstAPInfo->X_RDK_IpAddressFamily )
    {
        stProfileInput.PDPType = CELLULAR_PDP_TYPE_IPV6;
    }
    else if( INTERFACE_PROFILE_FAMILY_IPV4_IPV6 == pstAPInfo->X_RDK_IpAddressFamily )
    {
        stProfileInput.PDPType = CELLULAR_PDP_TYPE_IPV4_OR_IPV6;
    }

    //Delete Profile
    if ( RETURN_OK != cellular_hal_profile_delete( &stProfileInput, NULL ) )
    {
        CcspTraceError(("%s - Failed to delete profile information\n", __FUNCTION__));
        return RETURN_ERROR;
    }

    return RETURN_OK;
}

int CellularMgr_AccessPointModifyProfile( PCELLULAR_INTERFACE_ACCESSPOINT_INFO pstAPInfo )
{
    CellularProfileStruct stProfileInput = { 0 };

    stProfileInput.ProfileID                = pstAPInfo->ProfileIndex;
    stProfileInput.PDPContextNumber         = pstAPInfo->PDPContextNumber;
    stProfileInput.bIsThisDefaultProfile    = pstAPInfo->X_RDK_Default;
    stProfileInput.bIsAPNDisabled           = ( pstAPInfo->Enable ) ? FALSE : TRUE;
    snprintf(stProfileInput.ProfileName, sizeof(stProfileInput.ProfileName), pstAPInfo->Alias);
    snprintf(stProfileInput.APN, sizeof(stProfileInput.APN), pstAPInfo->APN);
    snprintf(stProfileInput.Username, sizeof(stProfileInput.Username), pstAPInfo->Username);
    snprintf(stProfileInput.Password, sizeof(stProfileInput.Password), pstAPInfo->Password);
    stProfileInput.PDPAuthentication        = pstAPInfo->X_RDK_ApnAuthentication;
    stProfileInput.PDPNetworkConfig         = pstAPInfo->X_RDK_PdpInterfaceConfig;
    stProfileInput.bIsNoRoaming             = ( pstAPInfo->X_RDK_Roaming ) ? FALSE : TRUE;

    if( INTERFACE_PROFILE_FAMILY_IPV4 == pstAPInfo->X_RDK_IpAddressFamily )
    {
        stProfileInput.PDPType = CELLULAR_PDP_TYPE_IPV4;
    }
    else if( INTERFACE_PROFILE_FAMILY_IPV6 == pstAPInfo->X_RDK_IpAddressFamily )
    {
        stProfileInput.PDPType = CELLULAR_PDP_TYPE_IPV6;
    }
    else if( INTERFACE_PROFILE_FAMILY_IPV4_IPV6 == pstAPInfo->X_RDK_IpAddressFamily )
    {
        stProfileInput.PDPType = CELLULAR_PDP_TYPE_IPV4_OR_IPV6;
    }

    //Modify Profile
    if ( RETURN_OK != cellular_hal_profile_modify( &stProfileInput, NULL ) )
    {
        CcspTraceError(("%s - Failed to modify profile information\n", __FUNCTION__));
        return RETURN_ERROR;
    }

    return RETURN_OK;
}

CELLULAR_RADIO_ENV_CONDITIONS CellularMgr_GetRadioEnvConditions( void ) 
{
    CELLULAR_RDK_STATUS            enRDKStatus = (CELLULAR_RDK_STATUS)CellularMgrSMGetCurrentState( );
    CELLULAR_RADIO_ENV_CONDITIONS  enRadioEnvCondition;
    
    switch( enRDKStatus )
    {
        case RDK_STATUS_DOWN:
        case RDK_STATUS_DEACTIVATED:
        case RDK_STATUS_DEREGISTERED:
        {
            enRadioEnvCondition = RADIO_ENV_CONDITION_UNAVAILABLE;
        }
        break;

        case RDK_STATUS_REGISTERED:
        case RDK_STATUS_CONNECTED:
        {
            CellularSignalInfoStruct stSignalInfo = { 0 };

            /*
            * EXCELLENT : ( RSRP > -85 )
            * GOOD:   (-85  >=  RSRP >  -95) 
            * FAIR:   (-95  >=  RSRP >  -105) 
            * POOR:   (-105 >= RSRP > -115) 
            */

            if( RETURN_OK == cellular_hal_get_signal_info( &stSignalInfo ) )
            {
                if( stSignalInfo.RSRP == 0 )
                {
                    enRadioEnvCondition = RADIO_ENV_CONDITION_UNAVAILABLE;
                }
                else
                {
                    if( stSignalInfo.RSRP > CELLULAR_RADIO_ENV_EXCELLENT_THRESHOLD )
                    {
                        enRadioEnvCondition = RADIO_ENV_CONDITION_EXCELLENT;
                    }
                    else if( ( CELLULAR_RADIO_ENV_GOOD_THRESHOLD_HIGH >= stSignalInfo.RSRP ) && ( stSignalInfo.RSRP > CELLULAR_RADIO_ENV_GOOD_THRESHOLD_LOW ) )
                    {
                        enRadioEnvCondition = RADIO_ENV_CONDITION_GOOD;
                    }
                    else if( ( CELLULAR_RADIO_ENV_FAIR_THRESHOLD_HIGH >= stSignalInfo.RSRP ) && ( stSignalInfo.RSRP > CELLULAR_RADIO_ENV_FAIR_THRESHOLD_LOW ) )
                    {
                        enRadioEnvCondition = RADIO_ENV_CONDITION_FAIR;
                    }
                    else if( ( CELLULAR_RADIO_ENV_POOR_THRESHOLD_HIGH >= stSignalInfo.RSRP ) && ( stSignalInfo.RSRP > CELLULAR_RADIO_ENV_POOR_THRESHOLD_LOW ) )
                    {
                        enRadioEnvCondition = RADIO_ENV_CONDITION_POOR;
                    }
                    else
                    {
                        enRadioEnvCondition = RADIO_ENV_CONDITION_POOR;
                    }
                }
            }
            else
            {
                enRadioEnvCondition = RADIO_ENV_CONDITION_UNAVAILABLE;
            }
        }
        break;

        default:
        {
            enRadioEnvCondition = RADIO_ENV_CONDITION_UNAVAILABLE;
        }
    }

    return enRadioEnvCondition;
}

int CellularMgr_ServingCellGetSignalInfo( CELLULAR_INTERFACE_SERVING_INFO *pstServingInfo )
{
    CellularSignalInfoStruct stSignalInfo = { 0 };

    if( RETURN_OK == cellular_hal_get_signal_info( &stSignalInfo ) )
    {
        pstServingInfo->TotalReceivedSignal   = stSignalInfo.RSSI;
        pstServingInfo->SignalToNoiseRatio    = stSignalInfo.SNR;
        pstServingInfo->ReceivedSignal        = stSignalInfo.RSRP;
        pstServingInfo->ReceivedSignalQuality = stSignalInfo.RSRQ;
    }

    return RETURN_OK;
}

int CellularMgr_ServingSystemInfo( CELLULAR_INTERFACE_INFO  *pstInterfaceInfo, CELLULAR_INTERFACE_CONTEXTPROFILE_INFO *pstContextProfileInfo)
{
    int registration_status;
    int roaming_status;
    int attach_status;

    cellular_get_serving_info(&registration_status, &roaming_status, &attach_status);
    if (pstInterfaceInfo != NULL) {
        if( DEVICE_NAS_STATUS_REGISTERED == (CellularDeviceNASStatus_t)registration_status ) {
            pstInterfaceInfo->Status = IF_UP;
        }
        else {
           pstInterfaceInfo->Status = IF_DOWN;
        }
    }
    if (pstContextProfileInfo != NULL) {
        if ( PROFILE_STATUS_ACTIVE == (CellularContextProfileStatus_t)attach_status ) {
             pstContextProfileInfo->Status = CONTEXTPROFILE_STATUS_ACTIVE;
        }
        else {
             pstContextProfileInfo->Status = CONTEXTPROFILE_STATUS_INACTIVE;
        }
    }
    return RETURN_OK;
}
int CellularMgr_SetModemEnable( BOOLEAN bEnable )
{
    CellularMgrSMSetCellularEnable(bEnable);
    return ( cellular_hal_set_modem_operating_configuration( ( TRUE == bEnable ) ?  CELLULAR_MODEM_SET_ONLINE : CELLULAR_MODEM_SET_OFFLINE ));
}

int CellularMgr_GetModemIMEI( char *pcIMEI )
{
    return ( cellular_hal_get_device_imei( pcIMEI ) );
}

int CellularMgr_GetModemIMEISoftwareVersion( char *pcIMEI_SV )
{
    return ( cellular_hal_get_device_imei_sv( pcIMEI_SV ) );
}

int CellularMgr_GetModemCurrentICCID( char *pcICCID )
{
    return ( cellular_hal_get_modem_current_iccid( pcICCID ) );
}

int CellularMgr_GetNetworkPacketStatisticsInfo( PCELLULAR_INTERFACE_STATS_INFO pstStatsInfo )
{
    CELLULAR_RDK_STATUS  enRDKStatus = (CELLULAR_RDK_STATUS)CellularMgrSMGetCurrentState( );

    //Needs to collect statistics only when network connected case
    if( enRDKStatus == RDK_STATUS_CONNECTED )
    {
        CellularPacketStatsStruct stNetworkPacket = { 0 };

        if( RETURN_OK == cellular_hal_get_packet_statistics( &stNetworkPacket ) )
        {
            pstStatsInfo->BytesSent             = stNetworkPacket.BytesSent;
            pstStatsInfo->BytesReceived         = stNetworkPacket.BytesReceived;
            pstStatsInfo->PacketsSent           = stNetworkPacket.PacketsSent;
            pstStatsInfo->PacketsReceived       = stNetworkPacket.PacketsReceived;
            pstStatsInfo->PacketsSentDrop       = stNetworkPacket.PacketsSentDrop;
            pstStatsInfo->PacketsReceivedDrop   = stNetworkPacket.PacketsReceivedDrop;
            pstStatsInfo->UpStreamMaxBitRate    = stNetworkPacket.UpStreamMaxBitRate;
            pstStatsInfo->DownStreamMaxBitRate  = stNetworkPacket.DownStreamMaxBitRate;

            return RETURN_OK;
        }
    }
    else
    {
        memset(pstStatsInfo, 0, sizeof(CELLULAR_INTERFACE_STATS_INFO));
        return RETURN_OK;
    }

    return RETURN_ERROR;
}

CELLULAR_CONTROL_INTERFACE_STATUS CellularMgr_GetModemControlInterfaceStatus( void )
{
    if( TRUE == cellular_hal_IsModemControlInterfaceOpened( ) )
    {
        return CONTROL_STATUS_OPENED;
    }
    else
    {   
        return CONTROL_STATUS_CLOSED;
    }
}

int CellularMgr_GetModemInterfaceStatus( CellularInterfaceStatus_t *interface_status )
{
    return ( cellular_hal_get_current_modem_interface_status( interface_status ) );
}

int CellularMgr_SetModemInterfaceEnable( BOOLEAN bEnable )
{
    //Network attach/detach operation
    if( bEnable )
    {
        return ( cellular_hal_set_modem_network_attach( ) );
    }
    else
    {
        return ( cellular_hal_set_modem_network_detach( ) );
    }
}

int CellularMgr_SetSIMPowerEnable( UINT uiSlotID, BOOLEAN bEnable )
{
    return cellular_hal_sim_power_enable(uiSlotID,bEnable);
}

int CellularMgr_GetTotalUICCSlots( UINT *puiTotalSlots )
{
    return cellular_hal_get_total_no_of_uicc_slots(puiTotalSlots);
}

int CellularMgr_GetUICCSlotInfo(UINT uiSlotID, PCELLULAR_UICC_SLOT_INFO  pstUICCSlotInfo)
{
    CellularUICCSlotInfoStruct stSlotInfo = {0};

    if( RETURN_OK == cellular_hal_get_uicc_slot_info( uiSlotID, &stSlotInfo ) )
    {
        pstUICCSlotInfo->PowerEnable = stSlotInfo.CardEnable;
        
        if( CELLULAR_UICC_STATUS_VALID == stSlotInfo.Status )
        {
            pstUICCSlotInfo->Status = SIM_STATUS_VALID;
        }
        else
        {
            pstUICCSlotInfo->Status = SIM_STATUS_EMPTY;
        }

        snprintf( pstUICCSlotInfo->Iccid, sizeof(pstUICCSlotInfo->Iccid), stSlotInfo.iccid );
        snprintf( pstUICCSlotInfo->Msisdn, sizeof(pstUICCSlotInfo->Msisdn), stSlotInfo.msisdn );
        snprintf( pstUICCSlotInfo->MnoName, sizeof(pstUICCSlotInfo->MnoName), stSlotInfo.MnoName );

        return RETURN_OK;
    }

    return RETURN_ERROR;
}

int CellularMgr_GetActiveCardStatus( CELLULAR_INTERFACE_SIM_STATUS *enCardStatus )
{
    CellularUICCStatus_t card_status;

    *enCardStatus = SIM_STATUS_EMPTY;

    if( RETURN_OK == cellular_hal_get_active_card_status( &card_status ) )
    {   
        if( CELLULAR_UICC_STATUS_VALID == card_status )
        {
            *enCardStatus = SIM_STATUS_VALID;
        }
        else
        {
            *enCardStatus = SIM_STATUS_ERROR;
        }

        return RETURN_OK;
    }

    return RETURN_ERROR;
}

int CellularMgr_GetModemFirmwareVersion(char *pcFirmwareVersion)
{
    return( cellular_hal_get_modem_firmware_version( pcFirmwareVersion ) );
}

int CellularMgr_GetPlmnInformation( PCELLULAR_PLMNACCESS_INFO pstPlmnAccessInfo)
{
    CellularCurrentPlmnInfoStruct stPlmnInfo = {0};

    if( RETURN_OK == cellular_hal_get_current_plmn_information( &stPlmnInfo ) )
    {
        pstPlmnAccessInfo->RoamingEnable = stPlmnInfo.roaming_enabled;

        if( TRUE == pstPlmnAccessInfo->RoamingEnable )
        {
            pstPlmnAccessInfo->RoamingStatus = ROAMING_STATUS_VISITOR;
        }
        else
        {
            pstPlmnAccessInfo->RoamingStatus = ROAMING_STATUS_HOME;
        }

        snprintf( pstPlmnAccessInfo->NetworkInUse_MCC, sizeof(pstPlmnAccessInfo->NetworkInUse_MCC), "%d", stPlmnInfo.MCC );
        snprintf( pstPlmnAccessInfo->NetworkInUse_MNC, sizeof(pstPlmnAccessInfo->NetworkInUse_MNC), "%d", stPlmnInfo.MNC );
        snprintf( pstPlmnAccessInfo->NetworkInUse_Name, sizeof(pstPlmnAccessInfo->NetworkInUse_Name), "%s", stPlmnInfo.plmn_name );

        snprintf( pstPlmnAccessInfo->HomeNetwork_MCC, sizeof(pstPlmnAccessInfo->HomeNetwork_MCC), "%d", stPlmnInfo.MCC );
        snprintf( pstPlmnAccessInfo->HomeNetwork_MNC, sizeof(pstPlmnAccessInfo->HomeNetwork_MNC), "%d", stPlmnInfo.MNC );
        snprintf( pstPlmnAccessInfo->HomeNetwork_Name, sizeof(pstPlmnAccessInfo->HomeNetwork_Name), "%s", stPlmnInfo.plmn_name );

        return RETURN_OK;
    }

    return RETURN_ERROR;
}

int CellularMgr_GetAvailableNetworksInformation( PCELLULAR_PLMN_AVAILABLENETWORK_INFO *ppAvailableNetworkInfo, unsigned int *puiTotalCount )
{
    CellularNetworkScanResultInfoStruct *network_info = NULL;
    unsigned int total_network_count = 0;

    *puiTotalCount = 0;

    if( NULL != *ppAvailableNetworkInfo )
    {
        free( *ppAvailableNetworkInfo );
        *ppAvailableNetworkInfo = NULL;
    }

    if( RETURN_OK == cellular_hal_get_available_networks_information( &network_info, &total_network_count ) )
    {
        if( 0 < total_network_count )
        {
            CELLULAR_PLMN_AVAILABLENETWORK_INFO *pstTmpInfo = NULL;
            int i = 0;

            pstTmpInfo = (CELLULAR_PLMN_AVAILABLENETWORK_INFO*) malloc( sizeof(CELLULAR_PLMN_AVAILABLENETWORK_INFO) * total_network_count );

            for( i = 0; i < total_network_count; i++ )
            {
                snprintf( pstTmpInfo[i].MCC, sizeof(pstTmpInfo[i].MCC), "%d", network_info[i].MCC );
                snprintf( pstTmpInfo[i].MNC, sizeof(pstTmpInfo[i].MNC), "%d", network_info[i].MNC );
                snprintf( pstTmpInfo[i].Name, sizeof(pstTmpInfo[i].Name), "%d", network_info[i].network_name );
                pstTmpInfo[i].Allowed = network_info[i].roaming_flag;
            }

            *ppAvailableNetworkInfo = pstTmpInfo;
        }
    }

    return RETURN_OK;
}
