Lightweight Cipher Benchmark (Ascon, PRESENT, Simon, Speck, AES-256)

This project benchmarks and compares five symmetric encryption algorithms across different platforms:

Ascon AEAD

PRESENT

Simon

Speck

AES-256 (ECB mode, Tiny-AES-C)

The goal is to evaluate how these algorithms perform under constrained IoT hardware (Raspberry Pi 3 Model A+) and a desktop-class machine (Windows 11 + Intel i7), focusing on:

Speed (encryption latency)

Memory usage (runtime RSS / working set)

Code size (compiled segment).text

And to interpret these results in light of known security analyses from existing literature.

1. Project Overview

This repository integrates multiple open-source implementations of lightweight ciphers into a unified benchmarking framework:

All algorithms are wrapped behind a common C/C++ interface.

The same driver () calls each algorithm in a loop with identical inputs.main.cpp

Timing, memory usage, and code size are measured and compared on:

A Windows 11 desktop (Visual Studio, MSVC).

A Raspberry Pi 3 Model A+ (GCC on Raspbian).

This allows an apples-to-apples comparison of the algorithms in terms of performance and resource consumption.

2. Algorithms Included

The project currently benchmarks the following algorithms:

Ascon AEAD
NIST lightweight cryptography finalist providing authenticated encryption (AEAD).

PRESENT
Ultra-lightweight block cipher designed for highly constrained devices.

Simon
NSA-designed lightweight block cipher (Feistel structure).

Speck
NSA-designed ARX (Add-Rotate-Xor) lightweight block cipher.

AES-256 (ECB, Tiny-AES-C)
Standard AES implementation used here as a non-lightweight baseline for comparison.

Each algorithm is used in a 128-bit security configuration where applicable (e.g., Ascon-128, Simon/Speck 128/64, AES-256).

3. Repository Structure

A typical folder layout looks like this:

CryptoBenchmark/
├─ ascon-c/                     # Ascon AEAD implementation
│   └─ crypto_aead/...
├─ present-master/              # PRESENT cipher implementation
│   ├─ include/
│   └─ src/
├─ Simon_Speck_Ciphers-master/  # Simon & Speck implementations
│   └─ C/
├─ tiny-AES-c-master/           # Tiny-AES-C (AES-256 ECB)
│   └─ aes.c, aes.h, ...
├─ main.cpp                     # Cross-platform benchmark driver
├─ wrapper.c                    # C wrappers (used on Raspberry Pi)
└─ (optional) .map / build files


Note: Folder names may differ slightly depending on how you extracted the archives.
Just make sure the include paths in your build system match this structure.

4. Prerequisites
4.1 Desktop (Windows 11)

Visual Studio 2022 (or later)

MSVC compiler (installed with Desktop development with C++ workload)

4.2 Raspberry Pi 3 Model A+

Raspbian / Raspberry Pi OS (e.g., Bullseye)

GCC / G++ toolchain (e.g., , gcc 10.2g++ 10.2)

Basic build tools:

sudo apt update
sudo apt install build-essential

5. Building & Running
5.1 Build on Windows (Visual Studio)

Create a new C++ Console Application (e.g., ).CryptoBenchmark

Add the source files to the project:

main.cpp

Ascon source: relevant and files from .c.hascon-c/crypto_aead/...

PRESENT: , and any required headers from present.cpresent.hpresent-master/include / src

Simon/Speck: , , , from simon.cspeck.csimon.hspeck.hSimon_Speck_Ciphers-master/C

AES: , from aes.caes.htiny-AES-c-master

Set include directories (Project → Properties → C/C++ → Additional Include Directories), e.g.:

ascon-c/crypto_aead/asconaead128/opt64

present-master/include

Simon_Speck_Ciphers-master/C

tiny-AES-c-master

Set configuration to Release and optimization:

Configuration: Release x64

C/C++ → Optimization → /O2

Build and run the project.
The console will print timing and memory statistics for each algorithm.

5.2 Build on Raspberry Pi

Copy the repository to your Raspberry Pi, e.g.:

git clone https://github.com/<your-username>/<your-repo>.git
cd <your-repo>


Compile the C sources into object files:

gcc -std=c11 -O2 -c \
  ascon-c/crypto_aead/asconaead128/opt64/aead.c \
  ascon-c/crypto_aead/asconaead128/opt64/permutations.c \
  present-master/src/present.c \
  Simon_Speck_Ciphers-master/C/simon.c \
  Simon_Speck_Ciphers-master/C/speck.c \
  tiny-AES-c-master/aes.c \
  wrapper.c \
  -I./ascon-c/crypto_aead/asconaead128/opt64 \
  -I./ascon-c/tests \
  -I./present-master/include \
  -I./present-master/src \
  -I./present-master/conf \
  -I./Simon_Speck_Ciphers-master/C \
  -I./tiny-AES-c-master


Compile and link the C++ benchmark driver:

g++ -std=c++17 -O2 \
  main.cpp \
  aead.o permutations.o present.o simon.o speck.o aes.o wrapper.o \
  -o CryptoBenchmark


Run the benchmark:

./CryptoBenchmark


You should see output lines similar to:

Ascon  code:  9760 B, total:  55105 ns, avg:   551 ns, mem: 2404 KB
PRESENT code: 6384 B, total: 715782 ns, avg:  7157 ns, mem: 2704 KB
Simon  code: 6016 B, total: 257186 ns, avg:  2571 ns, mem: 2704 KB
Speck  code: 4784 B, total: 126877 ns, avg:  1268 ns, mem: 2704 KB
AES256 code:11696 B, total: 337395 ns, avg:  3373 ns, mem: 2704 KB

6. Benchmark Methodology

The benchmark is intentionally simple and reproducible:

Plaintext:
A fixed 16-byte all-zero block:

uint8_t pt[16] = {0};


Key:
A fixed all-zero key of appropriate size (e.g., 128 or 256 bits) for each algorithm.

Unified Wrapper Functions:
Each algorithm is wrapped in a function of the form:

void wrap_ascon(const uint8_t* pt, uint8_t* ct, const uint8_t* key);
void wrap_present(const uint8_t* pt, uint8_t* ct, const uint8_t* key);
void wrap_simon(const uint8_t* pt, uint8_t* ct, const uint8_t* key);
void wrap_speck(const uint8_t* pt, uint8_t* ct, const uint8_t* key);
void wrap_aes(const uint8_t* pt, uint8_t* ct, const uint8_t* key);


Timing (latency):
Each algorithm is called in a loop of iterations:RUNS

auto t1 = high_resolution_clock::now();
algo.fn(pt, ct, key);
auto t2 = high_resolution_clock::now();
total_ns += duration_cast<nanoseconds>(t2 - t1).count();


Average latency is computed as:

Average Latency
=
Total Time
RUNS
(
in nanoseconds
)
Average Latency=
RUNS
Total Time
	​

(in nanoseconds)

Memory Usage:
After each algorithm run, a helper function getProcessMemoryKB() reads:

Windows: GetProcessMemoryInfo → WorkingSetSize

Linux (Pi): /proc/self/status → VmRSS

This gives an approximate runtime memory footprint (in KB).

Code Size:
Code size is obtained from the linker map file (.map) by extracting the size of the .text segment contributed by each algorithm’s object file.

7. Example Output (Windows)

On a Windows 11 desktop (i7-13700KF), typical results look like:

Ascon  code:  9760 B, total: 135100 ns, avg:  1351 ns, mem: 4140 KB
PRESENT code: 6384 B, total: 343400 ns, avg:  3434 ns, mem: 4364 KB
Simon  code: 6016 B, total:  52300 ns, avg:   523 ns, mem: 4372 KB
Speck  code: 4784 B, total:  36000 ns, avg:   360 ns, mem: 4380 KB
AES256 code:11696 B, total: 130400 ns, avg:  1304 ns, mem: 4384 KB


Note: Exact numbers will vary depending on compiler, optimization level, OS, and background load.
The important part is the relative comparison between algorithms on the same platform.

8. Interpretation (High-Level)

Speck tends to be the fastest on x86-64 (Windows desktop) due to its very simple ARX round function.

Ascon performs extremely well on ARM (Raspberry Pi), often achieving the lowest latency while also providing AEAD (encryption + authentication).

PRESENT is typically the slowest in software due to its many rounds and nibble-based S-box design.

Simon/Speck have very small code size; Speck in particular has the smallest .text footprint.

AES-256 is larger and slower in pure software but remains the most mature and widely standardized choice, and benefits greatly from hardware AES acceleration (not used in these tests).

9. Limitations

Only encryption performance is currently measured (no decryption benchmarks).

No attempt is made to exploit hardware acceleration (e.g., AES-NI, ARM Crypto Extensions).

Security analysis is not re-implemented here; instead, it relies on published cryptographic literature (see your paper/report for details).

10. License

MIT License

Copyright (c) 2025 nate zhao

[full license text]
在main.cpp中实现如下性能测试
