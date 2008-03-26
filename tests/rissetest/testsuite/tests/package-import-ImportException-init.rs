try
{
	import self_inclusion;
}
catch(e if e instanceof ImportException)
{
	return e.message + " at " + e.trace[0].toString();
		//=> /^"package \\"self_inclusion\\" is being initialized, cannot be imported at .*?self_inclusion.rs:1"$/
}

