#include "communication.h"
#include "esp_log.h"
#include "errno.h"

CMD_TYPE currentCommand = CMD_NONE;

void face_socket_init(int *sockfd) {
    // Initialize socket
    if (*sockfd < 0) {
        perror("Socket creation failed");
        return;
    }

    // Set up server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // Port number
    if (inet_pton(AF_INET, "192.168.12.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(*sockfd);
        return;
    }

    // Bind the socket to the address
    if (bind(*sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(*sockfd);
        return;
    }

    // Listen for incoming connections
    if (listen(*sockfd, 5) < 0) {
        perror("Listen failed");
        close(*sockfd);
        return;
    }

    printf("Server listening on port %d\n", ntohs(server_addr.sin_port));
}
char buffer[50];
void data_receive_task(void *pvParameters) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        vTaskDelete(NULL);
        return;
    }
    face_socket_init(&sockfd);
    while (true) {
        // Accept incoming connections
        ESP_LOGI("RECEIVE", "Waiting for incoming packet");
        int client_sockfd = accept(sockfd, NULL, NULL);
        if (client_sockfd < 0) {
            perror("Accept failed");
            continue;
        }

        // Receive data from the client
        ssize_t bytes_received = recv(client_sockfd, buffer, sizeof(buffer), 0);
        ESP_LOGI("RECEIVE", "Receive %d bytes", bytes_received);
        if (bytes_received < 0) {
            perror("Receive failed");
            close(client_sockfd);
            continue;
        }

        // Process the received data
        // printf("Received data: %.*s\n", (int)bytes_received, buffer);
        currentCommand = buffer[1]; // Assuming the first byte is the command type
        // ESP_LOGI("RECEIVE", "Received command: %d", currentCommand);
        for (int i = 0; i < bytes_received; i++) {
            printf("%c", buffer[i]);
        }
        printf("\n");
        // Close the client socket
        close(client_sockfd);
    }
}

CMD_TYPE get_command(void) {
    if (currentCommand != CMD_NONE) {
        CMD_TYPE command = currentCommand;
        currentCommand = CMD_NONE; // Reset the command after processing
        return command;
    }
    return CMD_NONE;
}

char *get_command_string(CMD_TYPE command) { return buffer; }

void reset_command(uint8_t num) { memset(buffer, 0, num); }

void send_data(uint8_t *data, size_t length, destination_t *destination) {
    if (data == NULL || length == 0 || destination == NULL) {
        ESP_LOGE("SEND", "Invalid data or destination");
        return;
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ESP_LOGE("SEND", "Socket creation failed");
        return;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(destination->port);
    if (inet_pton(AF_INET, destination->ip, &server_addr.sin_addr) <= 0) {
        ESP_LOGE("SEND", "Invalid IP address");
        close(sockfd);
        return;
    }
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE("SEND", "Connection failed");
        close(sockfd);
        return;
    }
    ssize_t bytes_sent = send(sockfd, data, length, 0);
    if (bytes_sent < 0) {
        ESP_LOGE("SEND", "Send failed");
        close(sockfd);
        return;
    }
    close(sockfd);
    ESP_LOGI("SEND", "Socket closed");
}

static uint16_t sequence_number = 0;
static uint32_t timestamp = 0;

// Create RTP Header
void build_rtp_header(uint8_t *header, uint16_t seq, uint32_t ts) {
    header[0] = 0x80; // Version 2
    header[1] = 26;   // Payload Type 26 = JPEG
    header[2] = seq >> 8;
    header[3] = seq & 0xFF;
    header[4] = ts >> 24;
    header[5] = ts >> 16;
    header[6] = ts >> 8;
    header[7] = ts & 0xFF;
    header[8] = 0x12; // SSRC - arbitrary
    header[9] = 0x34;
    header[10] = 0x56;
    header[11] = 0x78;
}

void send_frames_RTP(uint8_t *data, size_t length, destination_t *destination) {
    if (data == NULL || length == 0 || destination == NULL) {
        ESP_LOGE("SEND", "Invalid data or destination");
        return;
    }
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        ESP_LOGE("SEND", "Socket creation failed");
        return;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(destination->port);
    if (inet_pton(AF_INET, destination->ip, &server_addr.sin_addr) <= 0) {
        ESP_LOGE("SEND", "Invalid IP address");
        close(sockfd);
        return;
    }
    size_t offset = 0;
    size_t remaining = length;
    ESP_LOGW("SEND", "Timestamp: %lu", timestamp);
    while (remaining > 0) {
        size_t chunk_size = remaining > PACKET_SIZE ? PACKET_SIZE : remaining;
        ESP_LOGI("SEND", "Sending chunk of size: %zu", chunk_size);
        uint8_t rtp_header[12];
        build_rtp_header(rtp_header, sequence_number++, timestamp);
        // ESP_LOGI("RTP header", "version: %d, payload type: %d, seq: %d, timestamp: %d", rtp_header[0], rtp_header[1], rtp_header[2] << 8 | rtp_header[3], rtp_header[4] << 24 | rtp_header[5] << 16 | rtp_header[6] << 8 | rtp_header[7]);
        ssize_t bytes_sent = sendto(sockfd, rtp_header, sizeof(rtp_header), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (bytes_sent < 0) {
            ESP_LOGE("SEND", "Send failed, reason: %s", strerror(errno));
            close(sockfd);
            return;
        }
        bytes_sent = sendto(sockfd, data + offset, chunk_size, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (bytes_sent < 0) {
            ESP_LOGE("SEND", "Send failed, reason: %s", strerror(errno));
            close(sockfd);
            return;
        }
        remaining -= bytes_sent;
        offset += bytes_sent;
    }
    timestamp += 3000; // Increment timestamp for next frame
    close(sockfd);
}