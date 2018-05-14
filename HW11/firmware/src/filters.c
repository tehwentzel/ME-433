#include "filters.h"

int* get_firvals(void){
    static int fircoeffs[WINDOWLEN];
    fircoeffs[9] = 151;
    fircoeffs[8] = 366;
    fircoeffs[7] = 921;
    fircoeffs[6] = 1567;
    fircoeffs[5] = 1995;
    fircoeffs[4] = 1995;
    fircoeffs[3] = 1567;
    fircoeffs[2] = 921;
    fircoeffs[1] = 366;
    fircoeffs[0] = 151;
    return fircoeffs;
}


int fir(int * rawdata, int * fircoeffs, int idx){
    int loc = idx%WINDOWLEN;
    int val = 0;
    int j;
    int pos;
    for(j = 0; j < WINDOWLEN; j++){
        pos = (loc+j)%WINDOWLEN;
        val = val + fircoeffs[pos]*rawdata[pos];
    }
    return val/10000;
}

int mir(int * rawdata){
    int idx;
    int mirval = 0;
    for(idx = 0; idx < WINDOWLEN; idx++){
        mirval = mirval + rawdata[idx];
    }
    return mirval/WINDOWLEN;
}