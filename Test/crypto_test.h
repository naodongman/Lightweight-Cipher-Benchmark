#pragma once
#pragma once
#include <stdint.h>

// 所有算法统一用 128‐bit 明文／密文、256‐bit 密钥
// （根据实际算法可调整长度，但接口保持一致）
void ascon_encrypt(
    const uint8_t* plaintext,
    uint8_t* ciphertext,
    const uint8_t* key
);

void present_encrypt(
    const uint8_t* plaintext,
    uint8_t* ciphertext,
    const uint8_t* key
);

void simon_encrypt(
    const uint8_t* plaintext,
    uint8_t* ciphertext,
    const uint8_t* key
);

void speck_encrypt(
    const uint8_t* plaintext,
    uint8_t* ciphertext,
    const uint8_t* key
);

void aes256_encrypt(
    const uint8_t* plaintext,
    uint8_t* ciphertext,
    const uint8_t* key
);
