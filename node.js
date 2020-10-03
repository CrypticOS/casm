// Node CLI Compiler
// I'm bad at nodejs
var compiler = require("./compile.js");
var fs = require("fs");

fs.readFile(process.argv[2], "utf8", function(err, data) {
	var compile = compiler.compile(data.split("\n"));
	console.log(compile);
});
