// Execute CrytpicASM
var loop;
function execute(stack, code) {
	var labels = [];

	for (var c = 0; c < code.length; c++) {
		if (code[c] == "|") {
			labels.push(c);
		}
	}

	clearInterval(loop);

	var c = 0;
	loop = setInterval(function() {
		if (c > code.length) {
			output.value += "\nDone.";
			clearInterval(loop);
			return;
		}

		switch (code[c]) {
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
			output.value += String.fromCharCode(
				stack.bottom[stack.bottomP]
			);

			break;
		case ',':
			stack.bottom[stack.bottomP] = prompt("Enter char").charCodeAt(0);

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

		c++;
	}, 0);
}
