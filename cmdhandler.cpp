#include "cmdhandler.h"
#include "util.h"
using namespace std;

QuitHandler::QuitHandler()
{

}

QuitHandler::QuitHandler(Handler* next)
    : Handler(next)
{

}

bool QuitHandler::canHandle(const std::string& cmd) const
{
    return cmd == "QUIT";
}

Handler::HANDLER_STATUS_T QuitHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr)
{
    return HANDLER_QUIT;
}

PrintHandler::PrintHandler()
{

}

PrintHandler::PrintHandler(Handler* next)
    : Handler(next)
{

}

bool PrintHandler::canHandle(const std::string& cmd) const
{
    return cmd == "PRINT";

}

Handler::HANDLER_STATUS_T PrintHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr)
{
    string name;

    if (!(instr >> name)) {
        return HANDLER_ERROR;
    }
    try {
        eng->display_page(ostr, name);
    }
    catch (std::exception& e) {
        return HANDLER_ERROR;
    }
    return HANDLER_OK;
}

//INCOMING HANDLER
IncomingHandler::IncomingHandler() {

}

IncomingHandler::IncomingHandler(Handler* next) 
    : Handler(next)
{

}

bool IncomingHandler::canHandle(const std::string& cmd) const
{
    return cmd == "INCOMING";
}

Handler::HANDLER_STATUS_T IncomingHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr) {
    string term;
    WebPage* WP;
    WebPageSet set;
    instr >> term;

    if (term == "") {
        return HANDLER_ERROR;
    }
    WP = eng->retrieve_page(term);

    if(WP==NULL) {
        return HANDLER_ERROR;
    }

    set = WP->incoming_links();
    display_hits(set, ostr);

    return HANDLER_OK;   
}


//OUTGOING HANDLER
OutgoingHandler::OutgoingHandler() {

}

OutgoingHandler::OutgoingHandler(Handler* next) 
    : Handler(next)
{

}

bool OutgoingHandler::canHandle(const std::string& cmd) const
{
    return cmd == "OUTGOING";
}

Handler::HANDLER_STATUS_T OutgoingHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr) {

    string term;
    WebPage* WP;
    WebPageSet set;
    instr >> term;

    if (term == "") {
        return HANDLER_ERROR;
    }
    WP = eng->retrieve_page(term);

    if(WP==NULL) {
        return HANDLER_ERROR;
    }

    set = WP->outgoing_links();
    display_hits(set, ostr);

    return HANDLER_OK;   
}


//AND HANDLER
AndHandler::AndHandler() {

}

AndHandler::AndHandler(Handler* next) 
    : Handler(next)
{

}

bool AndHandler::canHandle(const std::string& cmd) const
{
    return cmd == "AND";
}

Handler::HANDLER_STATUS_T AndHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr) {

    string name;
    std::vector<std::string> term;
    AndWebPageSetCombiner comb;
    WebPageSet set;


    while(instr >> name) {
        term.push_back(name);
    }
    
    set = eng->search(term, &comb);

    display_hits(set, ostr);
    
    return HANDLER_OK;

}


//OR HANDLER
OrHandler::OrHandler() {

}

OrHandler::OrHandler(Handler* next) 
    : Handler(next)
{

}

bool OrHandler::canHandle(const std::string& cmd) const
{
    return cmd == "OR";
}

Handler::HANDLER_STATUS_T OrHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr) {

    string name;
    std::vector<std::string> term;
    OrWebPageSetCombiner comb;
    WebPageSet set;


    while(instr >> name) {
        term.push_back(name);
    }
    
    set = eng->search(term, &comb);

    display_hits(set, ostr);
    
    return HANDLER_OK;
}


//DIFF HANDLER

DiffHandler::DiffHandler() {

}

DiffHandler::DiffHandler(Handler* next) 
    : Handler(next)
{

}

bool DiffHandler::canHandle(const std::string& cmd) const
{
    return cmd == "DIFF";
}

Handler::HANDLER_STATUS_T DiffHandler::process(SearchEng* eng, std::istream& instr, std::ostream& ostr) {

    string name;
    std::vector<std::string> term;
    DiffWebPageSetCombiner comb;
    WebPageSet set;


    while(instr >> name) {
        term.push_back(name);
    }
    
    set = eng->search(term, &comb);

    display_hits(set, ostr);
    
    return HANDLER_OK;

}
