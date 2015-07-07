# Overview #

# Creating a database #
```
~/RDKit/Data/QSAR/Sullivan/3dqsar_sd >python $RDBASE/Projects/DbCLI/CreateDb.py --dbDir=dhfr --molFormat=sdf dhfr.sdf
[06:23:32] INFO: Reading molecules and constructing molecular database.
[06:23:32] INFO: Generating molecular database in file dhfr\Compounds.sqlt
[06:23:32] INFO:   Processing 397 molecules
[06:23:33] INFO:   done 100
[06:23:34] INFO:   done 200
[06:23:35] INFO:   done 300
[06:23:37] INFO: Generating fingerprints and descriptors:
```

For larger databases (more than ~10K compounds), it is recommended to use the `--maxRowsCached` argument in order to reduce memory consumption:
```
~/RDKit/Data/maybridge >python $RDBASE/Projects/DbCLI/CreateDb.py --dbDir=maybridge --maxRowsCached=1000 --molFormat=sdf maybridge.zinc.sdf
[07:49:03] INFO: Reading molecules and constructing molecular database.
[07:49:25] INFO: Generating molecular database in file maybridge\Compounds.sqlt
[07:49:25] INFO:   Processing 72764 molecules
```

# Searching the database #
> ## Search using substructure ##
```
~/RDKit/Data/QSAR/Sullivan/3dqsar_sd >python $RDBASE/Projects/DbCLI/SearchDb.py --dbDir=dhfr --outF=tmp.txt --smarts="c1ncnc1"
[06:30:05] INFO: Doing substructure query
[06:30:06] INFO: Found 18 molecules matching the query
[06:30:06] INFO: Creating output
[06:30:06] INFO: Done!
```
The output file (`tmp.txt` here) contains the ids of the matching molecules from the database. An SD file with matching molecules can also be created:
```
~/RDKit/Data/QSAR/Sullivan/3dqsar_sd >python $RDBASE/Projects/DbCLI/SearchDb.py --dbDir=dhfr --outF=tmp.txt --smarts="c1ncnc1" --sdfOut=out.sdf
```

> ## Search using properties from SD file ##
```
~/RDKit/Data/QSAR/Sullivan/3dqsar_sd >python $RDBASE/Projects/DbCLI/SearchDb.py --dbDir=dhfr --outF=tmp.txt -q "activity>6.0"
[06:35:24] INFO: Doing property query
[06:35:24] INFO: Found 203 molecules matching the query
[06:35:24] INFO: Creating output
[06:35:24] INFO: Done!
```

> ## Match properties and substructure ##
```
~/RDKit/Data/QSAR/Sullivan/3dqsar_sd >python $RDBASE/Projects/DbCLI/SearchDb.py --dbDir=dhfr --outF=tmp.txt -q "activity>6.0" --smarts='c1ncnc1'
[07:03:45] INFO: Doing substructure query
[07:03:45] INFO: Found 1 molecules matching the query
[07:03:45] INFO: Creating output
[07:03:45] INFO: Done!
```


> ## Similarity search ##
Find 5 nearest neighbors to each molecule in an SD file:
```
~/RDKit/Data/QSAR/Sullivan/3dqsar_sd >python $RDBASE/Projects/DbCLI/SearchDb.py --dbDir=dhfr --topN=5  queries.sdf
[07:16:50] INFO: Reading query molecules and generating fingerprints
[07:16:50] INFO: Finding Neighbors
[07:16:50] INFO: The search took 0.3 seconds
[07:16:50] INFO: Creating output
6603170,7-4,0.568,7-6,0.567,7-5,0.564,7-2,0.563,7-3,0.561
6602966,7-2,0.607,7-3,0.604,30-4,0.603,40-3,0.601,16-8a,0.600
6602949,1-144698,0.456,1-152737,0.454,33-2,0.453,1-107146,0.452,1-184692,0.450
6602842,20-5i,0.343,20-5c,0.342,20-5g,0.341,20-5f,0.341,20-5b,0.340
6602774,41-4i,0.739,7-2,0.732,7-4,0.732,7-3,0.731,7-6,0.731
[07:16:50] INFO: Done!
```

Same search, but use atom pairs instead of topological fingerprints:
```
~/RDKit/Data/QSAR/Sullivan/3dqsar_sd >python $RDBASE/Projects/DbCLI/SearchDb.py --dbDir=dhfr --topN=5 --similarityType=AtomPairs queries.sdf
[07:18:31] INFO: Reading query molecules and generating fingerprints
[07:18:31] INFO: Finding Neighbors
[07:18:31] INFO: The search took 0.4 seconds
[07:18:31] INFO: Creating output
6603170,15-14,0.310,15-11,0.310,41-5a,0.305,10-4a,0.303,15-13,0.302
6602966,41-5b,0.349,9-13t,0.332,19-8,0.328,20-5f,0.324,20-5e,0.324
6602949,1-184692,0.330,24-2e,0.313,34-5,0.301,19-7,0.299,33-2,0.297
6602842,9-13e,0.469,9-13d,0.452,9-13b,0.451,9-12e,0.445,9-12d,0.434
6602774,19-8,0.333,24-3e,0.332,30-4,0.327,30-3,0.323,19-6,0.316
[07:18:31] INFO: Done!
```

You can, of course, combine the property or substructure queries with the similarity search. For example, to find only active neighbors:
```
~/RDKit/Data/QSAR/Sutherland/3dqsar_sd > python $RDBASE/Projects/DbCLI/SearchDb.py --dbDir=dhfr --topN=5 --similarityType=AtomPairs -q "activity>6.0" queries.sdf
```