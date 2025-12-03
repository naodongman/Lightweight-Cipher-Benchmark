Markdown# Lightweight Cipher Benchmark
**Ascon · PRESENT · Simon · Speck · AES-256**

![Platform Windows](https://img.shields.io/badge/Platform-Windows_11_x64-blue) ![Platform RPi](https://img.shields.io/badge/Platform-Raspberry_Pi_3-red) ![Language C/C++](https://img.shields.io/badge/Language-C%2F%2B%2B-green)

This project benchmarks five symmetric encryption algorithms across two distinct hardware platforms: a high-performance **Windows 11 Desktop (Intel Core i7-13700KF)** and a constrained **Raspberry Pi 3 Model A+**.

The primary goal is to evaluate their behavior in constrained vs. unconstrained environments, focusing on:
* **Latency:** Encryption speed (nanoseconds).
* **Memory Footprint:** Runtime RSS / Working Set.
* **Code Size:** compiled `.text` segment size.
* **Suitability:** Viability for IoT and embedded devices.

---

## 1. Repository Structure

The project organizes core algorithm implementations into a unified structure for cross-compilation.

```text
CryptoBenchmark/
├── ascon-c/                    # Ascon AEAD implementation
│   └── crypto_aead/...
├── present-master/             # PRESENT cipher implementation
│   ├── include/
│   └── src/
├── Simon_Speck_Ciphers-master/ # Simon & Speck implementations
│   └── C/
├── tiny-AES-c-master/          # Tiny-AES-C (AES-256 ECB)
│   └── aes.c, aes.h
├── wrapper.c                   # Unified C wrappers (for easy benchmarking)
├── main.cpp                    # Primary Benchmark Driver
└── README.md
Note: Each algorithm folder contains only the necessary source (.c) and header (.h) files extracted from their original repositories.2. Algorithms IncludedAll algorithms are instantiated in 128-bit security configurations where applicable (e.g., Ascon-128, Simon/Speck 128/64).AlgorithmTypeDescriptionKey FeaturesAscon AEADAEAD SuiteNIST Lightweight Cryptography Winner. Authenticated encryption with associated data.Sponge construction, highly versatile.PRESENTBlock CipherUltra-lightweight cipher designed for extremely constrained hardware (RFID tags).SPN structure, bit-oriented permutation.SimonBlock CipherDesigned by the NSA for hardware performance.Feistel network, optimized for hardware efficiency.SpeckBlock CipherDesigned by the NSA for software performance.ARX (Add-Rotate-Xor) structure, fast in software.AES-256Block CipherThe industry standard (Tiny-AES-C implementation).Used here in ECB mode as a non-lightweight baseline.3. Build Instructions3.1 Windows 11 (Visual Studio)Create Project: Create a new C++ Console Application (e.g., CryptoBenchmark).Add Source Files: Import the .c and .h files from the subdirectories listed in the repository structure, plus main.cpp.Include Directories: Add the following paths to Project Properties → C/C++ → Additional Include Directories:ascon-c/crypto_aead/asconaead128/opt64present-master/includeSimon_Speck_Ciphers-master/Ctiny-AES-c-masterConfiguration:Configuration: ReleasePlatform: x64Optimization: /O2 (Maximize Speed)Linker: Enable "Generate Map File" (to measure code size).Build & Run: The program will output timing and memory stats to the console.3.2 Raspberry Pi 3 Model A+ (Linux / Raspbian)Ensure build-essential is installed:Bashsudo apt update && sudo apt install build-essential


Step 1: Compile C sources to object filesBashgcc -std=c11 -O2 -c \
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


Step 2: Compile Driver and LinkBashg++ -std=c++17 -O2 \
  main.cpp \
  aead.o permutations.o present.o simon.o speck.o aes.o wrapper.o \
  -Wl,-Map=CryptoBenchmark.map \
  -o CryptoBenchmark


Step 3: RunBash./CryptoBenchmark


4. Benchmark Methodology4.1 Test Input & InterfacePlaintext: Fixed 16-byte block of zeros (uint8_t pt[16] = {0};).Key: Zero-initialized key of appropriate length for the specific algorithm.Wrapper: A unified interface (void wrap_algorithm(...)) shields the driver from implementation-specific API differences.4.2 Speed MeasurementEncryption speed is derived using std::chrono::high_resolution_clock. We perform $N$ iterations (RUNS) of a single block encryption. The average latency is calculated as:$$Latency_{avg} = \frac{T_{total}}{N_{runs}}$$Note: No I/O operations occur inside the timing loop to ensure measurement accuracy.4.3 Memory & Code SizeMemory (RAM):Windows: GetProcessMemoryInfo (WorkingSetSize).Linux: Parses /proc/self/status (VmRSS).Code Size (Flash/Disk):Extracted from the linker-generated .map file. This represents the actual .text segment size of the compiled object files.5. Example Results5.1 Windows 11 (Intel Core i7-13700KF)Typical high-performance environment output:PlaintextAscon   code:  9760 B, total: 135100 ns, avg:  1351 ns, mem: 4140 KB
PRESENT code:  6384 B, total: 343400 ns, avg:  3434 ns, mem: 4364 KB
Simon   code:  6016 B, total:  52300 ns, avg:   523 ns, mem: 4372 KB
Speck   code:  4784 B, total:  36000 ns, avg:   360 ns, mem: 4380 KB
AES256  code: 11696 B, total: 130400 ns, avg:  1304 ns, mem: 4384 KB

5.2 Raspberry Pi 3 Model A+Typical constrained embedded environment output:PlaintextAscon   code:  9760 B, total:  55105 ns, avg:   551 ns, mem: 2404 KB
PRESENT code:  6384 B, total: 715782 ns, avg:  7157 ns, mem: 2704 KB
Simon   code:  6016 B, total: 257186 ns, avg:  2571 ns, mem: 2704 KB
Speck   code:  4784 B, total: 126877 ns, avg:  1268 ns, mem: 2704 KB
AES256  code: 11696 B, total: 337395 ns, avg:  3373 ns, mem: 2704 KB
6. High-Level InterpretationPerformance AnalysisSpeck is the fastest purely software-optimized cipher on x86, boasting the lowest latency and smallest code size.Ascon demonstrates exceptional performance on the ARM-based Raspberry Pi, outperforming others thanks to its 64-bit bit-sliced design and optimized permutations.PRESENT is consistently the slowest in software. This is expected, as PRESENT is engineered for hardware silicon area efficiency, not software instruction cycles.Architecture Suitabilityx86-64 Desktop: Speck and Simon dominate in raw speed.ARM Embedded: Ascon is the clear winner for modern embedded applications, offering a balance of high speed and AEAD security features.Security ContextAscon: Strong security guarantees; no practical attacks on full rounds.AES-256: The gold standard for security, though heavier in code size.Simon/Speck: Secure in full-round variants, though historically controversial due to their origin (NSA).7. LimitationsScope: Only encryption is benchmarked; decryption performance is not measured.Acceleration: No hardware acceleration (AES-NI or ARM Crypto Extensions) is utilized; these are pure C software implementations.Security: This project benchmarks performance only. It does not validate cryptographic correctness against test vectors, nor does it perform side-channel attack analysis.AES Mode: AES is implemented in ECB mode for benchmarking simplicity. ECB is not secure for production data as it does not hide data patterns.
