#pragma once
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

// Cleans a specific token by:
string cleanToken(string s) {
    int i, j;
    int endIdx = s.length();
    bool hasLetters = false;

    //checks for letters. if not returns empty string
    for (char c: s) {
        if (isalpha(c)) {
            hasLetters = true;
            break;
        }
    }

    if (!hasLetters) {
        return "";
    }

    // remove leading punctation
    for (i = 0; i < s.length(); i++) {
        if (!ispunct(s.at(i))) {
            break;
        }
    }

    //remove end punct
    for (j = s.length(); j > i; j--) {
        if (!ispunct(s.at(j - 1))) {
            break;
        }
    }

    // converts to lowercase
    string newS = s.substr(i, j - i);
    for (char& c: newS) {
        c = tolower(c);
    }

    return newS;
}


// Returns a set of the cleaned tokens in the given text.
set<string> gatherTokens(string text) {
    set<string> wordBank;
    istringstream iss(text);
    string word, cleanWord;

    while (iss >> word) {   //traverse until word is found
        cleanWord = cleanToken(word);
        if (!cleanWord.empty()) {       //cleans the word before inserting into set
            wordBank.insert(cleanWord);
        }
    }
    return wordBank;
}

// Builds inverted index from data file. Existing contents of `index` are left in place.
int buildIndex(string filename, map<string, set<string>>& index) {
    string url, content;
    ifstream infile(filename);
    int urlCtr = 0;

    if (infile.fail()) {
        cout << "Invalid filename." << endl;
        return 0;
    }

    while (!infile.eof()) {
        getline(infile, url);   //gets url lines from file
        getline(infile, content); //gets string content from file
        set<string> contentSet;
        if (url != "") {
            urlCtr++;
        } 

        contentSet = gatherTokens(content);
        
        if (!infile.fail()) {
            for (string currWord:contentSet) {
                if (url != "") {
                    index[currWord].emplace(url); //insert into index
                } 
            }
        }
    }
    infile.close();
    return urlCtr;
}

// Runs a search query on the provided index. returns a set of results.
// Search terms are processed using set operations (union, intersection, difference).
set<string> findQueryMatches(map<string, set<string>>& index, string sentence) {
    string word, cleanedWord;
    stringstream my_stream(sentence);
    set<string> urls;
    int count = 1;  //acounts for only the first word urls and later reassigns set correctly
    int flag;
    
    while (my_stream >> word) {
        flag = 0;   //rests flag for every word
        set<string> temp;

        // flagging the first char in a word to later correspond to the set operation 
        if (word.at(0) == '+') {
            flag  = 1;
        } else if (word.at(0) == '-') {
            flag = 2;
        } else  if (word.at(0) != '+' && word.at(0) != '-') {
            flag = 0;
        }

        cleanedWord = cleanToken(word);

        if (!cleanedWord.empty()) {
            set<string> wordUrls = index[cleanedWord]; 
            if (count == 1) {
                urls = wordUrls;    //adds the word's urls to the inital set of urls 
                count--;    //sets count back to 0 for later iterations of loop to perform off of the first word
            } else {
                if (flag == 0) {
                    set_union(urls.begin(), urls.end(), wordUrls.begin(), wordUrls.end(), inserter(temp, temp.begin()));
                } else if (flag == 1) {
                    set_intersection(urls.begin(), urls.end(), wordUrls.begin(), wordUrls.end(), inserter(temp, temp.begin()));
                } else if (flag == 2) {
                    set_difference(urls.begin(), urls.end(), wordUrls.begin(), wordUrls.end(), inserter(temp, temp.begin()));
                }
                urls = temp;
            }
        }
    }
    return urls;
}


// Runs the main command loop for the search program
void searchEngine(string filename) {
    map<string, set<string>> index;
    int numPages = 0;
    string userQuery;

    numPages = buildIndex(filename, index);
    cout << "Stand by while building index..." << endl;
    cout << "Indexed " << numPages << " pages containing " << index.size() << " unique terms" << endl;

    while (true) {
        cout << endl <<"Enter query sentence (press enter to quit): ";
        getline(cin, userQuery);

        if (userQuery.empty()) {
            cout << "Thank you for searching!";
            break;
        }

        set<string> matches = findQueryMatches(index, userQuery);
       
         cout << "Found " << matches.size() << " matching pages" << endl;
        
        // prints out the url for each query
        for (const auto& url: matches) {
            cout << url << endl;
        }
    }
}
