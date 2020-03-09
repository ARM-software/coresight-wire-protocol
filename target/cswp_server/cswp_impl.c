// cswp_impl.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "cswp_server_types.h"
#include "cswp_buffer.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdarg.h>

#define MAX_DEV_PATH 256

static FILE* logFile;
static int verbose;
#define V_ERR 0
#define V_INFO 1
#define V_DEBUG 2
#define V_TRACE 3

#define CORESIGHT_DEVICES "/sys/bus/coresight/devices"
#define CORESIGHT_MEMAP_CSW 0x80000000
#define CORESIGHT_CSW_ADDR_INC 0x10
#define CORESIGHT_CSW_OFFSET 0xD00
#define CORESIGHT_TAR_OFFSET 0xD04
#define CORESIGHT_DRW_OFFSET 0xD0C

// Bit masks for cswp_server_device_priv_t->supportedAccessWidths
#define WIDTH_8_MASK 1
#define WIDTH_16_MASK 1 << 1
#define WIDTH_32_MASK 1 << 2
#define WIDTHS_DETERMINED_MASK 1 << 7

void setup_logging(int level, const char* filename)
{
    verbose = level;
    if (filename)
        logFile = fopen(filename, "w");
    else
        logFile = stdout;
}

void close_logging()
{
    if (logFile != stdout)
        fclose(logFile);
}

void vlog(int level, const char* msg, ...)
{
    if (verbose >= level)
    {
        va_list args;
        va_start(args, msg);
        vfprintf(logFile, msg, args);
        fflush(logFile);
        va_end(args);
    }
}

typedef struct
{
    char* path;
    int regsDiscovered;
    uint32_t address;

    // [0] - byte access supported
    // [1] - half word access supported
    // [2] - word access supported
    // [7] - supported access widths determined
    uint8_t supportedAccessWidths;
} cswp_server_device_priv_t;

/*
 * Private data for server 
 */
typedef struct
{
    cswp_server_device_priv_t* devicePriv;
    uint8_t* lastPollData;
    uint32_t lastPollDataSize;
} cswp_server_priv_t;

/*
 * Server logging function
 */
static void cswp_server_impl_log(cswp_server_state_t* state, cswp_log_level_t level, const char* msg, ...)
{
    const char* levelStrs[] = {
        "Error", "Warn", "Info", "Debug"
    };
    int l;
    va_list args;

    switch (level)
    {
    case CSWP_LOG_ERROR: l = V_ERR; break;
    case CSWP_LOG_WARN:  l = V_ERR; break;
    case CSWP_LOG_INFO:  l = V_INFO; break;
    case CSWP_LOG_DEBUG: l = V_DEBUG; break;
    default:
        l = V_TRACE;
    }

    if (verbose >= l)
    {
        va_start(args, msg);
        fprintf(logFile, "%s: ", levelStrs[level <= CSWP_LOG_DEBUG ? level : CSWP_LOG_ERROR]);
        vfprintf(logFile, msg, args);
        fputc('\n', logFile);
        fflush(logFile);
        va_end(args);
    }
}

/*
 * Filter for scandir to remove this and parent directories
 *
 * Used when scanning CoreSight device registers
 */
static int special_dir_filter(const struct dirent * entry)
{
    if ((strcmp(entry->d_name, ".") == 0) ||
        (strcmp(entry->d_name, "..") == 0))
        return 0;
    else
        return 1;
}

/*
 * Filter for scandir to select directories (unless this or parent)
 *
 * Used when scanning CoreSight device registers
 */
static int dir_filter(const struct dirent * entry)
{
    if ((strcmp(entry->d_name, ".") == 0) ||
        (strcmp(entry->d_name, "..") == 0) ||
        entry->d_type != DT_DIR)
        return 0;
    else
        return 1;
}

/*
 * Filter for scandir to select ordinary files or links
 *
 * Used when scanning CoreSight device registers
 */
static int file_filter(const struct dirent * entry)
{
    if (entry->d_type == DT_REG ||
        entry->d_type == DT_LNK)
        return 1;
    else
        return 0;
}

static int is_mem_ap_type(const char* type)
{
    return (strncmp("mem-ap", type, 6) == 0);
}

static int cswp_server_impl_clear_devices(cswp_server_state_t* state);
static int cswp_server_impl_init_devices(cswp_server_state_t* state, unsigned int deviceCount);

static int cswp_server_impl_load_sdf(char* sdfPath, char** data, uint32_t* sdfSize)
{
    int res = CSWP_SUCCESS;

    FILE* f = fopen(sdfPath, "r");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        *sdfSize = ftell(f);
        fseek(f, 0, SEEK_SET);
        *data = malloc(*sdfSize + 1);
        if (data)
        {
            int read = fread(*data, 1, *sdfSize, f);
            if (read != *sdfSize)
            {
                res = CSWP_FAILED;
                free(*data);
            }
            else
            {
                (*data)[*sdfSize++] = '\0';
            }
        }
        else
        {
            vlog(V_DEBUG, "Failed to allocated SDF data\n");
            res = CSWP_FAILED;
        }
        fclose(f);
    }
    else
    {
        vlog(V_DEBUG, "Failed to load SDF file\n");
        res = CSWP_FAILED;
    }

    return res;
}

static int cswp_server_impl_init(cswp_server_state_t* state)
{
    int numEntries, numDevices;
    struct dirent **entries;
    cswp_server_priv_t* priv = {0};
    int i;

    /* Build device list */
    /*   Device 0 is system memory
     *   Other devices are entries in /sys/bus/coresight/devices */
    vlog(V_DEBUG, "Scanning " CORESIGHT_DEVICES "\n");

    numEntries = scandir(CORESIGHT_DEVICES, &entries,
                         special_dir_filter, alphasort);
    if (numEntries < 0) {
        perror("scandir");
        return CSWP_INVALID_DEVICE;
    }
    vlog(V_DEBUG, "Found %d devices in " CORESIGHT_DEVICES "\n", numEntries);

    numDevices = numEntries + 1;

    state->deviceCount = numDevices;
    state->deviceNames = calloc(numDevices, sizeof(char*));
    state->deviceTypes = calloc(numDevices, sizeof(char*));
    state->deviceInfo = calloc(numDevices, sizeof(cswp_device_info_t));

    uint8_t* sdfData = 0;
    uint32_t sdfSize = 0;
    int sdfLoaded = cswp_server_impl_load_sdf("/sdf/AMIS-PoC.sdf", (char**) &sdfData, &sdfSize);
    if (sdfLoaded == CSWP_SUCCESS)
    {
        state->systemDescription = sdfData;
        state->systemDescriptionSize = sdfSize;
        state->systemDescriptionFormat = 0; // Raw SDF text
    }
    else
    {
        state->systemDescription = NULL;
        state->systemDescriptionSize = 0;
        state->systemDescriptionFormat = 0;
    }

    priv = (cswp_server_priv_t*)calloc(1, sizeof(cswp_server_priv_t));
    state->priv = priv;
    cswp_server_impl_init_devices(state, numDevices);

    /* Use /dev/mem for device 0 */
    state->deviceNames[0] = strdup("/dev/mem");
    state->deviceTypes[0] = strdup("memory");
    priv->devicePriv[0].path = strdup("/dev/mem");
    priv->devicePriv[0].address = 0;
    /* Other devices use name from CoreSight driver */
    for (i = 0; i < numEntries; ++i)
    {
        state->deviceNames[i+1] = strdup(entries[i]->d_name);
        state->deviceTypes[i+1] = strdup("cscomp");
        priv->devicePriv[i+1].path = malloc(MAX_DEV_PATH);
        snprintf(priv->devicePriv[i+1].path, MAX_DEV_PATH,
                 "%s/%s", CORESIGHT_DEVICES, entries[i]->d_name);
        priv->devicePriv[i+1].address = 0;
        free(entries[i]);
    }
    free(entries);

    priv->lastPollDataSize = 0;

    return CSWP_SUCCESS;
}

static int cswp_server_impl_term(cswp_server_state_t* state)
{
    cswp_server_priv_t *priv;

    vlog(V_DEBUG, "Terminating\n");

    /* Device names etc cleared up by common server implementation */
    cswp_server_impl_clear_devices(state);

    priv = (cswp_server_priv_t*)state->priv;
    if (priv && priv->lastPollData)
        free(priv->lastPollData);
    free(priv);
    state->priv = NULL;

    return CSWP_SUCCESS;
}

static int cswp_server_impl_init_devices(cswp_server_state_t* state, unsigned int deviceCount)
{
    cswp_server_priv_t *priv = state->priv;

    priv->devicePriv = calloc(deviceCount, sizeof(cswp_server_device_priv_t));

    return CSWP_SUCCESS;
}

static int cswp_server_impl_clear_devices(cswp_server_state_t* state)
{
    cswp_server_priv_t *priv = state->priv;
    int i;

    if (priv != NULL)
    {
        for (i = 0; i < state->deviceCount; ++i)
            free(priv->devicePriv[i].path);
        free(priv->devicePriv);
    }

    return CSWP_SUCCESS;
}


static int cswp_server_impl_device_add(cswp_server_state_t* state, unsigned deviceIndex, const char* deviceType)
{
    cswp_server_priv_t *priv = state->priv;

    priv->devicePriv[deviceIndex].regsDiscovered = 0;
    priv->devicePriv[deviceIndex].supportedAccessWidths = 0;
    priv->devicePriv[deviceIndex].address = 0;

    free(priv->devicePriv[deviceIndex].path);
    if (is_mem_ap_type(deviceType) ||
        (strcmp("memory", deviceType) == 0))
    {
        priv->devicePriv[deviceIndex].path = strdup("/dev/mem");
    }
    else
    {
        priv->devicePriv[deviceIndex].path = NULL;
    }
    return CSWP_SUCCESS;
}

/*
 * Handler for CoreSight device open
 */
static int cswp_server_impl_cs_device_build_regs(cswp_device_info_t* devInfo, cswp_server_device_priv_t* devPriv)
{
    char subPath[256];
    int numSubdirs, numEntries;
    struct dirent **entries;

    /* Registers in a directory or sub-directory */
    struct {
        char* dirName;
        char** regNames;
        size_t numRegs;
    } *subdirRegs;

    unsigned totalRegs;
    int i, r, dr;

    if (devPriv->path == NULL)
    {
        return CSWP_NOT_INITIALIZED;
    }
    vlog(V_DEBUG, "Scanning %s\n", devPriv->path);

    /* Build list of top level and child directories */
    /*   scan top level, getting child directories */
    numSubdirs = scandir(devPriv->path, &entries,
                         dir_filter, alphasort);
    vlog(V_DEBUG, "Scan result %d\n", numSubdirs);
    if (numSubdirs < 0) {
        perror("scandir");
        return CSWP_INVALID_DEVICE;
    }
    subdirRegs = calloc(numSubdirs+1, sizeof(subdirRegs[0]));
    // subdir 0 is root
    for (i = 0; i < numSubdirs; ++i) {
        subdirRegs[i+1].dirName = strdup(entries[i]->d_name);
        vlog(V_DEBUG, " subdir %s\n", subdirRegs[i+1].dirName);
        free(entries[i]);
    }
    free(entries);

    /* Scan each directory for files to get registers in each */
    totalRegs = 0;
    for (i = 0; i < numSubdirs+1; ++i) {
        sprintf(subPath, "%s/%s", devPriv->path, subdirRegs[i].dirName ? subdirRegs[i].dirName : ".");
        vlog(V_DEBUG, "  subdir %s\n", subPath);
        numEntries = scandir(subPath, &entries,
                             file_filter, alphasort);
        if (numEntries < 0) {
            perror("scandir");
            free(subdirRegs);
            return CSWP_INVALID_DEVICE;
        }

        subdirRegs[i].numRegs = numEntries;
        totalRegs += numEntries;
        subdirRegs[i].regNames = calloc(numEntries, sizeof(char*));

        for (r = 0; r < numEntries; ++r) {
            subdirRegs[i].regNames[r] = strdup(entries[r]->d_name);
            free(entries[r]);
        }
    }

    vlog(V_DEBUG, " returning %d regs\n", totalRegs);

    /* Build device register list from per-directory register lists  */
    devInfo->registerCount = totalRegs;
    devInfo->registerInfo = calloc(devInfo->registerCount, sizeof(cswp_register_info_t));
    dr = 0;
    for (i = 0; i < numSubdirs+1; ++i) {
        for (r = 0; r < subdirRegs[i].numRegs; ++r) {
            /* Get the path relative to the device's top-level */
            if (subdirRegs[i].dirName)
                sprintf(subPath, "%s/%s", subdirRegs[i].dirName, subdirRegs[i].regNames[r]);
            else
                strcpy(subPath, subdirRegs[i].regNames[r]);
            vlog(V_DEBUG, "   R %d/%d %s\n", i, r, subPath);

            /* Add the register information */
            devInfo->registerInfo[dr].id = dr;
            devInfo->registerInfo[dr].name = strdup(subPath);
            devInfo->registerInfo[dr].size = 1;
            devInfo->registerInfo[dr].displayName = strdup(subPath);
            devInfo->registerInfo[dr].description = strdup(subPath);

            ++dr;

            free(subdirRegs[i].regNames[r]);
        }
    }
    free(subdirRegs);

    vlog(V_DEBUG, "Found %d registers in %s\n", totalRegs, devPriv->path);

    return CSWP_SUCCESS;
}

static int cswp_server_impl_device_discover_regs(cswp_server_state_t* state, unsigned deviceIndex)
{
    cswp_device_info_t* devInfo = &state->deviceInfo[deviceIndex];
    cswp_server_device_priv_t* devPriv = &((cswp_server_priv_t*)state->priv)->devicePriv[deviceIndex];
    int res = CSWP_SUCCESS;

    if (is_mem_ap_type(state->deviceTypes[deviceIndex]))
    {
      /* No driver for mem-ap so it will be access through /dev/mem */
      devInfo->registerCount = 4095;
    }
    else if (devPriv->path == 0 ||
        strcmp("/dev/mem", devPriv->path) == 0)
    {
        /* No regs on memory device */
    }
    else
    {
        res = cswp_server_impl_cs_device_build_regs(devInfo, devPriv);
    }

    devPriv->regsDiscovered = 1;

    return res;
}


static int cswp_server_impl_device_open(cswp_server_state_t* state, unsigned deviceIndex)
{
    /* No registers */
    cswp_device_info_t* devInfo = &state->deviceInfo[deviceIndex];
    devInfo->registerCount = 0;
    devInfo->registerInfo = NULL;

    cswp_server_priv_t *priv = state->priv;
    priv->devicePriv[deviceIndex].regsDiscovered = 0;

    return CSWP_SUCCESS;
}


static int cswp_server_impl_set_config(struct _cswp_server_state_t* state, unsigned deviceIndex, const char* name, const char* value)
{
    cswp_server_device_priv_t* devPriv = &((cswp_server_priv_t*)state->priv)->devicePriv[deviceIndex];
    int res = CSWP_SUCCESS;

    if (strcmp("linux.cscomp", state->deviceTypes[deviceIndex]) == 0)
    {
        if (strcmp("PATH", name) == 0)
        {
            if (strlen(value) >= MAX_DEV_PATH)
            {
                res = CSWP_BAD_ARGS;
            }
            else
            {
                if (devPriv->path == NULL ||
                    strcmp(devPriv->path, value) != 0)
                {
                    free(devPriv->path);
                    devPriv->path = strdup(value);
                    devPriv->regsDiscovered = 0;
                }
            }
        }
        else
        {
            res = CSWP_BAD_ARGS;
        }
    }
    else if (is_mem_ap_type(state->deviceTypes[deviceIndex]))
    {
        if (strcmp("BASE_ADDRESS", name) == 0)
        {
            devPriv->address = strtol(value, NULL, 16);
        }
        else
        {
            res = CSWP_BAD_ARGS;
        }
    }

    return res;
}

static int cswp_server_fill_config(char* buf, const char* value, size_t bufSize)
{
    if (bufSize <= strlen(value))
        return CSWP_BAD_ARGS;

    strcpy(buf, value);

    return CSWP_SUCCESS;
}

static int cswp_server_impl_get_config(struct _cswp_server_state_t* state, unsigned deviceIndex, const char* name, char* value, size_t valueSize)
{
    cswp_server_device_priv_t* devPriv = &((cswp_server_priv_t*)state->priv)->devicePriv[deviceIndex];
    int res;

    if (strcmp("CONFIG_ITEMS", name) == 0)
        res = cswp_server_fill_config(value, "PATH", valueSize);
    else if (strcmp("PATH", name) == 0)
        res = cswp_server_fill_config(value, devPriv->path != NULL ? devPriv->path : "", valueSize);
    else if (strcmp("ADDRESS", name) == 0)
    {
        char address[16];
        sprintf(address, "0x%08x", devPriv->address);
        res = cswp_server_fill_config(value, address, valueSize);
    }
    else
        /* Only "path" config item supported */
        res = CSWP_BAD_ARGS;

    return res;
}

static int cswp_server_impl_get_device_capabilities(struct _cswp_server_state_t* state, unsigned deviceIndex,  varint_t* capabilities, varint_t* capabilitiesData)
{
    *capabilitiesData = 0;
    if (strcmp("mem-ap.v2", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG | CSWP_CAP_MEM | CSWP_CAP_MEM_POLL;
    else if (strcmp("mem-ap.v1", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG | CSWP_CAP_MEM | CSWP_CAP_MEM_POLL;
    else if (strcmp("memory", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_MEM | CSWP_CAP_MEM_POLL;
    else if (strcmp("dap.v6", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG;
    else if (strcmp("dap.v5", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG;
    else if (strcmp("jtag.ap", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG;
    else if (strcmp("cscomp", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG;
    else if (strcmp("linux.cscomp", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG;
    else
      *capabilities = CSWP_CAP_REG | CSWP_CAP_MEM | CSWP_CAP_MEM_POLL;

    return CSWP_SUCCESS;
}

static int cswp_server_impl_reg_list_build(struct _cswp_server_state_t* state, unsigned deviceIndex)
{
    cswp_server_device_priv_t* devPriv = &((cswp_server_priv_t*)state->priv)->devicePriv[deviceIndex];
    int res = CSWP_SUCCESS;

    if (devPriv->regsDiscovered == 0)
    {
        res = cswp_server_impl_device_discover_regs(state, deviceIndex);
    }

    return res;
}

 static int copy_8(void* dest, const void* src, size_t size)
{
    const uint8_t* s = src;
    uint8_t* d = dest;
    while (size)
    {
        *d++ = *s++;
        size--;
    }
    return CSWP_SUCCESS;
}


static int copy_16(void* dest, const void* src, size_t size)
{
    const uint16_t* s = src;
    uint16_t* d = dest;
    if (size % 2 != 0)
        return CSWP_BAD_ARGS;
    while (size)
    {
        *d++ = *s++;
        size -= 2;
    }
    return CSWP_SUCCESS;
}


static int copy_32(void* dest, const void* src, size_t size)
{
    const volatile uint32_t* s = src;
    volatile uint32_t* d = dest;
    if (size % 4 != 0)
        return CSWP_BAD_ARGS;
    while (size)
    {
        *d++ = *s++;
        size -= 4;
    }
    return CSWP_SUCCESS;
}


static int copy_64(void* dest, const void* src, size_t size)
{
    const uint64_t* s = src;
    uint64_t* d = dest;
    if (size % 8 != 0)
        return CSWP_BAD_ARGS;
    while (size)
    {
        *d++ = *s++;
        size -= 8;
    }
    return CSWP_SUCCESS;
}


static int copy(void* dest, const void* src, size_t size, cswp_access_size_t accessSize)
{
    int res;

    switch (accessSize)
    {
    default:
    case CSWP_ACCESS_SIZE_DEF:
    case CSWP_ACCESS_SIZE_8:  res = copy_8(dest, src, size); break;
    case CSWP_ACCESS_SIZE_16: res = copy_16(dest, src, size); break;
    case CSWP_ACCESS_SIZE_32: res = copy_32(dest, src, size); break;
    case CSWP_ACCESS_SIZE_64: res = copy_64(dest, src, size); break;
    }

    return res;
}

static int do_32bit_read(int fd, uint64_t address, uint32_t* value)
{
    int res = CSWP_SUCCESS;
    char *addr;
    size_t pageSize, pages;
    off_t pageAddr;
    size_t size = 4;
    if (fd < 0)
    {
        perror("Open mem failed: ");
        return CSWP_MEM_FAILED;
    }
    pageSize = sysconf(_SC_PAGE_SIZE);
    pageAddr = address & ~(pageSize - 1);
    pages = (address + size - pageAddr + pageSize - 1) / pageSize;
    addr = mmap(NULL, pageSize * pages, PROT_READ,
                MAP_SHARED, fd, pageAddr);
    if (addr == MAP_FAILED)
    {
        perror("mmap failed: ");
        return CSWP_MEM_FAILED;
    }
    res = copy_32((uint8_t*)value, addr + (address - pageAddr), size);
    munmap(addr, pageSize * pages);
    return res;
}

static int do_32bit_write(int fd, uint64_t address, uint32_t value)
{
    int res = CSWP_SUCCESS;
    char *addr;
    size_t pageSize, pages;
    off_t pageAddr;
    size_t size = 4;

    if (fd < 0)
    {
        perror("Open mem failed: ");
        return CSWP_MEM_FAILED;
    }

    /* mmap requested address range, aligning to page size */
    /* TODO: limit max mmap size? */
    pageSize = sysconf(_SC_PAGE_SIZE);
    pageAddr = address & ~(pageSize - 1);
    pages = (address + size - pageAddr + pageSize - 1) / pageSize;
    addr = mmap(NULL, pageSize * pages, PROT_READ|PROT_WRITE,
                MAP_SHARED, fd, pageAddr);
    if (addr == MAP_FAILED)
    {
      perror("mmap failed: ");
      return CSWP_MEM_FAILED;
    }
    res = copy_32(addr + (address - pageAddr), (uint8_t*)(&value), size);
    munmap(addr, pageSize * pages);
    return res;
}

static int determine_supported_access_widths(int fd, cswp_server_device_priv_t * devPriv)
{
    uint32_t buf = 0x0;

    // check 8-bit
    uint32_t cswVal = 0x0;
    int res = do_32bit_write(fd,
                             devPriv->address | CORESIGHT_CSW_OFFSET,
                             cswVal);
    if (res != CSWP_SUCCESS)
        return res;
    res = do_32bit_read(fd,
                        devPriv->address | CORESIGHT_CSW_OFFSET,
                        &buf);
    if (res != CSWP_SUCCESS)
        return res;
    if ((buf & 0x7) == cswVal)
        devPriv->supportedAccessWidths |= WIDTH_8_MASK;

    // check 16-bit
    cswVal = 0x1;
    res = do_32bit_write(fd,
                         devPriv->address | CORESIGHT_CSW_OFFSET,
                         cswVal);
    if (res != CSWP_SUCCESS)
        return res;
    res = do_32bit_read(fd,
                        devPriv->address | CORESIGHT_CSW_OFFSET,
                        &buf);
    if (res != CSWP_SUCCESS)
        return res;
    if ((buf & 0x7) == cswVal)
        devPriv->supportedAccessWidths |= WIDTH_16_MASK;

    // check 32-bit
    cswVal = 0x2;
    res = do_32bit_write(fd,
                         devPriv->address | CORESIGHT_CSW_OFFSET,
                         cswVal);
    if (res != CSWP_SUCCESS)
        return res;
    res = do_32bit_read(fd,
                        devPriv->address | CORESIGHT_CSW_OFFSET,
                        &buf);
    if (res != CSWP_SUCCESS)
        return res;
    if ((buf & 0x7) == cswVal)
        devPriv->supportedAccessWidths |= WIDTH_32_MASK;

    devPriv->supportedAccessWidths |= WIDTHS_DETERMINED_MASK;

    return CSWP_SUCCESS;
}

int get_csw_size_value(cswp_server_device_priv_t* devPriv, cswp_access_size_t access_size, uint8_t* csw_size_value)
{
    if (!(devPriv->supportedAccessWidths & WIDTHS_DETERMINED_MASK))
    {
        int fd = open("/dev/mem", O_RDWR);
        int res = determine_supported_access_widths(fd, devPriv);
        close(fd);
        if (res != CSWP_SUCCESS)
        {
            return res;
        }
    }

    if ((access_size == CSWP_ACCESS_SIZE_32 ||
        access_size == CSWP_ACCESS_SIZE_DEF) &&
        devPriv->supportedAccessWidths & WIDTH_32_MASK)
    {
        *csw_size_value = 2;
        return CSWP_SUCCESS;
    }
    else if (access_size == CSWP_ACCESS_SIZE_16 &&
             devPriv->supportedAccessWidths & WIDTH_16_MASK)
    {
        *csw_size_value = 1;
        return CSWP_SUCCESS;
    }
    else if (access_size == CSWP_ACCESS_SIZE_8 &&
             devPriv->supportedAccessWidths & WIDTH_8_MASK)
    {
        *csw_size_value = 0;
        return CSWP_SUCCESS;
    }

    return CSWP_MEM_BAD_ACCESS_SIZE;
}

static int cswp_server_impl_reg_read(struct _cswp_server_state_t* state, unsigned deviceIndex, int registerID, uint32_t* value)
{
    int res = CSWP_SUCCESS;
    cswp_device_info_t* devInfo = &state->deviceInfo[deviceIndex];
    cswp_server_device_priv_t* devPriv = &((cswp_server_priv_t*)state->priv)->devicePriv[deviceIndex];

    if (devPriv->path == NULL)
    {
        vlog(V_INFO, "Path not set for device %d", deviceIndex);
        return CSWP_NOT_INITIALIZED;
    }

    if (state->deviceTypes[deviceIndex] == NULL)
    {
        vlog(V_INFO, "Type not set for device %d", deviceIndex);
        return CSWP_NOT_INITIALIZED;
    }

    if (devPriv->regsDiscovered == 0)
    {
        res = cswp_server_impl_device_discover_regs(state, deviceIndex);
        if (res != CSWP_SUCCESS)
            return res;
    }

    if (registerID >= devInfo->registerCount)
        return CSWP_BAD_ARGS;

    /* no driver for mem-ap so access it through /mem/dev */
    if (is_mem_ap_type(state->deviceTypes[deviceIndex]))
    {
            int fd = open("/dev/mem", O_RDONLY);
            if (fd < 0) {
                return CSWP_REG_FAILED;
            }

            res = do_32bit_read(fd, devPriv->address | registerID, value);
            close(fd);
    }
    else
    {
        /* register IDs are allocated sequentially, so can access directly */
        cswp_register_info_t* regInfo = &devInfo->registerInfo[registerID];

        char regPath[256];
        char rdBuf[64];

        /* Read from file in CoreSight device */
        snprintf(regPath, 255, "%s/%s",
                 devPriv->path,
                 regInfo->name);

        vlog(V_DEBUG, "Reading from %s\n", regPath);

        int fd = open(regPath, O_RDONLY);
        if (fd < 0) {
          return CSWP_REG_FAILED;
        }

        int readRes = read(fd, rdBuf, sizeof(rdBuf));
        if (readRes < 0) {
          close(fd);
          return CSWP_REG_FAILED;
        }
        close(fd);
        rdBuf[readRes] = '\0';

        /* Convert from hex - almost all props use
         * kstrtoul(16) apart from enable_xxx, but this is just testing for non-zero */
        errno = 0;
        *value = strtoul(rdBuf, NULL, 16);
        if (errno != 0)
          return CSWP_REG_FAILED;
    }

    return res;
}


static int cswp_server_impl_reg_write(struct _cswp_server_state_t* state, unsigned deviceIndex, int registerID, uint32_t value)
{
    int res = CSWP_SUCCESS;
    cswp_device_info_t* devInfo = &state->deviceInfo[deviceIndex];
    cswp_server_device_priv_t* devPriv = &((cswp_server_priv_t*)state->priv)->devicePriv[deviceIndex];

    if (devPriv->path == NULL)
    {
        vlog(V_INFO, "Path not set for device %d", deviceIndex);
        return CSWP_NOT_INITIALIZED;
    }

    if (state->deviceTypes[deviceIndex] == NULL)
    {
        vlog(V_INFO, "Type not set for device %d", deviceIndex);
        return CSWP_NOT_INITIALIZED;
    }

    if (devPriv->regsDiscovered == 0)
    {
        res = cswp_server_impl_device_discover_regs(state, deviceIndex);
        if (res != CSWP_SUCCESS)
            return res;
    }

    if (registerID >= devInfo->registerCount)
        return CSWP_BAD_ARGS;

    /* no driver for mem-ap so access it through /mem/dev */
    if (is_mem_ap_type(state->deviceTypes[deviceIndex]))
    {
         int fd = open("/dev/mem", O_RDWR);
         if (fd < 0) {
            return CSWP_REG_FAILED;
         }
         res = do_32bit_write(fd, devPriv->address | registerID, value);
         close(fd);
    }
    else
    {
        /* register IDs are allocated sequentially, so can access directly */
        cswp_register_info_t* regInfo = &devInfo->registerInfo[registerID];

        char regPath[256];
        char wrBuf[64];

        /* Write to file in CoreSight device */
        snprintf(regPath, 255, "%s/%s",
                 devPriv->path,
                 regInfo->name);

        /* Convert to hex - almost all props use
         * kstrtoul(16) apart from enable_xxx, but this is just testing for non-zero */
        snprintf(wrBuf, sizeof(wrBuf)-1, "%X", value);
        wrBuf[sizeof(wrBuf)-1] = '\0';

        vlog(V_DEBUG, "Writing %s to %s\n", wrBuf, regPath);

        int fd = open(regPath, O_WRONLY);
        if (fd < 0) {
            return CSWP_REG_FAILED;
        }

        int writeRes = write(fd, wrBuf, sizeof(wrBuf));
        if (writeRes < 0) {
          close(fd);
          return CSWP_REG_FAILED;
        }

        close(fd);
    }

    return res;
}


static int cswp_server_impl_mem_read(struct _cswp_server_state_t* state, unsigned deviceIndex,
                                     uint64_t address, size_t size,
                                     cswp_access_size_t accessSize, unsigned flags, uint8_t* pData)
{
    cswp_server_device_priv_t* devPriv = &((cswp_server_priv_t*)state->priv)->devicePriv[deviceIndex];
    char *addr;
    size_t pageSize, pages;
    off_t pageAddr;
    size_t i;
    int res = CSWP_SUCCESS;
    unsigned accessSizeBytes;

    if (devPriv->path == NULL)
    {
        vlog(V_INFO, "Path not set for device %d", deviceIndex);
        return CSWP_NOT_INITIALIZED;
    }

    if (state->deviceTypes[deviceIndex] == NULL)
    {
        vlog(V_INFO, "Type not set for device %d", deviceIndex);
        return CSWP_NOT_INITIALIZED;
    }

    /* Only device 0 supports mem access */
    if (deviceIndex != 0)
        return CSWP_UNSUPPORTED;

    if ((flags & CSWP_MEM_NO_ADDR_INC) && accessSize == CSWP_ACCESS_SIZE_DEF)
    {
        vlog(V_INFO, "Invalid access size for repeated read");
        return CSWP_BAD_ARGS;
    }
    accessSizeBytes = 1 << (accessSize-1);

    /* no driver for mem-ap so access it through /dev/mem
       operations: - write TAR register
                   - write CSW register
                   - read DRW register 
    */
    if (is_mem_ap_type(state->deviceTypes[deviceIndex]))
    {
      int fd = open("/dev/mem", O_RDWR);
      uint8_t cswSize;
      res = get_csw_size_value(devPriv, accessSize, &cswSize);
      if (res != CSWP_SUCCESS)
      {
          close(fd);
          return res;
      }

      // CSW
      uint32_t cswVal = CORESIGHT_MEMAP_CSW | cswSize;
      cswVal |= (flags & CSWP_MEM_NO_ADDR_INC) ? 0x0 : CORESIGHT_CSW_ADDR_INC;
      res = do_32bit_write(fd,
                           devPriv->address | CORESIGHT_CSW_OFFSET,
                           cswVal);
      if (res != CSWP_SUCCESS)
      {
          close(fd);
          return res;
      }
      // TAR
      res = do_32bit_write(fd,
                           devPriv->address | CORESIGHT_TAR_OFFSET,
                           address & 0xFFFFFFFF);
      if (res != CSWP_SUCCESS)
      {
          close(fd);
          return res;
      }

      for(i=0; i<size/accessSizeBytes; i++)
      {
          uint32_t data;
          res = do_32bit_read(fd,
                              devPriv->address | CORESIGHT_DRW_OFFSET,
                              &data);
          if (res != CSWP_SUCCESS)
          {
              close(fd);
              return res;
          }

          // Copy data from correct byte lane
          uint32_t copyData;
          uint32_t currAddr = address + (i*accessSizeBytes);
          if (accessSize == CSWP_ACCESS_SIZE_8)
              copyData = (data >> (8 * (currAddr & 0x3))) & 0xff;
          else if (accessSize == CSWP_ACCESS_SIZE_16)
              copyData = (data >> (8 * (currAddr & 0x2))) & 0xffff;
          else
              copyData = data;
          copy(&pData[i*accessSizeBytes], &copyData, accessSizeBytes, accessSize);
      }
      close(fd);
    }
    else
    {
        /* Use mmap with /dev/mem to get physical memory */
        int fd = open(devPriv->path, O_RDONLY);
        if (fd < 0)
        {
            perror("Open mem failed: ");
            return CSWP_MEM_FAILED;
        }

        /* mmap requested address range, aligning to page size */
        /* TODO: limit max mmap size? */
        pageSize = sysconf(_SC_PAGE_SIZE);
        pageAddr = address & ~(pageSize - 1);
        pages = (address + size - pageAddr + pageSize - 1) / pageSize;
        addr = mmap(NULL, pageSize * pages, PROT_READ,
                    MAP_SHARED, fd, pageAddr);
        if (addr == MAP_FAILED)
        {
            close(fd);
            perror("mmap failed: ");
            return CSWP_MEM_FAILED;
        }

        /* Copy from mmapd region */
        if (flags & CSWP_MEM_NO_ADDR_INC)
        {
            for (i = 0; i < size/accessSizeBytes && res == CSWP_SUCCESS; ++i)
            {
                res = copy(pData, addr + (address - pageAddr), accessSizeBytes, accessSize);
                pData += accessSizeBytes;
            }
        }
        else
        {
            res = copy(pData, addr + (address - pageAddr), size, accessSize);
        }
        munmap(addr, pageSize * pages);
        close(fd);
    }

    return res;
}


static int cswp_server_impl_mem_write(struct _cswp_server_state_t* state, unsigned deviceIndex,
                                      uint64_t address, size_t size,
                                      cswp_access_size_t accessSize, unsigned flags, const uint8_t* pData)
{
    cswp_server_device_priv_t* devPriv = &((cswp_server_priv_t*)state->priv)->devicePriv[deviceIndex];
    char *addr;
    size_t pageSize, pages;
    off_t pageAddr;
    size_t i;
    int res = CSWP_SUCCESS;
    unsigned accessSizeBytes;

    if (devPriv->path == NULL)
    {
        vlog(V_INFO, "Path not set for device %d", deviceIndex);
        return CSWP_NOT_INITIALIZED;
    }

    if (state->deviceTypes[deviceIndex] == NULL)
    {
        vlog(V_INFO, "Type not set for device %d", deviceIndex);
        return CSWP_NOT_INITIALIZED;
    }

    /* Only device 0 supports mem access */
    if (deviceIndex != 0)
        return CSWP_UNSUPPORTED;

    if ((flags & CSWP_MEM_NO_ADDR_INC) && accessSize == CSWP_ACCESS_SIZE_DEF)
    {
        vlog(V_INFO, "Invalid access size for repeated read");
        return CSWP_BAD_ARGS;
    }
    accessSizeBytes = 1 << (accessSize-1);

    /* no driver for mem-ap so access it through /dev/mem
       operations: - write TAR register
                   - write CSW register
                   - read DRW register 
    */
    if (is_mem_ap_type(state->deviceTypes[deviceIndex]))
    {
        int fd = open("/dev/mem", O_RDWR);
        uint8_t cswSize;
        res = get_csw_size_value(devPriv, accessSize, &cswSize);
        if (res != CSWP_SUCCESS)
        {
            close(fd);
            return res;
        }

        // CSW
        uint32_t cswVal = CORESIGHT_MEMAP_CSW | cswSize;
        cswVal |= (flags & CSWP_MEM_NO_ADDR_INC) ? 0x0 : CORESIGHT_CSW_ADDR_INC;
        res = do_32bit_write(fd,
                             devPriv->address | CORESIGHT_CSW_OFFSET,
                             cswVal);
        if (res != CSWP_SUCCESS)
        {
            close(fd);
            return res;
        }
        // TAR
        res = do_32bit_write(fd,
                             devPriv->address | CORESIGHT_TAR_OFFSET,
                             address & 0xFFFFFFFF);
        if (res != CSWP_SUCCESS)
        {
            close(fd);
            return res;
        }

        for(i=0; i<size/accessSizeBytes; i++)
        {
            uint32_t data;
            copy(&data, &pData[i*accessSizeBytes], accessSizeBytes, accessSize);

            // Move write data to correct byte lane
            uint32_t writeData;
            uint32_t currAddr = address + (i*accessSizeBytes);
            if (accessSize == CSWP_ACCESS_SIZE_8)
                writeData = (data << (8 * (currAddr & 0x3))) & 0xff;
            else if (accessSize == CSWP_ACCESS_SIZE_16)
                writeData = (data << (8 * (currAddr & 0x2))) & 0xffff;
            else
                writeData = data;
            res = do_32bit_write(fd,
                                 devPriv->address | CORESIGHT_DRW_OFFSET,
                                 writeData);
            if (res != CSWP_SUCCESS)
                break;
        }
        close(fd);
    }
    else
    {
        /* Use mmap with /dev/mem to get physical memory */
        int fd = open(devPriv->path, O_RDWR);
        if (fd < 0)
        {
            perror("Open mem failed: ");
            return CSWP_MEM_FAILED;
        }

        /* mmap requested address range, aligning to page size */
        /* TODO: limit max mmap size? */
        pageSize = sysconf(_SC_PAGE_SIZE);
        pageAddr = address & ~(pageSize - 1);
        pages = (address + size - pageAddr + pageSize - 1) / pageSize;
        addr = mmap(NULL, pageSize * pages, PROT_READ|PROT_WRITE,
                    MAP_SHARED, fd, pageAddr);
        if (addr == MAP_FAILED)
        {
            close(fd);
            perror("mmap failed: ");
            return CSWP_MEM_FAILED;
        }

        /* Copy to mmapd region */
        if (flags & CSWP_MEM_NO_ADDR_INC)
        {
            for (i = 0; i < size/accessSizeBytes && res == CSWP_SUCCESS; ++i)
            {
                res = copy(addr + (address - pageAddr), pData, accessSizeBytes, accessSize);
                pData += accessSizeBytes;
            }
        }
        else
        {
            res = copy(addr + (address - pageAddr), pData, size, accessSize);
        }

        munmap(addr, pageSize * pages);
        close(fd);
    }

    return res;
}


static int cswp_server_impl_check_last(struct _cswp_server_state_t* state,
                                       size_t size,
                                       unsigned flags, const uint8_t* pMask, const uint8_t* pValue,
                                       uint8_t* pData)
{
    cswp_server_priv_t* priv = (cswp_server_priv_t*)state->priv;
    uint8_t* cmpBuf;
    uint8_t* valBuf;
    int i;
    int res, cmpRes;

    if (!priv->lastPollData || priv->lastPollDataSize == 0)
    {
        vlog(V_INFO, "lastPollData not set");
        return CSWP_BAD_ARGS;
    }

    if (size > priv->lastPollDataSize)
    {
        vlog(V_INFO, "Check size larger than lastPollData");
        return CSWP_BAD_ARGS;
    }

    cmpBuf = malloc(size);
    valBuf = malloc(size);

    /* Copy values and apply mask */
    for (i = 0; i < size; ++i)
        cmpBuf[i] = pValue[i] & pMask[i];
    for (i = 0; i < size; ++i)
        valBuf[i] = priv->lastPollData[i] & pMask[i];

    res = CSWP_MEM_POLL_NO_MATCH;
    cmpRes = memcmp(valBuf, cmpBuf, size);
    if (((flags & CSWP_MEM_POLL_MATCH_NE) && (cmpRes != 0)) ||
        (((flags & CSWP_MEM_POLL_MATCH_NE) == 0) && (cmpRes == 0)))
    {
        res = CSWP_SUCCESS;
    }

    /* return last read value to caller */
    memcpy(pData, priv->lastPollData, size);

    free(cmpBuf);
    free(valBuf);

    return res;
}

static int cswp_server_impl_mem_poll(struct _cswp_server_state_t* state, int deviceIndex,
                                     uint64_t address, size_t size,
                                     cswp_access_size_t accessSize, unsigned flags,
                                     unsigned tries, unsigned interval,
                                     const uint8_t* pMask, const uint8_t* pValue,
                                     uint8_t* pData)
{
    cswp_server_priv_t* priv = (cswp_server_priv_t*)state->priv;
    cswp_server_device_priv_t* devPriv = &priv->devicePriv[deviceIndex];
    int fd;
    char *addr;
    size_t pageSize, pages;
    off_t pageAddr;
    uint8_t* cmpBuf;
    uint8_t* valBuf;
    int res;
    size_t i;

    unsigned accessSizeBytes;

    if (flags & CSWP_MEM_POLL_CHECK_LAST)
    {
        return cswp_server_impl_check_last(state, size, flags, pMask, pValue, pData);
    }

    if (devPriv->path == NULL)
    {
        vlog(V_INFO, "Path not set for device %d", deviceIndex);
        return CSWP_NOT_INITIALIZED;
    }

    if (state->deviceTypes[deviceIndex] == NULL)
    {
        vlog(V_INFO, "Type not set for device %d", deviceIndex);
        return CSWP_NOT_INITIALIZED;
    }

    /* Only device 0 supports mem access */
    if (deviceIndex != 0)
        return CSWP_UNSUPPORTED;

    /* Allocate buffers */
    cmpBuf = malloc(size);
    valBuf = malloc(size);

    if (cmpBuf == NULL)
      return CSWP_FAILED;

    if (valBuf == NULL)
    {
      free(cmpBuf);
      return CSWP_FAILED;
    }

    /* Copy value and apply mask */
    for (i = 0; i < size; ++i)
        cmpBuf[i] = pValue[i] & pMask[i];

    accessSizeBytes = 1 << (accessSize-1);
    if (is_mem_ap_type(state->deviceTypes[deviceIndex]))
    {
        int cmpRes;
        fd = open("/dev/mem", O_RDWR);
        uint8_t cswSize;
        res = get_csw_size_value(devPriv, accessSize, &cswSize);

        if (res == CSWP_SUCCESS)
          // Can assume access is a 32-bit access now
          res = do_32bit_write(fd,
                               devPriv->address | CORESIGHT_CSW_OFFSET,
                               CORESIGHT_MEMAP_CSW | cswSize);

        if (res == CSWP_SUCCESS)
          res = do_32bit_write(fd,
                               devPriv->address | CORESIGHT_TAR_OFFSET,
                               address & 0xFFFFFFFF);

        while ((tries-- > 0) && (res == CSWP_SUCCESS))
        {
            uint32_t data;
            for(i = 0; (i < size/accessSizeBytes) && (res == CSWP_SUCCESS); i++)
            {
                res = do_32bit_read(fd,
                                    devPriv->address | CORESIGHT_DRW_OFFSET,
                                    &data);

                // Copy data from correct byte lane
                uint32_t copyData;
                uint32_t currAddr = address + (i*accessSizeBytes);
                if (accessSize == CSWP_ACCESS_SIZE_8)
                    copyData = (data >> (8 * (currAddr & 0x3))) & 0xff;
                else if (accessSize == CSWP_ACCESS_SIZE_16)
                    copyData = (data >> (8 * (currAddr & 0x2))) & 0xffff;
                else
                    copyData = data;
                copy(&pData[i*accessSizeBytes], &copyData, accessSizeBytes, accessSize);
            }

            if (res != CSWP_SUCCESS)
                break;

            /* Copy and mask */
            for (i = 0; i < size; ++i)
                valBuf[i] = pData[i] & pMask[i];

            cmpRes = memcmp(valBuf, cmpBuf, size);
            if (((flags & CSWP_MEM_POLL_MATCH_NE) && (cmpRes != 0)) ||
                (((flags & CSWP_MEM_POLL_MATCH_NE) == 0) && (cmpRes == 0)))
            {
                res = CSWP_SUCCESS;
                break;
            }
            else
                res = CSWP_MEM_POLL_NO_MATCH;

            if (interval > 0)
                usleep(interval);
        }
        close(fd);
    }
    else
    {
        /* Use mmap with /dev/mem to get physical memory */
        fd = open(devPriv->path, O_RDONLY);
        if (fd < 0)
        {
            perror("Open mem failed: ");
            return CSWP_MEM_FAILED;
        }

        /* mmap requested address range, aligning to page size */
        /* TODO: limit max mmap size? */
        pageSize = sysconf(_SC_PAGE_SIZE);
        pageAddr = address & ~(pageSize - 1);
        pages = (address + size - pageAddr + pageSize - 1) / pageSize;
        addr = mmap(NULL, pageSize * pages, PROT_READ,
                    MAP_SHARED, fd, pageAddr);
        if (addr == MAP_FAILED)
        {
            close(fd);
            free(cmpBuf);
            free(valBuf);
            perror("mmap failed: ");
            return CSWP_MEM_FAILED;
        }

        while (tries-- > 0)
        {
            int cmpRes, cpyRes;

            /* Copy from mmapd region */
            cpyRes = copy(pData, addr + (address - pageAddr), size, accessSize);
            if (cpyRes != CSWP_SUCCESS)
                break;

            /* Copy and mask */
            for (i = 0; i < size; ++i)
                valBuf[i] = pData[i] & pMask[i];

            cmpRes = memcmp(valBuf, cmpBuf, size);
            if (((flags & CSWP_MEM_POLL_MATCH_NE) && (cmpRes != 0)) ||
                (((flags & CSWP_MEM_POLL_MATCH_NE) == 0) && (cmpRes == 0)))
            {
                res = CSWP_SUCCESS;
                break;
            }
            else
                res = CSWP_MEM_POLL_NO_MATCH;

            if (interval > 0)
                usleep(interval);
        }

        munmap(addr, pageSize * pages);
        close(fd);
    }

    /* Store or clear read data in priv for future check operations */
    free(priv->lastPollData);
    if (res == CSWP_SUCCESS)
    {
        priv->lastPollData = malloc(size);
        if (priv->lastPollData == NULL)
        {
            res = CSWP_FAILED;
        }
        else
        {
            memcpy(priv->lastPollData, pData, size);
            priv->lastPollDataSize = size;
        }
    }
    else
    {
        priv->lastPollData = 0;
        priv->lastPollDataSize = 0;
    }

    free(cmpBuf);
    free(valBuf);

    return res;
}


const cswp_server_impl_t cswpServerImpl = {
    .init = cswp_server_impl_init,
    .term = cswp_server_impl_term,
    .init_devices = cswp_server_impl_init_devices,
    .clear_devices = cswp_server_impl_clear_devices,
    .device_add = cswp_server_impl_device_add,
    .device_open = cswp_server_impl_device_open,
    .set_config = cswp_server_impl_set_config,
    .get_config = cswp_server_impl_get_config,
    .get_device_capabilities = cswp_server_impl_get_device_capabilities,
    .register_list_build = cswp_server_impl_reg_list_build,
    .register_read = cswp_server_impl_reg_read,
    .register_write = cswp_server_impl_reg_write,
    .mem_read = cswp_server_impl_mem_read,
    .mem_write = cswp_server_impl_mem_write,
    .mem_poll = cswp_server_impl_mem_poll,
    .log = cswp_server_impl_log
};
