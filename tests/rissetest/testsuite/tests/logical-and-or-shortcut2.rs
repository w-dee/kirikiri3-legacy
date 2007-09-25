{

var counter = 0;

false && (counter +=4) ; // no count
true  && (counter +=5) ; // count
false || (counter +=6) ; // count
true  || (counter +=7) ; // no count


counter ; //=> 11


}

