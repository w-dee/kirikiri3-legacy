try
{
	class E : 3.14 {} // the superclass is not a class    line no.3
}
catch(e if e instanceof ClassDefinitionException)
{
	return e.message + " at " + e.trace[0];
		//=> /^"the superclass is not a class at .*?:3"$/
}

