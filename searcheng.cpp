#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include "searcheng.h"

using namespace std;

// Helper function that will extract the extension of a filename
std::string extract_extension(const std::string& filename);

std::string extract_extension(const std::string& filename)
{
    size_t idx = filename.rfind(".");
    if (idx == std::string::npos) {
        return std::string();
    }
    return filename.substr(idx + 1);
}

SearchEng::SearchEng()
{

}

SearchEng::~SearchEng()
{

    //deallocate PageParsers
    std::map<std::string, PageParser*>::iterator it;
    for(it = parsers_.begin(); it != parsers_.end(); ++it) {
        delete it->second;
    }

    //deallocate WebPages
    std::map<std::string, WebPage*>::iterator it2;
    for(it2 = pages_.begin(); it2 != pages_.end(); ++it2) {
        delete it2->second;
    }
    
    //deallocate WebPageSet
    std::map<std::string, WebPageSet*>::iterator it3;
    for(it3 = term_set.begin(); it3 != term_set.end(); ++it3) {
        delete it3->second;
    }

}

void SearchEng::register_parser(const std::string& extension, PageParser* parser)
{
    if (parsers_.find(extension) != parsers_.end())
    {
        throw std::invalid_argument("parser for provided extension already exists");
    }
    parsers_.insert(make_pair(extension, parser));
}

void SearchEng::read_pages_from_index(const std::string& index_file)
{
    ifstream ifile(index_file.c_str());
    if(ifile.fail()) {
        cerr << "Unable to open index file: " << index_file << endl;
    }

    // Parse all the files
    string filename;
    while(ifile >> filename) {
#ifdef DEBUG
        cout << "Reading " << filename << endl;
#endif
        read_page(filename);
    }
    ifile.close();
}

void SearchEng::read_page(const string& filename)
{

    string ext = extract_extension(filename);
    
    //file has an extension but no registered parser 
        if(parsers_.find(ext) == parsers_.end()) {
             throw std::logic_error("the file has an extension but no parser is registered for that extension");
        }

    //check if filename exists
    ifstream ifile(filename.c_str());
    if(ifile.fail()) {
        throw std::invalid_argument("the filename does not exist");
    }

    //get or create a webpage depending if one doesnt exist
    WebPage* webp;
    
    if(pages_.find(filename) == pages_.end()) {
        webp = new WebPage(filename);
        pages_.insert(make_pair(filename, webp));
    }
    else {
        webp = pages_[filename];
    }

    //parser the given file with the appropriate parser
    std::set<std::string> allSearchableTerms;
    std::set<std::string> allOutgoingLinks;
 
    parsers_[ext]->parse(ifile, allSearchableTerms, allOutgoingLinks);


    //add parsed context to webp
        //terms
    webp->all_terms(allSearchableTerms);
        //links
    //need to create a new WebPage for links
    WebPage* links_page;
    std::set<std::string>::iterator it;

    for(it = allOutgoingLinks.begin(); it != allOutgoingLinks.end(); ++it) {
        if(pages_.find(*it) == pages_.end()) {
            links_page = new WebPage(*it);
            pages_.insert(make_pair(*it, links_page));
        }
        else {
            links_page = pages_[*it];
        }
        links_page->add_incoming_link(webp);
        webp->add_outgoing_link(links_page);
    }


    //add webpage info to pages_ and term_set
    pages_[filename] = webp;

    std::set<std::string>::iterator it3;
    
    for(it3 = allSearchableTerms.begin(); it3 != allSearchableTerms.end(); ++it3) {
        //check if the term is in the map <webpageset> .end() checker

        std::map<std::string, WebPageSet*>::iterator it2 = term_set.find(*it3);
        if(it2 == term_set.end()) {
            WebPageSet* set = new WebPageSet;
            //if not you create that term in your map.
          term_set[*it3] = set;
          term_set[*it3]->insert(webp);
          
        }
        else {
            (it2->second)->insert(webp);
        }    
        
    }    

}

WebPage* SearchEng::retrieve_page(const std::string& page_name) const
{
    std::map<std::string, WebPage*>:: const_iterator it = pages_.find(page_name);
    //if doesn't exist then return nullptr
    if(it== pages_.end()) {
        return NULL;
    }
    else {
        return it->second;
    }
}

void SearchEng::display_page(std::ostream& ostr, const std::string& page_name) const
{
    string ext = extract_extension(page_name);

    //check if page name exists
    if(pages_.find(page_name) == pages_.end()) {
        throw std::invalid_argument("the page_name does not exist");
    }

    std::map<string,PageParser*>::const_iterator parser;
    parser = parsers_.find(ext);

    if(parser == parsers_.end()) {
        throw std::logic_error("the file has an extension but no parser is registered for that extension");
    }

    ifstream ifile(page_name);

    string result = (parser->second)->display_text(ifile);

    ostr << result;
}

WebPageSet SearchEng::search(const std::vector<std::string>& terms, WebPageSetCombiner* combiner) const
{
    WebPageSet result;

    //no terms
    if(terms.size() == 0) {
        WebPageSet* res = new WebPageSet;
        return *res;
    }
    std::map<std::string, WebPageSet*>::const_iterator it = term_set.find(terms[0]);

    //make sure its in term_set
    if(it != term_set.end()) {
        result = *it->second;
    }

    std::string current_t;

    if(terms.size() == 1) {
        return result;
    }
    else { //iterate through and combine()
        for(size_t i = 1; i <terms.size(); i++) {
            current_t = terms[i];
            std::map<std::string, WebPageSet*>::const_iterator it2;
            it2 = term_set.find(current_t);

            if(it != term_set.end()) {
                result = combiner->combine(result, *it2->second);
            }
        }
    }
    return result;
}

