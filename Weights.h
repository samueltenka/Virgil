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
   void zero(int width, Vector &V) {for(int i=0; i<width; ++i) {V[i] = 0.0;}}
   void initialize(int width, Vector &V) {V.resize(width); zero(width, V);}
   void initialize(int height, int width, Matrix &M) {
      M.height = height; M.width = width;
      M.coef.resize(height);
      for(int i=0; i<height; ++i) {initialize(width, M.coef[i]);}
   } void randomize(Matrix &M) {
      for(int i=0; i<M.height; ++i) {
         for(int j=0; j<M.width; ++j) {
            M.coef[i][j] = SamRandom::get_random(0.1)-0.05;
         }
      }
   } void add(int width, Vector &a, const Vector b, double scale) {
      for(int j=0; j<width; ++j) {a[j] += scale*b[j];}
   } void copy(int width, Vector &a, const Vector b) {
      for(int j=0; j<width; ++j) {a[j] = b[j];}
   } void times(int height, int width, Vector &out, const Matrix &M, const Vector &in) {
      zero(height, out);
      for(int d=0; d<height; ++d) {
         for(int s=0; s<width; ++s) {
            out[d] += M.coef[d][s]*in[s];
         }
      }
   }
   void print(int width, Vector &V) {
      for(int j=0; j<width; ++j) {printf("%2f ", V[j]);}
   } void print(Matrix &M) {
      for(int i=0; i<M.height; ++i) {print(M.width, M.coef[i]); printf("\n");}
   }

   double sigma(double x) {return 1.0/(1.0+exp(-x));}
   double expon(double x) {return exp(x);}
   double ds_from_s(double s) {return s*(1-s);}
   double de_from_e(double e) {return e;}
   void sigmafy(int width, Vector &V) {for(int j=0; j<width; ++j) {V[j] = sigma(V[j]);}}
   void exponfy(int width, Vector &V) {for(int j=0; j<width; ++j) {V[j] = expon(V[j]);}}
   
   int wdim, mdim; // word and meaning dimensions
   Matrix C, R, D;
   int in_word_index;
   Vector memo, mind, pred, mind_err, pred_err;
   void initialize_brain(int num_words, int mental_richness) {
      wdim = num_words; mdim = mental_richness;
      initialize(mdim, wdim, C); randomize(C);
      initialize(mdim, mdim, R); randomize(R);
      initialize(wdim, mdim, D); randomize(D);
      initialize(mdim, memo);
      initialize(mdim, mind); initialize(mdim, mind_err);
      initialize(wdim, pred); initialize(wdim, pred_err);
   }

   void predict(std::string in_word, Word::Dictionary &dict) {
      if(dict.count(in_word)==0) {return;}
      int x = dict.count(in_word);
      in_word_index = dict[in_word];

      copy(mdim, memo, mind);
      for(int j=0; j<mdim; ++j) {mind[j] = C.coef[j][in_word_index];}
      Vector recall; initialize(mdim, recall);
      times(mdim, mdim, recall, R, memo);
      add(mdim, mind, recall, 1.0);
      sigmafy(mdim, mind);

      times(wdim, mdim, pred, D, mind);
      exponfy(wdim, pred);
   }
   std::string get_prediction(Word::Dictionary &dict) {
      std::string rtrn = "NO PREDICTION!"; double prob = -1.0;
      for(Word::Dictionary::const_iterator it=dict.begin(), end=dict.end(); it!=end; ++it) {
         //printf("%d %d\n", wdim, it->second);
         double new_prob = pred[it->second];
         if(new_prob > prob) {
            rtrn = it->first;
            prob = new_prob;
         }
      }
      return rtrn;
   }

   void correct(std::string actual_word, Word::Dictionary &dict, double dt) {
      if(dict.count(actual_word)==0) {return;}

      zero(wdim, pred_err);
      // TODO: normalize pred, etc.?
      copy(wdim, pred_err, pred);
      pred_err[dict[actual_word]] -= 1.0;
      for(int i=0; i<wdim; ++i) {pred_err[i] *= de_from_e(pred[i]);}

      for(int d=0; d<mdim; ++d) {add(mdim, mind_err, R.coef[d], pred_err[d]);}
      for(int s=0; s<mdim; ++s) {mind_err[s] *= ds_from_s(mind[s]);}

      for(int i=0; i<wdim; ++i) {add(mdim, D.coef[i], mind, -dt*pred_err[i]);}
      for(int j=0; j<mdim; ++j) {add(mdim, R.coef[j], memo, -dt*mind_err[j]);}
      for(int j=0; j<mdim; ++j) {C.coef[j][in_word_index] -= dt*mind_err[j];}
   }
}
