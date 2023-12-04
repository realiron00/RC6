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

/*********************************************************************
 * rc6_key_schedule: Generates the key schedule for the RC6 algorithm.
 *
 * Parameters:
 *   round_key: The buffer to store the key schedule in.
 *   keys: The key to use for the key schedule.
 *   key_len: The length of the key in bytes.
 ********************************************************************/
void rc6_key_schedule(uint32_t* round_key, void* keys, uint32_t key_len)
{
    /* temp_p = RC6_P */
    uint32_t temp_p;
    temp_p = RC6_P;

    /* store the key in temp */
    uint32_t *temp_k=(uint32_t*)keys; // temp_k = key

    /* round_key[0] = RC6_p */
    round_key[0] = temp_p;

    for(uint32_t i = 1; i < key_size; i++)
    {   
        /* round_key[i] = round_key[i-1] + RC6_Q */
        round_key[i] = round_key[i-1] + RC6_Q;
    }

    uint32_t temp_x, temp_y, i, j;
    temp_x = temp_y = i = j = 0;

    for(uint32_t k = 0; k < 3 * key_size; k++)
    {
        /* temp_x = round_key[i] = (round_key[i] + x + y) <<< 3 */
        temp_x = round_key[i] = ROTL(round_key[i] + temp_x + temp_y, 3);

        /* temp_y = temp_k[j] = (temp_k[j] + x + y) <<< (x + y) */
        temp_y = temp_k[j] = ROTL(temp_k[j] + temp_x + temp_y, temp_x + temp_y);

        /* i = (i + 1) % key_size */
        i++;
        i %= key_size;

        /* j = (j + 1) % key_len */
        j++;
        j %= key_len / 4;
    }
}


/*********************************************************************
 * rc6_encrypt: Encrypts a block of data using the RC6 algorithm.
 *
 * Parameters:
 *   key: The key schedule to use for encryption.
 *   plain: The data to encrypt.
 *   cipher: The buffer to store the encrypted data in.
 ********************************************************************/
void rc6_encrypt(uint32_t* key, void* plain, void* cipher)
{
    /* Divide the plain text into 4 32-bit words */
    uint32_t temp_a, temp_b, temp_c, temp_d;
    uint32_t t0, t1;
    uint32_t* temp_key = (uint32_t*)key;
    uint32_t* temp_plain = (uint32_t*)plain;

    /* store the plain text divided into 4 32-bit words in temp */
    temp_a = temp_plain[0];
    temp_b = temp_plain[1];
    temp_c = temp_plain[2];
    temp_d = temp_plain[3];

    /* temp_b = temp_b + round_key[0] */
    temp_b += *temp_key; 
    temp_key++;

    /* temp_d = temp_d + round_key[1] */
    temp_d += *temp_key; 
    temp_key++;

    for(uint32_t i = 0; i < ROUNDS; i++)
    {
        /* t0 = (temp_b * (2 * temp_b + 1)) <<< 5 */
        t0 = ROTL(temp_b * (2 * temp_b + 1), 5);

        /* t1 = (temp_d * (2 * temp_d + 1)) <<< 5 */
        t1 = ROTL(temp_d * (2 * temp_d + 1), 5);

        /* temp_a = ((temp_a ^ t0) <<< t1) + round_key[2 * i + 2] */
        temp_a = ROTL(temp_a ^ t0, t1) + *temp_key; temp_key++;

        /* temp_c = ((temp_c^t1) <<< t0) + round_key[2 * i + 3] */
        temp_c = ROTL(temp_c ^ t1, t0) + *temp_key; temp_key++;

        /* (temp_a, temp_b, temp_c, temp_d) = (temp_b, temp_c, temp_d, temp_a) */
        t0 = temp_a;
        temp_a = temp_b;
        temp_b = temp_c;
        temp_c = temp_d;
        temp_d = t0;
    }
    
    /* temp_a = temp_a + round_key[2 * ROUNDS + 2] */
    temp_a += *temp_key; temp_key++;

    /* temp_c = temp_c + round_key[2 * ROUNDS + 3] */
    temp_c += *temp_key; temp_key++;

    /* store the cipher text in cipher */
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