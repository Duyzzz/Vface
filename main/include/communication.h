#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#define PACKET_SIZE 1400
typedef enum {
    CMD_START = 0,
    CMD_STOP,
    CMD_RESTART,
    CMD_SHUTDOWN,
    CMD_SET_MODE,
    CMD_GET_MODE,
    CMD_SET_PARAM,
    CMD_GET_PARAM,
    CMD_DISABLE_FLASH,
    CMD_ENABLE_FLASH,
    CMD_SET_CAPTURE,
    CMD_NONE,
} CMD_TYPE;

typedef struct{
    char *ip;
    int port;
} destination_t;
char *get_command_string(CMD_TYPE command);
void face_socket_init(int *sockfd);
void data_receive_task(void *pvParameters);

void reset_command(uint8_t num);
CMD_TYPE get_command(void);
void send_data(uint8_t *data, size_t length, destination_t *destination);
void build_rtp_header(uint8_t *header, uint16_t seq, uint32_t ts);
void send_frames_RTP(uint8_t *data, size_t length, destination_t *destination);
#endif
