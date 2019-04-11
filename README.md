
# CoreSight Wire Protocol

The CoreSight Wire Protocol repository provides example implementations of the software components required to enable debug and trace over functional interfaces for a CoreSight [SoC-600](https://developer.arm.com/ip-products/system-ip/coresight-debug-and-trace/coresight-components/coresight-soc-600) target. Including:
 * Reference CoreSight Wire Protocol (CSWP) handlers.
 * Example debug and trace interface implementations.
   * RDDI MEM-AP library - debug interface to the debugger.
   * RDDI Streaming Trace library - trace interface to the debugger.
 * On-target debug agent (CSWP server) example.

The repository is structured as follows:

* cswp: Client and server libraries for the CSWP protocol
  * client: Client libraries
    These libraries implement the client interface for CSWP.
  * doc: CSWP documentation
  * usb_transport: USB client transport
    These libraries implement a client transport for CSWP over USB.
  * server: Server libraries
    These libraries implement the server interface for CSWP.

* ds: Configuration to add the RDDI MEM-AP and RDDI Streaming Trace libraries as a debug and trace connection type to Arm Development Studio.

* rddi: RDDI header files

* rddi-memap_cswp: RDDI MEM-AP library
  Implements the RDDI MEM-AP to interface with the debugger and uses CSWP client libraries to communicate with the CSWP server.
  * py_test: Simple python RDDI MEM-AP integration app.

* rddi_streaming_trace: RDDI Streaming Trace library
  Implements the RDDI Streaming Trace to interface with the debugger and uses USB commands to communicate with the Linux kernel ETR streaming trace drivers.
  * streaming_trace_capture: An example application that uses the RDDI Streaming Trace interface to capture and store trace data.

* target
  * cswp_server: Linux based example server for the CSWP protocol.
  Uses USB FunctionFS to send and receive data and the cswp/server library is used to process the received commands. Implements access to the kernel CoreSight devices and system memory.

* drivers: Linux and Windows drivers for the example USB interfaces.

* usb_client: USB abstraction library with Windows and Linux implementations.
  This provides tools such as rddi_streaming_trace and rddi-memap_cswp with a platform neutral interface to USB devices.


## Build

A 64-bit Windows or Linux platform is required to build these tools.

### Linux

#### Requirements

* g++
* libusb-1.0
* [Boost 1.64](https://www.boost.org/users/history/)
* [cmake 3.8 or later](https://cmake.org/download/)

The example CSWP server is also built on Linux which requires an aarch64-linux-gnu cross compiler (For example, [gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads)). Must be on the PATH.

#### Building

Boost 1.64 is required. Set the BOOST_ROOT environment variable to point to Boost installation location. If a non-standard Boost directory structure is used, then set BOOST_INCLUDEDIR and BOOST_LIBRARYDIR to point to the *include* and *library* directories respectively.

Open a command prompt in this directory. PATH must contain cmake and g++. Run:
`./build.sh [dbg|rel]`

The script uses *cmake* to create *Makefiles*, then run *make* to build the binaries. The binaries and necessary files to run the tools will be copied to:
`build/linux-x86_64/dbg/dist/`

### Windows

#### Requirements

* Visual Studio 2015
* [Windows Driver Kit 8.1](https://www.microsoft.com/en-gb/download/details.aspx?id=42273)
* [Boost 1.64](https://www.boost.org/users/history/) ([prebuilt windows](https://sourceforge.net/projects/boost/files/boost-binaries/) binaries are available)
* [cmake 3.8 or later](https://cmake.org/download/)

#### Building

Boost 1.64 is required. Set the BOOST_ROOT environment variable to point to Boost installation location. If a non-standard Boost directory structure is used, then set BOOST_INCLUDEDIR and BOOST_LIBRARYDIR to point to the *include* and *library* directories respectively.

Open a command prompt (e.g. Visual studio 2015 command prompt) in this directory. PATH must contain cmake and visual studio. Run:
`build.bat Debug`

This generates Visual Studio projects. Open in Visual Studio and build, the build the INSTALL target. This copies the binaries and necessary files to run the tools to:
`build/win_32-x86_64/dbg/dist/`

## CSWP server example

An example script that sets up a Linux target as USB gadgets that provide trace data and CSWP control can be found in target/gadget_setup. This creates the USB functions used in the example CSWP server and starts the CSWP server.

Once the server executable and script are copied over to the target root file system, it can be started with:
`/gadget_setup`

and cleaned up with:
`/gadget_setup stop`

To enable the optional `cswp_get_system_description()` call (target hosted SDF), also copy the target/sdf to the target root file system.

### Linux host drivers

* Copy driver setup file *drivers/AMIS_FPGA.rules* to */etc/udev/rules.d* (this requires root permissions)
* Power on the system and connect as a USB device to the host.
* Run /gadget_setup on the system (if not configured to do this on boot).
* Run *lsusb* to verify the device is present.

### Windows host drivers

* Power on the system and connect as a USB device to the host.
* Run /gadget_setup on the system (if not configured to do this on boot).
* Open Device Manager and find the AMIS PoC devices - there should be 2 (trace and control) interfaces.  Use the "Update driver" option to install the drivers from the *drivers/windows/* directory.


## RDDI Streaming Trace

### ETR Streaming Trace drivers

The Linux kernel ETR streaming trace drivers used for the RDDI Streaming Trace implementation are based on Linux-4.9. They are yet to be forward ported and up-streamed to the main Linux source.

A non-supported patch set for Linux-4.9 may be provided on [request](mailto:support-software@arm.com).

### Streaming trace capture

streaming_trace_capture is an example application that uses the rddi_streaming_trace interface to capture and save trace on the host.

On the host, go to the directory containing the tool binaries and run:
`./streaming_trace_capture amis_fpga.xml /path/to/trace.bin [max_capture_size]`

This starts the USB trace capture and write data to /path/to/trace.bin. The optional max_capture_size stops capture after the desired number of bytes captured. Stop capture with Ctrl-C.

## Arm Development Studio integration

Arm Development Studio 2019.0 added support for debug and trace target connections using functional interfaces. A new debug and trace connection type can be added that uses the functional interface implemented in the libraries. The RDDI MEM-AP and RDDI Streaming Trace libraries should be shipped alongside a probe definition file (example at ds/probes.xml) and necessary config files to users of Arm Development Studio.

Arm Development Studio User Guide: [Add a debug connection using functional interfaces](https://developer.arm.com/docs/101470/latest/platform-configuration/debug-hardware-configuration/add-a-debug-connection-using-functional-interfaces.html)


## Porting software to another system

### Host components

#### Debug

Both rddi-memap_cswp and cswp/client are generic.
cswp/usb_transport is the layer in charge of the USB communication on the host side. It must be rewritten when another transport layer is used.

#### Trace

rddi_streaming_trace/common is generic code.
rddi_streaming_trace/rddi_streaming_trace_amis contains code handling:
* USB connection: it must be rewritten if another transport layer is used.
 * Linux-specific ETR driver USB commands: It must be rewritten for another system.

### Target components

The target-side components in these examples are all Linux-based applications and drivers, running on a dedicated core. It is expected that a bare-metal solution is better suited for targets with limited resources.

#### Debug

CSWP server contains 2 layers:
* target agnostic code in cswp/server. This code deals with CSWP protocol. It uses function pointers to get information from the target-specific layer. It can be reused as it is.
* target specific code in target/cswp_server. This code uses Linux-specific mechanism to access SoC components. It must be rewritten for another system.

#### Trace

Trace is handled by a Linux-specific ETR driver, so it must be rewritten for another system.

## Licence

Arm CoreSight Wire Protocol is provided under The University of Illinois/NCSA Open Source License. See LICENCE.txt for more information.

## Contact

For enquiries contact: [support-software@arm.com](mailto:support-software@arm.com).
