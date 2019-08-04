
## Mini-search engine
Implementing a simple google-like search engine that will use an inverted index implemented as a trie to query big text files.

The engine gets documents from a text file in the following format 

    id ↘                    content ↘
        0 The quick brown fox leaped over the lazy lazy dog
        1 Quick brown foxes leaped over lazy dogs for fun

 - Stores them in an implemented dynamic map with the document id as the
   key and document content as the value.
   
 - Creates the inverted index, which actually maps each word to a posting list, a list with nodes containing the id of the document the word exists and word's frequency in that document.
 - The inverted index with be implemented as a trie, for fast access to words and their data.

![Trie](https://github.com/VangelisGara/Trie/blob/master/trie%20image/Screenshot_20190805_004837.png) ​
- It uses [relevance score](https://en.wikipedia.org/wiki/Okapi_BM25) ​ for each query in order to return the most relevant documents.

## Execution
To compile:

    make clean & make
   
To execute:

    ./minisearch -i <input file> -k <top-k relevant documents>
  
The following commands can then be executed:

 -  **/search q1 q2 q3 q4 ... q10**
Will return top-k relevant documents and the scores of each document

 -  **/df**
 Will print each word in our input file along with the documents it exists in

- **/tf id word**
Will print how many times this word occurs in document with the id given.

- **/exit**
To exit the engine.
