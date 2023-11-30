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