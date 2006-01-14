//! @file
//! @brief シングルトンを実現する自家製テンプレートクラスをテストするプログラム


#include "prec.h"
#include <stdio.h>
#include "Singleton.h"

class s0
{
public:
	s0() { printf("s0 construct\n"); }
	~s0() { printf("s0 destruct\n"); }
};

class s1
{
	tRisaSingleton<s0> referer_s0;
public:
	s1() { printf("s1 construct\n"); }
	~s1() { printf("s1 destruct\n"); }
};

class s2
{
	tRisaSingleton<s0> referer_s0;
public:
	s2() { printf("s2 construct\n"); }
	~s2() { printf("s2 destruct\n"); }
};

class s3
{
	tRisaSingleton<s0> referer_s0;
	tRisaSingleton<s1> referer_s1;
	tRisaSingleton<s2> referer_s2;
	int n;
public:
	s3() { printf("s3 construct\n"); n = 1; }
	~s3() { printf("s3 destruct\n"); n = 0; }
	int getN() const { return n; }
};

int main()
{
	printf("main begin\n");
	printf("n : %d\n", tRisaSingleton<s3>::instance()->getN());
	printf("main end\n");
}

