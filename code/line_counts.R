## total word counts in inst/include dirs for Rcpp, Rcpp11
## we strip out the initial headers

dirs <- c("~/Rcpp", "~/Rcpp11")
names(dirs) <- c("Rcpp", "Rcpp11")

strip_header <- function(txt, comment="//") {

  regex <- paste0("^", comment)

  ## assume the first line must be a header
  ## verify that it's commented
  if (!grepl(regex, txt[1])) {
    return(txt)
  }

  comments <- grep(regex, txt)
  diff <- diff(comments)
  if (all(diff == 1)) { ## all comments are part of the header
    return(txt[ (length(comments)+1):length(txt) ])
  } else {
    header_end <- which( diff != 1 )[1]
    return( txt[ (header_end+1):length(txt) ] )
  }
}

for (i in seq_along(dirs)) {
  dir <- dirs[[i]]
  files <- list.files( file.path(dir, "inst/include"), recursive=TRUE, full.names=TRUE, pattern=".h$")
  sizes <- lapply(files, function(file) {
    txt <- readLines(file)
    txt <- strip_header(txt)
    return (length(txt))
  })
  total <- sum( unlist(sizes) )
  cat("Total number of lines of code in 'inst/include' of ", names(dirs)[i],
    " is (excluding headers) ", total, ".\n", sep="")
}

## line count per feature
features <- c("module", "create", "generated")
feature <- features[[1]]
dir <- dirs[[1]]
output <- lapply(features, function(feature) {
  sapply(dirs, function(dir) {
    files <- list.files( file.path(dir, "inst/include"), recursive=TRUE, full.names=TRUE )
    files <- grep(feature, files, value=TRUE)
    sapply(files, function(file) {
      length( strip_header( readLines(file) ) )
    })
  })
})

names(output) <- features
for (i in seq_along(output)) {
  names( output[[i]] ) <- names(dirs)
}

## total sizes by feature
print(lapply(output, function(x) lapply(x, function(xx) sum( unlist(xx) ))))
