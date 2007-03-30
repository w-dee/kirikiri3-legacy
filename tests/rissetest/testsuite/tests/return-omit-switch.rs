{
	function test1(x)
	{
		switch(x)
		{
		case 0: "A"; break;
		case 1: "B"; break;
		default: "C"; break;
		}
	}
	var res1 =
		test1(0) + 
		test1(1) + 
		test1(3)     // "ABC"

	function test2(x)
	{
		switch(x)
		{
		case 0: "A"; break;
		case 1: "B"; break;
		default: "C";
		}
	}
	var res2 =
		test2(0) + 
		test2(1) + 
		test2(3)     // "ABC"

	function test3(x)
	{
		switch(x)
		{
		case 0: "A"; break;
		case 1: "B";
		default: "C";
		}
	}
	var res3 =
		test3(0) + 
		test3(1) + 
		test3(3)     // "ACC"


	function test4(x)
	{
		switch(x)
		{
		default: "C"; break;
		case 0: "A"; break;
		case 1: "B"; break;
		}
	}
	var res4 =
		test4(0) + 
		test4(1) + 
		test4(3)     // "ABC"

	function test5(x)
	{
		switch(x)
		{
		default: "C"; break;
		}
	}
	var res5 =
		test5(0) + 
		test5(1) + 
		test5(3)     // "CCC"

	function test6(x)
	{
		switch(x)
		{
		case 0: "A"; break;
		case 1: "B"; break;
		}
	}
	var res6 =
		test6(0) + 
		test6(1) + 
		test6(3)     // "AB"


	return res1 + ":" + res2 + ":" + res3 + ":" + res4 + ":" + res5 + ":" + res6
		//=> "ABC:ABC:ACC:ABC:CCC:AB"
}
