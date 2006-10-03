import RDConfig
import unittest
from ML.InfoTheory import rdInfoTheory, BitClusterer
from ML.Data import DataUtils
import DataStructs
import RDRandom

def getValLTM(i, j, mat):
    if (i > j) :
        id = (i*(i-1)/2) + j
        return mat[id]
    elif (j > i) :
        id = (j*(j-1)/2) + i
        return mat[id]
    else :
        return 0.0
        
class TestCase(unittest.TestCase):
    def setUp(self) :
        # here is what we are going to do to test this out
        # - generate bit vectrs of length nbits
        # - turn on a fraction of the first nbits/2 bits at random
        # - for each bit i turned on in the range (0, nbits/2) turn on the bit
        #   nbits/2 + i
        # - basically the first half of a fingerprint is same as the second half of the
        #   fingerprint
        # - if we repeat this process often enough we whould see strong correlation between
        #   the bits i (i < nbits/2) and (nbits/2 + i)
        DataUtils.InitRandomNumbers((100,23))
        self.nbits = 200
        self.d = 40
        self.nfp = 1000

        self.blist = range(self.nbits)
                
        self.fps = []
        for fi in range(self.nfp) :
            fp = DataStructs.ExplicitBitVect(self.nbits)
            #obits = range(self.nbits/2)
            #random.shuffle(obits)
	    
            obits = range(self.nbits/2)
	    RDRandom.shuffle(obits)
            obits = obits[0:self.d]
            for bit in obits :
                fp.SetBit(bit)
                fp.SetBit(bit + self.nbits/2)
            self.fps.append(fp)

    def test0CorrMat(self) :

        cmg = rdInfoTheory.BitCorrMatGenerator()
        cmg.SetBitList(self.blist)
        for fp in self.fps:
            cmg.CollectVotes(fp)

        corrMat = cmg.GetCorrMatrix()

        avr = 0.0
        navr = 0.0
        for i in range(self.nbits/2) :
            avr += getValLTM(i, i + self.nbits/2, corrMat)
            navr += getValLTM(i,i+1, corrMat)

        self.failUnless(2*avr/self.nbits == 400.0)
        self.failUnless(2*navr/self.nbits == 157.55,2*navr/self.nbits)

    def test1Cluster(self) :
        cmg = rdInfoTheory.BitCorrMatGenerator()
        cmg.SetBitList(self.blist)
        for fp in self.fps:
            cmg.CollectVotes(fp)

        corrMat = cmg.GetCorrMatrix()
        
        bcl = BitClusterer.BitClusterer(self.blist, self.nbits/2)
        bcl.ClusterBits(corrMat)
        cls = bcl.GetClusters()
        for cl in cls :
            self.failUnless(len(cl) == 2)
            self.failUnless((cl[0] + self.nbits/2) == cl[1])

        tfp = DataStructs.ExplicitBitVect(self.nbits)
        obits = range(0,self.nbits/4) + range(self.nbits/2, 3*self.nbits/4)
        tfp.SetBitsFromList(obits)
        rvc = bcl.MapToClusterScores(tfp)
        self.failUnless(len(rvc) == self.nbits/2)
        for i in range(self.nbits/2) :
            if i < self.nbits/4:
                self.failUnless(rvc[i] == 2)
            else :
                self.failUnless(rvc[i] == 0)

        nfp = bcl.MapToClusterFP(tfp)
        self.failUnless(len(nfp) == self.nbits/2)
        for i in range(self.nbits/2) :
           if i < self.nbits/4:
                self.failUnless(nfp[i])
           else :
               self.failUnless(not nfp[i])
               
if __name__ == '__main__':
    unittest.main()
