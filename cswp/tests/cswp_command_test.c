// cswp_command_test.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "cswp_server_commands.h"
#include "cswp_client_commands.h"
#include "cswp_test.h"
#include <string.h>

static void test_headers()
{
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    varint_t msgType, errCode;

    cswp_encode_command_header(buf, 0x66);
    CHECK_EQUAL(1, buf->pos);
    CHECK_EQUAL(1, buf->used);
    CHECK_CONTENTS("\x66", buf->buf, buf->used);

    /* cswp_buffer_clear(buf); */
    cswp_buffer_set(buf, "\x43", 1);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(1, buf->pos);
    CHECK_EQUAL(1, buf->used);
    CHECK_EQUAL(0x43, msgType);

    cswp_buffer_clear(buf);
    cswp_encode_response_header(buf, 0x66, 0x99);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(3, buf->used);
    CHECK_CONTENTS("\x66\x99\x01", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x43\x01", 2);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(2, buf->pos);
    CHECK_EQUAL(2, buf->used);
    CHECK_EQUAL(0x43, msgType);
    CHECK_EQUAL(0x01, errCode);

    cswp_buffer_free(buf);
}

static void test_error_response()
{
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    varint_t msgType, errCode;
    char msg[1024];

    cswp_buffer_clear(buf);
    cswp_encode_error_response(buf, 0x66, 0x33, "Ooops!");
    CHECK_EQUAL(9, buf->pos);
    CHECK_EQUAL(9, buf->used);
    CHECK_CONTENTS("\x66\x33\x06Ooops!", buf->buf, buf->used);

    cswp_buffer_clear(buf);
    cswp_buffer_set(buf, "\x43\x22\x15Something went wrong.", 24);
    cswp_decode_response_header(buf, &msgType, &errCode);
    cswp_decode_error_response_body(buf, msg, sizeof(msg));
    CHECK_EQUAL(24, buf->pos);
    CHECK_EQUAL(24, buf->used);
    CHECK_EQUAL(0, strcmp("Something went wrong.", msg));

    cswp_buffer_free(buf);
}

static void test_cmd_init()
{
    varint_t msgType, errCode;
    varint_t protoVer, svrVer;
    char ID[256];
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_init_command(buf, 7, "Test client");
    CHECK_EQUAL(14, buf->pos);
    CHECK_EQUAL(14, buf->used);
    CHECK_CONTENTS("\x01\x07\x0BTest client", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x01\x06\x08""A client", 11);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_INIT, msgType);
    CHECK_EQUAL(1, buf->pos);
    cswp_decode_init_command_body(buf, &protoVer, ID, sizeof(ID));
    CHECK_EQUAL(11, buf->pos);
    CHECK_EQUAL(6, protoVer);
    CHECK_EQUAL(0, strcmp("A client", ID));

    /* response */
    cswp_buffer_clear(buf);
    cswp_encode_init_response(buf, 8, "A server", 0x123);
    CHECK_EQUAL(14, buf->pos);
    CHECK_EQUAL(14, buf->used);
    CHECK_CONTENTS("\x01\x00\x08\x08""A server""\xA3\x2", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x01\x00\x03\x03""SVR""\x1", 9);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_INIT, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_init_response_body(buf, &protoVer, ID, sizeof(ID), &svrVer);
    CHECK_EQUAL(3, protoVer);
    CHECK_EQUAL(0, strcmp("SVR", ID));
    CHECK_EQUAL(1, svrVer);

    cswp_buffer_free(buf);
}

static void test_cmd_term()
{
    varint_t msgType, errCode;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_term_command(buf);
    CHECK_EQUAL(1, buf->pos);
    CHECK_EQUAL(1, buf->used);
    CHECK_CONTENTS("\x02", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x02", 1);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_TERM, msgType);
    CHECK_EQUAL(1, buf->pos);

    /* response */
    cswp_buffer_clear(buf);
    cswp_encode_term_response(buf);
    CHECK_EQUAL(2, buf->pos);
    CHECK_EQUAL(2, buf->used);
    CHECK_CONTENTS("\x02\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x02\x00", 9);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_TERM, msgType);
    CHECK_EQUAL(0x00, errCode);

    cswp_buffer_free(buf);
}

static void test_cmd_client_info()
{
    varint_t msgType, errCode;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    char msg[256];

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_client_info_command(buf, "A message from the client");
    CHECK_EQUAL(27, buf->pos);
    CHECK_EQUAL(27, buf->used);
    CHECK_CONTENTS("\x05\x19""A message from the client", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x05\x0B""Client Info", 13);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_CLIENT_INFO, msgType);
    cswp_decode_client_info_command_body(buf, msg, sizeof(msg));
    CHECK_EQUAL(0, strcmp("Client Info", msg));
    CHECK_EQUAL(13, buf->pos);

    /* response */
    cswp_buffer_clear(buf);
    cswp_encode_client_info_response(buf);
    CHECK_EQUAL(2, buf->pos);
    CHECK_EQUAL(2, buf->used);
    CHECK_CONTENTS("\x05\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x05\x00", 9);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_CLIENT_INFO, msgType);
    CHECK_EQUAL(0x00, errCode);

    cswp_buffer_free(buf);
}

static void test_cmd_set_devices()
{
    varint_t msgType, errCode;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    const char* devices[] = {
        "Device 1",
        "Device 2",
        "Device 3 / foo",
    };
    const char* types[] = {
      "Type 1",
      "Type 2",
      "Type 3",
    };
    varint_t deviceCount;
    char devName[256];

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_set_devices_command(buf, 3, devices, types);
    CHECK_EQUAL(56, buf->pos);
    CHECK_EQUAL(56, buf->used);
    CHECK_CONTENTS("\x10\x03" "\x08""Device 1" "\x06""Type 1" "\x08""Device 2" "\x06""Type 2" "\x0E""Device 3 / foo" "\x06""Type 3", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x10\x02" "\x05""Dev 1" "\x05""Dev 2", 14);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_SET_DEVICES, msgType);
    cswp_decode_set_devices_command_body(buf, &deviceCount);
    CHECK_EQUAL(2, deviceCount);
    cswp_buffer_get_string(buf, devName, sizeof(devName));
    CHECK_EQUAL(0, strcmp("Dev 1", devName));
    cswp_buffer_get_string(buf, devName, sizeof(devName));
    CHECK_EQUAL(0, strcmp("Dev 2", devName));
    CHECK_EQUAL(14, buf->pos);

    /* response */
    cswp_buffer_clear(buf);
    cswp_encode_set_devices_response(buf);
    CHECK_EQUAL(2, buf->pos);
    CHECK_EQUAL(2, buf->used);
    CHECK_CONTENTS("\x10\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x10\x00", 9);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_SET_DEVICES, msgType);
    CHECK_EQUAL(0x00, errCode);

    cswp_buffer_free(buf);
}

static void test_cmd_get_devices()
{
    varint_t msgType, errCode;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    const char* devices[] = {
        "Device 1",
        "Device 2",
        "Device 3 / foo",
    };
    const char* types[] = {
        "Type 1",
        "Type2", // Try with different size
        "Type 3",
    };
    varint_t deviceCount;
    char devName[256];

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_get_devices_command(buf);
    CHECK_EQUAL(1, buf->pos);
    CHECK_EQUAL(1, buf->used);
    CHECK_CONTENTS("\x11", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x11", 11);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_GET_DEVICES, msgType);
    CHECK_EQUAL(1, buf->pos);

    /* response */
    cswp_buffer_clear(buf);
    cswp_encode_get_devices_response(buf, 3, devices, types);
    CHECK_EQUAL(56, buf->pos);
    CHECK_EQUAL(56, buf->used);
    CHECK_CONTENTS("\x11\x00\x03" "\x08""Device 1" "\x06""Type 1" "\x08""Device 2" "\x05""Type2" "\x0E""Device 3 / foo" "\x06""Type 3", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x11\x00\x03" "\x08""Device 1" "\x08""Device 2" "\x0E""Device 3 / foo", 36);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_GET_DEVICES, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_get_devices_response_body(buf, &deviceCount);
    CHECK_EQUAL(3, deviceCount);
    cswp_buffer_get_string(buf, devName, sizeof(devName));
    CHECK_EQUAL(0, strcmp("Device 1", devName));
    cswp_buffer_get_string(buf, devName, sizeof(devName));
    CHECK_EQUAL(0, strcmp("Device 2", devName));
    cswp_buffer_get_string(buf, devName, sizeof(devName));
    CHECK_EQUAL(0, strcmp("Device 3 / foo", devName));
    CHECK_EQUAL(36, buf->pos);

    cswp_buffer_free(buf);
}

static void test_cmd_get_system_description()
{
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    varint_t format, size, msgType, errCode;
    char description[] = {"This is the SDF file."};
    uint8_t data[21];

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_get_system_description_command(buf);
    CHECK_EQUAL(1, buf->pos);
    CHECK_EQUAL(1, buf->used);
    CHECK_CONTENTS("\x12", buf->buf, buf->used);

    cswp_buffer_clear(buf);
    cswp_buffer_set(buf, "\x12", 1);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_GET_SYSTEM_DESCRIPTION, msgType);
    CHECK_EQUAL(1, buf->pos);

    /* response */
    cswp_buffer_clear(buf);
    cswp_encode_get_system_description_response(buf, 0, 21, (uint8_t*)(description));
    CHECK_EQUAL(25, buf->pos);
    CHECK_EQUAL(25, buf->used);
    CHECK_CONTENTS("\x12\x00\x00\x15" "This is the SDF file.", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x12\x00\x00\x15" "This is the SDF file.", 25);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_GET_SYSTEM_DESCRIPTION, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_get_system_description_response_body(buf, &format, &size, data, 21);
    CHECK_EQUAL(0, format);
    CHECK_EQUAL(21, size);
    CHECK_EQUAL(0, memcmp((uint8_t*)description, (uint8_t*)data, 21));

    cswp_buffer_free(buf);
}

static void test_cmd_dev_open()
{
    varint_t msgType, errCode;
    varint_t deviceNo;
    char info[256];
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_device_open_command(buf, 1);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(3, buf->used);
    CHECK_CONTENTS("\x80\x02\x01", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x80\x02\x08", 3);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_DEVICE_OPEN, msgType);
    CHECK_EQUAL(2, buf->pos);
    cswp_decode_device_open_command_body(buf, &deviceNo);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(8, deviceNo);

    /* response */
    cswp_buffer_clear(buf);
    cswp_encode_device_open_response(buf, "Device info");
    CHECK_EQUAL(15, buf->pos);
    CHECK_EQUAL(15, buf->used);
    CHECK_CONTENTS("\x80\x02\x00\x0B""Device info", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x80\x02\x00\x1A""Information about a device", 30);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_DEVICE_OPEN, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_device_open_response_body(buf, info, sizeof(info));
    CHECK_EQUAL(0, strcmp("Information about a device", info));
    CHECK_EQUAL(30, buf->pos);
    CHECK_EQUAL(30, buf->used);

    cswp_buffer_free(buf);
}

static void test_cmd_dev_close()
{
    varint_t msgType, errCode;
    varint_t deviceNo;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_device_close_command(buf, 3);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(3, buf->used);
    CHECK_CONTENTS("\x81\x02\x03", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x81\x02\x04", 3);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_DEVICE_CLOSE, msgType);
    CHECK_EQUAL(2, buf->pos);
    cswp_decode_device_close_command_body(buf, &deviceNo);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(4, deviceNo);

    /* response */
    cswp_buffer_clear(buf);
    cswp_encode_device_close_response(buf);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(3, buf->used);
    CHECK_CONTENTS("\x81\x02\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x81\x02\x00", 3);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_DEVICE_CLOSE, msgType);
    CHECK_EQUAL(0x00, errCode);

    cswp_buffer_free(buf);
}


static void test_cmd_set_config()
{
    varint_t msgType, errCode;
    varint_t deviceNo;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    char name[256];
    char value[256];

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_set_config_command(buf, 3, "foo", "bar");
    CHECK_EQUAL(11, buf->pos);
    CHECK_EQUAL(11, buf->used);
    CHECK_CONTENTS("\x82\x02\x03\x03""foo\x03""bar", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x82\x02\x04\x01""a\x05""hello", 11);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_SET_CONFIG, msgType);
    CHECK_EQUAL(2, buf->pos);
    cswp_decode_set_config_command_body(buf, &deviceNo, name, sizeof(name), value, sizeof(value));
    CHECK_EQUAL(11, buf->pos);
    CHECK_EQUAL(4, deviceNo);
    CHECK_EQUAL(0, strcmp("a", name));
    CHECK_EQUAL(0, strcmp("hello", value));

    /* response */
    cswp_buffer_clear(buf);
    cswp_encode_set_config_response(buf);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(3, buf->used);
    CHECK_CONTENTS("\x82\x02\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x82\x02\x00", 3);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_SET_CONFIG, msgType);
    CHECK_EQUAL(0x00, errCode);

    cswp_buffer_free(buf);
}


static void test_cmd_get_config()
{
    varint_t msgType, errCode;
    varint_t deviceNo;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    char name[256];
    char value[256];

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_get_config_command(buf, 3, "foo");
    CHECK_EQUAL(7, buf->pos);
    CHECK_EQUAL(7, buf->used);
    CHECK_CONTENTS("\x83\x02\x03\x03""foo", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x83\x02\x04\x06""wibble", 10);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_GET_CONFIG, msgType);
    CHECK_EQUAL(2, buf->pos);
    cswp_decode_get_config_command_body(buf, &deviceNo, name, sizeof(name));
    CHECK_EQUAL(10, buf->pos);
    CHECK_EQUAL(4, deviceNo);
    CHECK_EQUAL(0, strcmp("wibble", name));

    /* response */
    cswp_buffer_clear(buf);
    cswp_encode_get_config_response(buf, "abcdef");
    CHECK_EQUAL(10, buf->pos);
    CHECK_EQUAL(10, buf->used);
    CHECK_CONTENTS("\x83\x02\x00\x06""abcdef", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x83\x02\x00\x04""1234", 8);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_GET_CONFIG, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_get_config_response_body(buf, value, sizeof(value));
    CHECK_EQUAL(0, strcmp("1234", value));

    cswp_buffer_free(buf);
}


static void test_cmd_get_device_capabilities()
{
    varint_t msgType, errCode;
    varint_t deviceNo;
    varint_t capabilities, capabilityData;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_get_device_capabilities_command(buf, 6);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(3, buf->used);
    CHECK_CONTENTS("\x84\x02\x06", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x84\x02\x09", 3);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_GET_DEVICE_CAPABILITIES, msgType);
    CHECK_EQUAL(2, buf->pos);
    cswp_decode_get_device_capabilities_command_body(buf, &deviceNo);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(9, deviceNo);

    /* response */
    cswp_buffer_clear(buf);
    cswp_encode_get_device_capabilities_response(buf, 0x9, 0x4);
    CHECK_EQUAL(5, buf->pos);
    CHECK_EQUAL(5, buf->used);
    CHECK_CONTENTS("\x84\x02\x00" "\x09" "\x04", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x84\x02\x00" "\x07" "\x09", 5);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_GET_DEVICE_CAPABILITIES, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_get_device_capabilities_response_body(buf, &capabilities, &capabilityData);
    CHECK_EQUAL(capabilities, 0x7);
    CHECK_EQUAL(capabilityData, 0x9);

    cswp_buffer_free(buf);
}


static void test_cmd_reg_list()
{
    varint_t msgType, errCode;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    varint_t deviceNo, regCount;
    varint_t regID, regSize;
    char name[256], displayName[256], description[256];

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_reg_list_command(buf, 3);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(3, buf->used);
    CHECK_CONTENTS("\x80\x04\x03", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x80\x04\x04", 3);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_REG_LIST, msgType);
    CHECK_EQUAL(2, buf->pos);
    cswp_decode_reg_list_command_body(buf, &deviceNo);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(4, deviceNo);

    /* empty response */
    cswp_buffer_clear(buf);
    cswp_encode_reg_list_response(buf, 0);
    CHECK_EQUAL(4, buf->pos);
    CHECK_EQUAL(4, buf->used);
    CHECK_CONTENTS("\x80\x04\x00\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x80\x04\x00\x00", 4);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_REG_LIST, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_reg_list_response_body(buf, &regCount);
    CHECK_EQUAL(0, regCount);

    /* one register response */
    cswp_buffer_clear(buf);
    cswp_encode_reg_list_response(buf, 1);
    cswp_encode_reg_info(buf, 1234, "REG_1", 1, "Register 1", "First Register");
    CHECK_EQUAL(39, buf->pos);
    CHECK_EQUAL(39, buf->used);
    CHECK_CONTENTS("\x80\x04\x00\x01""\xD2\x09\x05REG_1\x01\x0ARegister 1\x0E""First Register", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x80\x04\x00\x01""\xD3\x09\x05REG_2\x01\x0ARegister 2\x0F""Second Register", 40);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_REG_LIST, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_reg_list_response_body(buf, &regCount);
    CHECK_EQUAL(1, regCount);
    cswp_decode_reg_info(buf, &regID, name, sizeof(name), &regSize,
                         displayName, sizeof(displayName),
                         description, sizeof(description));
    CHECK_EQUAL(40, buf->pos);
    CHECK_EQUAL(1235, regID);
    CHECK_EQUAL(0, strcmp("REG_2", name));
    CHECK_EQUAL(1, regSize);
    CHECK_EQUAL(0, strcmp("Register 2", displayName));
    CHECK_EQUAL(0, strcmp("Second Register", description));

    cswp_buffer_free(buf);
}

static void test_cmd_reg_read()
{
    varint_t msgType, errCode;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    varint_t regIDs[] = { 1234, 0x100 };
    varint_t deviceNo, regCount, regID;
    uint32_t regValues[] = { 1, 0xDEADBEEF, 42 };
    unsigned regVal;

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_reg_read_command(buf, 3, 1, regIDs);
    CHECK_EQUAL(6, buf->pos);
    CHECK_EQUAL(6, buf->used);
    CHECK_CONTENTS("\x81\x04\x03\x01\xD2\x09", buf->buf, buf->used);

    cswp_buffer_clear(buf);
    cswp_encode_reg_read_command(buf, 3, 2, regIDs);
    CHECK_EQUAL(8, buf->pos);
    CHECK_EQUAL(8, buf->used);
    CHECK_CONTENTS("\x81\x04\x03\x02\xD2\x09\x80\x02", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x81\x04\x03\x02\xD2\x09\x80\x02", 8);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_REG_READ, msgType);
    CHECK_EQUAL(2, buf->pos);
    cswp_decode_reg_read_command_body(buf, &deviceNo, &regCount);
    CHECK_EQUAL(4, buf->pos);
    CHECK_EQUAL(3, deviceNo);
    CHECK_EQUAL(2, regCount);
    cswp_buffer_get_varint(buf, &regID);
    CHECK_EQUAL(1234, regID);
    cswp_buffer_get_varint(buf, &regID);
    CHECK_EQUAL(0x100, regID);
    CHECK_EQUAL(8, buf->pos);

    /* empty response */
    cswp_buffer_clear(buf);
    cswp_encode_reg_read_response(buf, 0, 0);
    CHECK_EQUAL(4, buf->pos);
    CHECK_EQUAL(4, buf->used);
    CHECK_CONTENTS("\x81\x04\x00\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x81\x04\x00\x00", 4);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_REG_READ, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_reg_read_response_body(buf, &regCount);
    CHECK_EQUAL(0, regCount);

    /* filled response */
    cswp_buffer_clear(buf);
    cswp_encode_reg_read_response(buf, 3, regValues);
    CHECK_EQUAL(16, buf->pos);
    CHECK_EQUAL(16, buf->used);
    CHECK_CONTENTS("\x81\x04\x00\x03\x01\x00\x00\x00\xEF\xBE\xAD\xDE\x2A\x00\x00\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x81\x04\x00\x04\x05\x00\x00\x00\x0D\xF0\xAD\x0B\xAA\x55\xAA\x55\x00\x00\x00\x00", 20);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_REG_READ, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_reg_read_response_body(buf, &regCount);
    CHECK_EQUAL(4, regCount);
    cswp_buffer_get_uint32(buf, &regVal);
    CHECK_EQUAL(5, regVal);
    cswp_buffer_get_uint32(buf, &regVal);
    CHECK_EQUAL(0x0BADF00D, regVal);
    cswp_buffer_get_uint32(buf, &regVal);
    CHECK_EQUAL(0x55AA55AA, regVal);
    cswp_buffer_get_uint32(buf, &regVal);
    CHECK_EQUAL(0, regVal);

    cswp_buffer_free(buf);
}

static void test_cmd_reg_write()
{
    varint_t msgType, errCode;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    varint_t deviceNo, regCount, regID;
    uint32_t regVal;

    /* command */
    cswp_buffer_clear(buf);
    cswp_encode_reg_write_command(buf, 3, 0);
    CHECK_EQUAL(4, buf->pos);
    CHECK_EQUAL(4, buf->used);
    CHECK_CONTENTS("\x82\x04\x03\x00", buf->buf, buf->used);

    cswp_buffer_clear(buf);
    cswp_encode_reg_write_command(buf, 3, 1);
    cswp_buffer_put_varint(buf, 1234);
    cswp_buffer_put_uint32(buf, 0xDEADBEEF);
    CHECK_EQUAL(10, buf->pos);
    CHECK_EQUAL(10, buf->used);
    CHECK_CONTENTS("\x82\x04\x03\x01\xD2\x09\xEF\xBE\xAD\xDE", buf->buf, buf->used);

    cswp_buffer_clear(buf);
    cswp_encode_reg_write_command(buf, 3, 2);
    cswp_buffer_put_varint(buf, 1234);
    cswp_buffer_put_uint32(buf, 0xDEADBEEF);
    cswp_buffer_put_varint(buf, 1);
    cswp_buffer_put_uint32(buf, 0);
    CHECK_EQUAL(15, buf->pos);
    CHECK_EQUAL(15, buf->used);
    CHECK_CONTENTS("\x82\x04\x03\x02\xD2\x09\xEF\xBE\xAD\xDE\x01\x00\x00\x00\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x82\x04\x03\x02\xD2\x09\xEF\xBE\xAD\xDE\x01\x00\x00\x00\x00", 15);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_REG_WRITE, msgType);
    CHECK_EQUAL(2, buf->pos);
    cswp_decode_reg_write_command_body(buf, &deviceNo, &regCount);
    CHECK_EQUAL(4, buf->pos);
    CHECK_EQUAL(3, deviceNo);
    CHECK_EQUAL(2, regCount);
    cswp_buffer_get_varint(buf, &regID);
    CHECK_EQUAL(1234, regID);
    cswp_buffer_get_uint32(buf, &regVal);
    CHECK_EQUAL(0xDEADBEEF, regVal);
    cswp_buffer_get_varint(buf, &regID);
    CHECK_EQUAL(1, regID);
    cswp_buffer_get_uint32(buf, &regVal);
    CHECK_EQUAL(0, regVal);
    CHECK_EQUAL(15, buf->pos);

    /* empty response */
    cswp_buffer_clear(buf);
    cswp_encode_reg_write_response(buf);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(3, buf->used);
    CHECK_CONTENTS("\x82\x04\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x82\x04\x00", 3);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_REG_WRITE, msgType);
    CHECK_EQUAL(0x00, errCode);

    cswp_buffer_free(buf);
}

static void test_cmd_mem_read()
{
    varint_t msgType, errCode;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    uint64_t address;
    varint_t deviceNo, size, accSize, flags;
    const uint8_t data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    void* pData;

    /* command */

    cswp_buffer_clear(buf);
    cswp_encode_mem_read_command(buf, 3, 0xFFFF000080000000, 0x1000, CSWP_ACCESS_SIZE_DEF, 0x88);
    CHECK_EQUAL(16, buf->pos);
    CHECK_EQUAL(16, buf->used);
    CHECK_CONTENTS("\x80\x06\x03\x00\x00\x00\x80\x00\x00\xFF\xFF\x80\x20\x00\x88\x01", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x80\x06\x03\x00\x10\x00\x80\x00\x00\xFE\xFF\x80\x10\x01\x88\x02", 16);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_MEM_READ, msgType);
    CHECK_EQUAL(2, buf->pos);
    cswp_decode_mem_read_command_body(buf, &deviceNo, &address, &size, &accSize, &flags);
    CHECK_EQUAL(16, buf->pos);
    CHECK_EQUAL(3, deviceNo);
    CHECK_EQUAL(0xFFFE000080001000, address);
    CHECK_EQUAL(0x800, size);
    CHECK_EQUAL(CSWP_ACCESS_SIZE_8, accSize);
    CHECK_EQUAL(0x108, flags);

    /* empty response */
    cswp_buffer_clear(buf);
    cswp_encode_mem_read_response(buf, 0, 0);
    CHECK_EQUAL(4, buf->pos);
    CHECK_EQUAL(4, buf->used);
    CHECK_CONTENTS("\x80\x06\x00\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x80\x06\x00\x00", 4);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_MEM_READ, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_mem_read_response_body(buf, &size);
    CHECK_EQUAL(0, size);

    /* filled response */
    cswp_buffer_clear(buf);
    cswp_encode_mem_read_response(buf, 0x10, data);
    CHECK_EQUAL(20, buf->pos);
    CHECK_EQUAL(20, buf->used);
    CHECK_CONTENTS("\x80\x06\x00\x10\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x80\x06\x00\x10\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90", 20);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_MEM_READ, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_mem_read_response_body(buf, &size);
    CHECK_EQUAL(16, size);
    cswp_buffer_get_direct(buf, &pData, 16);
    CHECK_EQUAL(0, memcmp(pData, "\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90", 16));
    CHECK_EQUAL(20, buf->pos);

    cswp_buffer_free(buf);
}

static void test_cmd_mem_write()
{
    varint_t msgType, errCode;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    uint64_t address;
    varint_t deviceNo, size, accSize, flags;
    const uint8_t data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    void* pData;

    /* command */

    cswp_buffer_clear(buf);
    cswp_encode_mem_write_command(buf, 3, 0xFFFF000080000000, 16, CSWP_ACCESS_SIZE_DEF, 0x88, data);
    CHECK_EQUAL(31, buf->pos);
    CHECK_EQUAL(31, buf->used);
    CHECK_CONTENTS("\x81\x06\x03\x00\x00\x00\x80\x00\x00\xFF\xFF\x10\x00\x88\x01\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x81\x06\x03\x00\x00\x00\x80\x00\x00\xFF\xFF\x10\x00\x88\x01\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90", 31);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_MEM_WRITE, msgType);
    CHECK_EQUAL(2, buf->pos);
    cswp_decode_mem_write_command_body(buf, &deviceNo, &address, &size, &accSize, &flags);
    CHECK_EQUAL(15, buf->pos);
    CHECK_EQUAL(3, deviceNo);
    CHECK_EQUAL(0xFFFF000080000000, address);
    CHECK_EQUAL(16, size);
    CHECK_EQUAL(CSWP_ACCESS_SIZE_DEF, accSize);
    CHECK_EQUAL(0x88, flags);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_direct(buf, &pData, 16));
    CHECK_EQUAL(0, memcmp(pData, "\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90", 16));
    CHECK_EQUAL(31, buf->pos);

    /* empty response */
    cswp_buffer_clear(buf);
    cswp_encode_mem_write_response(buf);
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(3, buf->used);
    CHECK_CONTENTS("\x81\x06\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x81\x06\x00", 3);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_MEM_WRITE, msgType);
    CHECK_EQUAL(0x00, errCode);

    cswp_buffer_free(buf);
}

static void test_cmd_mem_poll()
{
    varint_t msgType, errCode;
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    uint64_t address;
    varint_t deviceNo, size, accSize, flags, tries, interval;
    const uint8_t data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    const uint8_t mask[] = { 0xFF, 0x7F, 0x3E, 0x1C };
    const uint8_t value[] = { 0x12, 0x34, 0x56, 0x78 };
    void* maskIn;
    void* valueIn;
    void* pData;

    /* command */

    cswp_buffer_clear(buf);
    cswp_encode_mem_poll_command(buf, 3, 0xFFFF000080000000, 0x4, CSWP_ACCESS_SIZE_DEF, 0x88, 37, 100, mask, value);
    CHECK_EQUAL(25, buf->pos);
    CHECK_EQUAL(25, buf->used);
    CHECK_CONTENTS("\x82\x06\x03\x00\x00\x00\x80\x00\x00\xFF\xFF\x04\x00\x88\x01\x25\x64\xFF\x7F\x3E\x1C\x12\x34\x56\x78", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x82\x06\x03\x00\x10\x00\x80\x00\x00\xFE\xFF\x04\x01\x88\x02\x0A\x0B\xAA\x55\xAA\x55\x81\x82\x83\x84", 25);
    cswp_decode_command_header(buf, &msgType);
    CHECK_EQUAL(CSWP_MEM_POLL, msgType);
    CHECK_EQUAL(2, buf->pos);
    cswp_decode_mem_poll_command_body(buf, &deviceNo, &address, &size, &accSize, &flags, &tries, &interval);
    CHECK_EQUAL(17, buf->pos);
    CHECK_EQUAL(3, deviceNo);
    CHECK_EQUAL(0xFFFE000080001000, address);
    CHECK_EQUAL(4, size);
    CHECK_EQUAL(CSWP_ACCESS_SIZE_8, accSize);
    CHECK_EQUAL(0x108, flags);
    CHECK_EQUAL(10, tries);
    CHECK_EQUAL(11, interval);
    /* Now read mask & value */
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_direct(buf, &maskIn, 4));
    CHECK_EQUAL(0, memcmp(maskIn, "\xAA\x55\xAA\x55", 4));
    CHECK_EQUAL(21, buf->pos);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_direct(buf, &valueIn, 4));
    CHECK_EQUAL(0, memcmp(valueIn, "\x81\x82\x83\x84", 4));
    CHECK_EQUAL(25, buf->pos);

    /* empty response */
    cswp_buffer_clear(buf);
    cswp_encode_mem_poll_response(buf, 0, 0);
    CHECK_EQUAL(4, buf->pos);
    CHECK_EQUAL(4, buf->used);
    CHECK_CONTENTS("\x82\x06\x00\x00", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x82\x06\x00\x00", 4);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_MEM_POLL, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_mem_poll_response_body(buf, &size);
    CHECK_EQUAL(0, size);

    /* filled response */
    cswp_buffer_clear(buf);
    cswp_encode_mem_poll_response(buf, 0x10, data);
    CHECK_EQUAL(20, buf->pos);
    CHECK_EQUAL(20, buf->used);
    CHECK_CONTENTS("\x82\x06\x00\x10\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x82\x06\x00\x10\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90", 20);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_MEM_POLL, msgType);
    CHECK_EQUAL(0x00, errCode);
    cswp_decode_mem_poll_response_body(buf, &size);
    CHECK_EQUAL(16, size);
    cswp_buffer_get_direct(buf, &pData, 16);
    CHECK_EQUAL(0, memcmp(pData, "\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F\x90", 16));
    CHECK_EQUAL(20, buf->pos);

    cswp_buffer_free(buf);
}

static void test_async_message()
{
    varint_t msgType, errCode;
    varint_t deviceNo, level;
    char msg[256];
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);

    /* response */
    cswp_encode_async_message(buf, 123, 6, 3, "Oh dear. Something happened");
    CHECK_EQUAL(33, buf->pos);
    CHECK_EQUAL(33, buf->used);
    CHECK_CONTENTS("\x80\x20\x7B\x06\x03\x1B""Oh dear. Something happened", buf->buf, buf->used);

    cswp_buffer_set(buf, "\x80\x20\x7B\x06\x03\x1B""Oh dear. Something happened", 33);
    cswp_decode_response_header(buf, &msgType, &errCode);
    CHECK_EQUAL(CSWP_ASYNC_MESSAGE, msgType);
    CHECK_EQUAL(123, errCode);

    cswp_decode_async_message_body(buf, &deviceNo, &level, msg, sizeof(msg));
    CHECK_EQUAL(6, deviceNo);
    CHECK_EQUAL(3, level);
    CHECK_EQUAL(0, strcmp(msg, "Oh dear. Something happened"));

    cswp_buffer_free(buf);
}

void test_commands()
{
    test_headers();
    test_error_response();
    test_cmd_init();
    test_cmd_term();
    test_cmd_client_info();
    test_cmd_set_devices();
    test_cmd_get_devices();
    test_cmd_get_system_description();
    test_cmd_dev_open();
    test_cmd_dev_close();
    test_cmd_set_config();
    test_cmd_get_config();
    test_cmd_get_device_capabilities();
    test_cmd_reg_list();
    test_cmd_reg_read();
    test_cmd_reg_write();
    test_cmd_mem_read();
    test_cmd_mem_write();
    test_cmd_mem_poll();
    test_async_message();
}
