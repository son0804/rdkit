// $Id$
//
//  Copyright (c) 2007, Novartis Institutes for BioMedical Research Inc.
//  All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met: 
//
//     * Redistributions of source code must retain the above copyright 
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following 
//       disclaimer in the documentation and/or other materials provided 
//       with the distribution.
//     * Neither the name of Novartis Institutues for BioMedical Research Inc. 
//       nor the names of its contributors may be used to endorse or promote 
//       products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <GraphMol/ChemReactions/Reaction.h>
#include <GraphMol/Substruct/SubstructMatch.h>
#include <boost/dynamic_bitset.hpp>
#include <map>
#include <algorithm>

namespace RDKit {
  typedef std::vector<MatchVectType> VectMatchVectType;
  typedef std::vector< VectMatchVectType > VectVectMatchVectType;
    
  namespace ReactionUtils {
    //! returns whether or not all reactants matched
    bool getReactantMatches(const MOL_SPTR_VECT &reactants,
                            const MOL_SPTR_VECT &reactantTemplates,
                            VectVectMatchVectType &matchesByReactant){
      PRECONDITION(reactants.size()==reactantTemplates.size(),"reactant size mismatch");
      
      matchesByReactant.clear();
      matchesByReactant.resize(reactants.size());

      bool res=true;
      for(unsigned int i=0;i<reactants.size();++i){
        std::vector< MatchVectType > matchesHere;
        // NOTE that we are *not* uniquifying the results.
        //   This is because we need multiple matches in reactions. For example, 
        //   The ring-closure coded as:
        //     [C:1]=[C:2] + [C:3]=[C:4][C:5]=[C:6] -> [C:1]1[C:2][C:3][C:4]=[C:5][C:6]1
        //   should give 4 products here:
        //     [Cl]C=C + [Br]C=CC=C ->
        //       [Cl]C1C([Br])C=CCC1
        //       [Cl]C1CC(Br)C=CC1
        //       C1C([Br])C=CCC1[Cl]
        //       C1CC([Br])C=CC1[Cl]
        //   Yes, in this case there are only 2 unique products, but that's
        //   a factor of the reactants' symmetry.
        //   
        //   There's no particularly straightforward way of solving this problem of recognizing cases
        //   where we should give all matches and cases where we shouldn't; it's safer to just
        //   produce everything and let the client deal with uniquifying their results.
        int matchCount=SubstructMatch(*(reactants[i]),*(reactantTemplates[i]),matchesHere,false,true,true);
        if(!matchCount){
          // no point continuing if we don't match one of the reactants:
          res=false;
          break;
        } else {
          matchesByReactant[i]=matchesHere;  
        }
      }
      return res;  
    } // end of getReactantMatches()
    
    void recurseOverReactantCombinations(const VectVectMatchVectType &matchesByReactant,
                                         VectVectMatchVectType &matchesPerProduct,
                                         unsigned int level,VectMatchVectType combination){
      unsigned int nReactants=matchesByReactant.size();
      RANGE_CHECK(0,level,nReactants-1);
      PRECONDITION(combination.size()==nReactants,"bad combination size");
      for(VectMatchVectType::const_iterator reactIt=matchesByReactant[level].begin();
          reactIt != matchesByReactant[level].end(); ++reactIt){
        VectMatchVectType prod=combination;
        prod[level] = *reactIt;
        if(level==nReactants-1){
          // this is the bottom of the recursion:
          matchesPerProduct.push_back(prod);
        } else {
          recurseOverReactantCombinations(matchesByReactant,matchesPerProduct,level+1,prod);  
        }
      }
    } //end of recurseOverReactantCombinations
    
    void generateReactantCombinations(const VectVectMatchVectType &matchesByReactant,
                                      VectVectMatchVectType &matchesPerProduct){
      matchesPerProduct.clear();
      VectMatchVectType tmp;
      tmp.clear();
      tmp.resize(matchesByReactant.size());
      recurseOverReactantCombinations(matchesByReactant,matchesPerProduct,0,tmp);
    } // end of generateReactantCombinations()

    RWMOL_SPTR initProduct(const ROMOL_SPTR prodTemplateSptr){
      const ROMol *prodTemplate=prodTemplateSptr.get();
      RWMol *res=new RWMol();

      // --------- --------- --------- --------- --------- --------- 
      // Initialize by making a copy of the product template as a normal molecule.
      // NOTE that we can't just use a normal copy because we do not want to end up
      // with query atoms or bonds in the product.
      
      // copy in the atoms:
      ROMol::ATOM_ITER_PAIR atItP = prodTemplate->getVertices();
      ROMol::GRAPH_MOL_ATOM_PMAP::const_type atMap = prodTemplate->getAtomPMap();
      while(atItP.first != atItP.second ){
        Atom *newAtom=new Atom(*(atMap[*(atItP.first++)]));
        int idx=res->addAtom(newAtom,false,true);
        if(newAtom->hasProp("molAtomMapNumber")){
          // set bookmarks for the mapped atoms:
          int mapNum;
          newAtom->getProp("molAtomMapNumber",mapNum);
          res->setAtomBookmark(newAtom,mapNum);
        }
        
      }
      // and the bonds:
      ROMol::BOND_ITER_PAIR bondItP = prodTemplate->getEdges();
      ROMol::GRAPH_MOL_BOND_PMAP::const_type bondMap = prodTemplate->getBondPMap();
      while(bondItP.first != bondItP.second ){
        const Bond *oldB=bondMap[*(bondItP.first++)];
        unsigned int bondIdx;
        bondIdx=res->addBond(oldB->getBeginAtomIdx(),oldB->getEndAtomIdx(),oldB->getBondType())-1;
        // make sure we don't lose the bond dir information:
        Bond *newB=res->getBondWithIdx(bondIdx);
        newB->setBondDir(oldB->getBondDir());
        // Special case/hack:
        //  The product has been processed by the SMARTS parser.
        //  The SMARTS parser tags unspecified bonds as single, but then adds
        //  a query so that they match single or double
        //  This caused Issue 1748846
        //   http://sourceforge.net/tracker/index.php?func=detail&aid=1748846&group_id=160139&atid=814650
        //  We need to fix that little problem now:
        if( oldB->hasQuery() &&
            oldB->getQuery()->getDescription()=="BondOr" &&
            oldB->getBondType()==Bond::SINGLE){
          if(newB->getBeginAtom()->getIsAromatic() && newB->getEndAtom()->getIsAromatic()){
            newB->setBondType(Bond::AROMATIC);
          } else {
            newB->setBondType(Bond::SINGLE);
          }
        }
      }
      
      return RWMOL_SPTR(res);  
    } // end of initProduct()
    
    void addReactantAtomsAndBonds(RWMOL_SPTR product,const ROMOL_SPTR reactantSptr,
                                  const MatchVectType &match,
                                  const ROMOL_SPTR reactantTemplate){

      // start by looping over all matches and marking the reactant atoms that
      // have already been "added" by virtue of being in the product. We'll also
      // mark "skipped" atoms: those that are in the match, but not in this
      // particular product (or, perhaps, not in any product)
      // At the same time we'll set up a map between the indices of those
      // atoms and their index in the product.
      boost::dynamic_bitset<> mappedAtoms(reactantSptr->getNumAtoms());
      boost::dynamic_bitset<> skippedAtoms(reactantSptr->getNumAtoms());
      std::map<unsigned int,unsigned int> reactProdAtomMap; // this maps atom indices from reactant->product
      for(unsigned int i=0;i<match.size();i++){
        const Atom *templateAtom=reactantTemplate->getAtomWithIdx(match[i].first);
        if(templateAtom->hasProp("molAtomMapNumber")){
          int molAtomMapNumber;
          templateAtom->getProp("molAtomMapNumber",molAtomMapNumber);
          
          if(product->hasAtomBookmark(molAtomMapNumber)){
            reactProdAtomMap[match[i].second] = product->getAtomWithBookmark(molAtomMapNumber)->getIdx();
            mappedAtoms[match[i].second]=1;
            //BOOST_LOG(rdInfoLog) << "  map: " << match[i].second << " " << reactProdAtomMap[match[i].second] << std::endl;
          } else {
            // this skippedAtom has an atomMapNumber, but it's not in this product 
            // (it's either in another product or it's not mapped at all).
            skippedAtoms[match[i].second]=1;  
          }
        } else {
          // This skippedAtom appears in the match, but not in a product:
          skippedAtoms[match[i].second]=1;  
        }
      }

      boost::dynamic_bitset<> visitedAtoms(reactantSptr->getNumAtoms());

      const ROMol *reactant=reactantSptr.get();

      // ---------- ---------- ---------- ---------- ---------- ---------- 
      // Now loop over the atoms in the match that were added to the product
      // From the corresponding atom in the reactant, do a graph traversal
      //  to find other connected atoms that should be added: 
      for(unsigned int matchIdx=0;matchIdx<match.size();matchIdx++){
        int reactantAtomIdx=match[matchIdx].second;
        if(mappedAtoms[reactantAtomIdx]){
          CHECK_INVARIANT(reactProdAtomMap.find(reactantAtomIdx)!=reactProdAtomMap.end(),
                          "mapped reactant atom not present in product.");
          
          // here's a pointer to the atom in the product:
          Atom *productAtom = product->getAtomWithIdx(reactProdAtomMap[reactantAtomIdx]);
          // and this is the corresponding atom in the reactant.
          const Atom *reactantAtom=reactant->getAtomWithIdx(reactantAtomIdx);

          // --------- --------- --------- --------- --------- --------- 
          // If the product atom has no atomic number, set it from the reactant:
          if(productAtom->getAtomicNum()<=0){
             productAtom->setAtomicNum(reactantAtom->getAtomicNum());
             productAtom->setFormalCharge(reactantAtom->getFormalCharge());
             productAtom->setNoImplicit(reactantAtom->getNoImplicit());
             productAtom->setNumExplicitHs(reactantAtom->getNumExplicitHs());
             productAtom->setMass(reactantAtom->getMass());
          }
          // One might be tempted to copy over the reactant atom's chirality into the
          // product atom if chirality is not specified on the product. This would be a
          // very bad idea because the bond order will almost certainly change on the
          // atom and the chirality is referenced to bond order.


          // --------- --------- --------- --------- --------- --------- 
          // While we're here, set the stereochemistry 
          // FIX: this should be free-standing, not in this function.
          if(reactantAtom->getChiralTag()!=Atom::CHI_UNSPECIFIED &&
             reactantAtom->getChiralTag()!=Atom::CHI_OTHER &&
             productAtom->hasProp("molInversionFlag")){
            int flagVal;
            productAtom->getProp("molInversionFlag",flagVal);
            switch(flagVal){
            case 0:
              // FIX: should we clear the chirality or leave it alone? for now we leave it alone 
              //productAtom->setChiralTag(Atom::ChiralType::CHI_UNSPECIFIED);
              break;
            case 1:
              // inversion
              switch(reactantAtom->getChiralTag()){
              case Atom::CHI_TETRAHEDRAL_CCW:
                productAtom->setChiralTag(Atom::CHI_TETRAHEDRAL_CW);
                break;
              case Atom::CHI_TETRAHEDRAL_CW:
                productAtom->setChiralTag(Atom::CHI_TETRAHEDRAL_CCW);
                break;
              default:
                BOOST_LOG(rdWarningLog) << "unsupported chiral type on reactant atom ignored\n";
              }
              break;
            case 2:
              // retention:
              productAtom->setChiralTag(reactantAtom->getChiralTag());
              break;
            default:
              BOOST_LOG(rdWarningLog) << "unrecognized chiral inversion/retention flag on product atom ignored\n";
            } 
          }
          
          
          // now traverse:
          std::list< const Atom * > atomStack;
          atomStack.push_back(reactantAtom);

          while(!atomStack.empty()){
            reactantAtom = atomStack.front();
            atomStack.pop_front();

            // each atom in the stack is guaranteed to already be in the product:
            CHECK_INVARIANT(reactProdAtomMap.find(reactantAtom->getIdx())!=reactProdAtomMap.end(),
                            "reactant atom on traversal stack not present in product.");
            int reactantAtomProductIndex=reactProdAtomMap[reactantAtom->getIdx()];
            productAtom = product->getAtomWithIdx(reactantAtomProductIndex); 
            //BOOST_LOG(rdInfoLog) << "stack traverse: " << reactantAtom->getIdx() << " (prod: " << reactantAtomProductIndex <<")" << std::endl;
            visitedAtoms[reactantAtom->getIdx()]=1;

            // Check our neighbors:
            ROMol::ADJ_ITER nbrIdx,endNbrs;
            boost::tie(nbrIdx,endNbrs) = reactant->getAtomNeighbors(reactantAtom);
            while(nbrIdx!=endNbrs){
              // Four possibilities here:
              //  0) the neighbor has been visited already: do nothing
              //  1) the neighbor is part of the match: set a bond to it (it's in the product already) 
              //  2) the neighbor has been added: set a bond to it
              //  3) the neighbor has not yet been added: add it, set a bond to it, and push it
              //     onto the stack
              //BOOST_LOG(rdInfoLog) << " nbr: " << *nbrIdx << " " << visitedAtoms[*nbrIdx] << std::endl;
              if(!visitedAtoms[*nbrIdx] && !skippedAtoms[*nbrIdx]){
                unsigned int productIdx;
                bool addBond=false;
                if(mappedAtoms[*nbrIdx]){
                  // this is case 1 (neighbor in match); set a bond to the neighbor if this atom
                  // is not also in the match (match-match bonds were set when the product template was
                  // copied in to start things off).;
                  if(!mappedAtoms[reactantAtom->getIdx()]){
                    CHECK_INVARIANT(reactProdAtomMap.find(*nbrIdx)!=reactProdAtomMap.end(),
                                  "reactant atom not present in product.");
                    addBond=true;
                    //BOOST_LOG(rdInfoLog) << "  bond1: " << reactantAtomProductIndex << " " << productAtom->getIdx() << std::endl;
                  }                
                } else if(reactProdAtomMap.find(*nbrIdx)!=reactProdAtomMap.end()){
                  // case 2, the neighbor has been added and we just need to set a bond to it:
                  addBond=true;
                  //BOOST_LOG(rdInfoLog) << "  bond2: " << reactantAtomProductIndex << " " << reactProdAtomMap[*nbrIdx] << std::endl;                
                } else {
                  // case 3, add the atom, a bond to it, and push the atom onto the stack
                  Atom *newAtom = new Atom(*reactant->getAtomWithIdx(*nbrIdx));
                  productIdx=product->addAtom(newAtom,false,true);
                  reactProdAtomMap[*nbrIdx]=productIdx;
                  //BOOST_LOG(rdInfoLog) << "  atom: " << *nbrIdx << " " << productIdx << std::endl;
                  addBond=true;
                  //BOOST_LOG(rdInfoLog) << "  bond3: " << reactantAtomProductIndex << " " << reactProdAtomMap[*nbrIdx] << std::endl;                
                  // update the stack:
                  atomStack.push_back(reactant->getAtomWithIdx(*nbrIdx));
                }
                if(addBond){
                  const Bond *origB=reactant->getBondBetweenAtoms(reactantAtom->getIdx(),*nbrIdx);
                  unsigned int bondIdx;
                  bondIdx=product->addBond(reactProdAtomMap[*nbrIdx],reactantAtomProductIndex,
                                           origB->getBondType())-1;
                  Bond *newB=product->getBondWithIdx(bondIdx);
                  newB->setBondDir(origB->getBondDir());
                }
              }
              nbrIdx++;
            }
          } // end of atomStack traversal        
        }
      } // end of loop over matched atoms
    } // end of addReactantAtomsAndBonds()
    
    
    
  } // End of namespace ReactionUtils
  

  MOL_SPTR_VECT ChemicalReaction::generateOneProductSet(const MOL_SPTR_VECT &reactants,
                                                        const std::vector<MatchVectType> &reactantsMatch) const {
    PRECONDITION(reactants.size()==reactantsMatch.size(),"vector size mismatch");                                                          
    MOL_SPTR_VECT res;
    res.resize(this->getNumProductTemplates());
    
    unsigned int prodId=0;
    for(MOL_SPTR_VECT::const_iterator pTemplIt=this->beginProductTemplates();
        pTemplIt!=this->endProductTemplates();++pTemplIt){
      //BOOST_LOG(rdInfoLog)  << "\n\n**********\ninitproduct: " << prodId << std::endl;
      RWMOL_SPTR product=ReactionUtils::initProduct(*pTemplIt);

      for(unsigned int reactantId=0;reactantId<reactants.size();++reactantId){
        ReactionUtils::addReactantAtomsAndBonds(product,reactants[reactantId],reactantsMatch[reactantId],
                                                this->m_reactantTemplates[reactantId]);  
      }                    
      
      product->clearAllAtomBookmarks();      
      res[prodId] = product;
      ++prodId;
    }
    
    return res;
  }
  
  std::vector<MOL_SPTR_VECT> ChemicalReaction::runReactants(const MOL_SPTR_VECT reactants) const {
    if(!this->df_needsInit) {
     throw ChemicalReactionException("initMatchers() must be called before runReactants()");
    }
    
    if(reactants.size() != this->getNumReactantTemplates()){
      throw ChemicalReactionException("Number of reactants provided does not match number of reactant templates.");    
    }
    std::vector<MOL_SPTR_VECT> productMols;
    productMols.clear();
    
    // if we have no products, return now:
    if(!this->getNumProductTemplates()){ 
      return productMols;
    }
    
    // find the matches for each reactant:
    VectVectMatchVectType matchesByReactant;
    if(!ReactionUtils::getReactantMatches(reactants,this->m_reactantTemplates,matchesByReactant)){
      // some reactants didn't find a match, return an empty product list:
      return productMols;
    }
    
    // -------------------------------------------------------
    // we now have matches for each reactant, so we can start creating products:
    
    // start by doing the combinatorics on the matches:
    VectVectMatchVectType reactantMatchesPerProduct;
    ReactionUtils::generateReactantCombinations(matchesByReactant,reactantMatchesPerProduct);
    productMols.resize(reactantMatchesPerProduct.size());                                
    
    for(unsigned int productId=0;productId!=productMols.size();++productId){
      MOL_SPTR_VECT lProds=this->generateOneProductSet(reactants,reactantMatchesPerProduct[productId]);
      productMols[productId]=lProds;
    }
    
    return productMols;
  } // end of ChemicalReaction::runReactants()
  
  void ChemicalReaction::initReactantMatchers() {
    df_needsInit=false;  
  }

  bool ChemicalReaction::validate(int &numWarnings,int &numErrors,bool silent) const {
    bool res=true;
    numWarnings=0;
    numErrors=0;
    
    if(!this->getNumReactantTemplates()){
      if(!silent) BOOST_LOG(rdErrorLog)<<"reaction has no reactants\n";
      numErrors++;
      res=false;
    }
    
    if(!this->getNumProductTemplates()){
      if(!silent) BOOST_LOG(rdErrorLog)<<"reaction has no products\n";
      numErrors++;
      res=false;
    }
    
    std::vector<int> mapNumbersSeen;
    unsigned int molIdx=0;
    for(MOL_SPTR_VECT::const_iterator molIter=this->beginReactantTemplates();
        molIter!=this->endReactantTemplates();++molIter){
      bool thisMolMapped=false;
      for(ROMol::AtomIterator atomIt=(*molIter)->beginAtoms();
          atomIt!=(*molIter)->endAtoms();++atomIt){
        if((*atomIt)->hasProp("molAtomMapNumber")){
          thisMolMapped=true;
          int mapNum;
          (*atomIt)->getProp("molAtomMapNumber",mapNum);
          if(std::find(mapNumbersSeen.begin(),mapNumbersSeen.end(),mapNum)!=mapNumbersSeen.end()){
            if(!silent) BOOST_LOG(rdErrorLog)<<"reactant atom-mapping number "<<mapNum<<" found multiple times.\n";
            numErrors++;
            res=false;
          } else {
            mapNumbersSeen.push_back(mapNum);
          }
        }
      }
      if(!thisMolMapped){
        if(!silent) BOOST_LOG(rdWarningLog)<<"reactant "<<molIdx<<" has no mapped atoms.\n";
        numWarnings++;
      }
      molIdx++;
    }

    std::vector<int> productNumbersSeen;  
    molIdx=0;
    for(MOL_SPTR_VECT::const_iterator molIter=this->beginProductTemplates();
        molIter!=this->endProductTemplates();++molIter){
      bool thisMolMapped=false;
      for(ROMol::AtomIterator atomIt=(*molIter)->beginAtoms();
          atomIt!=(*molIter)->endAtoms();++atomIt){
        if((*atomIt)->hasProp("molAtomMapNumber")){
          thisMolMapped=true;
          int mapNum;
          (*atomIt)->getProp("molAtomMapNumber",mapNum);
          if(std::find(productNumbersSeen.begin(),productNumbersSeen.end(),mapNum)!=productNumbersSeen.end()){
            if(!silent) BOOST_LOG(rdErrorLog)<<"product atom-mapping number "<<mapNum<<" found multiple times.\n";
            numErrors++;
            res=false;
          }
          std::vector<int>::iterator ivIt=std::find(mapNumbersSeen.begin(),mapNumbersSeen.end(),mapNum);
          if(ivIt==mapNumbersSeen.end()){
            if(!silent) BOOST_LOG(rdErrorLog)<<"product atom-mapping number "<<mapNum<<" not found in reactants.\n";
            numErrors++;
            res=false;
          } else {
            mapNumbersSeen.erase(ivIt); 
          }
        }
      }
      if(!thisMolMapped){
        if(!silent) BOOST_LOG(rdWarningLog)<<"product "<<molIdx<<" has no mapped atoms.\n";
        numWarnings++;
      }
    
      molIdx++;
    }
    if(!mapNumbersSeen.empty()){
      if(!silent){
        std::ostringstream ostr;
        ostr<<"mapped atoms in the reactants were not mapped in the products.\n";
        ostr<<"  unmapped numbers are: ";
        for(std::vector<int>::const_iterator ivIt=mapNumbersSeen.begin();
            ivIt!=mapNumbersSeen.end();++ivIt){
          ostr<< *ivIt << " ";
        }
        ostr<< "\n";
        BOOST_LOG(rdWarningLog)<<ostr.str();
      }
      numWarnings++;
    }
    
    return res;
  }

}
