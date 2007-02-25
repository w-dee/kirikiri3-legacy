try
{
	var r = "a".charAt(); // bad argument count   line no.3
}
catch(e if e instanceof BadArgumentCountException)
{
	return e.message + " at " + e.trace[0];
		//=> /^"bad argument count \(0 given, but 1 expected\) at .*?:3"$/
}

