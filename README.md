# GuiSE - Gui Scripting Engine
A typed scripting language implementation following along the Crafting Interpreters online book.

## Plans
GUI Features are not currently implemented. I want to experiment with functional programming concepts. Haskell, for example, is purely functional, so you can expect that the same input will result in the same output. Anything that breaks this rule is wrapped in an IO monad. Similarly, I want to have a signifier which marks functions to cache the inputs on calls allowing diffs to be called on update calls. 

## Implemented Features
- Comments (denoted by '#')
- Variables
```
x : num 0.5; # initial binding denoted by ':' followed by variable type and an expression
x = 0.8; // assignment statement
```
- Functions
All identifier bindings start with an identifier. For functions the colon is followed by an identifier and a type specifier. The function specifier goes at the end with the return type, if no return type is provided it is a void return.
```
identity : n int : fn int {
  return n;
}
```
