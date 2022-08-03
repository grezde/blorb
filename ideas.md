
# Main ideas

````
# Autodefs
var console: {
    readln: () -> string,
    readWord: () -> string,
    readChar: () -> char,
    print: <printable a> a -> (),
    println: <printable a> a -> ()
};

var compute: <integer i> (i, i) -> i = (a, b) => 2*a-b;

var calculate: <integer i> () -> [i] = () => {
    var ns: [i] = [];
    var n = int32.read(console.readln());
    for(var k in range(1, n)) {
        var x = i.read(console.readWord());
        var y = i.read(console.readWord());
        ns.push(compute(x, y));
    }
    failure {
        
        console.println("Input is broken");
    }
    return ns;
};

type Item<integer i> = {
    apples: i,
    oranges: i
};
type bool = true | false;
type Tree = BinaryExpr (char, Tree, Tree) | NumExpr int32 | UnaryExpr (char, Tree);


var main: () -> () = () => {
    hello("Michael");
    var item: Item<int16> = {
        apples: 23,
        oranges: 24
    };
    console.println("{item.apples}");
    item.apples++;
    console.println("{item.apples}");
};
````

````
type JSONvalue = JSONnull | JSONbool bool | JSONnumber float64;

var getNumber: JSONvalue -> float64 = json => {
    if(json as JSONnull)
        return 0;
    if(json as JSONbool b)
        return b ? 1 : 0;
    if(json as JSONNumber n)
        return n;
};

````