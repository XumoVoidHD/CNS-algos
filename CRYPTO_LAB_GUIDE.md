# Cryptography Lab Exam – Theory, Steps, Examples & Memory Tips

This guide covers **DES**, **S-DES**, **AES MixColumns/Inverse**, **RSA**, **Diffie-Hellman**, **MITM on DH**, and **ElGamal** with theory, step-by-step breakdown, code location, sample runs, and memory tips. Code is in separate `.c` files; no crypto libraries, no big-integer libs.

---

# 1. DES (Data Encryption Standard)

## 1.1 Theory

- **What it does:** Symmetric block cipher. Encrypts a **64-bit block** with a **56-bit key** (64-bit key with 8 parity bits) using **16 rounds** of Feistel structure.
- **Input:** 64-bit plaintext block, 64-bit key (56 effective).
- **Output:** 64-bit ciphertext block.
- **Core idea:** Each round: \(L_{i+1} = R_i\), \(R_{i+1} = L_i \oplus f(R_i, K_i)\). The function \(f\) expands \(R\) to 48 bits, XORs with round key, passes through 8 S-boxes (6→4 bits each), then P permutation.
- **Formulas:**  
  - After 16 rounds: swap \(L_{16}\) and \(R_{16}\), then apply **FP** (final permutation).  
  - Decryption: same as encryption with round keys in **reverse order** (K16…K1).

## 1.2 Step-by-Step Algorithm

1. **Key schedule:** Apply **PC1** (64→56), split into C0, D0. For rounds 1..16: left-rotate C and D by 1 or 2 (see table), concatenate, apply **PC2** → 48-bit round key.
2. **Encryption:**  
   - Apply **IP** (initial permutation) to 64-bit block → (L0, R0).  
   - For round r = 1..16:  
     - Expand R to 48 bits (**E**), XOR with K_r, pass 8×6 bits through **S1..S8** (6→4 each), concatenate to 32 bits, apply **P** → f_out.  
     - New L = R, New R = L ⊕ f_out.  
   - Swap L16↔R16, apply **FP** → ciphertext.
3. **Decryption:** Same as encryption but use keys K16, K15, …, K1.

## 1.3 Code

- File: **`des.c`**  
- Functions: `permute`, `get_bit`/`set_bit`, `key_schedule`, `f`, `round_fn`, `des_encrypt`, `des_decrypt`.  
- Tables: IP, FP, E, P, PC1, PC2, SHIFTS, S[8][4][16].

## 1.4 Example Execution

- **Key:** `13 34 57 79 9B BC DF F1`  
- **Plaintext:** `01 23 45 67 89 AB CD EF`  
- **Ciphertext:** `85 E8 13 54 0F 0A B4 05`  
- Decryption returns the original plaintext.

## 1.5 Memory Tips

- **Remember:** IP → 16 rounds (E → XOR key → S-boxes → P → L,R update) → swap → FP.  
- **Key schedule:** PC1 → split C/D → rotate (1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1) → PC2 per round.  
- **Common mistakes:** Forgetting to swap L and R before FP; using same key order for decrypt (must reverse).  
- **Tables:** At least remember structure: IP/FP 8×8, E expands 32→48 (with overlaps), P 32 bits, 8 S-boxes 4×16 each.

---

# 2. S-DES (Simplified DES)

## 2.1 Theory

- **What it does:** Educational 8-bit block cipher, 10-bit key, **2 rounds**.
- **Input:** 8-bit plaintext, 10-bit key.  
- **Output:** 8-bit ciphertext.  
- **Core idea:** Same Feistel idea as DES but with P10, P8, one left shift then two shifts for keys; 2 rounds; 2 S-boxes (S0, S1) 4→2 bits each; E/P expands 4→8.

## 2.2 Step-by-Step Algorithm

1. **Key schedule:** Apply **P10** to 10-bit key → (5,5) split. Left shift both by 1 → apply **P8** → K1. Left shift both by 2 more (total 3 on each) → **P8** → K2.  
2. **Encryption:** Apply **IP** → (L0, R0) 4 bits each.  
   - Round 1: L1=R0, R1=L0⊕f(R0,K1).  
   - Round 2: L2=R1, R2=L1⊕f(R1,K2).  
   - Output = **IP⁻¹**(R2‖L2).  
3. **Decryption:** Same structure with **K2** first, then **K1**.

## 2.3 Code

- File: **`sdes.c`**  
- Tables: P10, P8, IP, IPinv, EP, P4, S0, S1.  
- Functions: `key_schedule`, `f_k`, `sdes_encrypt`, `sdes_decrypt`.

## 2.4 Example Execution

- **Key (10 bits):** 0x2E8 → bytes 0xBA, 0x80.  
- **Plaintext:** 0x65.  
- **Ciphertext:** 0x3A.  
- Decryption restores plaintext.

## 2.5 Memory Tips

- **Flow:** P10 → LS-1 → P8=K1; LS-2 more → P8=K2. Encrypt: IP → 2 rounds (f with K1, then K2) → swap → IP⁻¹.  
- **f:** E/P 4→8 → XOR key → S0 (first 4 bits), S1 (last 4 bits) → P4.  
- **Common mistake:** Using K1 then K2 for decrypt (must be K2 then K1).

---

# 3. AES MixColumns

## 3.1 Theory

- **What it does:** MixColumns is a step in AES that mixes each **column** of the 4×4 state using a fixed matrix over **GF(2⁸)**.  
- **Input/Output:** 4×4 state (16 bytes).  
- **Core idea:** Multiply state column by the AES matrix. Multiplication in GF(2⁸) with modulus \(x^8+x^4+x^3+x+1\) (0x11B).  
- **Formulas:**  
  - **xtime(a)** = 2·a in GF(2⁸): left shift; if high bit set, XOR 0x1B.  
  - Matrix row 0: [02 03 01 01], so out[0] = 02·s0 ⊕ 03·s1 ⊕ 01·s2 ⊕ 01·s3, etc.

## 3.2 Step-by-Step Algorithm

1. For each column \(c\) of the state (4 bytes s0..s3):  
2. Compute four outputs using the matrix (each output is dot product of row with column):  
   - out0 = xtime(s0)⊕xtime(s1)⊕s1⊕s2⊕s3  
   - out1 = s0⊕xtime(s1)⊕xtime(s2)⊕s2⊕s3  
   - out2 = s0⊕s1⊕xtime(s2)⊕xtime(s3)⊕s3  
   - out3 = xtime(s0)⊕s0⊕s1⊕s2⊕xtime(s3)  
3. Replace column by (out0, out1, out2, out3).

## 3.3 Code

- File: **`aes_mixcolumns.c`**  
- Functions: `xtime`, `gf_mul`, `mix_columns`, `inv_mix_columns`.  
- Inverse matrix uses coefficients **0x0E, 0x0B, 0x0D, 0x09** (first row).

## 3.4 Example Execution

- **Original state:** e.g. first column 63,7C,77,F2; …  
- **After MixColumns:** C7, F0, FC, 51 (first column); …  
- **After InvMixColumns:** Restores original state.

## 3.5 Memory Tips

- **xtime:** shift left; if bit 8 set, XOR 0x1B.  
- **Forward matrix:** 02 03 01 01 / 01 02 03 01 / 01 01 02 03 / 03 01 01 02.  
- **Inverse:** 0E 0B 0D 09 / 09 0E 0B 0D / 0D 09 0E 0B / 0B 0D 09 0E.  
- **Common mistake:** Forgetting reduction in xtime or in gf_mul.

---

# 4. AES Inverse MixColumns

## 4.1 Theory

- **What it does:** Inverse of MixColumns; used in decryption.  
- **Input/Output:** 4×4 state.  
- **Core idea:** Multiply each column by the **inverse** of the MixColumns matrix. Each entry is multiplied in GF(2⁸) by 0x09, 0x0B, 0x0D, or 0x0E (powers of 2 and combinations).

## 4.2 Step-by-Step Algorithm

1. For each column, compute four outputs using the inverse matrix (e.g. first row 0E,0B,0D,09):  
   out0 = 0E·s0 ⊕ 0B·s1 ⊕ 0D·s2 ⊕ 09·s3, and similarly for other rows.  
2. Use **gf_mul(byte, 0x09/0x0B/0x0D/0x0E)** for each multiplication in GF(2⁸).

## 4.3 Code

- Same file **`aes_mixcolumns.c`**, function **`inv_mix_columns`**, helper **`gf_mul`**.

## 4.4 Example

- See MixColumns example: applying InvMixColumns after MixColumns returns the original state.

## 4.5 Memory Tips

- Memorize the four coefficients **09, 0B, 0D, 0E** and that each row is a rotation.  
- 0x09 = 8+1, 0x0B = 8+2+1, 0x0D = 8+4+1, 0x0E = 8+4+2 (helps with gf_mul via xtime).

---

# 5. RSA

## 5.1 Theory

- **What it does:** Public-key encryption. Encrypt with public key (e, n), decrypt with private key (d, n).  
- **Input:** Message \(m\) (integer &lt; n).  
- **Output:** Ciphertext \(c = m^e \bmod n\); decryption \(m = c^d \bmod n = m^{ed} \bmod n\).  
- **Core idea:** Choose \(e,d\) so that \(ed \equiv 1 \pmod{\phi(n)}\); then \(m^{ed} \equiv m \pmod n\) (Euler).  
- **Formulas:** \(n=pq\), \(\phi(n)=(p-1)(q-1)\), \(d = e^{-1} \bmod \phi(n)\).

## 5.2 Step-by-Step Algorithm

1. **Key generation:** Pick primes p, q; n = pq; φ = (p-1)(q-1). Choose e with gcd(e,φ)=1; compute d = e⁻¹ mod φ (Extended Euclidean). Public: (e,n), Private: (d,n).  
2. **Encryption:** c = m^e mod n (fast exponentiation).  
3. **Decryption:** m = c^d mod n.

## 5.3 Code

- File: **`rsa.c`**  
- Functions: `gcd`, `extended_gcd`, `mod_inverse`, `mod_mul`, `mod_pow`, `rsa_keygen`, `rsa_encrypt`, `rsa_decrypt`.

## 5.4 Example Execution

- p=61, q=53 → n=3233, φ=3120, e=17, d=2753.  
- m=123 → c=855 → decrypt → 123.

## 5.5 Memory Tips

- **Order:** n = pq → φ → choose e → d = e⁻¹ mod φ. Encrypt: c = m^e mod n; Decrypt: m = c^d mod n.  
- **Extended Euclidean:** recursion: gcd(b, a mod b); then x = y1, y = x1 - (a/b)*y1.  
- **Common mistakes:** Using φ instead of n in encryption/decryption; forgetting m < n.

---

# 6. Diffie-Hellman (DH)

## 6.1 Theory

- **What it does:** Key agreement. Two parties agree on a **shared secret** over a public channel.  
- **Input:** Public p (prime), g (generator); Alice private a, Bob private b.  
- **Output:** Shared secret K = g^(ab) mod p.  
- **Core idea:** Alice sends A = g^a mod p, Bob sends B = g^b mod p. Alice computes B^a, Bob computes A^b; both get g^(ab).

## 6.2 Step-by-Step Algorithm

1. Agree on prime p and generator g (primitive root mod p).  
2. Alice: pick private a, send A = g^a mod p.  
3. Bob: pick private b, send B = g^b mod p.  
4. Alice: K = B^a mod p. Bob: K = A^b mod p. K is the shared secret.

## 6.3 Code

- File: **`diffie_hellman.c`**  
- Functions: `mod_mul`, `mod_pow`, `is_primitive_root` (optional), main demonstrates exchange.

## 6.4 Example Execution

- p=23, g=5, a=6, b=15.  
- A=8, B=19.  
- Shared secret = 2 (both sides).

## 6.5 Memory Tips

- **Formula:** K = g^(ab) mod p; each side computes the other’s public value to the power of their own private value.  
- **Common mistake:** Confusing who computes A^b vs B^a (Alice uses B^a, Bob uses A^b).

---

# 7. MITM Attack on Diffie-Hellman

## 7.1 Theory

- **What it does:** Attacker sits between Alice and Bob, replaces A and B with own value (e.g. g^x).  
- **Result:** Alice shares secret with attacker (g^xa), Bob shares with attacker (g^xb). Alice and Bob do **not** share a secret with each other.

## 7.2 Step-by-Step Algorithm

1. Alice sends A = g^a; attacker intercepts and sends to Bob A' = g^x (attacker’s value).  
2. Bob sends B = g^b; attacker intercepts and sends to Alice B' = g^x.  
3. Alice computes K_A = (B')^a = g^(xa). Bob computes K_B = (A')^b = g^(xb).  
4. Attacker knows x, so computes (B')^a from overheard B' and a (if obtained) or (A')^b; in the basic MITM, attacker shares K_A with Alice and K_B with Bob and can decrypt/relay.

## 7.3 Code

- File: **`dh_mitm.c`**  
- Clearly prints Alice’s key, Bob’s key, and attacker’s keys with both.

## 7.4 Example Execution

- p=23, g=5, a=6, b=15, x=3.  
- A'=B'=10.  
- Alice’s key = 6, Bob’s key = 5 (different). Attacker has 6 with Alice and 5 with Bob.

## 7.5 Memory Tips

- **Idea:** Replace both A and B with g^x. Two different shared secrets (Alice–attacker, Bob–attacker).  
- **Defense:** Authentication (e.g. certificates) so parties verify they are talking to the right entity.

---

# 8. ElGamal Encryption

## 8.1 Theory

- **What it does:** Public-key encryption based on DH-like structure.  
- **Input:** Message m (integer mod p), public key (p, g, y), random k.  
- **Output:** Ciphertext (c1, c2) = (g^k mod p, m·y^k mod p).  
- **Core idea:** Decryption: m = c2 · (c1^x)^(-1) mod p, because c1^x = g^(xk) and y^k = g^(xk), so c2/m = y^k.

## 8.2 Step-by-Step Algorithm

1. **Key generation:** Prime p, generator g. Private key x; public key y = g^x mod p.  
2. **Encryption:** Pick random k. c1 = g^k mod p, c2 = m·y^k mod p. Send (c1, c2).  
3. **Decryption:** s = c1^x mod p; m = c2 · s⁻¹ mod p (modular inverse).

## 8.3 Code

- File: **`elgamal.c`**  
- Functions: `mod_mul`, `mod_pow`, `extended_gcd`, `mod_inverse`, `elgamal_keygen`, `elgamal_encrypt`, `elgamal_decrypt`.

## 8.4 Example Execution

- p=23, g=5, x=6 → y=8. m=10, k=3 → (c1,c2)=(10,14). Decrypt → 10.

## 8.5 Memory Tips

- **Encrypt:** (g^k, m·y^k). **Decrypt:** c2 · (c1^x)⁻¹ mod p.  
- **Common mistake:** Forgetting the modular inverse; using c2/c1^x without doing inverse mod p.

---

# 9. Comparison Table

| Criterion        | Symmetric (DES, AES)   | Asymmetric (RSA, ElGamal)   |
|-----------------|------------------------|-----------------------------|
| Keys             | One shared key         | Public + private pair       |
| Block/stream     | DES/AES: block         | RSA/ElGamal: not block in same sense |
| Speed            | Fast                   | Slower (large exponents)    |
| Use              | Bulk encryption        | Key exchange, signatures    |
| Key distribution | Must be secret         | Public key can be public    |

| Algorithm    | Type        | Key size / params   | Main operation              |
|-------------|-------------|---------------------|-----------------------------|
| DES         | Symmetric   | 56-bit effective    | 16 rounds Feistel           |
| S-DES       | Symmetric   | 10-bit              | 2 rounds Feistel            |
| AES MixCol  | Symmetric   | N/A                 | GF(2⁸) matrix multiply      |
| RSA         | Asymmetric  | n = pq, e, d        | m^e mod n, c^d mod n        |
| DH          | Key exchange| p, g, a, b          | g^a, g^b, shared g^(ab)     |
| ElGamal     | Asymmetric  | p, g, x, y = g^x    | (g^k, m·y^k); decrypt with x|

**Time complexity (high level):**  
- DES/AES: O(block_size × rounds); linear in message length.  
- RSA/DH/ElGamal: Modular exponentiation O(log(exp) × log(n)²) per operation; key generation and encryption/decryption are costlier than symmetric encryption.

---

# 10. Viva Questions (Examples)

1. **Why does DES use 16 rounds?** Balance between security and performance; fewer rounds are weaker to differential/linear cryptanalysis.  
2. **What is the role of S-boxes in DES?** Provide non-linearity; without them the cipher would be linear and easy to break.  
3. **What is the irreducible polynomial in AES MixColumns?** \(x^8 + x^4 + x^3 + x + 1\) (0x11B).  
4. **Why is RSA secure?** Factoring n = pq is believed hard; knowing e and n does not reveal d without φ(n).  
5. **What is the discrete log problem in DH?** Given g, p, and g^a mod p, finding a is hard.  
6. **How does MITM break DH?** Attacker replaces both A and B with g^x and shares a secret with each party separately.  
7. **Difference between DH and ElGamal:** DH is key agreement (shared secret); ElGamal uses the same math for encryption (ciphertext is a pair).  
8. **What is Extended Euclidean Algorithm used for in RSA?** Computing d = e⁻¹ mod φ(n).  
9. **Why does ElGamal need a random k per message?** Reusing k leaks the private key; same k gives same c1 and ratio c2/m reveals m.  
10. **Block vs stream cipher:** Block encrypts fixed-size blocks (DES, AES); stream encrypts bit/byte stream (e.g. RC4). DES and AES are block ciphers.

---

**File summary:**  
- **des.c** – DES encrypt/decrypt  
- **sdes.c** – S-DES encrypt/decrypt  
- **aes_mixcolumns.c** – MixColumns and InvMixColumns  
- **rsa.c** – RSA keygen, encrypt, decrypt  
- **diffie_hellman.c** – DH key exchange  
- **dh_mitm.c** – MITM on DH  
- **elgamal.c** – ElGamal keygen, encrypt, decrypt  

All code is self-contained, no external crypto or big-integer libraries, suitable for lab exam implementation and explanation.
