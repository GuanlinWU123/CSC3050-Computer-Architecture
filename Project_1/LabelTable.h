//
// Created by 吴冠霖 on 2022/2/26.
//

#ifndef ASS1_LABELTABLE_H
#define ASS1_LABELTABLE_H

#include <string>
#include <vector>

class LabelTable {

public:

/*
 * Constructor: LabelTable
 * Usage: LabelTable map;
 * ---------------------
 * Initializes a new empty map that uses strings as both keys and values.
 */

    LabelTable();

/*
 * Destructor: LableTablee
 * ----------------------
 * Frees any heap storage associated with this map.
 */

    ~LabelTable();

/*
 * Method: get
 * Usage: string value = map.get(key);
 * -----------------------------------
 * Returns the value for key or the empty string, if key is unbound.
 */

    std::string get(const std::string & key, int startPos) const;

/*
 * Method: put
 * Usage: map.put(key, value);
 * ---------------------------
 * Associates key with value in this map.
 */

    void put(const std::string & key, const std::string & value);

private:


/* Instance variables */

    std::vector<std::string> keys;
    std::vector<std::string> values;
/* Private function prototypes */
    int findKey(const std::string & key, int startPos) const;


};

#endif //ASS1_LABELTABLE_H
