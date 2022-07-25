#include "searcheng.h"
#include "combiners.h"

// Complete the necessary code

WebPageSet AndWebPageSetCombiner::combine(const WebPageSet& setA, const WebPageSet& setB) {
    //webpages that are in both setA and setB

    WebPageSet result;

    std::set<WebPage*>::iterator it;
    for(it = setA.begin(); it != setA.end(); ++it) {

        if(setB.find(*it) != setB.end()) {
            result.insert(*it);
        }
        
    } 
    return result; 

}

WebPageSet OrWebPageSetCombiner::combine(const WebPageSet& setA, const WebPageSet& setB) {
    //webpages that are in either

    WebPageSet result;

    std::set<WebPage*>::iterator it;
    for(it = setA.begin(); it != setA.end(); ++it) {
        result.insert(*it);
    }   

    for(it = setB.begin(); it != setB.end(); ++it) {
        result.insert(*it);
    }

    return result;
    
}

WebPageSet DiffWebPageSetCombiner::combine(const WebPageSet& setA, const WebPageSet& setB) {
   //webpages setA - setB
    WebPageSet result;

    std::set<WebPage*>::iterator it;
    for(it = setA.begin(); it != setA.end(); ++it) {
        if(setB.find(*it) == setB.end()) {
            result.insert(*it);
        }
    }

    return result;
    
}
