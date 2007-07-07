try
{
	Number.new(); // cannot create instance from this class    line no.3
}
catch(e if e instanceof InstantiationException)
{
	return e.message + " at " + e.trace[0].toString();
		//=> /^"cannot create instance from this class at .*?:3"$/
}

