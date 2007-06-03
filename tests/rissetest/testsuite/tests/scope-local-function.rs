if(1)
{
	var a = "hogehoge";
	function scope_local_function() { return a; }

	return scope_local_function(); //=> "hogehoge"
}
