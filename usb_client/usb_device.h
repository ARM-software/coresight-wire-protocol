// usb_device.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include <stdexcept>
#include <stdint.h>
#include <memory>
#include <vector>

/**
 * Exception thrown on USB error
 */
class USBException : public std::runtime_error
{
public:
    /**
     * Create exception
     *
     * @param msg Error message
     */
    USBException(const char* msg)
        : std::runtime_error(msg)
    {
    }

    /**
     * Create exception
     *
     * @param msg Error message
     */
    USBException(const std::string& msg)
        : std::runtime_error(msg)
    {
    }
};


// platform specific identifier for USB devices
#ifdef _WIN32
#include <Windows.h>

/**
 * Windows identifies USB interfaces with GUIDs
 *
 * Each interface of a device has a unique GUID
 */
struct USBDeviceIdentifier
{
    /**
     * Create interface identifier
     *
     * @param g GUID for the interface
     * @param i interface number (typically 0)
     */
    USBDeviceIdentifier(const GUID* g, int i)
        : guid(g), interfaceNumber(i)
    {
    }
    virtual ~USBDeviceIdentifier();

    const GUID* guid;
    int interfaceNumber;
};
#else

/**
 * Linux identifies USB devices with
 *   Vendor ID / Product ID / interface number
 */
struct USBDeviceIdentifier
{
    /**
     * Create interface identifier
     *
     * @param v Vendor ID for the device
     * @param p Product ID for the device
     * @param i interface number
     */
    USBDeviceIdentifier(int v, int p, int i)
        : vendorID(v), productID(p), interfaceNumber(i)
    {
    }
    virtual ~USBDeviceIdentifier();

    int vendorID;
    int productID;
    int interfaceNumber;
};
#endif

/**
 * Endpoint information
 */
struct USBEPInfo
{
    /**
     * Type of endpoint
     */
    enum Type
    {
        EP_TYPE_CONTROL,
        EP_TYPE_ISOCHRONOUS,
        EP_TYPE_BULK,
        EP_TYPE_INTERRUPT,
    };

    /**
     * Direction of bulk & interrupt endpoints
     */
    enum Dir
    {
        /**
         * From host to device
         */
        EP_DIR_OUT = 0x00,

        /**
         * From device to host
         */
        EP_DIR_IN = 0x80,

        /**
         * Mask to extract direction from address
         */
        EP_DIR_MASK = 0x80,
    };

    /**
     * Endpoint type
     */
    Type type;

    /**
     * Endpoint address
     *  [7] is the direction (see Dir)
     *  [6:0] is the endpoint number
     */
    int addr;
};


/**
 * USB device interface
 */
class USBDevice
{
public:
    /**
     * Create a USB device
     *
     * A serial number may be given to attach to a specific instance of a
     * device.  If empty, then the first attached instance of the device found
     * will be used.
     *
     * @param deviceID Identifier for the device
     * @param serialNumber Serial number of a specific instance to attach to
     */
    static std::auto_ptr<USBDevice> create(const USBDeviceIdentifier* deviceID, const std::string& serialNumber);

    /**
     * Control transfer flags
     *
     * Used to form the reqType parameter of controlTransfer()
     */
    enum Control_Transfer
    {
        /**
         * Recipient is a device
         */
        CONTROL_RECIPIENT_DEVICE    = 0,

        /**
         * Recipient is an interface
         */
        CONTROL_RECIPIENT_INTERFACE = 1,

        /**
         * Recipient is an endpoint
         */
        CONTROL_RECIPIENT_ENDPOINT  = 2,

        /**
         * Recipient is other
         */
        CONTROL_RECIPIENT_OTHER     = 3,

        /**
         * Standard request
         */
        CONTROL_TYPE_STANDARD       = (0 << 5),

        /**
         * Class request
         */
        CONTROL_TYPE_CLASS          = (1 << 5),

        /**
         * Vendor specific request
         */
        CONTROL_TYPE_VENDOR         = (2 << 5),

        /**
         * Transfer from device to host
         */
        CONTROL_DIR_IN              = (1 << 7),

        /**
         * Transfer from host to device
         */
        CONTROL_DIR_OUT             = (0 << 7),
    };

    /**
     * Transfer status
     */
    enum Transfer_Status
    {
        Transfer_SUCCESS,
        Transfer_CANCELLED,
        Transfer_ERROR,
        Transfer_IN_PROGRESS,
    };

    virtual ~USBDevice();

    /**
     * Open the connection to the device
     */
    virtual void connect() = 0;

    /**
     * Close the connection to the device
     */
    virtual void disconnect() = 0;

    /**
     * Get the endpoints of the device
     */
    virtual std::vector<USBEPInfo> getEndPoints() = 0;

    /**
     * Get the recommended size for async transfers
     */
    virtual size_t asyncTransferSize() const = 0;

    /**
     * Get the recommended number of simultaneous async transfers
     */
    virtual size_t asyncTransferCount() const = 0;

    /**
     * Control transfer
     *
     * @param reqType The type of request
     * @param request Request number
     * @param value Value to send
     * @param index Index to access
     * @param data Data to send
     * @param size Size of data
     * @param timeout Timeout
     */
    virtual int controlTransfer(uint8_t reqType, uint8_t request,
                                uint16_t value, uint16_t index,
                                void* data, uint16_t size, int timeout) = 0;

    /**
     * Start read request on bulk IN or interrupt IN endpoint
     *
     * @param endpoint Endpoint address
     * @param data Buffer to read into
     * @param size Number of bytes to read
     * @return A unique token for the transfer
     */
    virtual int submitReadTransfer(int endpoint, void* data, size_t size) = 0;

    /**
     * Start write request on bulk OUT or interrupt OUT endpoint
     *
     * @param endpoint Endpoint address
     * @param data Buffer to read into
     * @param size Number of bytes to read
     * @return A unique token for the transfer
     */
    virtual int submitWriteTransfer(int endpoint, const void* data, size_t size) = 0;

    /**
     * Return number of in-progress transfers
     *
     * @return number of submitted transfers that have not been completed
     */
    virtual size_t pendingTransfers() = 0;

    /**
     * Cancel in progress transfers
     *
     * Caller should call completeTransfer to return ownership of the buffers
     */
    virtual void cancelTransfers() = 0;

    /**
     * Complete a transfer
     *
     * Return when the next read / write request completes.  The returned
     * token identifies the transfer completed to the caller
     *
     * @param status Receives the transfer status
     * @param used Receives the number of bytes transferred
     * @return Token identifying the completed transfer
     */
    virtual int completeTransfer(Transfer_Status* status, size_t* used) = 0;

protected:
    USBDevice();

};

#endif // USB_DEVICE_H

// End of file usb_device.h
