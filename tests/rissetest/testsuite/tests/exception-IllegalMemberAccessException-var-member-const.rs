const global.c = 1;

try
{
	global.c = 1; // member is read-only   line no.5
}
catch(e if e instanceof IllegalMemberAccessException)
{
	return e.message + " at " + e.trace[0];
		//=> /^"member \\"c\\" is read-only at .*?:5"$/
}

