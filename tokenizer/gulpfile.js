'use strict';

var gulp = require('gulp');
var gutil = require('gulp-util');
var babel = require('gulp-babel');
var fs = require('fs');
var path = require('path');
var runSequence = require('run-sequence');

const kOutputPath = process.env.OUTPUT_PATH ||
                    path.join(__dirname, '../corpus/corpus.tok');

gulp.task('build', function() {
  return gulp.src('*.es6')
      .pipe(babel({optional: ['runtime']}))
      .pipe(gulp.dest('obj'));
});

gulp.task('run', function() {
  return new Promise (function (resolve, reject) {
    require('./obj/tokenize.js').then(function (cleanDocs) {
      var output = fs.createWriteStream(kOutputPath);
      cleanDocs.forEach(function (doc) { output.write(doc + '\n'); });
      resolve(true);
    });
  });
});

gulp.task('default', function() {
  runSequence('build', 'run');
});
