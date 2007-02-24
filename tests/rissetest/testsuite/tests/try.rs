// スクリプト言語「りせ」テスト用スクリプト
try
{
	try
	{
		throw "return by exception";
	}
	catch(e)
	{
		throw e;
	}
}
catch(e)
{
	return e.toString(); //=> "return by exception"
}
return "not caught";
