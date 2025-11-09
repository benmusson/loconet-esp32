#ifndef LIBM18_H
#define LIBM18_H

/*
 * libm18 — Milwaukee M18 Battery Protocol Library
 * -----------------------------------------------
 * Provides parsing and message preparation for M18 serial communication.
 */

#ifdef __cplusplus
extern "C" {



#endif


#include <stddef.h>
#include <stdint.h>

#ifndef LIBM18_API
#define LIBM18_API
#endif

#define M18_CMD_SYNC        0xAA
#define M18_CMD_CALIBRATE   0x55
#define M18_CMD_CONFIGURE   0x60
#define M18_CMD_READ        0x01
#define M18_CMD_SNAPCHAT    0x61
#define M18_CMD_KEEPALIVE   0x62

#define M18_MAX_PAYLOAD_LEN 256
#define M18_MAX_MESSAGE_LEN (M18_MAX_PAYLOAD_LEN + 8)

#define M18_CUTOFF_CURRENT  300
#define M18_MAX_CURRENT     6000

#define M18_MSB(value) ((uint8_t)(((uint16_t)(value) >> 8) & 0xFF))
#define M18_LSB(value) ((uint8_t)((uint16_t)(value) & 0xFF))

#define M18_DATA_LABELS(X) \
/* id, addr, len, type, label */ \
X(CELL_TYPE,            0x0000, 2, "uint",  "Cell type") \
X(SERIAL_INFO,          0x0004, 5, "sn",    "Capacity & Serial number") \
X(MANUFACTURE_DATE,     0x0011, 4, "date",  "Manufacture date") \
X(DATE_FIRST_CHARGE_FG, 0x0015, 4, "date",  "Date of first charge (Forge)") \
X(DATE_LAST_CHARGE_FG,  0x0019, 4, "date",  "Date of last charge (Forge)") \
X(NOTE,                 0x0023, 20, "ascii", "Note") \
X(CURRENT_DATE,         0x0037, 4, "date",  "Current date") \
X(CELL_VOLTAGES,        0x400A, 10, "cell_v","Cell voltages (mV)") \
X(TEMP_NON_FORGE,       0x4014, 2, "adc_t", "Temperature (C) (non-Forge)") \
X(TEMP_FORGE,           0x401F, 2, "dec_t", "Temperature (C) (Forge)") \
X(DATE_FIRST_CHARGE,    0x9000, 4, "date",  "Date of first charge (rounded)") \
X(DATE_LAST_TOOL_USE,   0x9004, 4, "date",  "Date of last tool use (rounded)") \
X(DATE_LAST_CHARGE,     0x9008, 4, "date",  "Date of last charge (rounded)") \
X(DAYS_SINCE_FIRST_CHG, 0x9010, 2, "uint",  "Days since first charge") \
X(TOTAL_DISCHARGE_AS,   0x9012, 4, "uint",  "Total discharge (amp-sec)") \
X(TOTAL_DISCHARGE_WS,   0x9016, 4, "uint",  "Total discharge (watt-sec or joules)") \
X(TOTAL_CHARGE_COUNT,   0x901A, 4, "uint",  "Total charge count") \
X(DUMB_CHARGE_COUNT,    0x901E, 2, "uint",  "Dumb charge count (J2>7.1V for >=0.48s)") \
X(REDLINK_CHARGE_COUNT, 0x9020, 2, "uint",  "Redlink (UART) charge count") \
X(COMPLETED_CHARGE_CNT, 0x9022, 2, "uint",  "Completed charge count (?)") \
X(TOTAL_CHARGE_TIME,    0x9024, 4, "hhmmss","Total charging time (HH:MM:SS)") \
X(FULL_CHARGE_TIME,     0x9028, 4, "hhmmss","Time on charger whilst full (HH:MM:SS)") \
X(CHG_CELL_LT_2_5V,     0x902E, 2, "uint",  "Charge started with a cell < 2.5V") \
X(DISCHARGE_TO_EMPTY,   0x9030, 2, "uint",  "Discharge to empty") \
X(OVERHEAT_ON_TOOL,     0x9032, 2, "uint",  "Num. overheat on tool (must be > 10A)") \
X(OVERCURRENT_EVENTS,   0x9034, 2, "uint",  "Overcurrent?") \
X(LOW_VOLTAGE_EVENTS,   0x9036, 2, "uint",  "Low voltage events") \
X(LOW_VOLT_BOUNCE,      0x9038, 2, "uint",  "Low-voltage bounce? (4 flashing LEDs)")

#define MAKE_ENUM(id, addr, len, type, label) M18_##id,

typedef enum {
    M18_DATA_LABELS(MAKE_ENUM)
    M18_DATA_COUNT
} M18_DataId;
#undef MAKE_ENUM

typedef struct {
    M18_DataId id;
    uint16_t addr;
    uint8_t len;
    const char *type;
    const char *label;
} M18_DataInfo;

#define MAKE_ENTRY(id, addr, len, type, label) { M18_##id, addr, len, type, label },
static const M18_DataInfo m18_data_table[] = {
    M18_DATA_LABELS(MAKE_ENTRY)
};
#undef MAKE_ENTRY

inline const M18_DataInfo *m18_get_data_info(const M18_DataId id) {
    if (id >= M18_DATA_COUNT) return NULL;
    return &m18_data_table[id];
}

typedef struct {
    uint8_t acc;
} M18_Context;

typedef enum {
    M18_MESSAGE_SYNC,
    M18_MESSAGE_CALIBRATE,
    M18_MESSAGE_CONFIGURE,
    M18_MESSAGE_KEEPALIVE,
    M18_MESSAGE_READ,
    M18_MESSAGE_SNAPCHAT
} M18_MessageType;

typedef struct {
    uint8_t command;
    uint16_t reserved;
    uint8_t address_high;
    uint8_t address_low;
    uint8_t length;
    uint8_t checksum;
} M18_MessageRead;

typedef struct {
    uint8_t command;
    uint8_t acc;
    uint8_t magic1;
    uint16_t cutoff_current;
    uint16_t max_current1;
    uint16_t max_current2;
    uint8_t state;
    uint8_t magic2;
    uint8_t checksum;
} M18_Message_Calibration;

typedef struct {
    uint8_t command;
    uint8_t acc;
    uint8_t magic1;
    uint16_t cutoff_current;
    uint16_t max_current1;
    uint16_t max_current2;
    uint8_t state;
    uint8_t magic2;
    uint8_t checksum;
} M18_Message_Configure;

typedef struct {
    uint8_t command;
    uint8_t acc;
    uint8_t checksum;
} M18_Message_KeepAlive;

typedef struct {
    uint8_t command;
    uint8_t acc;
    uint8_t checksum;
} M18_Message_Snapchat;

typedef struct {
    uint8_t command;
    uint8_t checksum;
} M18_Message_Sync;

LIBM18_API void m18_init_message_read(M18_Message *message, M18_DataId request_id);
LIBM18_API void m18_init_message_calibration(M18_Context *context, M18_Message_Calibration *message);
LIBM18_API void m18_init_message_configure(M18_Context *context, M18_Message_Configure *message, uint8_t state);
LIBM18_API void m18_init_message_keepalive(const M18_Context *context, M18_Message_KeepAlive *message);
LIBM18_API void m18_init_message_snapchat(M18_Context *context, M18_Message_Snapchat *message);
LIBM18_API void m18_init_message_sync(M18_Context *context, M18_Message_Sync *message);

inline uint8_t m18_checksum(const uint8_t *data, const size_t len) {
    uint8_t sum = 0;
    for (size_t i = 0; i < len; i++) sum += data[i];
    return sum;
}

int8_t m18_parse_message(const uint8_t *data, size_t length, M18_Message *message);
int8_t m18_validate_checksum(const M18_Message *message);
int8_t m18_encode(const M18_Message *message, uint8_t *buffer, size_t *length);

#ifdef __cplusplus
}
#endif

#endif /* !LIBM18_H */
