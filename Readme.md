# index-me

Inverted file with memory allocation.

## Compilation

```
make
```

## Data flow

### Tokenization/Stemming

Python script that outputs a document per line on which the words will be
separated by spaces.

```
python ./scripts/tokenize.py $CORPUS_LOCATION > tokens.data
```

### Zipf Law verification

Given a list of tokens, outputs data to check the zipf law.

```
./bin/zipf < tokens.data
```

### Intermediary file generator

Constructs a part of the index and serializes it to a set of files.

```
./bin/construct $INTERMEDIARY_LOCATION < tokens.data
```

### Intermediary files merger

Merges the intermediary files into one index

```
./bin/merger $INTERMEDIARY_LOCATION > index.data
```
