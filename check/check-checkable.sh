#!/bin/sh

# cluster with R and clusterol-tool, compare all comparable methods

testdir=$1
ctool=$2

echo generating test data at $testdir/data
mkdir $testdir
./generate-testdata.R >  $testdir/data

echo "single-link"
$ctool -d $testdir/data -m single-link > $testdir/csingle-link
./cluster-testdata.R $testdir/data single > $testdir/Rsingle-link
./compare-results.R $testdir/{c,R}single-link

echo "================================================================================"

echo "complete-link"
$ctool -d $testdir/data -m complete-link > $testdir/ccomplete-link
./cluster-testdata.R $testdir/data complete > $testdir/Rcomplete-link
./compare-results.R $testdir/{c,R}complete-link

echo "================================================================================"

echo "ward"
$ctool -d $testdir/data -m ward > $testdir/cward
./cluster-testdata.R $testdir/data ward > $testdir/Rward
./compare-results.R $testdir/{c,R}ward

echo "================================================================================"

echo "weighted-group-average"
$ctool -d $testdir/data -m weighted-group-average > $testdir/cweighted-group-average
./cluster-testdata.R $testdir/data mcquitty > $testdir/Rweighted-group-average
./compare-results.R $testdir/{c,R}weighted-group-average

echo "================================================================================"

echo "median"
$ctool -d $testdir/data -m median > $testdir/cmedian
./cluster-testdata.R $testdir/data median > $testdir/Rmedian
./compare-results.R $testdir/{c,R}median

