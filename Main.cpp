#include "WordToVector.h"
#include "SamRandom.h"
#include "Weights.h"
#include <iostream>
#include <fstream>

using namespace std;

void write_to(string filename, Matrix &M) {
   std::ofstream textfile;
   textfile.open(filename);
   for(int i=0; i<M.height; ++i) {
      for(int j=0; j<M.width; ++j) {
         textfile << M.coef[i][j] << " ";
      } textfile << endl;
   }
   textfile.close();
}

void main() {
   SamRandom::init_random();

   cout << "Hi!" << endl; cout << "I'm reading some books:" << endl;
   Word::WordList words; cout << "\tAlice..." << endl;
   //Word::add_words("test.txt", words);
   Word::add_words("AliceInWonderland.txt", words); cout << "\tA Connecticut Yankee in King Arthur's Court..." << endl;
   //Word::add_words("ConnecticutYankee.txt", words); cout << "\toh! and some poems by Allen Ginsburg!" << endl;
   //Word::add_words("AllenGinsburg.txt", words);
   Word::Dictionary dict; cout << "OK, now I'm initializing my dictionary..." << endl;   
   Word::init_dict(words, dict); 
   cout << "Wow! my dictionary has " << dict.size() << " words!" << endl;

   cout << "Now I'm preparing my system for remembering word associations!" << endl;
   Weights::initialize_brain(dict.size(), 8);

   std::ofstream textfile;
   textfile.open("hist.txt");
   for(Word::WordList::const_iterator it=words.begin(), end=words.end(); ; ) {
      Weights::predict(*it, dict);
      std::string pred = Weights::get_prediction(dict);
      cout << *it << " " << pred << endl;
      textfile << *it << " " << pred << endl;
      ++it; if(it==end) {break;}
      Weights::correct(*it, dict, 0.01);
   }
   textfile.close();

   write_to("C.txt", Weights::C);
   write_to("R.txt", Weights::R);
   write_to("D.txt", Weights::D);

   char L; cin >> L;
}
