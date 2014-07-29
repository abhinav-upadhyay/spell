#!/bin/sh

cd corpus
gunzip -k corpus.txt.gz
cd ..
./dictionary -i corpus/corpus.txt -o en_dictionary
rm corpus/corpus.txt
