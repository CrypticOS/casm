// Node runner
// I'm bad at nodejs
var compiler = require("./compile.js");
var fs = require("fs");

fs.readFile("/home/daniel/Documents/os/casm/compiler.casm", "utf8", function(err, data) {
	if (err) {
		return console.error("poop");
	}

	var compile = compiler.compile(data.split("\n"));

	var input = ">**>!*******.!******++.*******.*********++.*++.!%%**++.++++.!%%*.!%*********++++.!%*****++++.++++.!+^d!*+^$||!%*+++.!******++.<<,>>ddda!<<^>>a!%**+++^a!++++^!?!%*******.!%*****+++.!%*****.+++.+.*+++.!%*****+++.!******++.*******.**++.!%*****++..!%***.**+++.!%***+++.!+++^d!*+^$|!++^$|!%*******++++.!%***++++.**+.!*^d!*+^$|!++^$|!**.+++.a$0";
	var inputChar = 0;

	var labels = [];
	var stack = {
		top: [0, 0, 0, 0, 0, 0],
		topP: 0,
		bottom: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
		bottomP: 0
	};

	for (var c = 0; c < compile.length; c++) {
		if (compile[c] == "|") {
			labels.push(c);
		}
	}

	for (var c = 0; c < compile.length; c++) {
		switch (compile[c]) {
		case '+':
			stack.bottom[stack.bottomP]++;
			break;
		case '*':
			stack.bottom[stack.bottomP] += 5;
			break;
		case '%':
			stack.bottom[stack.bottomP] += 50;
			break;
		case '!':
			stack.bottom[stack.bottomP] = 0;
			break;
		case '-':
			stack.bottom[stack.bottomP]--;
			break;

		case '>':
			stack.bottomP++;
			break;
		case '<':
			stack.bottomP--;
			break;
		case 'd':
			stack.topP++;
			break;
		case 'a':
			stack.topP--;
			break;

		case 'v':
			stack.bottom[stack.bottomP] = stack.top[stack.topP];
			break;
		case '^':
			stack.top[stack.topP] = stack.bottom[stack.bottomP];
			break;

		case '.':
			process.stdout.write(String.fromCharCode(
				stack.bottom[stack.bottomP])
			);

			break;
		case ',':
			stack.bottom[stack.bottomP] = input[inputChar].charCodeAt(0);
			inputChar++;
			break;
		case '?':
			if (stack.top[stack.topP + 1] == stack.top[stack.topP + 2]) {
				c = labels[stack.top[stack.topP] - 1];
			}

			break;
		case '$':
			c = labels[stack.top[stack.topP] - 1];
			break;
		}
	}

	process.stdout.write("\n");
});
