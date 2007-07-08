( (boolean)null === false ? "ok" : "ng"    )+
( (boolean)"" === false ? "ok" : "ng"      )+
( (boolean)"0" === true ? "ok" : "ng"      )+
( (boolean)0 === false ? "ok" : "ng"       )+
( (boolean)1 === true ? "ok" : "ng"        )+
( (boolean)4 === true ? "ok" : "ng"        )+
( (boolean)<% %> === false ? "ok" : "ng"   )+
( (boolean)<% 01 %> === true ? "ok" : "ng" )+
( (bool)false === false ? "ok" : "ng"      )+
( (bool)true === true ? "ok" : "ng"        )


//=> "okokokokokokokokokok"
