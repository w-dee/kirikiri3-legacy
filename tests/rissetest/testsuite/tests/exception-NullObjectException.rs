try
{
	var global.n = null;
	var t = 0 + global.n; // null object was given     line no.4
}
catch(e if e instanceof NullObjectException)
{
	return e.message + " at " + e.trace[0];
		//=> /^"null object was given at .*?:4"$/
}

