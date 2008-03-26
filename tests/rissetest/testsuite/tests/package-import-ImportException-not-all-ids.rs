try
{
	import hoge, mage, hige, moge in test;
}
catch(e if e instanceof ImportException)
{
	return e.message + " at " + e.trace[0].toString();
		//=> /^"one or more identifiers could not be imported: .*?\.\.\. at .*?:3"$/
}

