#pragma once

typedef enum {
    ESPNOW_ATTRIBUTE_F1_CONTROL =           0x0400,
    ESPNOW_ATTRIBUTE_F1_LIMITER =           0x0401,
} espnow_f1_attribute_t;

void initialize_remote_control(void);
