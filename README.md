# Plant
Plant is a lightweight yet powerful programming language proudly introduced by Bright Software Foundation. Bright Software Foundation is a small but dedicated organization to developing free (as in "freedom") software that is for everyone.

Currently, Plant is in its beta tests, and hence IDE's or debuggers, etc. might not be served alongside. However, developments are under construction (?) and we are striving for the provision of an easy-to-use and free environment for everyone to develop and learn in Plant. In the future, Plant will be integrating with Tropic, which is another project by Bright Software Foundation; at that time, Tropic will be the only IDE of Plant. You will be debugging, testing and (of course) writing code with it.

## What is Plant?
So, the ultimate question is "what is Plant?" Well, as mentioned, Plant is a lightweight scripting language that is mainly used as a tool for automation and "gluing" (that is, integrating different programming languages together and generating tedious code). Currently, this kind of tasks is dominated by Ruby or Perl in text-processing fields, Lua for gaming industries (think of Roblox), while Python does so in a more generic domain. However, Plant decided to take a seat in it.

### Why?
Many scripting languages pursuits "elegance," "simplicity" and et cetera, but few cares about *familiarity*. For instance, programmers will be aghast if they plunge into the world of Python, Ruby or Lua, and find themselves bewildered in the world full of spaces or `end` statements to terminate a block of code instead of a little, cute curly brace; especially our targeted audience are to be programming with C/C++ or Java: these languages often requires verbose programming with monotomous code, and so they need a scripting (or gluing) language that is easy-to-learn to help them generate some code, while other programmers seek for an automation system for generating `make` files.

Here comes the disclaimer. We are not criticizing being "elegant" or "simple" (as it will be outrageous); but we do think that the *Pythonic* or *Luatic* style really is a huge tradeoff. So, Plant (and us, of course) tries its best to minimize the tradeoff and achieve a balance between elegance/ simplicity and familiarity.

### Sounds nice. Any work ahead?
Of course! As Plant is a new programming (scripting) language in its infancy (beta testing) with mighty alternatives like Python or Ruby, we have tons of work ahead. This includes:

* Speed. Plant is not that performant, even compared to Python, which is said to one of the slowest languages in the world. This is normal though, and to be fair, Plant is as performant as early versions of Python (hey, I'm talking about the bygone days where Python 1 is released on computers that counted memories as bytes or kilobytes). We are trying our best to optimize our little language to a really speedy one.
* Features. Well, classes, import, variables, standard libraries, ...

To be specific, here's a list of to-do's:

* Class access specifiers: `private`, `public`, `protected`
* Static members/ methods: accessed by `a::b`
* Multiple inheritance: allowing `class A : B, C` without keywords like `mixin` or `extends`, etc. => implemented :)
* Inheritance access: controlling which could step into the methods of which: `class A : public B`
* Friendships: go and search a C++ book and find out more ;)
* Enumerations: `enum Name {...}` and `enum class Name {...}`
* Final classes: a class which could not be inherited: `class A final {...}` => implemented :)
   - Changed in version 0.8.1: we decided to go `final class A` instead of the above order to match with C++ syntax.
* Constant variables: `const var a` or `var const a` => implemented :)
* Deletations: `delete a`, `delete a.b`, `delete a[b]` etc. => implemented :)
   - Note: only implemented `delete a` as for version 0.8.1
* Import and export: `import module` imports a file named `module.plant` around. This is expected to be similar to Lua's `dofile(...)` instead of Python or C++ (20)'s `import`.
   - Changed in version 0.8.1: after a thorough and prudent consideration we decided to implement the module import feature with Plant characteristics and feature with adequate allusion to multiple programming languages in maybe version 0.9.x or 0.10.x
* `try` `catch` `noexcept` `finally`: you could infer that...
* Default arguments: if we got, say, `func a(b = 3){print b;}`, calling `a()` prints `3` while `a(42)` prints `42`.
* Type annotations: personally I don't like TypeScript's `a: int` thing, and so I will figure out a way to implement `int a`, and for return values we will get `int a(str b)` instead of `func a(b: str) -> int`... Well, as in Python, this is ignored by the interpreter.
* Range-based for: `for (item : vector) print item` expands to `for (var i = 0; i < sizeof(vector); i++) print vector[item];`.
* `break` in switch with multiple cases: `case 1: case 2: if (n == 4) break; print "Hi";`
* `struct`: `struct A{var a; var b;}` you could only store members in a struct but not methods.
* Arbitrary number of parameters: `func a(b, c, ...d){}`
* More magic methods: `operator xxx(...){}`

Stay tuned for the future releases! (Will take quite a while because Bright Software Foundation is our avocation and we have our own things to do, sorry for raining down onto your exhilaration...)

## Dedications and acknowledgement

```
Plant 0.7.0 (Jul  3 2025, 19:34:38) [GNU/Mingw GCC v.13.1.0] on win32
Type "credits();" or "copyright();" for more information.
Plant [1] >>> credits();
    Thank you Bell Labs, it was you who created the most beautiful programming
    language in the world - C.

    Dedication of Plant: To the Python development team, you've made me know what a
    scripting language looks like. Python was the first language I picked up.

    Lastly, I would like to thank everyone who is or has been involving in the
    development of Plant. Had it not been for your support, Plant would not have
    been here.
```

> And also a huge thank to Robert Nystrom and his *Crafting Interpreters*. To be candid, this project of Plant is but an *evolved* version of Lox, the little toy language implemented in his book. If Lox is a harp where only chords could be played, Plant is but the harp in *Jack and the Beans*. She could sing, dance, and is sentimental, but is a harp after all. In fact, Plant is greatly influenced by Lox. Had there not been kind instructions from our predecesor, Plant would not have been here.

## Building
Our official implementation of Plant is completely in C (may include a bit C++ in the future).

You may build Plant anytime, on any platform that supports a C compiler. We build via `gcc` (on Windows it is `mingw`, which is `gcc`-compatible), but I think that Apple LLVM/ Clang and Microsoft(R) Visual C++(TM) should be fine too, as long as it supports ISO C.

Download the code. It is in the license of the GNU GPL version 3, so do whatever you love to our code. Just keep in mind, if unlikely you messed up your computer to a bluescript with a disappointed face, we would not be responsible for that, according to the License. Also according to the License, remember to share your modified source code if you make a wonderful change or rewrite some splendid comments or instructions, *and* you decided to distribute it to someone else.

Anyway, back to the topic, fire up any IDE. If you ask, for open-source freeware, we recommend Qt Creator; for free software, we recommend CLion(TM) (non-commercial) by JetBrains(R). They handle `CMakeLists.txt` *very* well.

If you opt not to do so, it's completely fine, just fetch a text shell and input the following instruction (hey, remember to install CMake also, at least version 
3.16!):

```
$ mkdir build; cd build
$ cmake ../
$ cmake -b .
$ mingw32-make # or make, or nmake etc.
$ release/Plant.exe # depending on your config
```

A little good news: so far, Plant does not have any external dependencies, so you don't need to install any additional libraries as prerequisite.

## Usage and docs
More documentation will be available when Plant is mature.

### At the source code level

#### Lines
The following is a line:

```

```

It is terminated by a `\n`. The following is a single-line comment:

```
// This is a single-line comment.
```
#### Comments
Any comments will be ignored by Plant. A single-line comment terminates at `\n`, spanning a line.

You could extend the line by `\`:

```
print \
    "Hello, World!";
```

is equivalent to

```
print "Hello, World!";
```

#### Naming things
An identifier is a name. You use an identifier to name variables, functions, etc.

A valid identifier
1. Starts with a capital or small letter, or an underscore; and
2. Arbitrary number of capital or small letters, numbers or underscores follow; and
3. Must not contain characters other than that, like `?`, `+`, space and non-ASCII characters (Chinese characters, Arabic scripts etc.).
4. Must not be the following:

```
and
break
case
class
continue
default
do
else
false
for
func
if
nobreak
null
operator
or
print
return
sizeof
super
switch
this
true
var
while
```

> Added circa version 0.8.0: a holistic keyword list is now presented as follows: `and, break, case, catch, class, const, continue, decltype, default, delete, do, else, false, final, finally, for, func, if, nobreak, noexcept, null, or, operator, print, private, protected, public, return, sizeof, static, std, super, switch, this, throw, true, try, var, while`

Those are known as *keywords* or *reserved words*, i.e. words with special meaning. Using them will raise a syntax error.

#### Strings
String is basically text in programming. To represent string, you use different delimiter. A delimiter encloses the content of a string. There are 3 delimiters in Plant, namely `"`, `'` and `\``.

The first two delimiters, named double quotation mark and single quotation mark (sometimes "apostrophe"), are almost identical; which is unlike C/C++, where single quoted content is a character while double quoted content is a string.

##### Escape sequence
You could represent different characters but using a predefined sets of conventions after a `\` symbol. They are:

* `\a` converts to BELL, ASCII `0x7`;
* `\b` converts to BACKSPACE, ASCII `0x8`;
* `\f` converts to FORMFEED, ASCII `0xc`;
* `\n` converts to NEWLINE, ASCII `0xa`;
* `\r` converts to RETURN, ASCII `0xd`;
* `\t` converts to TAB, ASCII `0x9`;
* `\v` converts to VERTICAL TAB, ASCII `0xb`;
* `\\` escapes the `\` character itself;
* `\'` escpaes the `'` character itself, preventing from string termination;
* `\"` escapes the `"` character itself, preventing from string termination;
* `\ddd` converts to ASCII `0oddd`, where `d` is an octal value. Note that `ddd` must be larger than or equal to 0 and smaller than `0o400`.
* `\xdd` converts to ASCII `0xdd`, where `d` is a hexadecimal value.
* `\uabcd` and `\Uabcdefgh` converts to Unicode `0xabcd` or UTF-32 `0xabcdefgh` respectively. Note that due to technical reason, `ab` or `abcdef` must be 0's respectively, because full Unicode is not yet supported in Plant. In other words, this sequence acts like `\xdd`.

#### Integers
All numeral values in Plant are `double` in C, and are known as `real`. However, integer notations are still supported:

`0b` or `0B` starts a binary notation; `0x` or `0X` starts a hexadecimal notation; `0o` or `0O` starts an octal notation; others (leading zero ignored) begins a decimal notation.

Scientific ntation is also supported. You use `e+` or `e-` (case insensitive) to join the power and the coefficient. A scientific notation is a notation in which, say, `1.3e+5`, represents "1.3 times 10 to the power of 5", i.e. 130,000.

You could use `'` any times as separators (even after the numeral); `'` themselves will be ignored by Plant.

These all print `13`:

```
print 0b1101;
print 0xd;
print 0o15;
print 013;
print 13;
print 1'3;
print 0'13;
print 0'b'1101;
print 0B1101;
print 0XD;
print 0xD;
print 0O15;
print 00000013.0;
print 1.3e+1;
print 1'.'3'E'+'1''''''';
print 13e+0;
print 130E-1;
```

### Variables declarations
Use `var iden = value` to declare a variable with name `iden` and initial value as `value`:

```
var a = 3;
print a;  // 3
```

Variable declarations without initial values are initialized with `null`:

```
var a;
print a;  // null
```

Variables could be redefined:

```
var a;
print a;  // null
var a = 3;
print a;  // 3
```

### Constants declarations
Use one of the following forms to declare a constant with name `iden` and initial value `value`:

```
const iden = value;
var const iden = value;
const var iden = value;
var const var iden = value;
```

A constant could not be modified or overwritten. However, they could be redefined by a definition of a variable:

```
const a = 42;
// a = 99 // Error!
print a;  // outputs 42
var a = 43;  // Doesn't have to be the original value
// of course, you could do "var a = a" where
// the first "a" is the new identifier and the second "a" points to the
// original constant
print a = 99; // outputs 99
print a += 1; // outputs 100, because 100 == 99 + 1
```

### Deletions of variables
You could delete a variable or constant if you do not want to use them anymore, by using the syntax `delete iden;`.

```
var x = 10;
print x;  // outputs 10
delete x;
// print x;  // error
```

### Expressions
Addition, Subtraction, Multiplication, Division, Modulo (taking out the remainder):

```
a + b; a - b; a * b; a / b; a % b;
```

Logical and, or:

```
a && b; a || b;
```

... is equivalent to

```
a and b; a or b;
```

Logical and evaluates all the item; and if one of them is false, then the whole expression is `false` and it stops evaluation. Logical or is the opposite: if one is true, then the whole expression is `true` and it stops evaluation:

```
true and true => true
true and false => false
false and true => false
false and false => false

true or true => true
true or false => true
false or true => true
false or false => false
```

A bitwise expression operates with numbers. In computers, any numbers (primarily integers) could be represented in binary form. For example, the binary form of `13` is `0b1101`, while the binary form of `6` is `0b0110`.

Think of `true` as `1`, and `false` as `0`. Now, bitwise and operation and bitwise or operation are operations that compare numbers bit-by-bit:

```
13 & 6 => 4

  0 b 1 1 0 1
& 0 b 0 1 1 0
-------------
  0 b 0 1 0 0 => 4

13 | 6 => 15

  0 b 1 1 0 1
| 0 b 0 1 1 0
-------------
  0 b 1 1 1 1 => 15
```

Now, `xor` is a bitwise expression where **when evaluating two values, two same one results in `false` and two different one results in `true`** (that's why it got its name, "exclusive or"):

```
true ^ true => false
true ^ false => true
false ^ true => true
false ^ false => false
```

And for the numeral bitwise xor operations, it's nothing but just extending the concepts to numbers as bits:

```
13 ^ 6 => 11

  0 b 1 1 0 1
^ 0 b 0 1 1 0
-------------
  0 b 1 0 1 1 => 11
```

To compare the sizes different values, use `<` (less than), `<=` (less than or equal to), `>` (greater than) and `>=` (greater than or equal to) respectively:

```
3 < 15 => true
15 <= 42 => true
42 <= 42 => true
43 <= 42 => false
52 > 1 => true
3 >= 3 => true
```

To test the equality of values, use `==` (equal to) and `!=` (not equal to):

```
true == true => true
true == false => false
true != true => false
true != false => true
```

This is equivalent to:

```
true is true
true is false
true isnt true
true isnt false
```

Plant also supports `<<` and `>>`, which is the same as C's.

You could assign something assign by `=`, resulting in the assigned value:

```
var a = 3;
print a;  // 3
print a = 4;  // 4
print a; // 4
```

You could not assign to a value which is not declared yet.

You could assign by `iden op= value`, where it expands to `iden = iden op value`. For instance, `a += 3` means `a = a + 3`. Supported `op` includes:

```
-
+
/
*
%
>>
<<
^
&
|
```

`+` to affirm, `-` to negate:

```
print +3;  // 3
print -3;  // -3
```

`!` to reverse:

```
print !true;  // false
print !false;  // true
```

... which is equivalent to

```
print not true;  // false
print not false;  // true
```

`~` is a bitwise version of `!`:

```
~13 => -14

~ 0 b 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 0 1
-----------------------------------------
  0 b 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 0 1 0
```

As all bits are reversed including the first bits, which acts as a sign marker (indicator for determining positive or negate), by definition, it becomes a negative number and is parsed negatively, resulting in `-14`. In fact, for any number `n`, `~n == -(n + 1)`.

You could increase or decrease a value of 1 by `++` or `--`; where `++a` returns the value after increment and `a++` returns the value before increment:

```
var a = 3;
print a++;  // 3
print a; // 4
print --a; // 3
print a; // 3
```

You could declare a conditional expression by `cond ? then : otherwise`. If `cond` is true, only `then` will be evaluated; otherwise, `otherwise` will be evaluated.

```
print true ? 3 : 4;  // 3
print false ? 3 : 4;  // 4
```

Note that they could be stacked:

```
print false ? 3 : true ? 5 : 4;  // 5
```

You could declare a vector by `[items,]` (*one* trailing comma is accepted):

```
var vector = [1, 2, 3];
print vector;  // [1, 2, 3]
```

You could declare a map by `{key: value,}` (*one* trailing comma is accepted):

```
var map = {2: 3, 61: 4};
print map;  // {61: 4, 2: 3}
```

Note that the insertion order of a map is not preserved.

To access the size of a vector, use `sizeof(vec)`; to access the number of keys in a map, use `sizeof(map)`. All other values passed into `sizeof(...)` returns the internal size of that object. This is somehow a fusion between Python's `len(...)` and C's `sizeof(...)`.

You could get the item of a vector by `vec[index]`, where `index` is a positive integer starting from `0` and smaller than the size of the vector. If you pass in a negative index, Plant adds the size of the vector automatically, making it counts from the end of the vector:

```
   var vec = [1, true, "hi"];
//            0  1     2
// OR         -3 -2    -1

print vec[0];  // 1
print vec[1];  // true
print vec[2];  // hi
print vec[-1]; // hi
print vec[-2]; // true
print vec[-3]; // 1
// print vec[-4]; print vec[-1235]; print vec[3]; print vec[1234];  => Error

print sizeof(vec);  // 3
```

You could change the value of a vector by `vec[index] = value`. The requirement of the index is same as the above, except that `index` could be the same as `sizeof(vec)`, where it inserts `value` to the end of the vector; and that negative indices will be invalid. The inserted vector is resulted:

```
var vec = [1, true, "hi"];

print vec[1] = false;  // [1, false, "hi"]
print vec[3] = "asdf"; // [1, false, "hi", "asdf"];
// print vec[235] = "error";  => error
```

Similar to a vector, a map acts like that way but keys are used instead of indices to search. Currently, only numerals, booleans, null values and strings could be keys; other will result an exception:

```
var map = {"Answer to Life": 42, "Init": 0};
print map;  // {"Init": 0, "Answer to Life": 42}
print map["Answer to Life"];  // 42
print map["Init"] = 1;  // {"Init": 1, "Answer to Life": 42}
```

Accessing a non-exist key results in an exception.

You could define lambda functions (c.f. below) via `[=](){}`:

```
[=](n){print n;}(3);  // prints 3
```

You could store them, but it is still a lambda function:

```
var fn = [=](){};
print fn;  // <lambda function at 00000xxxxxx>
```

### Statements
#### Control flows
This section is almost identical to C/C++'s, unless otherwise stated:

```
some_statements {
    multiple_statements_combined
}

// If there are nothing:

{  // Starts a scope
    var a = 3;
}

// print a;  => Error. Outside a scope.
```

> A scope is the lives of variables declared inside it. A function (including lambda function) or a class starts a scope, as well as a `for` loop. A variable *not* in *any* scopes is called a *global* variable. All other variables are called *local* variables.
>
> To access or modify *global* variables in local scopes, use `::` before the variable. That is, `var a = 3; {var a = 2; print a; print ::a;}` will print `2` and `3` respectively.

```
// If statements
if (expr)
    then_do_this
else
    otherwise_do_this
```

```
// For statements
for (initializer; condition; increment)
    do_something

// Is equal to
initializer
do_something
increment
do_something
increment
do_something
increment
... (until condition is false)
```

```
// While statements
while (condition)
    do_something

// Is equal to
do_something
do_something
do_something
... (until condition is false)
```

```
// Do statements
do
    something
while (condition);

// Is equal to
do_something  <= guaranteed even if condition is false at first
(If condition is not false)
do_something
do_something
... (until condition is false)
```

You could use `break` and `continue` statements *inside a loop (only)*, similar to C/C++'s.

```
// Function declarations
func name(parameters, param2, param3 <= if any) { // braces are must
    do_something
    return parameters + param2 + param3;  // Optional, returns null if omitted
}

var x = name(1, 2, 3);
print x;  // 6

// Is equal to
do_something  (whereas all references to "parameters", param2 and param3 become 1, 2 and 3 respectively)
var x = 1 + 2 + 3;
print x; // 6
```

```
// Class declarations
class Name {
    (function declarations called *methods*, if any)
    func asdf(){print "Name: asdf";}
    operator +(n) { // Operator overloading (if any)
        return this.a + n.a;
    } // "this" refers to this class
}

// Inheritance
class NameChild : Name {
    func asdf(){print "asdf";}
    func asdf_super() {super.asdf();}  // super can only access fields,
        // instead of the class instance its own
}

class EmptyClass {}  // Creates a CLASS of EmptyClass

var empty = EmptyClass();  // Creates an INSTANCE of EmptyClass
// empty.asdf();  // error
var n = NameChild();
n.asdf();  // valid, prints "asdf"
n.asdf_super();  // valid, fetches Name::asdf and prints "Name: asdf"
n.a = 3; // valid
var otherN = NameChild();
// print otherN.a;  // Invalid, only *instance* n has *field* a
print n + otherN;  // Same as operator+(otherN), which expands to n.a + otherN
```

Valid (overloadable) operators include:

```
+
-
*
/
%
&
|
^
~
<<
>>
-- => overloads unary -
++ => overloads unary +
>
<
==
```

The `switch-case-default` statements are same as C/C++'s ones, except that:

1. `break` couldn't be used. In fact, when a match is completed, all other matches are ignored and Plant exists the cases automatically.
2. `default` could *only* be put at the bottom or else an error will be raised. This is due to the technical reasons of an interpreted language.
3. Currently, multiple cases are not supported. You could only repeat the clauses.

This doc is incomplete and immature; it will be expanded and ported into a website specialized for documentation-viewing in the future, when Plant is more mature and developed.

Anyway, happy programming. After all, basic algorithms like loops, controls to binary searches and dictionaries could be accomplished so far because we got classes, inheritance, vectors (sometimes called lists or arrays) and maps (sometimes called hashes, "tables" in Lua).