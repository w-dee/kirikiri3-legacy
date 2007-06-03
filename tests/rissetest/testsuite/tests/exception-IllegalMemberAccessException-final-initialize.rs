try
	class global.D < String {} // member is final, cannot be overridden   line 3
catch(e if e instanceof IllegalMemberAccessException)
	return e.message + " at " + e.trace[0];
		//=> /^"member \\"(?:initialize|construct)\\" is final, cannot be overridden at .*?:2"$/



