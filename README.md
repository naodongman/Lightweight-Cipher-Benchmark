# Lightweight Cipher Benchmark  
Ascon · PRESENT · Simon · Speck · AES-256

This project benchmarks five symmetric encryption algorithms across two hardware platforms:

- A Windows 11 desktop PC (Intel Core i7-13700KF)
- A Raspberry Pi 3 Model A+

Algorithms evaluated:

- Ascon AEAD (NIST Lightweight Cryptography winner)
- PRESENT
- Simon
- Speck
- AES-256 (ECB mode, Tiny-AES-C)

The goal is to compare their behavior under constrained and unconstrained environments in terms of:

- Encryption speed (latency)
- Memory usage (runtime RSS / working set)
- Code size (.text segment)
- Lightweight suitability for IoT and embedded devices

---

## 1. Repository Structure

A typical layout of this project is:

```text
CryptoBenchmark/
├─ ascon-c/                     # Ascon AEAD implementation
│   └─ crypto_aead/...
├─ present-master/              # PRESENT cipher implementation
│   ├─ include/
│   └─ src/
├─ Simon_Speck_Ciphers-master/  # Simon & Speck implementations
│   └─ C/
├─ tiny-AES-c-master/           # Tiny-AES-C (AES-256 ECB)
│   └─ aes.c, aes.h
├─ wrapper.c                    # C wrappers (used on Raspberry Pi)
├─ main.cpp                     # Benchmark driver
└─ README.md
Each algorithm folder contains only the core encryption .c and .h files extracted from the original GitHub projects, placed into a unified structure for compilation and benchmarking.

2. Algorithms Included
The following algorithms are benchmarked:

Ascon AEAD
Authenticated encryption with associated data (AEAD), standardized by NIST as a lightweight cryptography scheme.

PRESENT
Ultra-lightweight block cipher designed for highly constrained hardware environments.

Simon
Lightweight block cipher family designed by NSA, based on a Feistel structure with simple bitwise operations.

Speck
Lightweight ARX (Add-Rotate-Xor) block cipher family designed by NSA, optimized for software implementations.

AES-256 (ECB, Tiny-AES-C)
Standard AES block cipher, 256-bit key, used here in ECB mode as a non-lightweight baseline.

All algorithms are instantiated in 128-bit security configurations where applicable (for example, Ascon-128, Simon/Speck 128/64, AES-256).

3. Build Instructions
3.1 Windows 11 (Visual Studio)
Create a new C++ Console Application project (for example, CryptoBenchmark).

Add the following source and header files to the project:

From ascon-c/crypto_aead/asconaead128/opt64: Ascon AEAD .c and .h files

From present-master/src and present-master/include: present.c, present.h, and required headers

From Simon_Speck_Ciphers-master/C: simon.c, speck.c, simon.h, speck.h, and cipher_constants.h

From tiny-AES-c-master: aes.c, aes.h

main.cpp

Configure the additional include directories in:

Project Properties → C/C++ → Additional Include Directories
Add paths to:

ascon-c/crypto_aead/asconaead128/opt64

present-master/include

Simon_Speck_Ciphers-master/C

tiny-AES-c-master

Set the configuration to:

Configuration: Release

Platform: x64

C/C++ → Optimization: /O2

Build and run the project.
The program will print timing, memory usage, and code size information for each algorithm.

3.2 Raspberry Pi 3 Model A+ (Linux / Raspbian)
On the Raspberry Pi, the project is built using GCC and G++.

Install the required toolchain:

bash
Copy code
sudo apt update
sudo apt install build-essential
Compile C sources to object files:

bash
Copy code
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

bash
Copy code
g++ -std=c++17 -O2 \
  main.cpp \
  aead.o permutations.o present.o simon.o speck.o aes.o wrapper.o \
  -o CryptoBenchmark
Run the benchmark:

bash
Copy code
./CryptoBenchmark
The program will print per-algorithm performance results to the terminal.

4. Benchmark Methodology
4.1 Test Input and Interface
All algorithms are tested under identical conditions:

Plaintext: a fixed 16-byte block of zeros

cpp
Copy code
uint8_t pt[16] = {0};
Key: zero-initialized key of the appropriate length for each algorithm

Unified wrapper functions: each algorithm is called through a wrapper of the form:

c
Copy code
void wrap_ascon(const uint8_t* pt, uint8_t* ct, const uint8_t* key);
void wrap_present(const uint8_t* pt, uint8_t* ct, const uint8_t* key);
void wrap_simon(const uint8_t* pt, uint8_t* ct, const uint8_t* key);
void wrap_speck(const uint8_t* pt, uint8_t* ct, const uint8_t* key);
void wrap_aes(const uint8_t* pt, uint8_t* ct, const uint8_t* key);
These wrappers hide algorithm-specific details and provide a unified interface for the benchmark driver.

4.2 Speed Measurement
Encryption speed is measured as follows:

Timer: std::chrono::high_resolution_clock (nanosecond precision)

For each algorithm:

Perform RUNS iterations of a single block encryption

Measure the elapsed time for each call and accumulate the total time

Compute the average latency:

Average Latency
=
Total Time
RUNS
(
nanoseconds
)
Average Latency= 
RUNS
Total Time
​
 (nanoseconds)
No I/O or printing is performed inside the timing loop to avoid polluting the measurement.

4.3 Memory Measurement
After each algorithm run, a helper function getProcessMemoryKB() is used to measure the process memory usage:

On Windows:

Uses the Win32 API GetProcessMemoryInfo

Reads the WorkingSetSize field (in KB)

On Linux (Raspberry Pi):

Parses /proc/self/status

Reads the VmRSS field (in KB)

Because most algorithms do not allocate significant dynamic memory, the RSS values remain stable after the first few calls. Ascon may show a slight increase during the very first runs due to code and state being paged in.

4.4 Code Size Measurement
Code size is measured by instructing the linker to generate a .map file:

On Windows: enable "Generate Map File" in the linker settings (Linker → Debugging → Generate Map File).

On Linux: pass -Wl,-Map=CryptoBenchmark.map to the linker.

The .text segment size for each algorithm’s object file is then extracted from the .map file. This reflects the compiled machine code size of each implementation.

5. Example Results
5.1 Windows 11 (Intel Core i7-13700KF)
Typical output on the desktop platform:

text
Copy code
Ascon  code:  9760 B, total: 135100 ns, avg:  1351 ns, mem: 4140 KB
PRESENT code: 6384 B, total: 343400 ns, avg:  3434 ns, mem: 4364 KB
Simon  code: 6016 B, total:  52300 ns, avg:   523 ns, mem: 4372 KB
Speck  code: 4784 B, total:  36000 ns, avg:   360 ns, mem: 4380 KB
AES256 code:11696 B, total: 130400 ns, avg:  1304 ns, mem: 4384 KB
5.2 Raspberry Pi 3 Model A+
Typical output on the Raspberry Pi:

text
Copy code
Ascon  code:  9760 B, total:  55105 ns, avg:   551 ns, mem: 2404 KB
PRESENT code:  6384 B, total: 715782 ns, avg:  7157 ns, mem: 2704 KB
Simon  code:  6016 B, total: 257186 ns, avg:  2571 ns, mem: 2704 KB
Speck  code:  4784 B, total: 126877 ns, avg:  1268 ns, mem: 2704 KB
AES256 code: 11696 B, total: 337395 ns, avg:  3373 ns, mem: 2704 KB
Exact numbers will vary depending on compiler version, optimization flags, OS version, and background load. The key value of this benchmark is in the relative comparison between algorithms on the same platform.

6. High-Level Interpretation
On the x86-64 desktop:

Speck shows the lowest average encryption latency.

Simon is slightly slower than Speck but still very fast.

Ascon and AES-256 are comparable in latency.

PRESENT is the slowest in pure software due to its many rounds and nibble-based S-box design.

On the ARM-based Raspberry Pi:

Ascon achieves the best performance, benefiting from its 64-bit bit-sliced design and optimized permutation.

Speck remains fast, followed by Simon and AES-256.

PRESENT is again the slowest.

In terms of code size:

Speck has the smallest .text size, followed by Simon and PRESENT.

Ascon is moderate in code size but provides AEAD.

AES-256 has the largest code size due to more complex transformations.

In terms of security (based on published cryptanalysis, not re-implemented here):

Ascon: no practical attacks on the full-round version; strong AEAD security.

PRESENT, Simon, Speck: full-round variants remain secure; only reduced rounds are attackable with non-practical complexity.

AES-256: remains the most mature and widely trusted block cipher, with no practical attacks on full 14-round AES-256.

7. Limitations
Only encryption performance is benchmarked (no decryption benchmarks).

No hardware acceleration (such as AES-NI or ARM Crypto Extensions) is used.

Security analysis is not implemented in code; it is based on existing literature.

