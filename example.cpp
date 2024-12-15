#include "simpLex.hpp"

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>


using namespace std;
using namespace SimpLex;

std::string readFileIntoString(std::string filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    
    if (file) {
        buffer << file.rdbuf();
    } else {
        // Handle error - for example, throw an exception or return a special value
    }

    return buffer.str();
}


//  eg: (true, "{","}",".")
#define ___pushSymbols(splitting,...) do{                                                    \
        const char *ARRAY[] = {__VA_ARGS__};                                         \
        for (unsigned int II = 0; II < sizeof(ARRAY) / sizeof(char *); II++)                  \
            l.registerNewSymbol(II, string(ARRAY[II]),splitting); \
    }while(0);
// eg: ("new","free","for","while")
#define ___pushIdents(...) do{                                      \
        const char *ARRAY[] = {__VA_ARGS__};                        \
        for (unsigned int II = 0; II < sizeof(ARRAY) / sizeof(char *); II++) \
            l.regIdent(string(ARRAY[II]));                     \
    }while(0);
 //idents.push_back(string(ARRAY[II]));

void __populateExampleTokens(Lex& l)
{ 

    ___pushSymbols( true,
                    "{", "}", "(", ")", "[", "]", 
                    ".","...", ",", ":", "::", ";", "?", 
                    "<", ">", "/", "*", "%", "+", "-", "=", "++", "--", "!", "&", "|", "&&", "||", 
                    ">>", "<<", ">=", "<=", "<<<", ">>>", "**", "***", "<=>", "==", "===","!==", "!=", "+=", "-=", "^", "^=", "/=", "*=", "%=",
                    "=>",
                    //".=", ".?", 
                    "?.", "??",  //"?:", "?,"
                    /*
                    //////// unique weird symbol ideas:
                    "~","~~","~>","<~", 
                    "{:",":}","[:",":]","(:",":)","<:",":>",
                    "[>]","[<]","(>)","(<)",
                    "<~>",">=<",">~<",">-<","<^>","<|>","<+>","<!>",
                    "<:>",":|:","|:|", "|_|",
                    //"@", "#", "##", "$",
                    ":=",
                    "\\",
                    ":::",
                    "<[", "]>", "<{", "}>", "<(", ")>",
                    "!.", "?\?=", "?try=",
                    */
                    );

}

void escapedJSON(string str, stringstream& s){
    for(int i = 0; i < str.size(); i++){
        auto c = str[i];
        if     (c == '\n') s << "\\n";
        else if(c == '\r') s << "\\r";
        else if(c == '\t') s << "\\t";
        else if(c == '\v') s << "\\v";
        else if(c == '\0') s << "\\0";
        else if(c == '\'') s << "'";
        else if(c == '"') s << "\\\"";
        else if(c == '\\') s << "\\\\";
        else if(c == '`') s << "`"; 
        else s << c;
    }
};

int main(int argc,char** argv){
    string txt = readFileIntoString(string(argv[1])); //read first arg. 0th is prog name

    Lex lex(txt);
    //lex.DBG = true;

    int reservedWordsIdx = -1;
    {
        auto&& l = lex;
        //___pushIdents("new", "free", "delete", "for", "while", "return", "break", "continue", "else", "elif", "try", "finally", "switch", "case");
        ___pushIdents("new", "delete", "class", "for", "while", "return", "break", "continue", "if", "else", //"elif",
         "try", "catch","finally", "switch", "case", "import","from","as",//"with","using", "struct"
         "const","let","var","async","await","function","export","default",
         //"constructor", "this", // ?
         "null","undefined","true","false"
         );
        reservedWordsIdx = l.stringStorage_entries.size();
    }


    __populateExampleTokens(lex);
    lex.sortRegisteredSymbols();
    
    lex.parseAll( );

    // We have parsed. Now to emit json:

    stringstream ss; 
    ss<<"{\"symbols\":[";
    for(int i = 0; i<lex.symbols.size();i++){
        ss<<"\"";
        escapedJSON(lex.symbols[i].str,ss);
        ss<<"\"";
        if(i!=(lex.symbols.size()-1)) ss<<",";
    }
    ss<<"],\"parsed\":";

    ss<<"[";

    for(int j = 0; j <lex.parsed.size(); j++){
        auto& s = lex.parsed[j];
        if(s.type == s.Symbol){
            ss << "[0,\"";
            escapedJSON(lex.symbols[s.data_index].str, ss);
            ss << "\"]";
            //std::cout<<"Symbol:"<< lex.symbols[s.data_index].str << std::endl;
        }else if(s.type == s.Identifier){
            auto sx = lex.stringStorage_entries[s.data_index].to_string(lex.stringStorage);
            if(s.data_index<reservedWordsIdx) ss<<"[0,\"";
            else ss << "[1,\"";
            escapedJSON(sx,ss);
            ss << "\"]";
        }else if(s.type == s.StringLiteral){
            auto sx = lex.stringStorage_entries[s.data_index].to_string(lex.stringStorage);
            ss << "[2,\"";
            escapedJSON(sx,ss);
            ss << "\"]";
        }else if(s.type == s.NumberLiteral){
            auto sx = lex.stringStorage_entries[s.data_index].to_string(lex.stringStorage);
            ss << "[3,";
            ss << sx;
            ss << "]";
        }
        else if(s.type == s.Newline){
            ss << "[4,0]";
        }

        if(j != (lex.parsed.size()-1)) 
            ss << ",";
    }
    ss << "]";

    ss<<"}";
    std::cout << ss.str();

    return 0;
}
