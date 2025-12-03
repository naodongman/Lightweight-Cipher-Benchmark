将每个下载的项目解压至单独文件夹，例如：

D:\CryptoTest\algorithms\
├─ ascon-c\
├─ present-cipher\
├─ Simon_Speck_Ciphers\
└─ tiny-AES-c\


导入源码文件

导入对应算法源码文件夹中的.c或.cpp与.h文件。

导入后的结构举例：

CryptoBenchmark
│
├─ Ascon
│    ├─ ascon.c
│    ├─ ascon.h
│    └─ ...
├─ PRESENT
│    ├─ present.c
│    ├─ present.h
│    └─ ...
├─ Simon_Speck
│    ├─ simon.c
│    ├─ speck.c
│    ├─ simon.h
│    └─ speck.h
└─ AES256
     ├─ aes.c
     ├─ aes.h
     └─ ...


封装调用对应算法代码。例如crypto_test.cpp：


在main.cpp中实现如下性能测试
