// $Id$
//
//  Copyright (C) 2001-2006 Greg Landrum and Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//
#include <RDGeneral/utils.h>
#include <RDGeneral/Invariant.h>
#include <GraphMol/RDKitBase.h>
#include <GraphMol/RDKitQueries.h>
#include "SubstructMatch.h"
#include "SubstructUtils.h"
#include <boost/smart_ptr.hpp>

#include <argedit.h>
#include <vf2_mono_state.h>
#include <match.h>

namespace RDKit{
  typedef ARGraph<const Atom, const Bond> AR_MOLGRAPH;

#ifdef CACHE_ARMOLGRAPHS
  // FIX: given that it's using a global and creates a core leak this is no 
  //      permanent solution. 
namespace SubstructLocal {
  std::vector<AR_MOLGRAPH *> molGraphCache;
  void clearMolGraphCache(){
    molGraphCache.clear(); 
  }
}
#endif  

  AR_MOLGRAPH *getMolGraph(const ROMol &mol){
    AR_MOLGRAPH *molG=0;
#ifdef CACHE_ARMOLGRAPHS
    if(mol.hasProp("_SubstructGraphPtr")){
      unsigned int idx;
      mol.getProp("_SubstructGraphPtr",idx);
      if(idx<SubstructLocal::molGraphCache.size()){
        molG = SubstructLocal::molGraphCache[idx];
        //std::cerr << " << " << &mol << " " << idx << " = " << molG << "("<<SubstructLocal::molGraphCache.size()<<")"<< std::endl;
      }
    } 
    if(!molG){
      ARGEdit mEd;
      MolToVFGraph(mol,&mEd);
      molG = new AR_MOLGRAPH(&mEd);
      unsigned int idx = SubstructLocal::molGraphCache.size();
      mol.setProp("_SubstructGraphPtr",idx,true);
      SubstructLocal::molGraphCache.push_back(molG);
      //std::cerr << " >> " << &mol << " " << idx << " = " << molG << std::endl; 
    }
#else
    ARGEdit mEd;
    MolToVFGraph(mol,&mEd);
    molG = new AR_MOLGRAPH(&mEd);
#endif
    return molG;
  }

  void MatchSubqueries(AR_MOLGRAPH *molG,QueryAtom::QUERYATOM_QUERY *q,bool useChirality);

  // ----------------------------------------------
  //
  // find one match
  //
  bool SubstructMatch(const ROMol &mol,const ROMol &query,MatchVectType &matchVect,
                      bool recursionPossible,bool useChirality)
  {
    AR_MOLGRAPH *molG = getMolGraph(mol);
    bool res = SubstructMatch(molG,query,matchVect,recursionPossible,useChirality);
#ifndef CACHE_ARMOLGRAPHS
    delete molG;
#endif
    return res;
  }

  bool SubstructMatch(AR_MOLGRAPH *molG,const ROMol &query,MatchVectType &matchVect,
                      bool recursionPossible,bool useChirality){
    PRECONDITION(molG,"bad molecule");

    if(recursionPossible){
      ROMol::ConstAtomIterator atIt;
      for(atIt=query.beginAtoms();atIt!=query.endAtoms();atIt++){
        if((*atIt)->getQuery()){
          MatchSubqueries(molG,(*atIt)->getQuery(),useChirality);
        }
      }
    }
    AR_MOLGRAPH *queryG = getMolGraph(query);
    if(!useChirality){
      queryG->SetNodeCompat(atomCompat);
    } else {
      queryG->SetNodeCompat(chiralAtomCompat);
    }
    queryG->SetEdgeCompat(bondCompat);

    int nQueryAtoms;
    nQueryAtoms = query.getNumAtoms();
    node_id *ni1 = new node_id[nQueryAtoms];
    node_id *ni2 = new node_id[nQueryAtoms];
    MatcherState s0(queryG,molG);

    bool res;
    int n;
    matchVect.clear();
    matchVect.resize(0);
    if(match(&s0,&n,ni1,ni2)){
      matchVect.resize(nQueryAtoms);
      for(int i=0;i<nQueryAtoms;i++) matchVect[i]=std::pair<int,int>(ni1[i],ni2[i]);
      res = true;
    } else {
      res = false;
    }
    delete [] ni1;
    delete [] ni2;
#ifndef CACHE_ARMOLGRAPHS
    delete queryG;
#endif

    return res;
  }


  // ----------------------------------------------
  //
  // find all matches
  //
  //  NOTE: this blows out the contents of matches
  //
  unsigned int SubstructMatch(const ROMol &mol,const ROMol &query,
			      std::vector< MatchVectType > &matches,
			      bool uniquify,bool recursionPossible,
			      bool useChirality) {
    AR_MOLGRAPH *molG = getMolGraph(mol);
    unsigned int res = SubstructMatch(molG,query,matches,uniquify,recursionPossible,useChirality);
#ifndef CACHE_ARMOLGRAPHS
    delete molG;
#endif
    return res;
  }
  unsigned int SubstructMatch(AR_MOLGRAPH *molG,const ROMol &query,
			      std::vector< MatchVectType > &matches,
			      bool uniquify,bool recursionPossible,
			      bool useChirality)
  {
    PRECONDITION(molG,"bad molecule pointer");

    if(recursionPossible){
      ROMol::ConstAtomIterator atIt;
      for(atIt=query.beginAtoms();atIt!=query.endAtoms();atIt++){
        if((*atIt)->getQuery()){
          MatchSubqueries(molG,(*atIt)->getQuery(),useChirality);
        }
      }
    }

    AR_MOLGRAPH *queryG = getMolGraph(query);
    if(!useChirality){
      queryG->SetNodeCompat(atomCompat);
    } else {
      queryG->SetNodeCompat(chiralAtomCompat);
    }
    queryG->SetEdgeCompat(bondCompat);

    MatcherState s0(queryG,molG);

    matches.clear();
    matches.resize(0);
    unsigned int res;
    res = match(&s0,substructVisitor,(void *)&matches);

    if(res){
      if(uniquify){
           removeDuplicates(matches);
           res = matches.size();
      }
    } else {
      matches.clear();
      matches.resize(0);
    }

#ifndef CACHE_ARMOLGRAPHS
    delete queryG;
#endif

    return res;
  }


  // ----------------------------------------------
  //
  // Intended for internal use 
  //
  unsigned int RecursiveMatcher(AR_MOLGRAPH *molG,const ROMol &query,
				std::vector< int > &matches,bool useChirality)
  {
    PRECONDITION(molG,"bad molecule");
    //std::cout << " >>> RecursiveMatcher: " << int(query) << std::endl;
    ARGEdit qEd;

    ROMol::ConstAtomIterator atIt;
    for(atIt=query.beginAtoms();atIt!=query.endAtoms();atIt++){
      if((*atIt)->getQuery()){
        MatchSubqueries(molG,(*atIt)->getQuery(),useChirality);
      }
    }
 
    AR_MOLGRAPH *queryG = getMolGraph(query);
    if(!useChirality){
      queryG->SetNodeCompat(atomCompat);
    } else {
      queryG->SetNodeCompat(chiralAtomCompat);
    }
    queryG->SetEdgeCompat(bondCompat);

    MatcherState s0(queryG,molG);

    matches.clear();
    matches.resize(0);
    unsigned int res;
    res = match(&s0,substructHeadVisitor,(void *)&matches);

    if(!res){
      matches.clear();
      matches.resize(0);
    }
    //std::cout << " <<< RecursiveMatcher: " << int(query) << std::endl;
#ifndef CACHE_ARMOLGRAPHS
    delete queryG;
#endif
  
    return res;
  }


  void MatchSubqueries(AR_MOLGRAPH  *molG,QueryAtom::QUERYATOM_QUERY *query,bool useChirality){
    PRECONDITION(molG,"bad molecule");
    PRECONDITION(query,"bad query");
    //std::cout << "*-*-* MS: " << (int)query << std::endl;
    //std::cout << "\t\t" << typeid(*query).name() << std::endl;
    if(query->getDescription()=="RecursiveStructure"){
      RecursiveStructureQuery *rsq=(RecursiveStructureQuery *)query;
      ROMol const *queryMol = rsq->getQueryMol();
      // in case we are reusing this query, clear its contents now.
      rsq->clear();
      if(queryMol){
        std::vector< int > matchStarts;
        unsigned int res = RecursiveMatcher(molG,*queryMol,matchStarts,useChirality);
        if(res){
          for(std::vector<int>::iterator i=matchStarts.begin();
              i!=matchStarts.end();
              i++){
            rsq->insert(*i);
          }
        }
      }
    } else {
      //std::cout << "\tmsq1: "; 
    }
  
    // now recurse over our children (these things can be nested)
    Queries::Query<int,Atom const*,true>::CHILD_VECT_CI childIt;
    //std::cout << query << " " << query->endChildren()-query->beginChildren() <<  std::endl;
    for(childIt=query->beginChildren();childIt!=query->endChildren();childIt++){
      MatchSubqueries(molG,childIt->get(),useChirality);
    }
    //std::cout << "<<- back " << (int)query << std::endl;
  }

};
