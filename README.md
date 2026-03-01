# Cryptography Lab Exam – Build & Run

No external crypto or big-integer libraries. Plain C, GCC.

## Compile and run

```bash
# DES
gcc -o des des.c -Wall && ./des

# S-DES
gcc -o sdes sdes.c -Wall && ./sdes

# AES MixColumns / Inverse MixColumns
gcc -o aes_mixcolumns aes_mixcolumns.c -Wall && ./aes_mixcolumns

# RSA
gcc -o rsa rsa.c -Wall && ./rsa

# Diffie-Hellman
gcc -o diffie_hellman diffie_hellman.c -Wall && ./diffie_hellman

# MITM on DH
gcc -o dh_mitm dh_mitm.c -Wall && ./dh_mitm

# ElGamal
gcc -o elgamal elgamal.c -Wall && ./elgamal
```

## Files

| File               | Algorithm(s)                    |
|--------------------|---------------------------------|
| `des.c`            | DES (full 16 rounds, key schedule, S-boxes) |
| `sdes.c`           | S-DES (2 rounds, P10/P8, 2 S-boxes) |
| `aes_mixcolumns.c` | AES MixColumns + Inverse MixColumns (GF(2^8)) |
| `rsa.c`            | RSA (GCD, EEA, mod inverse, mod exp, keygen, encrypt/decrypt) |
| `diffie_hellman.c` | Diffie-Hellman key exchange     |
| `dh_mitm.c`        | MITM attack on DH               |
| `elgamal.c`        | ElGamal encryption               |
| `CRYPTO_LAB_GUIDE.md` | Theory, steps, examples, memory tips, comparison table, viva |

Full theory, step-by-step breakdown, and memory tips are in **CRYPTO_LAB_GUIDE.md**.
