let readline = require('readline');

let rl = readline.createInterface({input: process.stdin});

let dict = {'a': [], 'incredible': []};
let lNb = 0;

rl.on('line', line => {
  let words = line.split(' ');
  words.forEach(word => {
    if (dict.hasOwnProperty(word)) {
      if (!dict[word][lNb]) dict[word].push(1);
      else dict[word][lNb]++;
    }
  });
  lNb++;
});

rl.on('close', () => {
  for (let key in dict) {
    process.stdout.write(`${key} `);
    for (let i = 0; i < dict[key].length; ++i) {
      process.stdout.write(`${i} ${dict[key][i]} `);
    }
    process.stdout.write('\n');
  }
});
