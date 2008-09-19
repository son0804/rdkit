//
//  Copyright (C) 2004-2008 Greg Landrum and Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//

//! \file RankAtoms.h
/*!
    \brief Utility functionality used by atom rankers.
*/
#ifndef _RD_RANKATOMS_H_
#define _RD_RANKATOMS_H_

#include <queue>
#include <vector>
#include <list>
#include <algorithm>

namespace RankAtoms {
  typedef std::vector<int> INT_VECT;
  typedef std::list<int> INT_LIST;

  //! utility function for ranking atoms
  void updateInPlayIndices(const INT_VECT &ranks,INT_LIST &indicesInPlay);

  //! returns the count of unique items in an std::vector
  template <typename T>
  unsigned int countClasses(const std::vector<T> &vect){
    std::vector<T> sortedVect = vect;
    std::sort(sortedVect.begin(),sortedVect.end(),std::less<T>());
    typename std::vector<T>::iterator newEnd=std::unique(sortedVect.begin(),sortedVect.end());
    return newEnd-sortedVect.begin();
  }

  //! functor for implementing > on two std::pairs.  The first entries are compared.
  template <typename T>
  struct pairGTFunctor {
    bool operator() (const std::pair<T,int> &v1,const std::pair<T,int> &v2){
      return v1.first > v2.first;
    }
  };

  //! function for implementing < on two std::pairs.  The first entries are compared.
  template <typename T>
  bool pairLess(const std::pair<T,int> &v1,const std::pair<T,int> &v2){
    return v1.first < v2.first;
  }

  template <typename T>
  class argless {
  public:
    argless(const T& c) : container(c) {};
    bool operator() (unsigned int v1,unsigned int v2){
      return container[v1]<container[v2];
    }
    const T &container;
  };

  
  //! ranks the entries in a vector
  /*!
    \param vect the vector to rank
    \param res  is used to return the ranks of each entry
  */
  template <typename T>  
  void rankVect(const std::vector<T> &vect,INT_VECT &res){
    PRECONDITION(res.size()>=vect.size(),"vector size mismatch");
    unsigned int nEntries = vect.size();

    std::vector< unsigned int > indices(nEntries);
    for(unsigned int i=0;i<nEntries;++i) indices[i]=i; 
    std::sort(indices.begin(),indices.end(),argless< std::vector<T> >(vect) );

    int currRank=0;
    T lastV = vect[indices[0]];
    for(unsigned int i=0;i<nEntries;++i){
      T v = vect[indices[i]];
      if(v==lastV){
        res[indices[i]] = currRank;
      } else {
        res[indices[i]] = ++currRank;
        lastV = v;
      }
    }
  }    

  //! finds the relative rankings of the entries in \c vals.
  /*!
    \param nAtoms         the number of points in play
    \param vals           the values to be ranked
    \param indicesInPlay  a list containing the indices that
           are being considered (only those entries in \c ranks
           that appear in \c indicesInPlay will be modified)
    \param ranks          the current ranks of entries, this is updated
           with new ranks
  */
  template <typename T>
  void sortAndRankVect(unsigned int nAtoms,
                       const std::vector<T> &vals,
                       const INT_LIST &indicesInPlay,
                       INT_VECT &ranks) {
    // --------------
    //
    // start by getting the internal ranking of the values passed in
    //
    // --------------
    INT_VECT newRanks;
    newRanks.resize(vals.size());
    rankVect(vals,newRanks);

    // --------------
    //  
    // At the end of this operation, fixedRanks will contain the ranks
    // of atoms that are no longer active (-1 for active atoms).
    //
    // --------------
    for(INT_LIST::const_iterator ilCIt=indicesInPlay.begin();
        ilCIt!=indicesInPlay.end();++ilCIt){
      ranks[*ilCIt] = -1;
    }

#ifdef VERYVERBOSE_CANON
    std::cout << "new: ";
    debugVect(newRanks);
    std::cout << "fixed: ";
    debugVect(ranks);
#endif

    INT_VECT idxVect;
    idxVect.assign(indicesInPlay.begin(),indicesInPlay.end());

    // -------------
    //
    //  Loop over all the new ranks.  We'll know that we're done
    //  when currNewRank > maxNewRank
    //
    // -------------
    INT_VECT::iterator ivIt;
    int currNewRank= *(std::min_element(newRanks.begin(),newRanks.end()));
    int maxNewRank = *(std::max_element(newRanks.begin(),newRanks.end()));
    while(currNewRank<=maxNewRank){
      //
      // If this rank is already present in ranks, increment
      //  this rank and all new ranks that are higher:
      //
      while(std::find(ranks.begin(),ranks.end(),currNewRank)!=ranks.end()){
        for(ivIt=newRanks.begin();ivIt!=newRanks.end();++ivIt){
          if(*ivIt>=currNewRank)
            *ivIt += 1;
        }
        // increment both thie current rank *and* the maximum new rank
        ++currNewRank;
        ++maxNewRank;
      }

      //
      //  now grab all entries with this new rank and copy them into
      //  the ranks list
      //
      ivIt=std::find(newRanks.begin(),newRanks.end(),currNewRank);
      while(ivIt!=newRanks.end()){
        int offset=ivIt-newRanks.begin();
        int idx = idxVect[offset];
        ranks[idx] = currNewRank;
        ivIt++;
        ivIt=std::find(ivIt,newRanks.end(),currNewRank);
      }
      ++currNewRank;
    }
  }
}
#endif
