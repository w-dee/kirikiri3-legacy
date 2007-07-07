try
{
	(Array::pop incontextof 1)(); // given context is not compatible with this method/property   line no.3
}
catch(e if e instanceof BadContextException)
{
	return e.message + " at " + e.trace[0];
		//=> /^"given context is not compatible with this method\/property at .*?:3"$/
}

