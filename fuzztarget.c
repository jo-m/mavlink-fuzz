#include "c_library_v2/ardupilotmega/mavlink.h"
// #include "c_library_v1/ardupilotmega/mavlink.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>

uint8_t* read_file_to_buffer(const char *filepath, size_t *size) {
    FILE *file = NULL;
    struct stat st;
    uint8_t *buffer = NULL;
    size_t total_read = 0;
    size_t bytes_read = 0;

    if (stat(filepath, &st) != 0) {
        perror("stat error");
        exit(EXIT_FAILURE);
    }

    *size = st.st_size;

    if (*size > 1024 * 1024 * 10) {
        fprintf(stderr, "Error: File size exceeds the maximum allowed size of 1MB.\n");
        exit(EXIT_FAILURE);
    }

    file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("fopen error");
        exit(EXIT_FAILURE);
    }

    buffer = (uint8_t*)malloc(*size);
    if (buffer == NULL) {
        perror("malloc error");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    while (total_read < *size) {
        bytes_read = fread(buffer + total_read, 1, *size - total_read, file);
        if (bytes_read == 0) {
            if (ferror(file)) {
                perror("fread error");
                free(buffer);
                fclose(file);
                exit(EXIT_FAILURE);
            }
            break;
        }
        total_read += bytes_read;
    }

    fclose(file);

    return buffer;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <mavlink_bytes_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    size_t sz = 0;
    uint8_t *buf = read_file_to_buffer(argv[1], &sz);

    // Parse messages
    mavlink_message_t msg = {0};
    mavlink_status_t status = {0};
    // int count = 0;
    for (size_t i = 0; i < sz; i++) {
        mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status);
        // const uint8_t result = mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status);
        // if (result) {
        //     count++;
        //     printf("Parsed packet #%d at %lu\n", count, i);
        // }
    }

    free(buf);
}
