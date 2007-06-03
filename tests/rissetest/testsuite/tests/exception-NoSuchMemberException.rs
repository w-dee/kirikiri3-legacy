try
{
	var r = global.v; // member not found   line no.3
}
catch(e if e instanceof NoSuchMemberException)
{
	return e.message + " at " + e.trace[0];
		//=> /^"member \\"v\\" not found at .*?:3"$/
}

