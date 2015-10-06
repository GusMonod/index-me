from subprocess import Popen, PIPE, STDOUT
import sys
from os import listdir
from os.path import isfile, join
import fnmatch

def load_z_file(file_path):
    cmd = 'gzip -d -c %s' % (file_path)
    p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
    return p.stdout.read()

def load_corpus(corpus_path):
    file_names = listdir(corpus_path)
    return map(lambda n: load_z_file("%s/%s" % (corpus_path, n)),
        filter(lambda n: fnmatch.fnmatch(n, '*.z'), file_names))

print(load_corpus("./corpus"))
