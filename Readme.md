# index-me

[![Build Status](https://travis-ci.org/GusMonod/index-me.svg?branch=master)](https://travis-ci.org/GusMonod/index-me)

Inverted file with memory allocation.

## Compilation

```
make
```

## Data flow

### Tokenization/Stemming

Node js script that outputs a document per line on which the words will be
separated by spaces.

```
export CORPUS_PATH=/path/to/corpus
make corpus  # creates the file corpus/corpus.tok, can take a few minutes
```

### Zipf Law verification

Given a list of tokens, outputs data to check the zipf law.

```
./bin/zipf < corpus/corpus.tok
```

### Intermediary file generator

Constructs a part of the index and serializes it to a set of files, whose paths
are written to the standard output.

```
./bin/construct < corpus/corpus.tok > partfile.paths  # can take a few minutes
```

### Intermediary files merger

Merges the intermediary files into a single index.

```
./bin/merger < partfile.paths > index.data
```
