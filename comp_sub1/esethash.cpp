#include <iostream>
#include <string>
#include "sha1.h"
#include "sha512.h"

const wchar_t pw[] = L"123456";

const unsigned char hard_bytes[] = "\xE4\x62\x2C\xDB\x5F\xA8\x45\x1E\xA9\xBE\x3D\xB6\xC3\x2F\x06\xA5"
                                   "\xDD\x51\xB1\x9E\x1D\x47\x4A\x12\x5C\xDC\x7B\xAB\xB4\x07\xCB\xC4";

void print_buffer(uint8_t* buf, size_t size)
{
    for (size_t i = 0; i < size; i++)
        printf("%.2X ", buf[i] & 0xff);

    putchar('\n');
}

int main()
{
    ///////////////
    // Setup
    ///////////////
    
    SHA1_CTX sha;

    // Final hash
    uint8_t hash[20], fin_hash[40];
    memset(hash, 0, 20);

    // Buffers repeatedly used during hashing loop
    uint8_t pw_digest[3][64];

    // SHA-1 digest of widechar-format password if necessary to shrink it,
    // otherwise use as is. Padded to 64 bytes with 00
    size_t pw_len = wcslen(pw);
    memset(pw_digest, 0, 64);

    if (pw_len > 32)
    {
            SHA1Init(&sha);
            SHA1Update(&sha, (uint8_t*)pw, pw_len * 2);
            SHA1Final(pw_digest[0], &sha);
    }
    else
    {
        memcpy(pw_digest, pw, pw_len * 2);
    }

    // XOR-encoded versions of password digest
    for (size_t i = 0; i < 64; i++)
    {
        char c = pw_digest[0][i] ^ 0x36;
        pw_digest[1][i] = c;
        pw_digest[2][i] = c ^ 0x6A;
    }

    puts("Password digest");
    print_buffer(pw_digest[0], 64);

    puts("Password digest ^ 0x36");
    print_buffer(pw_digest[1], 64);

    puts("Password digest ^ 0x36 ^ 0x6A");
    print_buffer(pw_digest[2], 64);

    // Intermediate digests
    uint8_t intermed[2][64];

    SHA1Init(&sha);
    SHA1Update(&sha, pw_digest[1], 64);
    SHA1Update(&sha, hard_bytes, 32);
    SHA1Update(&sha, (const uint8_t *) "\x00\x00\x00\x01", 4);

    for (size_t loop_count = 0; loop_count < 10000; loop_count++)
    {
        SHA1Final(intermed[0], &sha);

        SHA1Init(&sha);
        SHA1Update(&sha, pw_digest[2], 64);
        SHA1Update(&sha, intermed[0], 20);
        SHA1Final(intermed[1], &sha);

        // Update hash
        for (size_t i = 0; i < 20; i++)
            hash[i] ^= intermed[1][i];

        SHA1Init(&sha);
        SHA1Update(&sha, pw_digest[1], 64);
        SHA1Update(&sha, intermed[1], 20);
    }

    printf("hash0:\t");
    print_buffer(hash, 20);

    memcpy(fin_hash, hash, 20);
    memset(hash, 0, 20);

    SHA1Init(&sha);
    SHA1Update(&sha, pw_digest[1], 64);
    SHA1Update(&sha, hard_bytes, 32);
    SHA1Update(&sha, (const uint8_t*)"\x00\x00\x00\x02", 4);

    for (size_t loop_count = 0; loop_count < 10000; loop_count++)
    {
        SHA1Final(intermed[0], &sha);

        SHA1Init(&sha);
        SHA1Update(&sha, pw_digest[2], 64);
        SHA1Update(&sha, intermed[0], 20);
        SHA1Final(intermed[1], &sha);

        // Update hash
        for (size_t i = 0; i < 20; i++)
            hash[i] ^= intermed[1][i];

        SHA1Init(&sha);
        SHA1Update(&sha, pw_digest[1], 64);
        SHA1Update(&sha, intermed[1], 20);
    }

    printf("hash1:\t");
    print_buffer(hash, 20);
    memcpy(fin_hash + 20, hash, 32 - 20);

    printf("combined hash:\t");
    print_buffer(fin_hash, 32);

    // SHA-512 hashing
    uint8_t sha512_digest[64];
    SHA512Digest(sha512_digest, fin_hash, 32);

    puts("\nSHA512");
    print_buffer(sha512_digest, 64);
    
    // Prepare hash buffer for next stage
    // This splits it up into 48-bit numbers, with the most significant bits
    // of the least significant bytes constrained on the first two
    //uint8_t mod_sha512_digest[40];
    //*((uint64_t*) &mod_sha512_digest[0]) = *((uint64_t*) sha512_digest[0]) & 0xffffffffffff

    return 0;
}