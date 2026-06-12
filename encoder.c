#define _CRT_SECURE_NO_WARNINGS

#include "func.h"

#define PI 3.14159265358979323846
#define SAMPLE_RATE 8000

char text[] = "Hello, this is a BPSK test.";

int main() {
    int bitrate = 10;
    int bit_per_sample = SAMPLE_RATE / bitrate; // 8000 / 10 = 800 samples per bit
    int char_count = strlen(text);
    int packet_length = char_count * 8;
    int total_samples = packet_length * bit_per_sample;
    int* packet = (int*)malloc(packet_length * sizeof(int));
    float *signal_buffer = (float *)malloc(total_samples * sizeof(float));
    if(packet == NULL || signal_buffer == NULL) {
    printf("ERROR: Failed to allocate memory!\n");
    if(packet != NULL) free(packet);
    if(signal_buffer != NULL) free(signal_buffer);
    return 1;
    }

    int bit_index = 0;
    for(int c = 0; c < char_count; c++) {
        char current_char = text[c];
        for(int i = 7; i >= 0; i--) {
            packet[bit_index] = (current_char >> i) & 1;
            bit_index++;
        }
    }

    bpsk_encoding(packet, packet_length, signal_buffer);
    write_wav("bpsk_output.wav", signal_buffer, total_samples, SAMPLE_RATE);

    printf("BPSK signal saved to bpsk_output.wav\n");
    free(signal_buffer);
    free(packet);
    return 0;
}