#include "SkipList.h"
#include <iostream>
typedef SkipList::Entry Entry;
using namespace std;

SkipList::SkipList() : listHeads() {
    makeNewLevelList();
    makeNewLevelList();
}

// makes a new list on the top level of existing list.
// call only when top list is NULL or just the two sentinels. 
void SkipList::makeNewLevelList() {
    SkipList::Entry* minusInfinity = new Entry("!!", "");	// "!!" < any other string.
    SkipList::Entry* plusInfinity = new Entry("}}", "");	// "}}" > any other key.

    Quad* first = new Quad(minusInfinity);
    Quad* last = new Quad(plusInfinity);

    int numLists = listHeads.size();
    Quad* oldFirst = numLists == 0 ? NULL : listHeads[numLists - 1];
    Quad* oldLast  = numLists == 0 ? NULL : oldFirst->next;

    first->prev = NULL;
    first->next = last;
    first->above = NULL;
    first->below = oldFirst;

    last->prev = first;
    last->next = NULL;
    last->above = NULL;
    last->below = oldLast;

    if(oldFirst != NULL) {
        oldFirst->above = first;
        oldLast->above = last;
    }
    listHeads.push_back(first);
}

void SkipList::printOneList(int listNum) {
	Quad* bottomCurrent = listHeads[0];
	Quad* current = listHeads[listNum];

	while(bottomCurrent->next != NULL) {
		std::string toPrint;
		if(current->entry->getKey() == bottomCurrent->entry->getKey()) {
			toPrint = current->entry->getKey();
			current = current->next;
		}
		else {
			toPrint = "--";
		}
		cout << "--" << toPrint;
		bottomCurrent = bottomCurrent->next;
	}
	cout << "--" << bottomCurrent->entry->getKey() << "--" << endl;
}

void SkipList::print() {
	int numLists = listHeads.size();
	for(int i = numLists - 1; i >= 0; i--) {
		printOneList(i);
	}
} 

SkipList::Entry* SkipList::find(Key k) {
    int numLists = listHeads.size();
    Quad* current = listHeads[numLists - 1];

    while (current->below != NULL) {
        current = current->below;			// drop down
        while(k >= current->next->entry->getKey()) {	// scan forward
            current = current->next;
        }
    } 

    if(current->entry->key == k) {
        return current->entry;
    }
    else {
        return NULL;
    }
}

std::vector<SkipList::Quad*>* SkipList::findWithTrail(Key k) {
    std::vector<SkipList::Quad*>* trail = new std::vector<Quad*>();

    int numLists = listHeads.size();
    Quad* current = listHeads[numLists - 1];

    while (current != NULL) {
        while(k >= current->next->entry->getKey()) {	// scan forward
            current = current->next;
        }
	trail->push_back(current);
        current = current->below;			// drop down
    } 
    return trail;
}

///CUSTOM FIND FEATURE FOR INSERT FUNCTION
std::vector<SkipList::Quad*>* SkipList::CustomTrail(Key k, int QuadHeight){
    std::vector<SkipList::Quad*>* trail = new std::vector<Quad*>();

    int numLists = listHeads.size();
    Quad* current = listHeads[numLists - 1 - (numLists - QuadHeight)];

    while (current != NULL) {
        while(current->next->entry->getKey() < k && (current->next->entry->getKey() != "}}") ){
            current = current->next;
        }
	trail->push_back(current);     
    current = current->below;			// drop down
    } 
    return trail;
}

///CUSTOM HEIGHT FUNCTION
int SkipList::FlipCoinHeight(){
    int retHeight = 0;
    int looping = 1;
    while(looping){
        if((std::rand() % 2) == 1){
            retHeight++;
        }else{
            looping = 0;
        }
    }
    if(retHeight >0){return retHeight;} else{return 1;}
}

void SkipList::insert(Key k, Value v){
    //GENERATE HEIGHT, CREATE NEW LEVELS
    int QuadHeight = FlipCoinHeight();
    if(QuadHeight >= listHeads.size()){
        int additionalLevels = ((QuadHeight- listHeads.size()) + 1);
        for(int i =0; i < additionalLevels; i++){
            makeNewLevelList();
        }
    }

    //MAKE TRAIL FOR INSERTION, USING CUSTOM TRAIL FUNCTION
    std::vector<SkipList::Quad*>* trail = CustomTrail(k, QuadHeight);
    //Entry* newEntry = new Entry(k, v);

    //INSERT NEW QUAD IN TRAIL
    for(int i = 0; i < trail->size(); i++ ){
        Entry* newEntry = new Entry(k, v);
        Quad* newQuad = new Quad(newEntry);
        
        newQuad->next= trail->at(i)->next;
        trail->at(i)->next->prev = newQuad;
        trail->at(i)->next = newQuad;
        newQuad->prev = trail->at(i);

        if(i > 0){
            newQuad->above = trail->at(i-1)->next;
            newQuad->above->below = newQuad;
        }
    }

    delete trail; trail = NULL;
}

void SkipList::remove(Key k){
    //FIND ALL INSTANCES KEY WITH TRAIL
    std::vector<SkipList::Quad*>* trail = findWithTrail(k);

    //REMOVE ALL INSTANCES OF KEY
    for(int i = 0; i < trail->size(); i++ ){
        if(trail->at(i)->entry->getKey() != k){

        }else{
            trail->at(i)->prev->next = trail->at(i)->next;
            trail->at(i)->next->prev = trail->at(i)->prev;
            delete trail->at(i)->entry;
            delete trail->at(i);
        }   
    }

    //REMOVE NEW EMPTY LAYERS
    for(int i =0; i < listHeads.size()-1; i++){
        if(listHeads.size() == 2){

        }else{
            if(listHeads.at(i)->next->entry->getKey() == "}}"){
                printOneList(i);cout<<endl;
                listHeads.erase(listHeads.begin() + i);
            }
        }
    }
    delete trail; trail = NULL;
}

Entry* SkipList::ceilingEntry(Key k){
   Quad* Iterator;
   Iterator = listHeads.at(0);
   while(1){
        if(Iterator->next->entry->getKey() == "}}" || Iterator->next->entry->getKey() >= k){
            return Iterator->next->entry;
        }
        else{
            Iterator = Iterator->next;
        }
   }
}

Entry* SkipList::floorEntry(Key k){    
    Quad* Iterator;
    Iterator = listHeads.at(0);
    while(1){
        if(Iterator->next->entry->getKey() == "}}" || Iterator->next->entry->getKey() > k){
            return Iterator->entry;
        }
        else{
            Iterator = Iterator->next;
        }
    }
}

Entry* SkipList::greaterEntry(Key k){
    Quad* Iterator;
    Iterator = listHeads.at(0);
    while(1){
        if(Iterator->next->entry->getKey() == "}}" || Iterator->next->entry->getKey() > k){
            return Iterator->next->entry;
        }
        else{
            Iterator = Iterator->next;
        }
    }
}

Entry* SkipList::lesserEntry(Key k){
    Quad* Iterator;
    Iterator = listHeads.at(0);
    while(1){
        if(Iterator->next->entry->getKey() == "}}" || Iterator->next->entry->getKey() >= k){
            return Iterator->entry;
        }
        else{
            Iterator = Iterator->next;
        }
    }
}