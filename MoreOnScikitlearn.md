# Introduction #

More learners available within scikit-learn will be presented:
  * **SVM**
  * **RandomForest**
  * **NaiveBayes**
You can use the Huuskonen dataset for training: http://rdkit.svn.sourceforge.net/viewvc/rdkit/trunk/Docs/Book/data/solubility.train.sdf?revision=1724 and testing: http://rdkit.svn.sourceforge.net/viewvc/rdkit/trunk/Docs/Book/data/solubility.test.sdf

# The Code #

Loading and preparing the dataset:
```
import sys,cPickle
from rdkit import Chem 
from rdkit.Chem import Descriptors
from rdkit.ML.Descriptors import MoleculeDescriptors
from sklearn.ensemble import RandomForestClassifier
from sklearn import svm
from sklearn.naive_bayes import GaussianNB
from sklearn import cross_validation
from sklearn import metrics

trainset_file = sys.argv[1]
testset_file  = sys.argv[2]
trainset      = [x for x in Chem.SDMolSupplier(trainset_file) if x is not None]
testset       = [x for x in Chem.SDMolSupplier(testset_file) if x is not None]
```

Calculation of the descriptors:
```
nms=[x[0] for x in Descriptors._descList]
nms.remove('MolecularFormula') 
calc = MoleculeDescriptors.MolecularDescriptorCalculator(nms)
calc = MoleculeDescriptors.MolecularDescriptorCalculator(nms)
trainDescrs = [calc.CalcDescriptors(x) for x in trainset]
testDescrs  = [calc.CalcDescriptors(x) for x in testset]
cPickle.dump(trainDescrs,file('trainDescrs.pkl','wb+'))
cPickle.dump(testDescrs,file('testDescrs.pkl','wb+'))
```

Let's do the training:
```
classes={'(A) low':0,'(B) medium':1,'(C) high':2}
trainActs = [classes[x.GetProp('SOL_classification')] for x in trainset]
testActs = [classes[x.GetProp('SOL_classification')] for x in testset]
```

First, start with RandomForest:
```
print "RandomForest"
nclf = RandomForestClassifier(n_estimators=100, max_depth=5,\
                              random_state=0,n_jobs=1)
nclf      = nclf.fit(trainDescrs,trainActs)

preds     = nclf.predict(testDescrs)
print metrics.confusion_matrix(testActs,preds)
accuracy    = nclf.score(testDescrs,testActs)
```
Here comes the SVM implementation:
```
print "SVM"
clf_svm = svm.SVC(gamma=0.001, C=100.)
clf_svm = clf_svm.fit(trainDescrs,trainActs)
preds_SVM = clf_svm.predict(testDescrs)
print metrics.confusion_matrix(testActs,preds_SVM)
accuracy    = clf_svm.score(testDescrs,testActs)
print accuracy
```
Last but not least NaiveBayes
```
print "NB"
gnb    = GaussianNB()
clf_NB = gnb.fit(trainDescrs,trainActs) 
preds_NB  = clf_NB.predict(testDescrs)
print metrics.confusion_matrix(testActs,preds_NB)
accuracy    = clf_NB.score(testDescrs,testActs)
print accuracy
```