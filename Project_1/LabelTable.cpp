//
// Created by 吴冠霖 on 2022/2/26.
//

#include "LabelTable.h"
#include <string>
using namespace std;

/*
 * Implementation notes: StringMap constructor and destructor
 * ----------------------------------------------------------
 * All dynamic allocation is handled by the Vector class.
 */

LabelTable::LabelTable() { }
LabelTable::~LabelTable() { }

/*
 * Implementation notes: put, get
 * ------------------------------
 * These methods use findKey to search for the specified key.
 */

string LabelTable::get(const string & key, int startPos) const {
    int index = findKey(key, startPos);
    return (index == -1) ? "" : values.at(index);
}

void LabelTable::put(const string & key, const string & value) {
    keys.push_back(key);
    values.push_back(value);
}
/*
 * Private method: findKey
 * -----------------------
 * Returns the index at which the key appears, or -1 if the key is not found.
 */

int LabelTable::findKey(const string & key, int startPos) const {
    for (int i = startPos; i < keys.size(); i++) {
        if (keys.at(i) == key) return i;
    }
    return -1;
}
