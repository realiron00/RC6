

// RC6 in C
// Odzhan

#include "rc6.h"

void rc6_setkey(RC6_KEY *key, void *K, uint32_t keylen)
{  
  uint32_t i, j, k, A, B, L[8], *kptr=(uint32_t*)K; 
  
  // initialize L with key
  for (i=0; i<keylen/4; i++) {
    L[i] = kptr[i];
  }
  
  A=RC6_P;
  
  // initialize S with constants
  for (i=0; i<RC6_KR; i++) {
    key->x[i] = A;
    A += RC6_Q;
  }
  
  A=B=i=j=k=0;
  
  // mix with key
  for (; k < RC6_KR*3; k++)
  { 
    A = key->x[i] = ROTL32(key->x[i] + A+B, 3);  
    B = L[j]      = ROTL32(L[j] + A+B, A+B);
    
    i++;
    i %= RC6_KR;
    
    j++;
    j %= keylen/4;
  } 
}

void rc6_crypt(RC6_KEY *key, void *input, void *output, int enc)
{
  rc6_blk *in, *enc_plain;
  uint32_t A, B, C, D, T0, T1, i;
  uint32_t *k=(uint32_t*)key->x;
  
  in =(rc6_blk*)input;
  enc_plain=(rc6_blk*)output;
  
  // load plaintext/ciphertext
  A=in->v32[0];
  printf("A: %08x\n", A);
  B=in->v32[1];
  printf("B: %08x\n", B);
  C=in->v32[2];
  printf("C: %08x\n", C);
  D=in->v32[3];
  printf("D: %08x\n", D);
  
  if (enc==RC6_ENCRYPT)
  {
    B += *k; k++;
    printf("B: %08x\n", B);
    D += *k; k++;
    printf("D: %08x\n", D);
  } else {
    k += 43;
    C -= *k; k--;
    A -= *k; k--;
  }
  
  for (i=0; i<RC6_ROUNDS; i++)
  {
    if (enc==RC6_ENCRYPT)
    {
      T0 = ROTL32(B * (2 * B + 1), 5);
      T1 = ROTL32(D * (2 * D + 1), 5);
      
      A = ROTL32(A ^ T0, T1) + *k; k++;
      C = ROTL32(C ^ T1, T0) + *k; k++;
      // rotate 32-bits to the left
      T0 = A;
      A  = B;
      B  = C;
      C  = D;
      D  = T0;
    } else {
      T0 = ROTL32(A * (2 * A + 1), 5);
      T1 = ROTL32(C * (2 * C + 1), 5); 
      
      B  = ROTR32(B - *k, T0) ^ T1; k--;
      D  = ROTR32(D - *k, T1) ^ T0; k--;
      // rotate 32-bits to the right
      T0 = D;
      D  = C;
      C  = B;
      B  = A;
      A  = T0;
    }
  }
  
  if (enc==RC6_ENCRYPT)
  {
    A += *k; k++;
    C += *k; k++;
  } else {
    D -= *k; k--;
    B -= *k; k--;
  }
  printf("A: %08x\n", A);
    printf("B: %08x\n", B);
    printf("C: %08x\n", C);
    printf("D: %08x\n", D);


  // save plaintext/ciphertext
  enc_plain->v32[0]=A;
  enc_plain->v32[1]=B;
  enc_plain->v32[2]=C;
  enc_plain->v32[3]=D;
}

int main() {
    // 키와 평문 설정
    uint8_t key[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t plaintext[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t ciphertext[16]; // 암호문 저장
    
    // RC6 키 설정
    RC6_KEY key_schedule;
    rc6_setkey(&key_schedule, key, 16); // 키 길이는 16바이트
    
    printf("key: ");
    for (int i = 0; i < 16; ++i) {
        printf("%02X ", key[i]);
    }
    printf("\n");

    printf("key schedule: ");
    for (int i = 0; i < 44; ++i) {
        printf("%08X ", key_schedule.x[i]);
    }
    printf("\n");
    // 평문 출력
    printf("plain: ");
    for (int i = 0; i < 16; ++i) {
        printf("%02X ", plaintext[i]);
    }
    printf("\n");
    
    // 평문을 암호화
    rc6_crypt(&key_schedule, plaintext, ciphertext, RC6_ENCRYPT);
    
    // 암호문 출력
    printf("cipher: ");
    for (int i = 0; i < 16; ++i) {
        printf("%02X ", ciphertext[i]);
    }
    printf("\n");
    
    return 0;
}