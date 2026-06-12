#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define PI 3.14159265358979323846
#define SAMPLE_RATE 8000


struct WAVHeader
{
    char      riff_id[4];
    uint32_t  riff_size;
    char      wave_id[4];
    char      fmt_id[4];
    uint32_t  fmt_size;
    uint16_t  audio_format;
    uint16_t  num_channels;
    uint32_t  sample_rate;
    uint32_t  byte_rate;
    uint16_t  block_align;
    uint16_t  bits_per_sample;
    char      data_id[4];
    uint32_t  data_size;

};

void bpsk_encoding(const int *bits, int packet_length, float *output_buffer) {
    float amplitude = 1.0f;
    float frequency = 1000.0f;
    float phase = 0.0f; // initial phase
    float t = 0.0f; // time variable
    int bitrate = 10;
    float bit_per_sample = SAMPLE_RATE / bitrate; // number of samples per bit
    // s(t) = amplitude * (cos(2 * PI * frequency * t + phase)) <- I used this one
    // s(t) = sin(2 * PI * frequency * t + phase)

    int count = 0;
    for(int b = 0; b < packet_length; b++) {
        int current_bit = bits[b];
        if(current_bit == 1) {
        // encode logic for bit 1
        // keep original phase
        phase = 0.0f;
        } else {
        // encode logic for bit 0
        // invert the phase by adding pi
        phase = PI;
        } 
        for(int i = 0; i < bit_per_sample; i++) {
        float time = (float)i / SAMPLE_RATE;

        float s_t = amplitude * sinf(2.0f * PI * frequency * time + phase);
        output_buffer[count] = s_t;
        count++;
        }
    }
}

void write_wav(const char *filename, const float *samples, int num_samples, int sample_rate) {
    struct WAVHeader header;
    
    int bytes_per_sample = 2;
    uint32_t total_data_size = num_samples * bytes_per_sample;

    for(int i = 0; i < 4; i++) {
      header.riff_id[i] = "RIFF"[i];
      header.wave_id[i] = "WAVE"[i];
      header.fmt_id[i]  = "fmt "[i];
      header.data_id[i] = "data"[i];
    }
    
    header.fmt_size = 16;
    header.audio_format = 1;
    header.num_channels = 1;
    header.sample_rate = sample_rate;
    header.bits_per_sample = 16;
    
    header.block_align = header.num_channels * bytes_per_sample;
    header.byte_rate = header.sample_rate * header.block_align;
    header.data_size = total_data_size;
    header.riff_size = total_data_size + 36;

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("ERROR: Can't create filename %s\n", filename);
        return;
    }

    fwrite(&header, sizeof(struct WAVHeader), 1, fp);

    for (int i = 0; i < num_samples; i++) {
        float current_sample = samples[i];
        
        if (current_sample > 1.0f)  current_sample = 1.0f;
        if (current_sample < -1.0f) current_sample = -1.0f;

        int16_t pcm_sample = (int16_t)(current_sample * 32767.0f);
        
        fwrite(&pcm_sample, sizeof(int16_t), 1, fp);
    }

    fclose(fp);
}

float* read_wav(const char *filename, int *num_samples, int *sample_rate) {
    struct WAVHeader header;

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("ERROR: Can't open file %s\n", filename);
        return NULL;
    }

    if (fread(&header, sizeof(struct WAVHeader), 1, fp) != 1) {
        printf("ERROR: File too short or damaged.\n");
        fclose(fp);
        return NULL;
    }

    if (header.audio_format != 1 || header.num_channels != 1 || header.bits_per_sample != 16) {
        printf("ERROR: Supporting only 16bit Mono PCM WAV files.\n");
        fclose(fp);
        return NULL;
    }

    *sample_rate = header.sample_rate;
    
    *num_samples = header.data_size / 2;

    float *samples = (float *)malloc((*num_samples) * sizeof(float));
    if (samples == NULL) {
        printf("ERROR: Can't allocate memory.\n");
        fclose(fp);
        return NULL;
    }

    int16_t pcm_sample;
    for (int i = 0; i < *num_samples; i++) {
        if (fread(&pcm_sample, sizeof(int16_t), 1, fp) != 1) {
            printf("WARNING: File ended unexpectedly at %d. sample.\n", i);
            *num_samples = i; 
            break;
        }

        samples[i] = (float)pcm_sample / 32767.0f;
    }

    fclose(fp);
    return samples;
}


int bpsk_decoding(const float *samples, int num_samples, int bitrate, int *output_bits) {
    int bit_per_sample = SAMPLE_RATE / bitrate;
    int expected_bits = num_samples / bit_per_sample;
    int count = 0;

    for(int b = 0; b < expected_bits; b++) {
        float integrator_sum = 0.0;
        for(int i = 0; i < bit_per_sample; i++) {
            float time = (float)count / SAMPLE_RATE;
            float reference = sinf(2.0f * PI * 1000.0f * time);
            float mixed = samples[count] * reference;
            integrator_sum += mixed;
            count++;
        }
        if(integrator_sum > 0.0) {
            output_bits[b] = 1;
        } else {
            output_bits[b] = 0;
        }
    }
    return expected_bits;
}