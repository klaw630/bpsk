#define _CRT_SECURE_NO_WARNINGS

#include "func.h"

int main() {
  int bitrate = 10;
  int bit_per_sample = SAMPLE_RATE / bitrate;

  int read_samples_count = 0;
  int read_sample_rate = 0;
  float* read_buffer = read_wav("bpsk_output.wav", &read_samples_count, &read_sample_rate);
  if(read_buffer == NULL) {
    printf(" Error opening \"bpsk_output.wav\" file");
    return 1;
  }
  printf("Wave file read successfully\n");
  printf("Sampling rate: %d Hz\n", read_sample_rate);
  printf("Samples read: %d\n ", read_samples_count);


  int decoded_length = read_samples_count / bit_per_sample;
  int *decoded_packet = (int *)malloc(decoded_length * sizeof(int));
  if (decoded_packet == NULL) {
    printf("ERROR: Failed to allocate memory for bits!\n");
    free(read_buffer);
    return 1;
  }

  bpsk_decoding(read_buffer, read_samples_count, bitrate, decoded_packet);

  int decoded_char_count = decoded_length / 8;
  char *decoded_text = (char*)malloc((decoded_char_count + 1) * sizeof(char));
  if(decoded_text == NULL) {
    free(read_buffer);
    free(decoded_packet);
    return 1;
  }

  int bit_index = 0;
  for(int c = 0; c < decoded_char_count; c++) {
    unsigned char current_char_value = 0;
    for(int i = 7; i >= 0; i--) {
      int current_bit = decoded_packet[bit_index];
      current_char_value = current_char_value + (current_bit << i);
      bit_index++;
    }
    decoded_text[c] = (char)current_char_value;
  }

  decoded_text[decoded_char_count] = '\0';
  printf("Decoded text: %s\n", decoded_text);

  free(read_buffer);
  free(decoded_packet);
  free(decoded_text);
  
  return 0;

}
