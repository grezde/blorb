
# IDEAS FOR BLORB

## Data types

### Default data types
- int8 = byte
- int16 = short
- int32 = int
- int64 = long
- uint32 = unsigned int
- bool = true | false
- char
- string

### Tuples
A tuple can be constructed from any n types. Tuple variables can be constructed the same as types. They must be sorrounded by parenthesis.
- (int, int, string)
- (bool, char)
- (int, int, int)  

To access values do:
```
(int, int) t = (2, 3);
a = t[0];
b = t[1];
# or
int (a, b) = t;
```

### Lists
Lists can be constructed by placing a type in square brackets. List declaration can be done the same as in Javascript.
```
[string] ss = ["", "h", "he", "hel", "hell", "hello"];
ss[0];    # ""
ss[3];    # "hel"
ss[6];    # throws error
ss.length # 6
ss += ["aa", "b"]; # appends the second array
ss += "a";         # appends the element
ss.append("a");    # appends the element
ss.prepend("a");
ss.pop();   # returns the last element
ss.shift(); # return the first element
```

### Structures
Structures behave like in C.
```
{ int x; int y; int z; } vec = {
    x: 5,
    y: 6,
    z: 7
};
vec.x; # returns 5
vec.y; # returns 6
```

### Aliases
Aliases can be created with typedef
```
typedef vec3 {
    int x; int y; int z;
};
typedef syntaxError (string, int, int);
typedef name string;
```

### Vectors / Tensors
__!! More work to do on this !!__

Vectors (or tensors) are basically numpy arrays. They are declared by their type followed by their dimension in brackets. To declare a vector literal type the number of dimensions times the character `%` followed by a list or a list of lists that have the same number of elements.
```
int[1] a = %[1, 2, 3, 4];
int[]  b = %[5, 6, 7, 8];
int[]  c = %[10, 20, 30, 40, 50];
int[2] M = %%[
    [1, 0, 0, 0],
    [0, 2, 0, 0],
    [0, 0, 2, 0],
    [0, 0, 0, 1]
];

a + b;   # %[6, 8, 10, 12]
a * b;   # %[5, 12, 21, 24]
a * 5;   # %[5, 10, 15, 20]
a + M;   
# %%[
#     [2, 1, 1, 1],
#     [2, 4, 2, 2],
#     [3, 3, 5, 3],
#     [4, 4, 4, 5]
# ]
M * a;

f(a);    # %[f(1), f(2), f(3), f(4)]
f(a, b); # %[f(1, 5), f(2, 6), f(3, 7), f(4, 8)]
f(a, c); # %[f(1, 10), f(2, 20), f(3, 30), f(4, 40), f(0, 50)]

``` 

### Functions
Functions are first class types. The type is denoted by a single arrow `->` while literlas are denoted with double arrow `=>`.
```
int -> int f;
f = x => x+2;
int -> int g = x => x-2;
f(3);               # returns 4
int -> int h = f*g; # funtion composition
h(5);               # returns 5
int -> (int -> int) add = a => x => x+a;
add(2)(5);          # returns 7
(string, int, int) -> string display = (s, l, r) => "Line {l} row {r}: {s}";
display("ERROR", 1, 1); # returns "Line 1 row 1: ERROR";
```

### Dictionaries
```
int[string] numbers = {
    ["hello"]: 3
};
```