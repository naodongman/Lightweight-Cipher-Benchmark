// crypto_test.cpp
#include <iostream>
#include <chrono>
#include <cstring>       // for memcpy

extern "C" {
    // Ascon AEAD
#include "api.h"           // 声明 crypto_aead_encrypt
// Present block cipher
#include "present.h"       // 声明 present_encrypt_block
// Simon/Speck
#include "simon.h"         // 声明 Simon_Init, Simon_Encrypt
#include "speck.h"         // 声明 Speck_Init, Speck_Encrypt
// AES256 (Tiny-AES-C)
#include "aes.h"           // 声明 AES_init_ctx, AES_ECB_encrypt
}

#include "crypto_test.h"        // 你的统一接口声明

using namespace std;
using namespace std::chrono;

int main() {
    const int RUNS = 10;
    uint8_t pt[16] = { 0 };
    uint8_t ct[16];
    uint8_t key[32] = { 0 };

    struct Algo { const char* name; void(*fn)(const uint8_t*, uint8_t*, const uint8_t*); };
    Algo algos[] = {
        { "Ascon",   ascon_encrypt   },
        { "PRESENT", present_encrypt },
        { "Simon",   simon_encrypt   },
        { "Speck",   speck_encrypt   },
        { "AES256",  aes256_encrypt  }
    };

    for (auto& a : algos) {
        uint64_t total = 0;
        for (int i = 0; i < RUNS; ++i) {
            auto t1 = high_resolution_clock::now();
            a.fn(pt, ct, key);
            auto t2 = high_resolution_clock::now();
            total += duration_cast<microseconds>(t2 - t1).count();
        }
        cout << a.name << " avg: " << (total / RUNS) << " μs\n";
    }
    return 0;
}
