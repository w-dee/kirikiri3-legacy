class C
{
	const final function final_func() { }
}


try
{
	class global.D < C
	{
		function final_func() { } // member is final, cannot be overridden   line 11
	}
}
catch(e if e instanceof IllegalMemberAccessException)
{
	return e.message + " at " + e.trace[0];
		//=> /^"member \\"final_func\\" is final, cannot be overridden at .*?:11"$/
}





