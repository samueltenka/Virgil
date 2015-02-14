#ifndef WORDTOVECTOR_H
#define WORDTOVECTOR_H

#include <string>
#include <list>
#include <fstream>
#include <algorithm>
#include <map>
#include <vector>

namespace Word {
   typedef std::list<std::string> WordList;
   typedef std::map<std::string, int> Dictionary;

   int is_punctuation(int c) {
      return !(c==' ' || c=='\n' || c=='\t' || ('a'<=c && c<='z') || ('A'<=c && c<='Z'));
   }

   void standardize(std::string &word) {
      auto remainder = std::remove_if(word.begin(), word.end(), is_punctuation);
      word.erase(remainder);
      std::transform(word.begin(), word.end(), word.begin(), ::tolower);
   }

   void add_words(std::string filename, WordList &words) {
      std::ifstream textfile;
      textfile.open(filename);
      std::string word;
      while(textfile >> word) {
         standardize(word);
         words.push_back(word);
      } textfile.close();
   }

   void init_dict(WordList &words, Dictionary &dict) {
      int n=0;
      for(WordList::const_iterator it=words.begin(), end=words.end(); it!=end; ++it) {
         if(dict.count(*it)==0) {
            dict.insert(std::pair<std::string, int>(*it, n));
            n += 1;
         }
      }
   }
}

#endif
