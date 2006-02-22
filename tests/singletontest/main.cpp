//! @file
//! @brief シングルトンを実現する自家製テンプレートクラスをテストするプログラム
#include "prec.h"


#include <stdio.h>
#include "Singleton.h"

class s0 : singleton_of<s0>
{
public:
	s0() { printf("s0 construct\n"); }
	~s0() { printf("s0 destruct\n"); }
};

class s1 : singleton_of<s1>, depends_on<s0>
{
public:
	s1() { printf("s1 construct\n"); }
	~s1() { printf("s1 destruct\n"); }
};

class s2 : singleton_of<s2>, depends_on<s0>
{
public:
	s2() { printf("s2 construct\n"); }
	~s2() { printf("s2 destruct\n"); }
};

class s3 : singleton_of<s3>,
		depends_on<s0>,
		depends_on<s1>,
		depends_on<s2>
{
	int n;
public:
	s3() { printf("s3 construct\n"); n = 1; }
	~s3() { printf("s3 destruct\n"); n = 0; }
	int getN() const { return n; }
};

class s4 : singleton_of<s4>, manual_start<s4>
{
	int n;
public:
	s4() { printf("s4 construct\n"); n = 2; }
	~s4() { printf("s4 destruct\n"); n = 0; }
	int getN() const { return n; }
};


int main()
{
//	s3::referrer s3;
	fprintf(stderr, "main begin\n");
	fprintf(stderr, "InitAll begin\n");
	tRisaSingletonManager::InitAll();
	fprintf(stderr, "InitAll end\n");
	fprintf(stderr, "s3::n : %d\n", s3::instance()->getN());
	fprintf(stderr, "s4::n : %d\n", s4::instance()->getN());
	fprintf(stderr, "Disconnect begin\n");
	tRisaSingletonManager::DisconnectAll();
	fprintf(stderr, "Disconnect end\n");
	tRisaSingletonManager::ReportAliveObjects();
	fprintf(stderr, "main end\n");
}
