\# Plant



Plant is a lightweight yet powerful programming language proudly introduced by Bright Software Foundation. Bright Software Foundation is a small but dedicated organization to developing free (as in "freedom") software that is for everyone.



Currently, Plant is in its beta tests, and hence IDE's or debuggers, etc. might not be served alongside. However, developments are under construction (?) and we are striving for the provision of an easy-to-use and free environment for everyone to develop and learn in Plant. In the future, Plant will be integrating with Tropic, which is another project by Bright Software Foundation; at that time, Tropic will be the only IDE of Plant. You will be debugging, testing and (of course) writing code with it.



\## What is Plant?

So, the ultimate question is "what is Plant?" Well, as mentioned, Plant is a lightweight scripting language that is mainly used as a tool for automation and "gluing" (that is, integrating different programming languages together and generating tedious code). Currently, this kind of tasks is dominated by Ruby or Perl in text-processing fields, Lua for gaming industries (think of Roblox), while Python does so in a more generic domain. However, Plant decided to take a seat in it.



\### Why?

Many scripting languages pursuits "elegance," "simplicity" and et cetera, but few cares about \*familiarity\*. For instance, programmers will be aghast if they plunge into the world of Python, Ruby or Lua, and find themselves bewildered in the world full of spaces or `end` statements to terminate a block of code instead of a little, cute curly brace; especially our targeted audience are to be programming with C/C++ or Java: these languages often requires verbose programming with monotomous code, and so they need a scripting (or gluing) language that is easy-to-learn to help them generate some code, while other programmers seek for an automation system for generating `make` files.



Here comes the disclaimer. We are not criticizing being "elegant" or "simple" (as it will be outrageous); but we do think that the \*Pythonic\* or \*Luatic\* style really is a huge tradeoff. So, Plant (and us, of course) tries its best to minimize the tradeoff and achieve a balance between elegance/ simplicity and familiarity.



\### Sounds nice. Any work ahead?

Of course! As Plant is a new programming (scripting) language in its infancy (beta testing) with mighty alternatives like Python or Ruby, we have tons of work ahead. This includes:



\* Speed. Plant is not that performant, even compared to Python, which is said to one of the slowest languages in the world. This is normal though, and to be fair, Plant is as performant as early versions of Python (hey, I'm talking about the bygone days where Python 1 is released on computers that counted memories as bytes or kilobytes). We are trying our best to optimize our little language to a really speedy one.

\* Features. Well, classes, import, variables, standard libraries, ...



To be specific, here's a list of to-do's:



\* Class access specifiers: `private`, `public`, `protected`

\* Static members/ methods: accessed by `a::b`

\* Multiple inheritance: allowing `class A : B, C` without keywords like `mixin` or `extends`, etc.

\* Inheritance access: controlling which could step into the methods of which: `class A : public B`

\* Friendships: go and search a C++ book and find out more ;)

\* Enumerations: `enum Name {...}` and `enum class Name {...}`

\* Final classes: a class which could not be inherited: `class A final {...}`

\* Constant variables: `const var a` or `var const a`

\* Deletations: `del a`, `del a.b`, `del a\[b]` etc.

\* Import and export: `import module` imports a file named `module.plant` around. This is expected to be similar to Lua's `dofile(...)` instead of Python or C++ (20)'s `import`.

\* `try` `catch` `noexcept` `finally`: you could infer that...

\* Default arguments: if we got, say, `func a(b = 3){print b;}`, calling `a()` prints `3` while `a(42)` prints `42`.

\* Type annotations: personally I don't like TypeScript's `a: int` thing, and so I will figure out a way to implement `int a`, and for return values we will get `int a(str b)` instead of `func a(b: str) -> int`... Well, as in Python, this is ignored by the interpreter.

\* Range-based for: `for (item : vector) print item` expands to `for (var i = 0; i < sizeof(vector); i++) print vector\[item];`.

\* `break` in switch with multiple cases: `case 1: case 2: if (n == 4) break; print "Hi";`

\* `struct`: `struct A{var a; var b;}` you could only store members in a struct but not methods.

\* Arbitrary number of parameters: `func a(b, c, ...d){}`

\* More magic methods: `operator xxx(...){}`



Stay tuned for the future releases! (Will take quite a while because Bright Software Foundation is our avocation and we have our own things to do, sorry for raining down onto your exhilaration...)



\## Dedications and acknowledgement

```

Plant 0.7.0 (Jul  3 2025, 19:34:38) \[GNU/Mingw GCC v.13.1.0] on win32

Type "credits();" or "copyright();" for more information.

Plant \[1] >>> credits();

&nbsp;   Thank you Bell Labs, it was you who created the most beautiful programming

&nbsp;   language in the world - C.



&nbsp;   Dedication of Plant: To the Python development team, you've made me know what a

&nbsp;   scripting language looks like. Python was the first language I picked up.



&nbsp;   Lastly, I would like to thank everyone who is or has been involving in the

&nbsp;   development of Plant. Had it not been for your support, Plant would not have

&nbsp;   been here.

```



