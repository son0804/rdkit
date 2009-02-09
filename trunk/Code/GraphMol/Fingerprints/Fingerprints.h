//
//  Copyright (C) 2003-2008 Greg Landrum and Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//
#ifndef _RD_FINGERPRINTS_H_
#define _RD_FINGERPRINTS_H_

class ExplicitBitVect;
namespace RDKit{
  class ROMol;
  //! \brief Generates a topological (Daylight like) fingerprint for a molecule
  /*!
    \deprecated NOTE: This function is deprecated. Please use RDKFingerprintMol()
                instead.

    \param mol:          the molecule to be fingerprinted
    \param minPath:      the minimum path length (in bonds) to be included
    \param maxPath:      the minimum path length (in bonds) to be included
    \param fpSize:       the size of the fingerprint
    \param nBitsPerHash: the number of bits to be set by each path
    \param useHs:        toggles inclusion of Hs in distinguishing paths from
                         each other.
    \param tgtDensity:   if the generated fingerprint is below this density, it will
                         be folded until the density is reached.
    \param minSize:      the minimum size to which the fingerprint will be
                         folded

    \return the molecular fingerprint, as an ExplicitBitVect

    <b>Notes:</b>
      - the caller is responsible for <tt>delete</tt>ing the result
    
  */
  ExplicitBitVect *DaylightFingerprintMol(const ROMol &mol,
					  unsigned int minPath=1,unsigned int maxPath=7,
					  unsigned int fpSize=2048,unsigned int nBitsPerHash=4,
					  bool useHs=true,
					  double tgtDensity=0.0,unsigned int minSize=128);

  //! \brief Generates a topological (Daylight like) fingerprint for a molecule
  //!        using an alternate (faster) hashing algorithm  
  /*!

    \param mol:          the molecule to be fingerprinted
    \param minPath:      the minimum path length (in bonds) to be included
    \param maxPath:      the minimum path length (in bonds) to be included
    \param fpSize:       the size of the fingerprint
    \param nBitsPerHash: the number of bits to be set by each path
    \param useHs:        toggles inclusion of Hs in distinguishing paths from
                         each other.
    \param tgtDensity:   if the generated fingerprint is below this density, it will
                         be folded until the density is reached.
    \param minSize:      the minimum size to which the fingerprint will be
                         folded

    \return the molecular fingerprint, as an ExplicitBitVect

    <b>Notes:</b>
      - the caller is responsible for <tt>delete</tt>ing the result
    
  */
  ExplicitBitVect *RDKFingerprintMol(const ROMol &mol,
                                    unsigned int minPath=1,unsigned int maxPath=7,
                                    unsigned int fpSize=2048,unsigned int nBitsPerHash=4,
                                    bool useHs=true,
                                     double tgtDensity=0.0,unsigned int minSize=128);

  //! \brief Generates a topological (Daylight like) fingerprint for a molecule
  //!        using an alternate (faster) hashing algorithm  
  /*!

    \param mol:          the molecule to be fingerprinted
    \param layers:       the layers to be included (see below)
    \param minPath:      the minimum path length (in bonds) to be included
    \param maxPath:      the minimum path length (in bonds) to be included
    \param fpSize:       the size of the fingerprint
    \param tgtDensity:   if the generated fingerprint is below this density, it will
                         be folded until the density is reached.
    \param minSize:      the minimum size to which the fingerprint will be
                         folded

    \return the molecular fingerprint, as an ExplicitBitVect

    <b>Notes:</b>
      - the caller is responsible for <tt>delete</tt>ing the result

    <b>Layer definitions:</b>
       - 0x01: pure topology
       - 0x02: bond order
       - 0x04: atom types
       - 0x08: presence of rings
       - 0x10: ring sizes
  */
  ExplicitBitVect *LayeredFingerprintMol(const ROMol &mol,
                                         unsigned int layerFlags=0xFFFFFFFF,
                                         unsigned int minPath=1,unsigned int maxPath=7,
                                         unsigned int fpSize=2048,
                                         double tgtDensity=0.0,unsigned int minSize=128);
}

#endif
