AKA
===
What it does !!
---

AKA is a simple implementation of a shell in C.  It demonstrates the basics of
how a shell works.  That is: read, parse, fork, exec, and wait.  So I think it's
pretty cool.  Since it's a simple implementation, it has some limitations.

* Commands must be on a single line.
* Arguments must be separated by whitespace.
* No quoting arguments or escaping whitespace.
* No piping or redirection.
* Only builtins are: `cd`, `help`, `exit`, `hist`.

Prerequisite
---

* Unix/Linux Environment
* GCC Compiler

Running the Shell !!
---

1. Compile and execute `AKA-shell/main.c`.
2. Enter *User: User* and *Password: Password*.

Future Work:
---
More than one commands could execute simultaneously (Ex: `man ls,pwd`) <br>
(First *manual of ls* will be displayed, followed by *Print Working Directory*)

<br><br><br>
**NOTE:** Copy this, Use this or Tweak this :)
