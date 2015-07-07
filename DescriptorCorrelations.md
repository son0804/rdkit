# Heatmaps #

Click the images to download the raw data.

## ZINC drug-like set ##
[![](http://www.rdkit.org/Images/DrugLikeT80_CorrMat.png)](http://rdkit.svn.sourceforge.net/viewvc/rdkit/trunk/Docs/Analysis/Descriptors/Correlations/DrugLikeT80_Correlation.csv)

## ZINC lead-like set ##
[![](http://www.rdkit.org/Images/LeadLikeT80_CorrMat.png)](http://rdkit.svn.sourceforge.net/viewvc/rdkit/trunk/Docs/Analysis/Descriptors/Correlations/LeadLikeT80_Correlation.csv)

## ZINC fragment-like set ##
[![](http://www.rdkit.org/Images/FragmentLikeT80_CorrMat.png)](http://rdkit.svn.sourceforge.net/viewvc/rdkit/trunk/Docs/Analysis/Descriptors/Correlations/FragmentLikeT80_Correlation.csv)

# Process #

From Kirk DeLisle:

I've compiled my descriptor correlation results and attached them as various files here.  Here is some explanation:

I used 3 datasets from ZINC (http://zinc.docking.org/), sp. The Drug-like dataset (66796 structures), Lead-like (97433 structures), Fragment-like (15937 structures), and an in-house set of compounds (~24,000 structures).  For all of the ZINC sets, I downloaded the T80 versions, which have been somewhat filtered for similar compounds in an attempt to get the pairwise Tanimoto below 0.8.  I generated all descriptors from RDKIT including all VSA descriptors that I had access to.

The procedure I used for filtering descriptors consisted of eliminating those with less than 10% variance followed by a pairwise correlation analysis.  The correlation analysis consists of giving each column a score of the number of other columns with which it is correlated above the set threshold (0.85).  The column with the most correlated columns is kept and the others are deleted.  I did this using a KNIME workflow.

In all 4 datasets (internal, ZINC Fragment-like, Drug-like and Lead-like)
there were 10 VSA descriptors that are eliminated due to <10% variance:
SMR\_VSA8 (all zero), Estate\_VSA11 (only 2 compounds with values),
VSA\_Estate1 through 7 (all zero), and SlogP\_VSA9 (all zero).
In general, the Kappa and Chi descriptors show significant intercorrelation and many are eliminated.  As expected, the VSA descriptors are not significantly correlated and so all those passing the variance filter are kept.  Here are the lists of retained descriptors for each set:

Drug-like:  BalabanJ, BertzC, IPc, HallKierAlpha, Kappa2, Kappa3, Chi2n, Chi0v, Chi2v, MolLogP, NHOHCount, NOCount, NumHAcceptors, NumRotatableBonds.
Lead-like:  BalabanJ, BertzC, IPc, HallKierAlpha, Kappa2, Kappa3, Chi1, Chi2n, Chi2v, MolLogP, NHOHCount, NOCount, NumHAcceptors.
Fragment-like:  BalabanJ, BertzC, IPc, HallKierAlpha, Kappa2, Kappa3, Chi0n, Chi3n, Chi2v, MolLogP, MolWt, NHOHCount, NOCount, NUMHAcceptors, NumRotatableBonds, TPSA.
Internal:  BalabanJ, BertzC, IPc, HallKierAlpha, Kappa2, Chi3n, Chi1v, MolLogP, NHOHCount, NOCount.

In addition, there were some interesting correlations.  Specifically,
SlogP\_VSA6 and SMR\_VSA7 had a correlation of 0.91 across all datasets.
SlogP\_VSA5 and SMR\_VSA5 also had a high correlation of 0.81 - 0.85.  Others
that were not consistently this high but did show up occasionally were
Estate\_VSA10 and Estate\_VSA1, SlogP\_VSA2 and SMR\_VSA6, and SlogP7 and
VSA\_Estate10.