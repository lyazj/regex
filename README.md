# regex

This little project implements the minimized DFA based lexical analyzer generator described in *Compilers: Principles, Techniques, and Tools (2nd Edition) - Chap. 3*.

Main procedures:

* Parse the given regular expression
* Compute `nullable`, `firstpos`, `lastpos`, `followpos`
* Merge non-distinguishable character groups into classes
* Make DFA states
* Minimize DFA states

Highlights:

* Outputs state transition tables into mermaid graphs
* Tested on simplified C lexical rules

```mermaid
graph LR
0 -- "\." --> 1
0 -- "[0-9]" --> 6
0 -- "[\+\-]" --> 7
1 -- "[0-9]" --> 2
2 -- "[Ee]" --> 3
2 -- "[0-9]" --> 2
2("2#0")
3 -- "[0-9]" --> 4
3 -- "[\+\-]" --> 5
4 -- "[0-9]" --> 4
4("4#0")
5 -- "[0-9]" --> 4
6 -- "[Ee]" --> 3
6 -- "\." --> 2
6 -- "[0-9]" --> 6
6("6#0")
7 -- "\." --> 1
7 -- "[0-9]" --> 6
```
