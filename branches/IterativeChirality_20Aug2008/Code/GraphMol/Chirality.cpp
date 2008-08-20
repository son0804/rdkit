// $Id$
//
//  Copyright (C) 2004-2008 Greg Landrum and Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//
#include <GraphMol/RDKitBase.h>
#include <GraphMol/RankAtoms.h>
#include <RDGeneral/types.h>
#include <sstream>
#include <RDGeneral/utils.h>
#include <RDGeneral/Invariant.h>
#include <RDGeneral/RDLog.h>

#include <boost/dynamic_bitset.hpp>
#include <Geometry/point.h>

namespace Chirality {
  using namespace RDKit;

  typedef std::pair<int,int> INT_PAIR;
  typedef std::vector<INT_PAIR> INT_PAIR_VECT;
  typedef std::vector<INT_PAIR>::iterator INT_PAIR_VECT_I;
  typedef std::vector<INT_PAIR>::const_iterator INT_PAIR_VECT_CI;

  template <typename T>
  void debugVect(const std::vector<T> arg){
    typename std::vector<T>::const_iterator viIt;
    std::stringstream outS;
    for(viIt=arg.begin();viIt!=arg.end();viIt++){
      outS << *viIt << " ";
    }
    BOOST_LOG(rdDebugLog) << outS.str() << std::endl;
  }
  
  // compare the first elements of two pairs of integers
  int _pairComp(const INT_PAIR &arg1,const INT_PAIR &arg2){
    return (arg1.first < arg2.first);
  }

  // --------------------------------------------------
  //
  // Calculates chiral invariants for the atoms of a molecule
  //  These are based on Labute's proposal in:
  //  "An Efficient Algorithm for the Determination of Topological
  //   RS Chirality" Journal of the CCG (1996)
  //
  // --------------------------------------------------
  void buildCIPInvariants(const ROMol &mol,DOUBLE_VECT &res){
    PRECONDITION(res.size()>=mol.getNumAtoms(),"res vect too small");
    int atsSoFar=0;
    //
    // NOTE:
    // If you make modifications to this, keep in mind that it is
    // essential that the initial comparison of ranks behave properly.
    // So, though it seems like it would makes sense to include
    // information about the number of Hs (or charge, etc) in the CIP
    // invariants, this will result in bad rankings.  For example, in
    // this molecule: OC[C@H](C)O, including the number of Hs would
    // cause the methyl group (atom 3) to be ranked higher than the CH2
    // connected to O (atom 1).  This is totally wrong. 
    //
    for(ROMol::ConstAtomIterator atIt=mol.beginAtoms();atIt!=mol.endAtoms();++atIt){
      Atom const *atom = *atIt;
      unsigned long invariant = 0;
      int num = atom->getAtomicNum() % 128;
      // get an int with the deviation in the mass from the default:
      int mass = static_cast<int>(atom->getMass() -
                                  PeriodicTable::getTable()->getAtomicWeight(atom->getAtomicNum()));
      mass += 8;
      if(mass < 0) mass = 0;
      else mass = mass % 16;

      // NOTE: the inclusion of hybridization in the invariant (as
      // suggested in the original paper), leads to the situation
      // that 
      //   C[C@@](O)(C=C)C(C)CC 
      // and
      //   C[C@@](O)(C=C)C(C)CO 
      // are assigned S chirality even though the rest of the world
      // seems to agree that they ought to be R (atom 3, sp2, is ranked
      // higher than atom 5, sp3, no matter what their environments)
      int hyb=0;
      switch(atom->getHybridization()) {
      case Atom::SP: hyb=6;break;
      case Atom::SP2: hyb=5;break;
      case Atom::SP3: hyb=1;break;
      case Atom::SP3D: hyb=3;break;
      case Atom::SP3D2: hyb=2;break;
      default: break;
      }

      invariant = num; // 7 bits here
      invariant = (invariant << 4) | mass;
      // See above for why this is commented out.
      //invariant = (invariant << 3) | hyb;
      res[atsSoFar++] = invariant;
    }
  }

  // construct a vector with <atomIdx,direction> pairs for 
  // neighbors of a given atom.  This list will only be
  // non-empty if at least one of the bonds has its direction
  // set.
  void findAtomNeighborDirHelper(const ROMol &mol,const Atom *atom,
                                 const Bond *refBond,
                                 const INT_VECT &ranks,
                                 INT_PAIR_VECT &neighbors){
    PRECONDITION(atom,"bad atom");
    PRECONDITION(refBond,"bad bond");

    bool seenDir=false;
    ROMol::OEDGE_ITER beg,end;
    boost::tie(beg,end) = mol.getAtomBonds(atom);
    ROMol::GRAPH_MOL_BOND_PMAP::const_type pMap = mol.getBondPMap();
    while(beg!=end){
      Bond *bond = pMap[*beg];
      Bond::BondDir dir=bond->getBondDir();
      if( bond->getIdx() != refBond->getIdx()){
        if(dir == Bond::ENDDOWNRIGHT || dir == Bond::ENDUPRIGHT){
          seenDir=true;
          // If we're considering the bond "backwards", (i.e. from end
          // to beginning, reverse the effective direction:
          if(atom != bond->getBeginAtom()){
            if(dir==Bond::ENDDOWNRIGHT) dir=Bond::ENDUPRIGHT;
            else dir=Bond::ENDDOWNRIGHT;
          }
        }
        Atom *nbrAtom=bond->getOtherAtom(atom);
        neighbors.push_back( std::make_pair(nbrAtom->getIdx(), dir) );
      }
      ++beg;
    }
    if(!seenDir) {
      neighbors.clear();
    } else if( neighbors.size() == 2 &&
               ranks[neighbors[0].first] == ranks[neighbors[1].first] ){
      // the two substituents are identical, no stereochemistry here
      neighbors.clear();
    } else {
      // it's possible that direction was set only one of the bonds, set the other
      // bond's direction to be reversed:
      if( neighbors[0].second != Bond::ENDDOWNRIGHT &&
          neighbors[0].second != Bond::ENDUPRIGHT ){
        CHECK_INVARIANT(neighbors.size()>1,"too few neighbors");
        neighbors[0].second =
          neighbors[1].second==Bond::ENDDOWNRIGHT ? Bond::ENDUPRIGHT : Bond::ENDDOWNRIGHT;
      } else if ( neighbors.size()>1 &&
                  neighbors[1].second != Bond::ENDDOWNRIGHT &&
                  neighbors[1].second != Bond::ENDUPRIGHT ){
        neighbors[1].second =
          neighbors[0].second==Bond::ENDDOWNRIGHT ? Bond::ENDUPRIGHT : Bond::ENDDOWNRIGHT;
      }
    }
  }

  // find the neighbors for an atoms that are not connected by single bond that is not refBond
  // if checkDir is true only neighbor atoms with bonds marked with a direction will be returned
  void findAtomNeighborsHelper(const ROMol &mol,const Atom *atom,const Bond *refBond,
                               INT_VECT &neighbors, bool checkDir=false) {
    PRECONDITION(atom,"bad atom");
    PRECONDITION(refBond,"bad bond");
    ROMol::OEDGE_ITER beg, end;
    boost::tie(beg, end) = mol.getAtomBonds(atom);
    ROMol::GRAPH_MOL_BOND_PMAP::const_type pMap = mol.getBondPMap();
    while (beg != end) {
      const Bond *bond=pMap[*beg];
      Bond::BondDir dir = bond->getBondDir();
      if (bond->getBondType()==Bond::SINGLE && bond->getIdx() != refBond->getIdx()) {
        if (checkDir) {
          if ((dir != Bond::ENDDOWNRIGHT) && (dir != Bond::ENDUPRIGHT)) {
            ++beg;
            continue;
          }
        }
        Atom *nbrAtom = bond->getOtherAtom(atom);
        neighbors.push_back(nbrAtom->getIdx());
      }
      ++beg;
    }
  }

  // clear the ENDUPRIGHT or ENDDOWNRIGHT tags on any bonds in a ring 
  // FIX: At some point we may want to consider cis/trans stuff in rings as well
  void clearCisTransRingBonds(ROMol &mol) {
    ROMol::OEDGE_ITER beg, end;
    ROMol::GRAPH_MOL_BOND_PMAP::type pMap = mol.getBondPMap();
    // on the first pass mark all single bond neighbors to non-ring double bonds
    // that have cis/trans
    for(ROMol::BondIterator bondIt=mol.beginBonds();
        bondIt!=mol.endBonds();
        ++bondIt){
      unsigned int bid = (*bondIt)->getIdx();
      if (!(mol.getRingInfo()->numBondRings(bid)) &&
          ((*bondIt)->getBondType() == Bond::DOUBLE)) {
        Atom *begAtom = (*bondIt)->getBeginAtom();
        Atom *endAtom = (*bondIt)->getEndAtom();
        unsigned int nBegDir = 0;
        unsigned int nEndDir = 0;
          
        boost::tie(beg, end) = mol.getAtomBonds(begAtom);
        while (beg != end) {
          const Bond *bond= pMap[*beg];
          Bond::BondDir dir = bond->getBondDir();
          if( bond->getIdx() != bid)  {
            if (dir == Bond::ENDDOWNRIGHT || dir == Bond::ENDUPRIGHT) {
              ++nBegDir;
            }
          }
          ++beg;
        }

        boost::tie(beg, end) = mol.getAtomBonds(endAtom);
        while (beg != end) {
          const Bond *bond= pMap[*beg];
          Bond::BondDir dir = bond->getBondDir();
          if( bond->getIdx() != bid)  {
            if (dir == Bond::ENDDOWNRIGHT || dir == Bond::ENDUPRIGHT) {
              ++nEndDir;
            }
          }
          ++beg;
        }
        
        // now mark these single bond is both end of the double bond has atleast one direction bond neighbor
        if ((nBegDir>0) && (nEndDir>0)) {
          boost::tie(beg, end) = mol.getAtomBonds(begAtom);
          while (beg != end) {
            const Bond *bond= pMap[*beg];
            Bond::BondDir dir = bond->getBondDir();
            if( bond->getIdx() != bid)  {
              if (dir == Bond::ENDDOWNRIGHT || dir == Bond::ENDUPRIGHT) {
                // check if _keepDir has already been set on this bond (from  a previous double bond)
                // in this case this single bond is involved in more than one cis/trans systems
                int val = 0;
                if (bond->hasProp("_keepDir")) {
                  bond->getProp("_keepDir", val);
                }
                ++val;
                bond->setProp("_keepDir", val, 1);
              }
            }
            ++beg;
          }
          boost::tie(beg, end) = mol.getAtomBonds(endAtom);
          while (beg != end) {
            const Bond *bond= pMap[*beg];
            Bond::BondDir dir = bond->getBondDir();
            if( bond->getIdx() != bid)  {
              if (dir == Bond::ENDDOWNRIGHT || dir == Bond::ENDUPRIGHT) {
                int val = 0;
                if (bond->hasProp("_keepDir")) {
                  bond->getProp("_keepDir", val);
                }
                ++val;
                bond->setProp("_keepDir", val, 1);
              }
            }
            ++beg;
          } // end of while over neighbor bonds around endAtom
          // finally mark this double bond itself as containing neihbors with directions
          (*bondIt)->setProp("_hasDirNbrs", 1);
        } // end of if both end of double bond have dir bonds
      } // loop over non-ring double bonds
    } // loop over all bonds
 
    // clear directions on any bonds that do not have the "_keepDir" property set on
    for(ROMol::BondIterator bondIt=mol.beginBonds();
        bondIt!=mol.endBonds();
        ++bondIt){
      Bond::BondDir dir = (*bondIt)->getBondDir();
      if (!(*bondIt)->hasProp("_keepDir") &&
          ((dir == Bond::ENDDOWNRIGHT) || (dir == Bond::ENDUPRIGHT)) ) {
        (*bondIt)->setBondDir(Bond::NONE);
      }
    }
  }


  // returns true if the atom is allowed to have stereochemistry specified
  bool checkChiralAtomSpecialCases(ROMol &mol,const Atom *atom){
    PRECONDITION(atom,"bad atom");

    if(!mol.getRingInfo()->isInitialized()){
      VECT_INT_VECT sssrs;
      MolOps::symmetrizeSSSR(mol, sssrs);
    }

    const RingInfo *ringInfo=mol.getRingInfo();
    if(ringInfo->numAtomRings(atom->getIdx())){
      // the atom is in a ring, so the "chirality" specification may actually
      // be handling ring stereochemistry, check for another chiral tagged
      // atom in this atom's rings:
      const VECT_INT_VECT atomRings=ringInfo->atomRings();
      for(VECT_INT_VECT::const_iterator ringIt=atomRings.begin();
          ringIt!=atomRings.end();++ringIt){
        if(std::find(ringIt->begin(),ringIt->end(),static_cast<int>(atom->getIdx()))!=ringIt->end()){
          for(INT_VECT::const_iterator idxIt=ringIt->begin();
              idxIt!=ringIt->end();++idxIt){
            if(*idxIt!=static_cast<int>(atom->getIdx()) &&
               mol.getAtomWithIdx(*idxIt)->getChiralTag()!=Atom::CHI_UNSPECIFIED){
              // we get to keep the stereochem specification on this atom:
              return true;
            }
          }
        }
      }
    }
    return false;
  }

} // end of chirality namespace

namespace RDKit{

  namespace MolOps {
    typedef INT_VECT CIP_ENTRY;
    typedef std::vector< CIP_ENTRY > CIP_ENTRY_VECT;


    // ************************************************************
    //
    // Figure out the CIP ranks for the atoms of a molecule
    //
    // ARGUMENTS:
    //    mol: the molecule to be altered
    //  ranks: used to store the set of ranks.  Should be at least
    //         mol.getNumAtoms() long.
    //
    // NOTES: 
    //    - All atoms also gain a property "_CIPRank" with their overall
    //      CIP ranking.
    //
    // ************************************************************
    void assignAtomCIPRanks(const ROMol &mol, INT_VECT &ranks){
      PRECONDITION(ranks.size()>=mol.getNumAtoms(),"bad ranks size");

      if(!mol.hasProp("_CIPCalculated")){
        int numAtoms = mol.getNumAtoms();
        CIP_ENTRY_VECT cipEntries(numAtoms);
        INT_LIST allIndices;
        INT_LIST activeIndices;

        for(int i=0;i<numAtoms;++i){
          activeIndices.push_back(i);
          allIndices.push_back(i);
        }

        // get the initial invariants:
        DOUBLE_VECT invars(numAtoms,0);
        Chirality::buildCIPInvariants(mol,invars);

#ifdef VERBOSE_CANON
        BOOST_LOG(rdDebugLog) << "invariants:" << std::endl;
        for(int i=0;i<numAtoms;i++){
          BOOST_LOG(rdDebugLog) << i << ": " << invars[i] << std::endl;
        }
#endif  

        // rank those:
        RankAtoms::sortAndRankVect(numAtoms,invars,allIndices,ranks);
#ifdef VERBOSE_CANON
        BOOST_LOG(rdDebugLog) << "initial ranks:" << std::endl;
        for(int i=0;i<numAtoms;++i){
          BOOST_LOG(rdDebugLog) << i << ": " << ranks[i] << std::endl;
        }
#endif  
        RankAtoms::updateInPlayIndices(ranks,activeIndices);
        // Start each atom's rank vector with its atomic number:
        //  Note: avoid the temptation to use invariants here, those lead to
        //  incorrect answers
        for(int i=0;i<numAtoms;i++){
          cipEntries[i].push_back(mol.getAtomWithIdx(i)->getAtomicNum());
        }

        // Loop until either:
        //   1) all classes are uniquified
        //   2) we've gone through numAtoms/2 times
        //      We only do numAtoms/2 because that's the maximum
        //      number of steps required for two atoms to "feel" each
        //      other (each influences one additional neighbor shell
        //      per iteration). 
        //      EFF: This number could be reduced further using the
        //           distance matrix.  
        int numIts=0;
        while( numIts<numAtoms/2 && !activeIndices.empty()){
          unsigned int longestEntry=0;
          // ----------------------------------------------------
          //
          // for each atom, get a sorted list of its neighbors' ranks:
          //
          for(INT_LIST_I it=allIndices.begin();
              it!=allIndices.end();
              ++it){
            CIP_ENTRY localEntry;

            // start by pushing on our neighbors' ranks:
            ROMol::ADJ_ITER nbr,endNbrs;
            boost::tie(nbr,endNbrs) = mol.getAtomNeighbors(mol.getAtomWithIdx(*it));
            while(nbr != endNbrs){
              int rank=ranks[*nbr]+1;
              unsigned int count=static_cast<int>(ceil(mol.getBondBetweenAtoms(*nbr,*it)->getBondTypeAsDouble()));
              localEntry.insert(localEntry.end(),count,rank);
              ++nbr;
            }
            // add a zero for each coordinated H:
            // (as long as we're not query atom)
            if(!mol.getAtomWithIdx(*it)->hasQuery()){
              localEntry.insert(localEntry.end(),
                                mol.getAtomWithIdx(*it)->getTotalNumHs(),
                                0);
            }

            // get a sorted list of our neighbors' ranks:
            std::sort(localEntry.begin(),localEntry.end());
            // and copy it on in reversed order:
            cipEntries[*it].insert(cipEntries[*it].end(),
                                   localEntry.rbegin(),
                                   localEntry.rend());
            if(cipEntries[*it].size() > longestEntry){
              longestEntry = cipEntries[*it].size();
            }
          }
          // ----------------------------------------------------
          //
          // pad the entries so that we compare rounds to themselves:
          // 
          for(INT_LIST_I it=allIndices.begin();it!=allIndices.end();
              ++it){
            unsigned int sz=cipEntries[*it].size();
            if(sz<longestEntry){
              cipEntries[*it].insert(cipEntries[*it].end(),
                                     longestEntry-sz,
                                     -1);
            }
          }
          // ----------------------------------------------------
          //
          // sort the new ranks and update the list of active indices:
          // 
          RankAtoms::sortAndRankVect(numAtoms,cipEntries,allIndices,ranks);
          RankAtoms::updateInPlayIndices(ranks,activeIndices);
          ++numIts;
#ifdef VERBOSE_CANON
          BOOST_LOG(rdDebugLog) << "strings and ranks:" << std::endl;
          for(int i=0;i<numAtoms;i++){
            BOOST_LOG(rdDebugLog) << i << ": " << ranks[i] << " > ";
            debugVect(cipEntries[i]);
          }
#endif
        }
        // copy the ranks onto the atoms:
        for(int i=0;i<numAtoms;i++){
          mol.getAtomWithIdx(i)->setProp("_CIPRank",ranks[i],1);
        }
        mol.setProp("_CIPCalculated",1,1);
      } else {
        // we've already calculated the ranks, so just read them
        // from the atoms
        for(unsigned int i=0;i<mol.getNumAtoms();++i){
          mol.getAtomWithIdx(i)->getProp("_CIPRank",ranks[i]);
        }
      }
    }

    // ************************************************************
    //
    // Figure out the CIP chiral codes for the atoms of a molecule
    //
    // ARGUMENTS:
    //       mol: the molecule to be altered
    //   cleanIt: (OPTIONAL) if this bool is set, atoms marked chiral
    //            that do not meet the minimum requirements for chirality 
    //            will have their chiral flags cleared.
    //   force: (OPTIONAL) if this bool is set, the calculation will be repeated
    //            even if it has already been done
    //
    // NOTES:
    //    - The codes are stored on atoms as a string property named
    //      "_CIPCode".
    //    - Throughout we assume that we're working with a hydrogen-suppressed
    //      graph.
    //
    // ************************************************************
    void assignAtomChiralCodes(ROMol &mol,bool cleanIt,bool force){
      if(!force && mol.hasProp("_AtomChiralitySet")) return;
      
      INT_VECT ranks;
      ranks.resize(mol.getNumAtoms());
      bool cipDone=false;


      // ------------------
      // now loop over each atom and, if it's marked as chiral,
      //  figure out the appropriate CIP label:
      for(ROMol::AtomIterator atIt=mol.beginAtoms();
          atIt!=mol.endAtoms();++atIt){
        Atom *atom=*atIt;
        Atom::ChiralType tag=atom->getChiralTag();

        // only worry about this atom if it has a marked chirality
        // we understand:
        if(tag != Atom::CHI_UNSPECIFIED &&
           tag != Atom::CHI_OTHER){
          if(!cipDone){
            //  if we need to, get the "CIP" ranking of each atom:
            assignAtomCIPRanks(mol,ranks);
            cipDone=true;
          }
          // loop over all neighbors and form a decorated list of their
          // ranks:
          bool legalCenter;
          bool hasDupes=false;
          bool hasTruePrecedingAtom=false;
          Chirality::INT_PAIR_VECT nbrs;
          if(atom->getDegree()+atom->getTotalNumHs()>4){
            // we only know tetrahedral chirality
            legalCenter=false;
          } else {
            boost::dynamic_bitset<> codesSeen(mol.getNumAtoms());
            ROMol::OEDGE_ITER beg,end;
            boost::tie(beg,end) = mol.getAtomBonds(atom);
            ROMol::GRAPH_MOL_BOND_PMAP::type pMap = mol.getBondPMap();
            while(beg!=end){
              unsigned int otherIdx=pMap[*beg]->getOtherAtom(atom)->getIdx();
              CHECK_INVARIANT(ranks[otherIdx]<static_cast<int>(mol.getNumAtoms()),
                              "CIP rank higher than the number of atoms.");
              // watch for neighbors with duplicate ranks, which would mean
              // that we cannot be chiral:
              if(codesSeen[ranks[otherIdx]]){
                // we've already seen this code, it's a dupe
                hasDupes = true;
                if( cleanIt ){
                  break;
                }
              }
              codesSeen[ranks[otherIdx]]=1;
              nbrs.push_back(std::make_pair(ranks[otherIdx],
                                            pMap[*beg]->getIdx()));

              // check to see if the neighbor is a "true preceder"
              // (i.e. it occurs before the atom both in the atom
              // ordering and the bond starts at the neighbor):
              if(otherIdx<atom->getIdx() &&
                 pMap[*beg]->getBeginAtomIdx()==otherIdx){
                hasTruePrecedingAtom=true;
              }
              ++beg;
            }

            // figure out if this is a legal chiral center or not:
            if(hasDupes){
              // if we have dupes, we need to check for special cases:
              legalCenter=Chirality::checkChiralAtomSpecialCases(mol,atom);
            } else if(nbrs.size()<3 ||
                      (nbrs.size()==3 && atom->getTotalNumHs()!=1 )){
              // we only handle 3-coordinate atoms that have an implicit H
              legalCenter=false;
            } else {
              // if we haven't disqualified the center, it must be ok.
              legalCenter=true;
            }
          }        
          if(cleanIt && !legalCenter){
            // we can remove the chiral tag from this atom:
            atom->setChiralTag(Atom::CHI_UNSPECIFIED);
            if(atom->hasProp("_CIPCode"))
              atom->clearProp("_CIPCode");

            // If the atom has an explicit hydrogen and no charge, that H
            // was probably put there solely because of the chirality.
            // So we'll go ahead and remove it.
            // This was Issue 194
            if(atom->getNumExplicitHs()==1 &&
               atom->getFormalCharge()==0 &&
               !atom->getIsAromatic() ){
              atom->setNumExplicitHs(0);
              atom->calcExplicitValence(false);
              atom->calcImplicitValence(false);
            }
          } else if( legalCenter && !hasDupes ) {
            // stereochem is possible and we have no duplicate neighbors, assign
            // a CIP code:

            // sort the list of neighbors by their CIP ranks:
            std::sort(nbrs.begin(),nbrs.end(),Chirality::_pairComp);

            // collect the list of neighbor indices:
            std::list<int> nbrIndices;
            for(Chirality::INT_PAIR_VECT_CI nbrIt=nbrs.begin();
                nbrIt!=nbrs.end(); ++nbrIt){
              nbrIndices.push_back((*nbrIt).second);
            }
            // ask the atom how many swaps we have to make:
            int nSwaps = atom->getPerturbationOrder(nbrIndices);

            // if the atom has 3 neighbors and a hydrogen, add a swap:
            // This is reasonable for: F[C@H](Cl)Br, where the H is "between"
            // the heavy atoms, but it screws up for the same molecule if it's
            // numbered like this: [C@H](Cl)(F)Br, here no swap is required.
            if(nbrIndices.size()==3 && atom->getTotalNumHs()==1){
              // we recognize the second case above ([C@H](Cl)(F)Br) using the
              // hasTruePrecedingAtom flag:
              if(hasTruePrecedingAtom) ++nSwaps;
            }
          
            // if that number is odd, we'll change our chirality:
            if(nSwaps%2){
              if(tag == Atom::CHI_TETRAHEDRAL_CCW) tag=Atom::CHI_TETRAHEDRAL_CW;
              else tag=Atom::CHI_TETRAHEDRAL_CCW;
            }
            // now assign the CIP code:
            std::string cipCode;
            if(tag==Atom::CHI_TETRAHEDRAL_CCW) cipCode="S";
            else cipCode="R";
            atom->setProp("_CIPCode",cipCode,true);
          } else {
            // FIX: this is where we should be handling meso cases
          }
        }
      }
      mol.setProp("_AtomChiralitySet", 1, true);
    }

    // ************************************************************
    //
    //
    // ************************************************************
    void assignBondStereoCodes(ROMol &mol,bool cleanIt,bool force){
      if(!force && mol.hasProp("_BondStereoSet") ) return;
      // FIX: it would be nice to call findPotentialStereoBonds here
      // and loop only over the bonds marked with CIPcodes in that function 
      // but that will be nice to once we take care of cis/trans bond in rings 

      INT_VECT ranks;
      ranks.resize(mol.getNumAtoms());
      bool cipDone=false;
    
      // find the double bonds:
      for(ROMol::BondIterator bondIt=mol.beginBonds();
          bondIt!=mol.endBonds();
          ++bondIt){
        if( (*bondIt)->getBondType()==Bond::DOUBLE ){
          Bond *dblBond=*bondIt;
          if(cleanIt){
            dblBond->setStereo(Bond::STEREONONE);
            dblBond->getStereoAtoms().clear();
          }
          // at the moment we are ignoring stereochem on ring bonds.
          if(!mol.getRingInfo()->numBondRings(dblBond->getIdx())){
            Atom *begAtom=dblBond->getBeginAtom();
            Atom *endAtom=dblBond->getEndAtom();
        
            // we're only going to handle 2 or three coordinate atoms:
            if( (begAtom->getDegree()==2 || begAtom->getDegree()==3) &&
                (endAtom->getDegree()==2 || endAtom->getDegree()==3) ){
              // look around each atom and see if it has at least one bond with
              // direction marked:
          
              // ------------------
              // get the "CIP" ranking of each atom if we haven't done
              // so already:
              if(!cipDone){
                assignAtomCIPRanks(mol,ranks);
                cipDone=true;
              }

              // the pairs here are: atomrank,bonddir
              Chirality::INT_PAIR_VECT begAtomNeighbors,endAtomNeighbors;
              Chirality::findAtomNeighborDirHelper(mol,begAtom,dblBond,
                                                   ranks,begAtomNeighbors);
              Chirality::findAtomNeighborDirHelper(mol,endAtom,dblBond,
                                                   ranks,endAtomNeighbors);

              // FIX: for now if we are in a ring we will clear out hte
              // direction tags on the neighboring
              if(begAtomNeighbors.size() && endAtomNeighbors.size()){
                // Each atom has at least one neighboring bond with marked
                // directionality.  Find the highest-ranked directionality
                // on each side:
                int begDir,endDir, endNbrAid, begNbrAid;
            
                if(begAtomNeighbors.size()==1 ||
                   ranks[begAtomNeighbors[0].first] >
                   ranks[begAtomNeighbors[1].first] ){
                  begDir = begAtomNeighbors[0].second;
                  begNbrAid = begAtomNeighbors[0].first;
                } else {
                  begDir = begAtomNeighbors[1].second;
                  begNbrAid = begAtomNeighbors[1].first;
                }
                if(endAtomNeighbors.size()==1 ||
                   ranks[endAtomNeighbors[0].first] >
                   ranks[endAtomNeighbors[1].first]){
                  endDir = endAtomNeighbors[0].second;
                  endNbrAid = endAtomNeighbors[0].first;
                } else {
                  endDir = endAtomNeighbors[1].second;
                  endNbrAid = endAtomNeighbors[1].first;
                }
                dblBond->getStereoAtoms().push_back(begNbrAid);
                dblBond->getStereoAtoms().push_back(endNbrAid);
                if( begDir == endDir ){
                  // In findAtomNeighborDirHelper, we've set up the
                  // bond directions here so that they correspond to
                  // having both single bonds START at the double bond.
                  // This means that if the single bonds point in the same
                  // direction, the bond is cis, "Z"
                  dblBond->setStereo(Bond::STEREOZ);
                } else {
                  dblBond->setStereo(Bond::STEREOE);
                }
              }
            }
          }
        }
      }
      mol.setProp("_BondStereoSet", 1, true);
    }

   
    // Find bonds than can be cis/trans in a molecule and mark them as "any"
    // - this function finds any double bonds that can potentially be part 
    //   of a cis/trans system. No attempt is made here to mark them cis or trans
    // 
    // This function is useful in two situations
    //  1) when parsing a mol file; for the bonds marked here, coordinate informations 
    //     on the neighbors can be used to indentify cis or trans states
    //  2) when writing a mol file; bonds that can be cis/trans but not marked as either 
    //     need to be specially marked in the mol file
    //
    //  The CIPranks on the neighboring atoms are check in this function. The _CIPCode property
    //  if set to any on the double bond.
    // 
    // ARGUMENTS:
    //   mol - the molecule of interest
    //   cleanIt - if this option is set to true, any previous marking of _CIPCode 
    //               on the bond is cleared - otherwise it is left untouched
    void findPotentialStereoBonds(ROMol &mol,bool cleanIt) {
      // FIX: The earlier thought was to provide an optional argument to ignore or consider
      //  double bonds in a ring. But I am removing this optional argument and ignoring ring bonds 
      //  completely for now. This is because finding a potential stereo bond in a ring involves
      //  more than just checking the CIPranks for the neighbors - SP 05/04/04

      // make this function callable multiple times
      if ((mol.hasProp("_BondsPotentialStereo")) && (!cleanIt)) {
        return;
      } else {
        INT_VECT ranks;
        ranks.resize(mol.getNumAtoms());
        bool cipDone=false;

        ROMol::BondIterator bondIt;
        for(bondIt=mol.beginBonds(); bondIt!=mol.endBonds(); ++bondIt){
          if( (*bondIt)->getBondType()==Bond::DOUBLE  && 
              !(mol.getRingInfo()->numBondRings((*bondIt)->getIdx())) ) {
            // we are ignoring ring bonds here - read the FIX above
            Bond *dblBond=*bondIt;
            // if the bond is flagged as EITHERDOUBLE, we ignore it:
            if(dblBond->getBondDir()==Bond::EITHERDOUBLE){
              break;
            }
            // proceed only if we either want to clean the stereocode on this bond
            // or if none is set on it yet
            if ( cleanIt || dblBond->getStereo()==Bond::STEREONONE ) {
              dblBond->setStereo(Bond::STEREONONE);
              const Atom *begAtom=dblBond->getBeginAtom(),*endAtom=dblBond->getEndAtom();
              // we're only going to handle 2 or three coordinate atoms:
              if( (begAtom->getDegree()==2 || begAtom->getDegree()==3) &&
                  (endAtom->getDegree()==2 || endAtom->getDegree()==3) ){
                // ------------------
                // get the CIP ranking of each atom if we need it:
                if(!cipDone){
                  assignAtomCIPRanks(mol,ranks);
                  cipDone=true;
                }
                // find the neighbors for the begin atom and the endAtom
                INT_VECT begAtomNeighbors,endAtomNeighbors;
                Chirality::findAtomNeighborsHelper(mol,begAtom,dblBond,begAtomNeighbors);
                Chirality::findAtomNeighborsHelper(mol,endAtom,dblBond,endAtomNeighbors);
                if(begAtomNeighbors.size()>0 && endAtomNeighbors.size()>0){
                  if ((begAtomNeighbors.size() == 2) && (endAtomNeighbors.size() == 2) ) {
                    // if both of the atoms have 2 neighbors (other than the one connected
                    // by the double bond) and ....
                    if ( (ranks[begAtomNeighbors[0]] != ranks[begAtomNeighbors[1]]) &&
                         (ranks[endAtomNeighbors[0]] != ranks[endAtomNeighbors[1]]) ) {
                      // the neighbors ranks are different at both the ends,
                      // this bond can be part of a cis/trans system
                      if(ranks[begAtomNeighbors[0]] > ranks[begAtomNeighbors[1]]){
                        dblBond->getStereoAtoms().push_back(begAtomNeighbors[0]);
                      } else {
                        dblBond->getStereoAtoms().push_back(begAtomNeighbors[1]);
                      }
                      if(ranks[endAtomNeighbors[0]] > ranks[endAtomNeighbors[1]]){
                        dblBond->getStereoAtoms().push_back(endAtomNeighbors[0]);
                      } else {
                        dblBond->getStereoAtoms().push_back(endAtomNeighbors[1]);
                      }
                    }
                  } else if (begAtomNeighbors.size() == 2) {
                    // if the begAtom has two neighbors and ....
                    if (ranks[begAtomNeighbors[0]] != ranks[begAtomNeighbors[1]]) {
                      // their ranks are different
                      if(ranks[begAtomNeighbors[0]] > ranks[begAtomNeighbors[1]]){
                        dblBond->getStereoAtoms().push_back(begAtomNeighbors[0]);
                      } else {
                        dblBond->getStereoAtoms().push_back(begAtomNeighbors[1]);
                      }
                      dblBond->getStereoAtoms().push_back(endAtomNeighbors[0]);
                    }
                  } else if (endAtomNeighbors.size() == 2) {
                    // if the endAtom has two neighbors and ...
                    if (ranks[endAtomNeighbors[0]] != ranks[endAtomNeighbors[1]]) {
                      // their ranks are different
                      dblBond->getStereoAtoms().push_back(begAtomNeighbors[0]);
                      if(ranks[endAtomNeighbors[0]] > ranks[endAtomNeighbors[1]]){
                        dblBond->getStereoAtoms().push_back(endAtomNeighbors[0]);
                      } else {
                        dblBond->getStereoAtoms().push_back(endAtomNeighbors[1]);
                      }
                    }
                  } else {
                    // end and beg atoms has only one neighbor each, it doesn't matter what the ranks are:
                    dblBond->getStereoAtoms().push_back(begAtomNeighbors[0]);
                    dblBond->getStereoAtoms().push_back(endAtomNeighbors[0]);
                  } // end of different number of neighbors on beg and end atoms
                } // end of check that beg and end atoms have at least 1 neighbor:
              }// end of 2 and 3 coordinated atoms only
            } // end of we want it or CIP code is not set
          } // end of double bond
        } // end of for loop over all bonds
        mol.setProp("_BondsPotentialStereo", 1, true);
      }
    }

    // removes chirality markers from sp and sp2 hybridized centers:
    void cleanupChirality(RWMol &mol){
      for(ROMol::AtomIterator atomIt=mol.beginAtoms();
          atomIt!=mol.endAtoms();
          ++atomIt){
        if( (*atomIt)->getChiralTag()!=Atom::CHI_UNSPECIFIED &&
            (*atomIt)->getHybridization() < Atom::SP3 ){
          (*atomIt)->setChiralTag(Atom::CHI_UNSPECIFIED);
        }
      }
    }

    void assignChiralTypesFrom3D(ROMol &mol,int confId,bool replaceExistingTags){
      const double ZERO_VOLUME_TOL=0.1;
      if(!mol.getNumConformers()) return;
      const Conformer &conf=mol.getConformer(confId);
      if(!conf.is3D()) return;

      for(ROMol::AtomIterator atomIt=mol.beginAtoms();atomIt!=mol.endAtoms();++atomIt){
        Atom *atom=*atomIt;
        // if we aren't replacing existing tags and the atom is already tagged, punt:
        if(!replaceExistingTags && atom->getChiralTag()!=Atom::CHI_UNSPECIFIED){
          continue;
        }
        atom->setChiralTag(Atom::CHI_UNSPECIFIED);
        // additional rasons to skip the atom:
        if(atom->getDegree()<3 || // not enough explicit neighbors
           atom->getTotalDegree()!=4 ||  // not enough total neighbors
           atom->getTotalNumHs(true)>1 // more than two Hs
           ){
          continue;
        }
        bool hasPreceder=false;
        const RDGeom::Point3D &p0=conf.getAtomPos(atom->getIdx());
        ROMol::ADJ_ITER nbrIdx,endNbrs;
        boost::tie(nbrIdx,endNbrs) = mol.getAtomNeighbors(atom);
        const RDGeom::Point3D &p1=conf.getAtomPos(*nbrIdx);
        if(*nbrIdx<atom->getIdx()) hasPreceder=true;
        ++nbrIdx;
        const RDGeom::Point3D &p2=conf.getAtomPos(*nbrIdx);
        ++nbrIdx;
        const RDGeom::Point3D &p3=conf.getAtomPos(*nbrIdx);

        RDGeom::Point3D v1=p1-p0;
        RDGeom::Point3D v2=p2-p0;
        RDGeom::Point3D v3=p3-p0;
        
        double chiralVol= v1.dotProduct(v2.crossProduct(v3));
        if(chiralVol<-ZERO_VOLUME_TOL){
          atom->setChiralTag(Atom::CHI_TETRAHEDRAL_CW);
        } else if (chiralVol>ZERO_VOLUME_TOL){
          atom->setChiralTag(Atom::CHI_TETRAHEDRAL_CCW);
        } else {
          atom->setChiralTag(Atom::CHI_UNSPECIFIED);
        }
      
        //BOOST_LOG(rdErrorLog)<<"   Atom: "<<atom->getIdx()<<" "<<chiralVol<<std::endl;

        if(atom->getDegree()==3 && !hasPreceder){
          // usual story: we'll need to flip the chiral tag
          // if there's no preceding atom and we only have three
          // neighbors:
          atom->invertChirality();
        }
      }
        
    }

  }  // end of namespace MolOps
}  // end of namespace RDKit
