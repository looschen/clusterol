#!/usr/bin/env Rscript
## compare results of clusterol-tool --join-file and cluster-testdata.R
## (or generally of two join-reports)

argv = commandArgs(trailingOnly=TRUE)

data.1 = read.table(argv[1])
data.2 = read.table(argv[2])

if(! identical(dim(data.1), dim(data.2))){
  print("Dimensions of input files are different.")
  exit()
}

N = nrow(data.1)
d = ncol(data.1) - 1

for(i in 1:N){
  eq = setequal(data.1[i, 1:d], data.2[i, 1:d])
  eq[2] = (data.1[i, d+1] == data.2[i, d+1])
  delta = data.1[i, d+1] - data.2[i, d+1]

  if(FALSE %in% eq)
    cat(i, eq, delta, "\n")
}

  

