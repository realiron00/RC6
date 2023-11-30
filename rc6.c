#include <stdint.h>
#include <stdio.h>

#define ROUNDS 20
#define key_size     (2*(ROUNDS+2))
#define RC6_P      0xB7E15163
#define RC6_Q      0x9E3779B9

#define UV(v) ((uint32_t)(v) & 0xFFFFFFFFUL)
#define ROTL(v, n) \
  (UV((v) << (n)) | ((v) >> (32 - (n))))
#define ROTR(v, n) ROTL(v, 32 - (n))

void rc6_key_schedule(uint32_t* key, void* round_keys, uint32_t key_len)
{
    uint32_t temp_a, temp_b;
    uint32_t temp[8];
    uint32_t *temp_rk=(uint32_t*)round_keys;

    for(uint32_t i = 0; i < key_len / 4; i++)
        temp[i] = temp_rk[i];
    
    temp_a = RC6_P;

    for(uint32_t i = 0; i < key_size; i++)
    {
        key[i] = temp_a;
        temp_a += RC6_Q;
    }
    uint32_t j, temp_key;
    temp_a = temp_b = j = temp_key = 0;

    for(uint32_t i=0; i < key_size * 3; i++)
    {
        temp_a = key[j] = ROTL(key[j] + temp_a + temp_b, 3);

        temp_b = temp[temp_key] = ROTL(temp[temp_key] + temp_a + temp_b, temp_a + temp_b);
        
        j++;
        j %= key_size;

        temp_key++;
        temp_key %= key_len / 4;
    }
}

void rc6_encrypt(uint32_t* key, void* plain, void* cipher)
{
    uint32_t temp_a, temp_b, temp_c, temp_d;
    uint32_t t0, t1;
    uint32_t* temp_key = (uint32_t*)key;
    uint32_t* temp_plain = (uint32_t*)plain;

    temp_a = temp_plain[0];
    temp_b = temp_plain[1];
    temp_c = temp_plain[2];
    temp_d = temp_plain[3];

    temp_b += *temp_key; 
    temp_key++;
    temp_d += *temp_key; 
    temp_key++;

    for(uint32_t i = 0; i < ROUNDS; i++)
    {
        t0 = ROTL(temp_b * (2 * temp_b + 1), 5);
        t1 = ROTL(temp_d * (2 * temp_d + 1), 5);

        temp_a = ROTL(temp_a ^ t0, t1) + *temp_key; temp_key++;
        temp_c = ROTL(temp_c ^ t1, t0) + *temp_key; temp_key++;

        t0 = temp_a;
        temp_a = temp_b;
        temp_b = temp_c;
        temp_c = temp_d;
        temp_d = t0;
    }

    temp_a += *temp_key; temp_key++;
    temp_c += *temp_key; temp_key++;

    uint8_t* out = (uint8_t*)cipher;
    out[3] = temp_a >> 24;
    out[2] = temp_a >> 16;
    out[1] = temp_a >> 8;
    out[0] = temp_a;
    out[7] = temp_b >> 24;
    out[6] = temp_b >> 16;
    out[5] = temp_b >> 8;
    out[4] = temp_b;
    out[11] = temp_c >> 24;
    out[10] = temp_c >> 16;
    out[9] = temp_c >> 8;
    out[8] = temp_c;
    out[15] = temp_d >> 24;
    out[14] = temp_d >> 16;
    out[13] = temp_d >> 8;
    out[12] = temp_d;
}

int main() {
    // key and plain text
    uint32_t key[key_size];
    uint8_t plain[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // key schedule
    uint8_t user_key[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    rc6_key_schedule(key, user_key, sizeof(user_key));

    printf("User Key: ");
    for(int i = 0; i <16; i++) {
        printf("%x ", user_key[i]);
    }
    printf("\n");
    printf("Key Schedule: ");
    for(int i = 0; i < 44; i++) {
        printf("%08x ", key[i]);
    }
    printf("\n");

    printf("PlainText: ");
    for(int i = 0; i < 16; i++) {
        printf("%x ", plain[i]);
    }
    printf("\n");

    // encryption
    uint8_t cipher[16];
    rc6_encrypt(key, plain, cipher);

    printf("CipherText: ");
    for(int i = 0; i < 16; i++) {
        printf("%02x", cipher[i]);
    }
    printf("\n");
}