# simpLex
Simple and fast lexer in C++ header (and example of json output)     
   
Easy to modify. Can register new symbols (operators) at runtime.  
Useful if your language needs user-defined operators at runtime (operators 'split' text like whitespace would)   
  
Features:   
- Identifiers with numbers at end or certain symbols at start.   
- Strings using " , ' , ` and ``` . escaped chars.     
- Single and multiline comments, even nested (toggleable).     
- Numbers, even `.3` (0.3) or `3.` (3.0) or `-3` (toggleable `-` merging) , scientific notation.   
- Registering new operators at runtime.   
- Optional newline emitting (useful for one-line statement ending like `#define` in C), escaped newlines.  
      
Comes with an example usage file `example.cpp + example.txt` which prints out the json of all symbols parsed.  
  
