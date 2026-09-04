#ifndef __DATA_POOL_H
#define __DATA_POOL_H
#include <stdint.h>

typedef struct
{
    float Target;
    float Actual;
    float Output;
    float P;
    float I;
    float D;
}PidMonitor_t;

typedef struct
{
    uint16_t raw[4];
    float volt[4];
}AdcMonitor_t;

typedef struct
{
    float speed;
    float voltage;
    float angle;
}SensorMonitor_t;

#define VOFA_RX_STR_LEN 32
typedef struct
{
    uint8_t valid;
    char str[VOFA_RX_STR_LEN];
}VofaRxMonitor_t;

extern PidMonitor_t      PID_mon;
extern AdcMonitor_t       ADC_mon;
extern SensorMonitor_t   Sensor_mon;
extern VofaRxMonitor_t   Vofa_rx_mon;

#endif
