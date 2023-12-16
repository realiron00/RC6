# RC6
RC6 encryption algorithm

## 2023 UROP
3학년 2학기 연구참여과정 프로젝트

## Files

### rc5.c
RC5 Encryption Algorithm
- RC5_ENCRYPT(WORD *pt, WORD *ct)
- RC5_DECRYPT(WORD *ct, WORD *pt)
- RC5_SETUP(unsigned char *K)

### rc6.c
RC6 Encryption Algorithm
- rc6_key_schedule(uint32_t* round_key, void* keys, uint32_t key_len)
- rc6_encrypt(uint32_t* key, void* plain, void* cipher)
