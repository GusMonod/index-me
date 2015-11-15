How do this code works ?
========================

This code is an implementation of a merge-based index construction algorithm. We will explain the role of the different files of this project and the different choices we made.

Memory limitation
-----------------

We use artificial memory limitation to simulate the constraints encountered while dealing with large amounts of data. We chose to do that ny using our own malloc, free and realloc functions, available in pmemory.c and .h in util. We did a basic implementation which counts the memory used each they're called, and do not allocate memory if the user is trying to use more memory than is allowed by the constant kMemorySizeBytes. We chose here to fix this maximum amount of memory at 8 MB, which is little enough for us not to be able to construct our whole index in that memory with the data we use, thus simulating the constraints leading to a merge-based approach.

Data structure
--------------

Our structure is composed of term entries which are stored in a hash table. We chose to use a preexisting implementation for the hash table, which allows us to easily add, delete or retrieve an element by its key, and to browse and sort the table. The complete implementation and documentation of the hash table is available in util/uthash.h.
A term entry (see struct TermEntry in util/index.h) is a structure containing all the information about a term needed for the index. It is composed of a the token to which the information refers, a posting list and some metadata about the posting list. A posting list is a contiguous list of pieces of information about the documents in which the term appears. We chose to store in each posting list entry (see struct PostingListEntry in util/index.h) the id of the document and the number of occurences of the term in the document (the Term Frequency). Since all posting list entries must be contiguous, our implementation of the posting list is a dynamic array of posting list entries. Each time an item is added to the array, if the array is full, we multiply the space it can use by a fixed factor. Here, we chose 10 for the initial number of items a posting list can contain (see kInitialPostingListSize in util/index.c) and 2 as the factor we use to multiply it everytime it is full (see kSizeAugmentFactor in util/index.c). For each term, we then store the actual number of items stored in its posting list, and the current number of terms allocated.

Constructing the index parts
----------------------------

The construction of the index parts is based on two operations : trying to add a token in the index in memory, and purging the index when there is no more memory available. Additionally, the current index in memory can be outputed in various forms for test purposes. (see construct/main.c)
For each occurence of a document, we try to add it to the index in memory (see function tryToAddToken in util/index.*). Two cases may happen : if the token is not yet present in the index, we create a new term entry for the token and a posting list with one occurence in the document it appears (see function addNewTermEntry in util/index.c) . If, however, the token has a term entry in the index, then we add one occurence of term in the desired document (see addToTermEntry in util/index.*). This, in turn, can have two possible outcomes : either the term entry already contains a posting list entry for the desired document, and in that case we simply update this posting list entry, or the term entry doesn't have a posting list entry (i.e. the token has already appeared in the corpus but not in the current document), then we add a new item to the posting list.
At some points in the process, the program may run out of memory to store the index. In that case, we sort the index we have by its tokens and serialize it by printing it into a text file (see function fprintTerm in util/index.*) and we clear the memory. We then try again to add the token, which will be the first in a new index part. (see function fpurgeIndex in util/index.*).
At the end of the execution of construct, we have serialized all index parts in separate files, and we output the path to this files to allow merge to find them.

Merging the index parts
-----------------------

To merge the different index parts into one index, we need to repeatedly read the first entry in each index part, select the ones with the token that appears first in alphabetical order, merge them into one entry, and serialize that entry in the big index. It is essential that the index parts are sorted.
The merge program works by continuously merging and printing one term entry, each time containing information about the first remaining token in the corpus, in alphabetical order. (see function mergeFirst). At each step, we start by electing the next entries to be merged (for example, all the entries in the different index parts about the word "a") (see function electedEntries in merger/main.c), then we merge them into one and replace them in the eligible entries by the next entry in the corresponding index part. To merge several entries into one, we recursively merge two entries until the last entry is merge with the fusion of all other entries. To merge two entries, we use the same function that we used in construct to add one occurence of a term in a document, addToTermEntry, so we add each number of occurences of the term in each document of the second entry, to the number of occurences of the term in the corresponding document in the first entry, potentially creating a posting list entry in the first term entry.
We iterate through the process until every entry of every index part has been merged and serialized.
Add the end of the execution of merge, we have created an index file, sorted by token in alphabetical order, containing all information needed to efficiently search for a word in the original corpus.