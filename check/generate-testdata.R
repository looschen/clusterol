#!/usr/bin/env Rscript
## generate simple 2d test data

N = 2000
d = 5
range.min = -1e6
range.max = 1e6
## df = data.frame(x=runif(N, range.min, range.max), y=runif(N, range.min, range.max))
df = matrix(runif(N*d, range.min, range.max), nrow=N, ncol=d)

write.table(df, row.names=FALSE, col.names=FALSE)
