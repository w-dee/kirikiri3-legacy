try
{
	1(); // cannot call non-function object   line no.3
}
catch(e if e instanceof UnsupportedOperationException)
{
	return e.message + " at " + e.trace[0];
		//=> /^"cannot call non-function object at .*?:3"$/
}

