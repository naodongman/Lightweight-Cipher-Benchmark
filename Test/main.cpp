// main.cpp
#include <iostream>
#include <chrono>
#include <string.h>        // for memcpy

extern "C" {
    // Ascon AEAD
#include "crypto_aead.h"           // ascon-c/crypto_aead/crypto_aead.h
// PRESENT
#include "present.h"               // present-master/include/present.h
// Simon/Speck
#include "cipher_constants.h"      // 定义 enum mode_t, cipher_config_t
#include "simon.h"                 // Simon_Init, Simon_Encrypt
#include "speck.h"                 // Speck_Init, Speck_Encrypt
// Tiny-AES-C
#include "aes.h"                   // AES_init_ctx, AES_ECB_encrypt
}
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#include <iomanip>

struct MemUsage {
    const char* name;
    size_t      code;       // .text 段大小
    size_t      rdata;      // .rdata 段大小
    size_t      data;       // .data 段大小
    size_t      bss;        // .bss  段大小
};
static constexpr MemUsage mems[] = {
  { "Ascon",   9760,  840, 128, 0 },
  { "PRESENT", 6384,    0,   0, 0 },
  { "Simon",   6016,    0,   0, 0 },
  { "Speck",   4784,    0,   0, 0 },
  { "AES256", 11696,    0,   0, 0 },
};

static size_t getProcessMemoryKB() {
    PROCESS_MEMORY_COUNTERS info;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info))) {
        return info.WorkingSetSize / 1024;
    }
    return 0;
}
// —— 五个算法的包装函数 ——
// Ascon
void wrap_ascon(const uint8_t* pt, uint8_t* ct, const uint8_t* key) {
    unsigned long long clen;
    uint8_t nonce[16] = { 0 };      // 全零 nonce，仅作对比实验用
    crypto_aead_encrypt(
        ct, &clen,
        pt, 16,
        NULL, 0,              // 关联数据 AD 长度 0
        NULL,                  // nsec 保留参数，仍可传 NULL
        nonce,                 // 一定要传合法的 nonce 指针
        key
    );
}
// PRESENT
static void wrap_present(const uint8_t* pt, uint8_t* ct, const uint8_t* key) {
    // 分组长度用 literal 8，或改为 PRESENT_STATE_SIZE
    memcpy(ct, pt, 8);
    present_encrypt(ct, key);
}

// Simon
static void wrap_simon(const uint8_t* pt, uint8_t* ct, const uint8_t* key) {
    SimSpk_Cipher ctx;
    Simon_Init(&ctx, cfg_128_64, (mode_t)ECB, (void*)key, NULL, NULL);
    Simon_Encrypt(ctx, pt, ct);
}

// Speck
static void wrap_speck(const uint8_t* pt, uint8_t* ct, const uint8_t* key) {
    SimSpk_Cipher ctx;
    Speck_Init(&ctx, cfg_128_64, (mode_t)ECB, (void*)key, NULL, NULL);
    Speck_Encrypt(ctx, pt, ct);
}

// AES-256 ECB
static void wrap_aes(const uint8_t* pt, uint8_t* ct, const uint8_t* key) {
    struct AES_ctx a;
    AES_init_ctx(&a, key);
    memcpy(ct, pt, 16);
    AES_ECB_encrypt(&a, ct);
}

int main() {
    using namespace std;
    using namespace chrono;

    const int RUNS = 100;
    uint8_t pt[16] = { 0 }, ct[16 + 16], key[32] = { 0 };

    struct Algo { const char* name; void(*fn)(const uint8_t*, uint8_t*, const uint8_t*); };
    Algo algos[] = {
        { "Ascon",  wrap_ascon },
        { "PRESENT",wrap_present },
        { "Simon",  wrap_simon },
        { "Speck", wrap_speck },
        { "AES256", wrap_aes }
    };
    size_t idx = 0;
    for (auto& a : algos) {
        uint64_t sum = 0;
        for (int i = 0; i < RUNS; ++i) {
            auto t1 = high_resolution_clock::now();
            a.fn(pt, ct, key);
            auto t2 = high_resolution_clock::now();
            sum += duration_cast<nanoseconds>(t2 - t1).count();
        }
        size_t memKB = getProcessMemoryKB();
        size_t codeSize = mems[idx].code;   // 从静态数组里取代码段大小

        cout << a.name
            << " code: " << setw(5) << codeSize << " B,"
            << " total: " << setw(7) << sum << " ns,"
            << " avg: " << setw(5) << (sum / RUNS) << " ns,"
            << " mem: " << setw(5) << memKB << " KB\n";

        ++idx;
    }

    return 0;
}
