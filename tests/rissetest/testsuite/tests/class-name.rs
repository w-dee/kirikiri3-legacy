class T extends Object
{
};

var t = T.new();
t.class.name + ":" +
T.name + ":" +
Object.name + ":" +
Object.class.name + ":" +
Class.name + ":" +
Module.name + ":" +
Integer.name + ":" +
Real.name + ":" +
Primitive.name + ":" +
Object.p.class.name + ":" +
4 .class.name + ":" +
4.4 .class.name +  ":" +
"string".class.name + ":" //=> "T:T:Object:Class:Class:Module:Integer:Real:Primitive:Function:Integer:Real:String:"
