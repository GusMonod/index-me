Pipelining data in the indexer
==============================

The indexer is a complete solution that connects many different processing operations.

In order to break down our code efficiently and to use a proper level of language (from low-level for memory-related concerns to high-level for I/O-intensive parts) on each steps, we used UNIX pipelines between each steps.

Step 1: Tokenize the corpus
---------------------------

### Goal

Merge the complete corpus inside of a single, easily parsable file.

The corpus is composed of a set of compressed XML documents.

Here's what we do for each document in the specified corpus directory:

1. Launch the `uncompress` command as a subprocess to get a clear file
2. Filter out HTML and unwanted chars with some regexes
3. Take each remaining word and output a line on STDOUT with each word separated by a space

### Tech used

This kind of operation needs to do a lot of I/O (subprocesses, readdir, read file stream, output on the STDOUT, ...) so we chose an high-level scripting for that. Since our python was a little bit rusty and not our javascript, we chose to write the script with Node.js. Since the level of language in standard node distributions distributed via package managers doesn't support the recent JS ES6 level of language, we chose to transpile it using babel and gulp.

### How to use it

First, you need to install node (> 0.10) & npm. Then, you can install the required packages:

```sh
cd tokenizer
npm i 
npm i -g gulp-cli
```

Now, you can run the script against the corpus:

```sh
CORPUS_PATH=<the absolute path to your corpus> gulp run > ../tokens.txt
```

Step 2: Build the index chunks
------------------------------

### Goal

With the tokens, build the index chunks up to the memory limit of the process.

The goal is to output a partial vocabulary with partial posting-lists (only the part that fitted in memory). The chunks are written as files in a dir. The tokens are feeded via the STDIN.

### Tech used

As we need a fine grained control on our memory allocations (to simulate insufficient memory), using a low level language was a requirement. C was a perfect candidate!

### How to use it

Make the C binaries first:

```sh
make
```

Then it's simple, we just feed the tokens to the executable:

```sh
bin/construct < ./tokens.txt
```

### Bonus: Zipf law checking

As the constructor, zipf law can be checked at this point by feeding the tokens:

```sh
bin/zipf < ./tokens.txt
```

