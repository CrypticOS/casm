function compile(array) {
	var output = "";

	var variables = {};
	var labels = {};

	for (var l = 0; l < array.length; l++) {
		var tokens = lex(array[l]);

		// Skip blank lines
		if (tokens.length == 0) {
			continue;
		}

		if (tokens[0].type == "label") {
			labels[tokens[0].value] = Object.keys(labels).length;
		} else if (tokens[0].type == "text" && tokens[0].value == "call") {
			labels[l] = Object.keys(labels).length + labels.length;
		}
	}

	var memoryUsed = 0;
	var memoryPlace = 0;

	for (var l = 0; l < array.length; l++) {
		var tokens = lex(array[l]);

		// Skip blank lines
		if (tokens.length == 0) {
			continue;
		}

		if (tokens[0].type == "label") {
			output += "|";
			continue;
		}

		switch(tokens[0].value) {
		case "var":
			if (tokens[2].type == "text") {
				runAt(rawPosition(tokens[2]), "^");
				output += "v";
			} else {
				output += "!";
				output += putChar(parseTokenData(tokens[2]));
			}

			output += ">"; // navigate to next cell

			var length = 1; // temp

			variables[tokens[1].value] = {
				length: length,
				position: memoryUsed
			}

			memoryUsed += length;
			memoryPlace += length;
			break;
		case "print":
			if (tokens[1].type == "string") {
				// A little bit of optmization.
				// characters won't be added in twice.
				// Ex: two 'l's in hello
				var lastChar;
				for (var i = 0; i < tokens[1].value.length; i++) {
					if (lastChar != tokens[1].value[i]) {
						output += "!";
						output += putChar(tokens[1].value[i].charCodeAt(0));
					}

					output += ".";
					lastChar = tokens[1].value[i];
				}
			} else if (tokens[1].type == "text") {
				runAt(rawPosition(tokens[1]), ".");
			} else {
				output += "!" + putChar(parseTokenData(tokens[1]));
			}

			break;
		case "add":
			runAt(
				rawPosition(tokens[1]),
				putChar(parseTokenData(tokens[2]))
			);

			break;
		case "set":
			if (tokens[2].type == "text") {
				if (tokens[2].value == "getchar") {
					runAt(rawPosition(tokens[1]), ",");
					continue;
				}

				runAt(rawPosition(tokens[1]), "^");
				runAt(rawPosition(tokens[2]), "v");
			} else {
				runAt(
					rawPosition(tokens[1]),
					"!" + putChar(parseTokenData(tokens[2]))
				);
			}

			break;
		case "sub":
			runAt(
				rawPosition(tokens[1]),
				"-".repeat(parseTokenData(tokens[2]))
			);

			break;
		case "inline":
			output += tokens[1].value;
			break;
		case "goto":
			//output += "d^a"; // Store current cell up // old
			output += "!"; // Reset cell for next adding
			output += putChar(labels[tokens[1].value] + 1);
			//output += "^dva"; // move up, then restore original value // old
			output += "^$";
			break;
		case "cmp":
			// Store temp in register 4. Don't go all back since
			// We will use the previous 3 registers
			// output += "ddd^a"; // old
			output += "ddda";

			// Copy variable
			output += "!";
			runAt(rawPosition(tokens[1]), "^");

			// Copy char
			output += "a!";
			output += putChar(parseTokenData(tokens[2]));
			output += "^";

			// Copy in label
			output += "a!"
			output += putChar(labels[tokens[3].value] + 1); // put label
			output += "^!";

			// Restore value from register 4
			//output += "dddvaaa"; // old
			output += "?";
			break;
		}

		// Run code at after moving to a certain spot, then return.
		function runAt(spot, code) {
			var oldSpot = memoryPlace;
			var movement = moveTo(memoryPlace, spot);
			output += movement[0];
			output += code;

			// Move back to original spot
			var movementBack = moveTo(movement[1], oldSpot);
			output += movementBack[0];

			memoryPlace = movementBack[1];
		}

		function rawPosition(token) {
			var position = 0;
			position += variables[token.value].position;
			if (token.selector) {
				position += eval(token.selector);
			}

			return position;
		}

	}

	// Minimize output
	while (output.includes("><")) {
		output = output.replace("><", "");
	}

	return output;
}

// Navigate to place in memory
// Returns the output, and where the
// current place in mem is after movement.
function moveTo(from, to) {
	var output = "";
	if (from < to) {
		output += ">".repeat(to - from);
		from += to - from;
	} else if (from > to) {
		output += "<".repeat(from - to);
		from -= from - to;
	}

	return [output, from];
}

function parseTokenData(token) {
	if (token.type == "int") {
		return eval(token.value);
	} else if (token.type == "char") {
		return token.value.charCodeAt(0);
	}
}

// Turn integer into % / * / + ASM
function putChar(code) {
	var out = "";
	while (code != 0) {
		if (code >= 50) {
			out += '%';
			code -= 50;
		} else if (code >= 5) {
			out += '*';
			code -= 5;
		} else {
			out += '+';
			code -= 1;
		}
	}

	return out;
}
