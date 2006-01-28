#ifndef FFTSG_H
#define FFTSG_H

/*
	Generic FFT package from
	http://momonga.t.u-tokyo.ac.jp/~ooura/fft-j.html
*/


    void cdft(int, int, float *, int *, float *);
    void rdft(int, int, float *, int *, float *);
    void ddct(int, int, float *, int *, float *);
    void ddst(int, int, float *, int *, float *);
    void dfct(int, float *, float *, int *, float *);
    void dfst(int, float *, float *, int *, float *);


#endif
