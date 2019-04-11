/*
 * rddi_configinfo.h - Configuration Information header for RDDI
 * Copyright (C) 2017 ARM Limited. All rights reserved.
 */


#ifndef RDDI_CONFIGINFO_H
#define RDDI_CONFIGINFO_H

#include "rddi.h"
 
/**  \file

     \brief This file describes the Configuration File functionality of the 
     RDDI module along with the necessary definitions. 

     \details Many debug agents (such as DSTREAM) use a configuration file to hold
     information such as the connection details, the list of devices on the
     scan chain and configuration details for each device etc. To make a connection
     to such a debug agent, you will need to inform RDDI of the configuration file
     to use. This must be done after calling RDDI_Open() but before attempting to
     make connections to the debug agent.
*/

#if DOXYGEN_ACTIVE
/** 

    \brief Open and parse a debug agent configuration file

    The contents of this file are used by the JTAG and 
    Debug interfaces to ensure complete and correct configuration of target 
    connections.

    \param[in] handle Session handle previously created by RDDI_Open()

    \param[in] pFilename Fully qualified name and path of the configuration file.

    \dontinclude init.c
    \skip void Init
    \skip /
    \until {
    \skip pConfigFile
    \until }
    \until }

    For a further example usage see JTAG_Connect()
*/
#endif
RDDI int ConfigInfo_OpenFile(
        RDDIHandle handle,
        const char *pFilename);


/** 

    \brief Open and parse a debug agent configuration file and retarget the
    connection to use the specified debug agent address

    The contents of this file are used by the JTAG and 
    Debug interfaces to ensure complete and correct configuration of target 
    connections.

    \param[in] handle Session handle previously created by RDDI_Open()

    \param[in] pFilename Fully qualified name and path of the configuration file.
    \param[in] pAddress Address of the debug agent to be used. 
      It will be used to replace any address present in the configuration file.
      The address can be obtained from the Browse() functions.,
      e.g.: With the RDDI_Debug_RVI implementation, for DSTREAM Family vehicles,
      "TCP:xxx.xxx.xxx.xxx", "TCP:my-rvi-name" or "USB:<serial number>". 

*/

RDDI int ConfigInfo_OpenFileAndRetarget(
        RDDIHandle handle,
        const char *pFilename,
        const char *pAddress);

/** 
    \brief Change a configuration file so that it now uses the supplied debug agent
           connection address.

    \param[in] pFilename Fully qualified name and path of the configuration 
               file to retarget.
    \param[in] pAddress Address of the debug agent to be used. This address
               can be obtained from the Browse() functions.,
      e.g.: With the RDDI_Debug_RVI implementation, for DSTREAM Family vehicles,
      "TCP:xxx.xxx.xxx.xxx", "TCP:my-rvi-name" or "USB:<serial number>". 

    \returns 
    \li RDDI_BADARG if the filename or address parameters are NULL 
    \li RDDI_PARSE_FAILED if the address information in the specified config 
        file cannot be found (i.e. it was not a valid config file to begin 
        with)
    \li RDDI_NO_CONFIG_FILE if the specified file cannot be read
    \li RDDI_FAILED if the config file cannot be written to

    \details Calling this function will result in a change to the specified 
    configuration file so that it will subsequently refer to the debug agent at the supplied
    address. The function will leave behind a copy of the original file,
    \em "<oldname>.old". It is recommended to call ConfigInfo_RetargetFile() 
    before calling ConfigInfo_OpenFile() as multiple calls to ConfigInfo_OpenFile()
    could cause undefined behaviour.
    For example usage see Browse_Search()

*/
RDDI int ConfigInfo_RetargetFile(
        const char *pFilename,
        const char *pAddress);

/** 
    \brief Creates a blank debug agent configuration file.

    \param[in] pFilename Fully qualified name and path of the configuration 
    file to create. Note that if the specified file already exists, its 
    contents will be lost.
*/
RDDI int ConfigInfo_CreateBlankFile(
        const char *pFilename);

#if DOXYGEN_ACTIVE
/** 
    \brief Runs a tool to edit/create a configuration file and waits for the tool
           to exit before returning.

    \param[in] pFilename Fully qualified name and path of the configuration 
               file to edit. If this is NULL then no filename will be 
               passed to tool. Calling the function in this
               manner is not recommended (as any resulting configuration file
               will not be available to the calling application). 
               An application would typically provide an existing file or
               create a blank one first using ConfigInfo_CreateBlankFile(),
               as this will let the application know the name of the file.
    
    \note This function will wait for the edit tool to complete before returning.

    \dontinclude rvconfig.c
    \skip static void RunRvconfig
    \line garbage
    \skip static void RunRvconfig
    \skip configFile
    \until }
    \until }
    \until }
*/
#endif
RDDI int ConfigInfo_EditFile(
        const char *pFilename);

#if DOXYGEN_ACTIVE
/** 
    \brief Starts a tool to edit/create a configuration file and then returns.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] pFilename Fully qualified name and path of the configuration 
               file to edit. See ConfigInfo_EditFile() for details.
    \param[in] pFn Pointer to callback function to receive asynchronous
    notification that the edit tool has finished running.
    \param[in] pContext User specific context to be returned in callback

    This function will not wait for the edit tool to complete but will return
    immediately. A return value of RDDI_SUCCESS does not indicate that
    the tool has run successfully, just that this call has been made with
    valid parameters. 
    When the tool finishes running (or fails to run) the function pointed to
    by pFn will be called, with parameters indicating the exit result of 
    the tool and the handle passed into this function. The handle serves only
    as an identifier by which the user can distinguish which instance of 
    the tool has exited.

    <B>Example</B> Run RVConfig asynchronously, act on result in callback
    function.
    
    \dontinclude rvconfig.c
    \skip static void AsyncRvconfig
    \line garbage
    \skip static void AsyncRvconfig
    \skip const
    \until }
    \until }
    \until }

    \skip RvConfigCBack
    \until }
    \until }
    \until }
*/
#endif
RDDI int ConfigInfo_EditFileAsync(
        RDDIHandle handle,
        const char *pFilename,
        void (*pFn)(void *pContext, int returnCode, RDDIHandle handle),
        void *pContext);

/**
   \brief Get the connection string of the debug agent.  For a DSTREAM, this will be a string
          of the form TCP:hostname, TCP:xxx.xxx.xxx.xxx or USB:nnnnnn. When using a CADI RTSM, this 
          string will be the name and path of the library file containing the simulation.

   \param[in]  handle Session handle previously created by RDDI_Open()
   \param[out] pString Buffer to receive the connection string.
               This may be a null pointer if the information is not required.
   \param[in]  stringLen If pString is null, the parameter is ignored. Otherwise,
               it specifies the number of bytes available in the buffer. If the buffer is too small
               then the error RDDI_BUFFER_OVERFLOW is returned.
   \param[out] stringUsed If not NULL, receives the number of bytes which 
               are to be copied to pString. Note that if pString is NULL or stringLen is zero then
               no copy actually takes place but the value received in stringUsed is unaffected: 
               this provides a means for callers to determine the buffer size dynamically.

  Note that if the call cannot copy the string to the supplied buffer (pString is NULL or stringLen is zero)
  AND the required space cannot be returned (stringUsed is NULL) then an error occurs and RDDI_BADARG is returned.
 */
RDDI int ConfigInfo_GetConnectionString(
        RDDIHandle handle,
        char       *pString, size_t stringLen,
        size_t     *stringUsed);


/**
   \brief Get the Trace path from a specified device (normally the core) to the TPIU.

   \param[in]  handle Session handle previously created by RDDI_Open()
   \param[in]  deviceNo The ID of the start device, normally the core to be traced.
   \param[out] pRoute A client-supplied buffer to receive the route.
   \param[in]  routeLen Specifies the number of device IDs that can be received by the client buffer
   \param[out] pRouteUsed Receives the number of device IDs in the route.

   The route is returned as an array of device IDs, not including the start point.
   As an example, in a system containing a core (ID 2), ETM (ID 3), Trace Funnel (ID 4), ETB (ID 5) and TPIU (ID 6)
   then the path to the TPIU from the core (device #2) would be returned as an array containing 3 device IDs:
   {3, 4, 6} representing [core]->ETM->TF->TPIU.

   If the supplied buffer is too small to receive the route, then RDDI_BUFFER_OVERFLOW is returned but pRouteUsed
   still received the size of the route, allowing the client to allocate a buffer of the correct size. To facilitate 
   this, pRoute is ignored if it is NULL or if routeLen is zero - no error is returned.
 */
RDDI int ConfigInfo_GetPathToTPIU(
        RDDIHandle   handle,
        int          deviceNo,
        int          *pRoute, size_t routeLen,
        size_t       *pRouteUsed);

/**
   \brief Get the Trace path from a specified device (normally the core) to the ETB.

   For details see ConfigInfo_GetPathToTPIU().
 */
RDDI int ConfigInfo_GetPathToETB(
        RDDIHandle   handle,
        int          deviceNo,
        int          *pRoute, size_t routeLen,
        size_t       *pRouteUsed);

/**
   \brief Get the Trace path from a specified device (normally the core) to the ETM.

   For details see ConfigInfo_GetPathToTPIU().
 */
RDDI int ConfigInfo_GetPathToETM(
        RDDIHandle   handle,
        int          deviceNo,
        int          *pRoute, size_t routeLen,
        size_t       *pRouteUsed);

/** 
    \brief Get the platform identification, if any, from the configuration file.
           If a relevant tag is not found in the file, but no other error
           occurs, the function will return an empty string in the
           corresponding buffer.

    \param[in]  handle Session handle previously created by RDDI_Open()
    \param[out] pName Buffer to receive the platform name.
                This may be a null pointer if the information is not required.
    \param[in]  nameLen If pName is null, the parameter is ignored. Otherwise, 
                it specifies the number of bytes available in the buffer.
    \param[out] pNameUsed If pName is null, returns the number of bytes which 
                would have been needed in the buffer.  This provides a means
                for callers to determine the buffer size dynamically; no error
                is returned.
                Otherwise, the meaning of the returned value depends on whether
                the buffer was large enough:
                If so, the returned value is the number of bytes which were 
                actually copied to the buffer.  
                If not, the value represents the number of bytes which would 
                have been needed in the buffer, and the call returns the 
                error RDDI_BUFFER_OVERFLOW.
    \param[out] pManufacturer Buffer to receive the manufacturer's name.
    \param[in]  manufacturerLen If pManufacturer is null, the parameter is 
                ignored. Otherwise, it specifies the number of bytes available
                in the buffer.
    \param[out] pManufacturerUsed If pManufacturer is null, returns the number
                of bytes which would have been needed in the buffer.  This 
                provides a means for callers to determine the buffer size 
                dynamically; no error is returned.
                Otherwise, the meaning of the returned value depends on whether
                the buffer was large enough:
                If so, the returned value is the number of bytes which were 
                actually copied to the buffer.  
                If not, the value represents the number of bytes which would 
                have been needed in the buffer, and the call returns the 
                error RDDI_BUFFER_OVERFLOW.
*/
RDDI int ConfigInfo_GetPlatform(
        RDDIHandle   handle,
        char         *pName, size_t nameLen,
        size_t       *pNameUsed,
        char         *pManufacturer, size_t manufacturerLen,
        size_t       *pManufacturerUsed);

#endif /* def RDDI_CONFIGINFO_H */


/* end of file rddi_configinfo.h */
