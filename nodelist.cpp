#include <Arduino.h>
#include <cstdlib> //only use for debugging in VS
#include "nodelist.h"
#include "node.h"

//constructor
NodeList::NodeList() {
	//initialize list to max size and set size counter to 0
	
	currentSize = 0;
}


void NodeList::addChild(Node node) {
	//first ensure that list has enough room
	//arduino environment has no implicit exception handling, so we have to hope this situation doesn't arise
	if (currentSize >= MAX_SIZE) {

	}
	//else, add the passed node
	else {
		list[currentSize + 1] = node;
		currentSize++;
	}

}

void NodeList::removeChild(int ID) {
	//function should do nothing if array size is 0
	if (currentSize = 0) {
		true;
	}

	//iterate through list and check for id, if item is in middle of list move every following item down one position
	for (int i = 0; i < currentSize; i++) {
		if (list[i].getID() == ID){

			//we have found the element, so this address in the arry will be
			//overwritten with the next, so on until the end
			for (int j = i; j < currentSize; j++) {
				list[j] = list[j + 1];
			}

			
		}
		//item was not found, do nothing
		else {
			Serial.println("");
		}
	}
}