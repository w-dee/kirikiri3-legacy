property p
{
	getter() { return 1; }
}

try
{
	global.p = 1; // property cannot be written   line no.8
}
catch(e if e instanceof IllegalMemberAccessException)
{
	return e.message + " at " + e.trace[0].toString();
		//=> /^"property \\"p\\" cannot be written at .*?:8"$/
}

