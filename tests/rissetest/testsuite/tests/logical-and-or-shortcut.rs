var counter = 0;

false && (counter ++) ; // no count
true  && (counter ++) ; // count
false || (counter ++) ; // count
true  || (counter ++) ; // no count


counter ; //=> 2


