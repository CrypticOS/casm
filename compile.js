function compile(array) {
	var output = "";

	var variables = {};
	var labels = {};

	for (var l = 0; l < array.length; l++) {
		var tokens = lex(array[l]);
		if (tokens[0].type == "label") {
			labels[tokens[0].value] = Object.keys(labels).length;
		}
	}

	var memoryUsed = 0;
	var memoryPlace = 0;

	for (var l = 0; l < array.length; l++) {
		var tokens = lex(array[l]);
		switch(tokens[0].value) {
		case "var":
			var code = putChar(parseTokenData(tokens[2]));

			output += "!"; // Reset cell just in case
			output += code; // Insert the integer
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
			} else {
				runAt(variables[tokens[1].value].position, ".");
			}

			break;
		case "add":
			runAt(variables[tokens[1].value].position, "+".repeat(tokens[2].value));
			break;
		case "inline":
			output += tokens[1].value;
			break;
		case "goto":
			output += "d^a"; // Store current cell up
			output += putChar(labels[tokens[1].value] + 1);
			output += "^dva"; // move up, then restore original value
			output += "$";
		}

		if (tokens[0].type == "label") {
			output += "|";
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
