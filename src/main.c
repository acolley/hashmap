#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t rotate_left_uint32(uint32_t value, int amount) {
    return (value << amount) | (value >> (sizeof(value) * 8 - amount));
}

uint8_t* md5(uint8_t* source, int length) {
    /**
     * source: The source data.
     * length: Length of the source data in bytes.
     */
    // https://en.wikipedia.org/wiki/MD5
    static int s[64] = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
    };
    
    uint32_t K[64];
    for (int i = 0; i < 64; i++) {
        K[i] = floor(pow(2, 32) * fabs(sin(i + 1)));
    }
    
    uint8_t* data = source;
    int length_bits = length * 8;
    int remainder = (512 / 8) - length % (512 / 8);
    if (remainder > 0) {
        int new_length = length + remainder;
        uint8_t* new_data = malloc(new_length);
        memcpy(new_data, data, length);
        
        new_data[length] = (uint8_t)0b10000000;
        for (int i = length + 1; i < new_length - 8; i++) {
            new_data[i] = 0;
        }
        // Now we have 8 bytes remaining. (This isn't necessarily true).
        uint64_t append = floor(fmod(length_bits, pow(2.0, 64.0)));
        new_data[new_length - 8] = append & 0xff;
        new_data[new_length - 7] = (append >> 8) & 0xff;
        new_data[new_length - 6] = (append >> 16) & 0xff;
        new_data[new_length - 5] = (append >> 24) & 0xff;
        new_data[new_length - 4] = (append >> 32) & 0xff;
        new_data[new_length - 3] = (append >> 40) & 0xff;
        new_data[new_length - 2] = (append >> 48) & 0xff;
        new_data[new_length - 1] = (append >> 56) & 0xff;
        
        length = new_length;
        data = new_data;
    }
    
    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe;
    uint32_t d0 = 0x10325476;
    int chunksize = 512 / 8;
    for (int c = 0; c < length; c += chunksize) {
        uint32_t A = a0;
        uint32_t B = b0;
        uint32_t C = c0;
        uint32_t D = d0;
        
        uint32_t* M = (uint32_t*)(data + c);
        
        uint32_t F = 0;
        uint32_t g = 0;
        for (int i = 0; i < 64; i++) {
            if (i < 16) {
                F = (B & C) | (~B & D);
                g = i;
            } else if (i >= 16 && i < 32) {
                F = (D & B) | (~D & C);
                g = (5 * i + 1) % 16;
            } else if (i >= 32 && i < 48) {
                F = B ^ C ^  D;
                g = (3 * i + 5) % 16;
            } else {
                F = C ^ (B | ~D);
                g = (7 * i) % 16;
            }
            
            uint32_t temp_d = D;
            D = C;
            C = B;
            B = B + rotate_left_uint32(A + F + K[i] + M[g], s[i]);
            A = temp_d;
        }
        
        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }
    
    uint32_t* digest = malloc(16);
    digest[0] = a0;
    digest[1] = b0;
    digest[2] = c0;
    digest[3] = d0;
    
    return (uint8_t*)digest;
}

int main() {
    char* data = "The quick brown fox jumps over the lazy dog";
    //char* data = "";
    uint8_t* digest = md5((uint8_t*)data, strlen(data));
    for (int i = 0; i < 16; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
}
