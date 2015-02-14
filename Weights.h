#include "WordToVector.h"
#include <string>
#include <math.h>
#include "SamRandom.h"

typedef std::vector<double> Vector;
struct Matrix {
   int height, width;
   std::vector<Vector> coef;
};

namespace Weights {
   void initialize(int width, Vector &V) {
      V.resize(width);
      for(int i=0; i<width; ++i) {V[i] = 0.0;}
   } void initialize(int height, int width, Matrix &M) {
      M.height = height; M.width = width;
      M.coef.resize(height);
      for(int i=0; i<height; ++i) {initialize(width, M.coef[i]);}
   } void randomize(Matrix &M) {
      for(int i=0; i<M.height; ++i) {
         for(int j=0; j<M.width; ++j) {
            M.coef[i][j] = SamRandom::get_random(1.0);
         }
      }
   } void add(int width, Vector &a, const Vector b, double scale) {
      for(int j=0; j<width; ++j) {a[j] += scale*b[j];}
   }

   double sigma(double x) {return 1.0/(1.0+exp(-x));}
   void softmax(int width, Vector &V) {
      Vector e; initialize(width, e);
      double sum = 0.0;
      for(int j=0; j<width; ++j) {e[j] = exp(V[j]); sum += V[j];}
      for(int j=0; j<width; ++j) {V[j] = e[j]/sum;}
   }
   
   int wdim, mdim; // word and meaning dimensions
   Matrix C, R, D, Cgrad, Rgrad, Dgrad;
   Vector memory, out;

   void predict(std::string in_word, Word::Dictionary &dict) {
      if(dict.count(in_word)==0) {return;}

      initialize(wdim, out);
      Vector mvect = C.coef[dict[in_word]];
      add(mdim, memory, memory, -0.2); // memorize mvect
      add(mdim, memory, mvect, +0.2);
      for(int j=0; j<mdim; ++j) {memory[j] = sigma(memory[j]);}
      add(mdim, mvect, mvect, -0.8); // TODO: replace by R
      add(mdim, mvect, memory, +0.8);
      for(int j=0; j<mdim; ++j) {add(wdim, out, D.coef[j], mvect[j]);}
      softmax(wdim, out);
   }

   void correct(std::string actual_word, Word::Dictionary &dict) {
      Vector avect = C.coef[dict[actual_word]];
      Vector error; initialize(wdim, error);
      add(wdim, error, out, +1.0);
      add(wdim, error, avect, -1.0);


   }
}
