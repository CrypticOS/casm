function lex(string) {
	var tokens = [];

	for (var c = 0; c < string.length; c++) {
		// Skip certain tokens
		if (" \n".includes(string[c])) {
			continue;
		}

		tokens.push({value: "", type: ""});
		var current = tokens[tokens.length - 1];

		if (isAlpha(string[c])) {
			current.type = "text";
			while (isAlpha(string[c])) {
				current.value += string[c];
				c++;
			}

			c--;
		} else if (isNum(string[c])) {
			current.type = "int";
			while (isNum(string[c])) {
				current.value += string[c];
				c++;
			}

			c--;
		} else if (string[c] == "'") {
			current.type = "char";
			c++;
			current.value = string[c];
			c += 2;

		} else if (string[c] == "\"") {
			current.type = "string";
			c++;
			while (string[c] != "\"") {
				current.value += string[c];
				c++;
			}
		} else if (string[c] == ":" && tokens.length == 1) {
			current.type = "label";
			c++;
			while (isAlpha(string[c])) {
				current.value += string[c];
				c++;
			}

			c--;
		}
	}

	return tokens;
}

function isAlpha(char) {
	if ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_".includes(char)) {
		return true;
	} else {
		return false;
	}
}

function isNum(char) {
	if ("0123456789".includes(char)) {
		return true;
	} else {
		return false;
	}
}
