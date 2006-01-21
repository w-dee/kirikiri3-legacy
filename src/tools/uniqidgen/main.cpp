// RISSE_DEFINE_SOURCE_ID マクロで使用するための ID を生成する
// /dev/urandom が使用できる環境であればそれを利用するが、利用できない場合は
// Windows 環境であるとみなし、UuidCreate を使う。

#include <fstream>
#include <iostream>
#ifdef WIN32
	#include <rpc.h>
#endif

using namespace std;

int main()
{
	// 疑似乱数発生源を開く
	ifstream urandom;
	unsigned short int num[8];

	urandom.open("/dev/urandom", ios::in);

	if(!urandom)
	{
		// /dev/urandom が使えないので他の方法を考える
#ifdef WIN32
		UuidCreate(reinterpret_cast<UUID*>(num));
#else
		cerr << "Could not open /dev/urandom" << endl;
		return 1;
#endif
	}

	urandom.read(reinterpret_cast<char *>(num), sizeof(num));

	// id は 8 つの 16bit 整数で表現される
	for(int i = 0; i < 8; i++)
	{
		if(i != 0) cout << ",";
		cout << num[i];
	}
}

