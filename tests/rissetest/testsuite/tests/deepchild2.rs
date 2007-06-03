// スクリプト言語「りせ」テスト用スクリプト
{

	var n = 0;
	var f = function()
	{
		try
		{
			n++;
		}
		catch(e)
		{
		}
	};

	f();

	return n; //=> 1
}
