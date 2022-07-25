#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cctype>
#include "md_parser.h"
#include "util.h"

using namespace std;

typedef enum { NORMALTEXT, LINKTEXT, ISLINK, LINKURL } PARSE_STATE_T;

void MDParser::parse(std::istream& istr, std::set<std::string>& allSearchableTerms, std::set<std::string>& allOutgoingLinks)
{

    // Remove any contents of the sets before starting to parse.
    allSearchableTerms.clear();
    allOutgoingLinks.clear();

    // Complete your code below

    string term = "";
    string link = "";
    char c = istr.get();

    while(!istr.fail()) {
        //take in one char at a time, see if its valid, if so append to string. 
        //once string is complete add to terms/links set and the clear it to add the next

        //anchor text
        if(c == '[') { //will the if exit if c moves in the while loop
            while(c != ']' && !istr.fail()) { 
                // Is c a character to split terms?
                if (!isalnum(c)) {
                    // If we have a term to add, convert it to a standard case and add it
                    if(term != "") {
                        term = conv_to_lower(term);
                        allSearchableTerms.insert(term);
                    }
                    term.clear();
                }
                // Otherwise we continually add to the end of the current term.
                else {
                    term += c;
                }
                // Attempt to get another character from the file.
                c = istr.get();
            }
            if(term != "") {
                term = conv_to_lower(term);
                allSearchableTerms.insert(term);
            }
            term.clear();
        }

        //link      //edge case 4
        char nextc = istr.get();
        if(c == ']' && nextc == '(') { //edge case 5
            c = istr.get(); //has to pull from after the next char '('
            while(c != ')') { //edge case 1
                link += c;
                c = istr.get();
            }
            if(link != "") {
                allOutgoingLinks.insert(link);
                link.clear();
            }
            nextc = c; 
        }

        //all other text
        else { //nextc is the new c that is valid
            if(isalnum(c)) {
                term += c;
            }        
            while(isalnum(nextc)) {
                //continually add to the end of the current term.
                term += nextc;
                // Attempt to get another character from the file.
                 nextc = istr.get();
            }
            if(!isalnum(nextc)) {
                // If we have a term to add, convert it to a standard case and add it
                if(term != "") {
                    term = conv_to_lower(term);
                    allSearchableTerms.insert(term);
                }
                term.clear();
            }
            c = nextc;
        }        
    }

    //edge cases to consider: example 2 
    if(term != "") {
        term = conv_to_lower(term);
        allSearchableTerms.insert(term);  
        term.clear();
    }
    //edge cases to consider: example 3
    if(link != "") {
        allOutgoingLinks.insert(link);
        link.clear();
    }
}

std::string MDParser::display_text(std::istream& istr)
{
    string result = "";

    //go through the istr and collect a char one by one. when its 

    char c = istr.get();
    char ccopy;

    while(!istr.fail()) {
        //checking for the start of the link and runs to the end of it
        if(c == ']') {
            ccopy = c;
            c = istr.get();
            if(c == '(') {
                result += ccopy;
                while(c != ')') {
                    c = istr.get();
                }
                if(istr.peek()!=EOF) {
                    c = istr.get();
                }
            }
            else {
                result += ccopy;
            }
        }
        if(istr.peek()!=EOF) {
            result += c;
        }
        c = istr.get();
    }
    return result;
}
