property p
{
	setter(x) { return; }
}

try
{
	var r = global.p; // property cannot be read   line no.8
}
catch(e if e instanceof IllegalMemberAccessException)
{
	return e.message + " at " + e.trace[0].toString();
		//=> /^"property \\"p\\" cannot be read at .*?:8"$/
}

