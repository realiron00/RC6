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
    uint32_t i, j, k, A, B, L[8], *kptr=(uint32_t*)round_keys;

    for(i = 0; i < key_len / 4; i++)
        L[i] = kptr[i];
    
    A = RC6_P;

    for(i = 0; i < key_size; i++)
    {
        key[i] = A;
        A += RC6_Q;
    }

    A = B = i = j = 0;

    for(k=0; k < key_size * 3; k++)
    {
        A = key[i] = ROTL(key[i] + A + B, 3);

        B = L[j] = ROTL(L[j] + A + B, A + B);
        
        i++;
        i %= key_size;

        j++;
        j %= key_len / 4;
    }
}

void rc6_encrypt(uint32_t* key, void* input, void* output)
{
    uint32_t A, B, C, D, T0, T1, i;
    uint32_t* k = (uint32_t*)key;
    uint32_t* in = (uint32_t*)input;
    uint32_t* out = (uint32_t*)output;

    A = in[0];
    printf("A: %08x\n", A);
    B = in[1];
    printf("B: %08x\n", B);
    C = in[2];
    printf("C: %08x\n", C);
    D = in[3];
    printf("D: %08x\n", D);

    B += *k; k++;
    printf("B: %08x\n", B);
    D += *k; k++;
    printf("D: %08x\n", D);

    for(i = 0; i < ROUNDS; i++)
    {
        T0 = ROTL(B * (2 * B + 1), 5);
        T1 = ROTL(D * (2 * D + 1), 5);

        A = ROTL(A ^ T0, T1) + *k; k++;
        C = ROTL(C ^ T1, T0) + *k; k++;

        T0 = A;
        A = B; 
        B = C; 
        C = D; 
        D = T0;
    }

    A += *k; k++;
    C += *k; k++;

    printf("A: %08x\n", A);
    printf("B: %08x\n", B);
    printf("C: %08x\n", C);
    printf("D: %08x\n", D);

    // uint32에 있는 값을 4개의 uint8_t로 나누어 저장
}

void test_rc6() {
    // 키와 평문 초기화
    uint32_t key[key_size];
    uint8_t plain[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // 키 스케줄링
    uint8_t user_key[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    rc6_key_schedule(key, user_key, sizeof(user_key));

    printf("User Key: ");
    for(int i = 0; i < 4; i++) {
        printf("%08x ", user_key[i]);
    }
    printf("\n");
    printf("Key Schedule: ");
    for(int i = 0; i < 44; i++) {
        printf("%08x ", key[i]);
    }
    printf("\n");

    printf("PlainText: ");
    for(int i = 0; i < 4; i++) {
        printf("%08x ", plain[i]);
    }
    printf("\n");

    // 암호화 수행
    uint8_t cipher[16];
    rc6_encrypt(key, plain, cipher);

    printf("CipherText: ");
    for(int i = 0; i < 4; i++) {
        printf("%08x ", cipher[i]);
    }
    printf("\n");
}

int main() {
    test_rc6();
    return 0;
}