"use strict";

var co = require("co");
var bluebird = require("bluebird");
var fs = require("fs");
var path = require("path");
var childProcess = require('child_process');
bluebird.promisifyAll(fs);

const CORPUS_PATH = path.resolve(__dirname, "../corpus");

co(function*() {
  var corpusFiles = yield fs.readdirAsync(CORPUS_PATH);
  var documents = yield bluebird.map(corpusFiles.filter(fn => /\.z$/.test(fn)),
    fn => new Promise((resolve, reject) => {
      const cmd = 'bash -c "zcat < ' + path.join(CORPUS_PATH, fn) + '"';
      childProcess.exec(cmd, {maxBuffer: 1024 * 10240}, (err, stdout, stderr) => {
        if(err) {
          console.log(err)
          return reject(err);
        }
        resolve(stdout);
      })
    }), {concurrency: 5});
})
.catch(e => console.error(e));
