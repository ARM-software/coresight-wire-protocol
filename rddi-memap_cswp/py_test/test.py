import sys

from ctypes import *

# RDDI MEM-AP typedefs
MEM_AP_ACC_8 = c_int(0)
MEM_AP_ACC_16 = c_int(1)
MEM_AP_ACC_32 = c_int(2)

class MEM_AP_OP_DATA(Union):
    _fields_ = [("data", c_uint32),
                ("pData", POINTER(c_uint32))]

class MEM_AP_OP(Structure):
    _fields_ = [("op", c_uint8),
                ("indirect", c_uint8),
                ("addr", c_uint32),
                ("accSize", c_int),
                ("flags", c_uint),
                ("count", c_uint),
                ("data", MEM_AP_OP_DATA)]

MEM_AP_BATCH_OP_WRITE = c_uint8(0)

# type conversion methods
def to_u8(val):
    return val < 0 and val + 256 or val

def to_s8(val):
    return val > 127 and val - 256 or val

def to_u32(buf):
    return (to_u8(buf[3]) << 24) | (to_u8(buf[2]) << 16) | (to_u8(buf[1]) << 8) | to_u8(buf[0])

# test methods
def haltV8Core(ap, ctiBaseAddress):
    ops = (MEM_AP_OP * 5)()

    # CTICONTROL - Enable CTI mapping
    ops[0] = MEM_AP_OP(MEM_AP_BATCH_OP_WRITE, 0, 0x0, MEM_AP_ACC_32, 0x0, 4, MEM_AP_OP_DATA(0x1))
    # CTIGATE - Prevent channel events from passing from internal channels 0 and 1 to the CTM
    ops[1] = MEM_AP_OP(MEM_AP_BATCH_OP_WRITE, 0, 0x140, MEM_AP_ACC_32, 0x0, 4, MEM_AP_OP_DATA(0x0))
    # CTIOUTEN0 - Event on channel 0 will cause output trigger 0 to be asserted
    ops[2] = MEM_AP_OP(MEM_AP_BATCH_OP_WRITE, 0, 0xA0, MEM_AP_ACC_32, 0x0, 4, MEM_AP_OP_DATA(0x1))
    # CTIOUTEN1 - Never assert output trigger 1
    ops[3] = MEM_AP_OP(MEM_AP_BATCH_OP_WRITE, 0, 0xA4, MEM_AP_ACC_32, 0x0, 4, MEM_AP_OP_DATA(0x0))
    # CTIAPPPULSE - Generate event on channel 0
    ops[4] = MEM_AP_OP(MEM_AP_BATCH_OP_WRITE, 0, 0x1C, MEM_AP_ACC_32, 0x0, 4,  MEM_AP_OP_DATA(0x1))

    opsCompleted = c_int(0)
    ret = hdll.MEM_AP_AccessBatch(rddiHHandle, c_int(ap), c_uint64(ctiBaseAddress), pointer(ops), c_int(5), byref(opsCompleted))
    if ret != 0:
        raise RuntimeError("MEM_AP_AccessBatch error: %d" % ret)

def restartV8Core(ap, ctiBaseAddress):
    ops = (MEM_AP_OP * 5)()

    # CTIINEN1 - Input trigger 1 to all output channels are disabled
    ops[0] = MEM_AP_OP(MEM_AP_BATCH_OP_WRITE, 0, 0x24, MEM_AP_ACC_32, 0x0, 4, MEM_AP_OP_DATA(0x0))
    # CTIGATE - Prevent channel events from passing from internal channels 0 and 1 to the CTM
    ops[1] = MEM_AP_OP(MEM_AP_BATCH_OP_WRITE, 0, 0x140, MEM_AP_ACC_32, 0x0, 4, MEM_AP_OP_DATA(0x0))
    # CTIOUTEN0 - Never assert output trigger 0
    ops[2] = MEM_AP_OP(MEM_AP_BATCH_OP_WRITE, 0, 0xA0, MEM_AP_ACC_32, 0x0, 4, MEM_AP_OP_DATA(0x0))
    # CTIOUTEN1 - Event on channel 1 will cause output trigger 1 to be asserted
    ops[3] = MEM_AP_OP(MEM_AP_BATCH_OP_WRITE, 0, 0xA4, MEM_AP_ACC_32, 0x0, 4, MEM_AP_OP_DATA(0x2))
    # CTIAPPPULSE - Generate event on channel 1
    ops[4] = MEM_AP_OP(MEM_AP_BATCH_OP_WRITE, 0, 0x1C, MEM_AP_ACC_32, 0x0, 4, MEM_AP_OP_DATA(0x2))

    opsCompleted = c_int(0)
    ret = hdll.MEM_AP_AccessBatch(rddiHHandle, c_int(ap), c_uint64(ctiBaseAddress), pointer(ops), c_int(5), byref(opsCompleted))
    if ret != 0:
        raise RuntimeError("MEM_AP_AccessBatch error: %d" % ret)

def readMIDRThroughITR(ap, coreBaseAddress):
    instructions = [0xD4A00003,  # DCPS3
                    0xD5380000,  # MRS      x0,MIDR_EL1
                    0xD5130500]  # MSR      DBGDTRTX_EL0,x0
    buf = (c_byte * 4)()
    # Write instructions to ITR
    for i in range(3):
        buf[0] = to_s8(instructions[i] & 0xFF)
        buf[1] = to_s8((instructions[i] >> 8) & 0xFF)
        buf[2] = to_s8((instructions[i] >> 16) & 0xFF)
        buf[3] = to_s8((instructions[i] >> 24) & 0xFF)
        ret = hdll.MEM_AP_Write(rddiHHandle, c_int(ap), c_uint64(coreBaseAddress | 0x84), MEM_AP_ACC_32, c_int(0x0), c_int(4), buf)
        if ret != 0:
            raise RuntimeError("MEM_AP_Write error: %d" % ret)

    # Read DTR register
    ret = hdll.MEM_AP_Read(rddiHHandle, c_int(ap), c_uint64(coreBaseAddress | 0x8C),  MEM_AP_ACC_32, c_int(0x0), c_int(4), buf)
    if ret != 0:
        raise RuntimeError("MEM_AP_Read error: %d" % ret)
    return to_u32(buf)

def clearOSLock(ap, coreBaseAddress):
    buf = (c_uint8 * 4)()
    hdll.MEM_AP_Write(rddiHHandle, c_int(ap), c_int(coreBaseAddress | 0x300), MEM_AP_ACC_32, c_int(0x0), c_int(4), buf)
    if ret != 0:
        raise RuntimeError("MEM_AP_Write error: %d" % ret)
    return

def getEDPRSR(ap, coreBaseAddress):
    buf = (c_byte * 4)()
    ret = hdll.MEM_AP_Read(rddiHHandle, ap, coreBaseAddress | 0x314, MEM_AP_ACC_32, 0x0, 4, byref(buf))
    if ret != 0:
        raise RuntimeError("MEM_AP_Read error: %d" % ret)
    return to_u32(buf)

def readMIDR(ap, coreBaseAddress):
    buf = (c_byte * 4)()
    ret = hdll.MEM_AP_Read(rddiHHandle, ap, coreBaseAddress | 0xD00, MEM_AP_ACC_32, 0x0, 4, byref(buf))
    if ret != 0:
        raise RuntimeError("MEM_AP_Read error: %d" % ret)
    return to_u32(buf)

def readMemory(ap, startAddress, length):
    buf = (c_byte * length)()
    ret = hdll.MEM_AP_Read(rddiHHandle, ap, startAddress, MEM_AP_ACC_32, 0x0, length, byref(buf))
    if ret != 0:
        raise RuntimeError("MEM_AP_Read error: %d" % ret)
    return buf

#main
lib = sys.argv[1]
configFile = sys.argv[2]

hdll = cdll.LoadLibrary(lib)
rddiPHandle = c_int(0)
rddiHHandle = c_int(0)

ret = hdll.RDDI_Open(byref(rddiPHandle), c_int(0))
if ret != 0:
    raise RuntimeError("RDDI_Open error: %d" % ret)

rddiHHandle = rddiPHandle.value

ret = hdll.ConfigInfo_OpenFile(rddiHHandle, configFile)
if ret != 0:
    raise RuntimeError("ConfigInfo_OpenFile error: %d" % ret)

connString = create_string_buffer("test_connection")
clientInfoStr = create_string_buffer(128)
ret = hdll.MEM_AP_Connect(rddiHHandle, connString, clientInfoStr, 128)
if ret != 0:
    raise RuntimeError("MEM_AP_Connect error: %d" % ret)

ret = hdll.MEM_AP_Open(rddiHHandle, c_int(0))
if ret != 0:
    raise RuntimeError("MEM_AP_Open error: %d" % ret)

# AP IDR read
idr = c_int(0)
ret = hdll.MEM_AP_ReadReg(rddiHHandle, c_int(0), c_int(0xDFC), byref(idr))
if ret != 0:
    raise RuntimeError("MEM_AP_ReadReg error: %d" % ret)
print "AP0-IDR=", hex(idr.value)

clearOSLock(0, 0x20510000)

# V8 core debug
haltV8Core(0, 0x20520000)

edprsr = getEDPRSR(0, 0x20510000)
if edprsr & 0x10:
    print "Core halted"
else:
    print "Error when halting core"

midrValue = readMIDRThroughITR(0, 0x20510000)
midrValue2 = readMIDR(0, 0x20510000)

# Compare ITR read value with direct
if midrValue == midrValue2:
    print "MIDR value is ", hex(midrValue)
else:
    print "Error: MIDR values do not match!"

# Read multiple core debug registers
# EDPFR[31:0], EDPFR[63:32], EDDFR[31:0], EDDFR[63:32]
data = readMemory(0, 0x20510D20, 16)

# Compare with individual reads
edpfr = readMemory(0, 0x20510D20, 4)
if to_u32(edpfr) == to_u32([data[0], data[1], data[2], data[3]]):
    print "EDPFR value is ", hex(to_u32(edpfr))
else:
    print "Error: EDPFR values do not match!"

eddfr = readMemory(0, 0x20510D28, 4)
if to_u32(eddfr) == to_u32([data[8], data[9], data[10], data[11]]):
    print "EDDFR value is ", hex(to_u32(eddfr))
else:
    print "Error: EDDFR values do not match!"

restartV8Core(0, 0x20520000)

edprsr = getEDPRSR(0, 0x20510000)
if edprsr & 0x800:
    print "Core running"
else:
    print "Error when restarting the core"

hdll.MEM_AP_Close(rddiHHandle, c_int(0))
if ret != 0:
    raise RuntimeError("MEM_AP_Close error: %d" % ret)

hdll.MEM_AP_Disconnect(rddiHHandle)

hdll.RDDI_Close()