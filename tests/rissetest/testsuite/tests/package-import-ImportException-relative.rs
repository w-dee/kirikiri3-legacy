try
{
	import ...hoge;
}
catch(e if e instanceof ImportException)
{
	return e.message + " at " + e.trace[0].toString();
		//=> /^"invalid relative package name \\"...hoge\\" against \\"main\\" at .*?:3"$/
}

