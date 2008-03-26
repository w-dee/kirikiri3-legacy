try
{
	import aadfliguhaer.lig.h.ua.leigha;
}
catch(e if e instanceof ImportException)
{
	return e.message + " at " + e.trace[0].toString();
		//=> /^"package \\"aadfliguhaer.lig.h.ua.leigha\\" not found at .*?:3"$/
}

