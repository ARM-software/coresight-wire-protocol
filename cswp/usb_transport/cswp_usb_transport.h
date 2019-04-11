// cswp_usb_transport.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/**
 * @file cswp_usb_transport.h
 * @brief CSWP USB client
 */

#ifndef CSWP_USB_CLIENT_H
#define CSWP_USB_CLIENT_H

void cswp_client_usb_transport_init(cswp_client_transport_t* transport,
                                    const char* serialNumber);

#endif // CSWP_USB_CLIENT_H
