#!/usr/bin/env Rscript
## cluster testdata and print hclust$merge

argv = commandArgs(trailingOnly=TRUE)

data = read.table(argv[1])
M = dist(data, method="euclidean")

hc = hclust(M, method=argv[2])

## library(cluster)
## hc = as.hclust(agnes(M, method="single"))

write.table(cbind(hc$merge, hc$height), row.names=FALSE, col.names=FALSE)
