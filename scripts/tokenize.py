from subprocess import Popen, PIPE, STDOUT
import sys
import re
from os import listdir
from os.path import isfile, join
import fnmatch

def load_z_file(file_path):
    return Popen(
        'gzip -d -c %s' % (file_path),
        shell=True,
        stdin=PIPE,
        stdout=PIPE,
        stderr=STDOUT,
        close_fds=True
    ).stdout.read()

def tokenize_text(text):
    return " ".join(
        map(
            lambda s: s.lower(),
            re.compile(r"(\s|\n)+").split(text)))

def tokenize_corpus(corpus_path):
    return "\n".join(
        map(
            lambda n: " ".join(
                tokenize_text(
                    load_z_file(
                        "%s/%s" % (corpus_path, n)))),
            filter(
                lambda n: fnmatch.fnmatch(n, '*.z'),
                listdir(corpus_path))))

print(tokenize_corpus("./corpus"))
