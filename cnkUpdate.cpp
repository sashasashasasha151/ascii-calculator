#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;


struct Result {

	Result(string in) {
		expression = in;
	}

	Result(string in, Result a) {
		expression = in;
		args.push_back(a);
	}

	Result(string in, Result a, Result b) {
		expression = in;
		args.push_back(a);
		args.push_back(b);
	}

	string expression;
	vector<Result> args;
};

class Parser {
private:
	int index;
	string input;

	int makeResult(Result e) {
		switch (e.args.size()) {
		case 2: {
			int a = makeResult(e.args[0]);
			int b = makeResult(e.args[1]);
			if (e.expression == "+") return a + b;
			if (e.expression == "-") return a - b;
			if (e.expression == "*") return a * b;
			if (e.expression == "/") {
				if (b == 0) {
					return INT_MIN;
				}
				return a / b;
			}
			if (e.expression == "%") {
				if (b == 0) {
					return INT_MIN;
				}
				return a % b;
			}
			if (e.expression == ",") {
				if (b < a || a < 0 || b < 0) {
					return INT_MIN;
				}
				return fact(b) / fact(a) / fact(b - a);
			}
		}

		case 1: {
			int a = makeResult(e.args[0]);
			if (e.expression == "+") return +a;
			if (e.expression == "-") return -a;
		}

		case 0:
			return atoi(e.expression.c_str());
		}
	}

	Result Parse() {
		return binaryParser(0);
	}

	Result binaryParser(int currentPriority) {
		Result left = expressionsParser();

		while (true) {
			string op = charParser();
			int priority = getPriority(op);
			if (priority <= currentPriority) {
				index -= op.length();
				return left;
			}

			Result right = binaryParser(priority);
			left = Result(op, left, right);
		}
	}

	Result expressionsParser() {
		string currentChar = charParser();

		if (currentChar == "(") {
			Result result = Parse();
			index++;
			return result;
		}

		if (isdigit(currentChar[0])) {
			return Result(currentChar);
		}

		return Result(currentChar, expressionsParser());
	}

	string charParser() {
		while (index < input.length() && (isspace(input[index]) || input[index] == 'C')) {
			index++;
		}

		if (index == input.length()) {
			return "";
		}

		if (isdigit(input[index])) {
			string number = "";
			while (index < input.length() && isdigit(input[index])) {
				number = number + input[index++];
			}
			return number;
		}

		string operation[] = { "+", "-", "*", "/", "(", ")", ",", "%"};
		for (int j = 0; j < 8; j++) {
			if (operation[j][0] == input[index]) {
				index += operation[j].length();
				return operation[j];
			}
		}
		return "";
	}


	int getPriority(string operation) {
		if (operation == ",") return 1;
		if (operation == "+") return 2;
		if (operation == "-") return 2;
		if (operation == "*") return 3;
		if (operation == "/") return 3;
		if (operation == "%") return 3;
		return 0;
	}

	int fact(int i) {
		int k = 1;
		for (int q = 1; q <= i; q++) {
			k *= q;
		}
		return k;
	}

public:
	int parse(string expression) {
		input = expression;
		index = 0;
		return makeResult(Parse());
	}
};

bool check(string expression) {
	if (expression[0] == '*' || expression[0] == '/' || expression[0] == ',' || expression[0] == ')' || expression[0] == '%' || expression[expression.length() - 1] == '+' ||
		expression[expression.length() - 1] == '*' || expression[expression.length() - 1] == '/' || expression[expression.length() - 1] == '-' || 
		expression[0] == ',' || expression[expression.length() - 1] == 'C' || expression[expression.length() - 1] == '(' || expression[0] == '%') {
		return false;
	}

	string language = "1234567890-+*/( )%,C";

	for (int i = 0; i < expression.length(); i++) {
		bool keyCoincidence = false;
		for (int j = 0; j < language.length(); j++) {
			if (expression[i] == language[j]) {
				keyCoincidence = true;
			}
		}
		if (!keyCoincidence) {
			return false;
		}
	}

	int depth = 0;
	for (int i = 0; i < expression.length(); i++) {
		if (expression[i] == '(') {
			depth++;
		}
		else {
			if (expression[i] == ')') {
				depth--;
			}
		}
		if (depth < 0) {
			return false;
		}
	}
	if (depth != 0) {
		return false;
	}

	for (int i = 0; i < expression.length() - 1; i++) {
		if ((expression[i] == '-' || expression[i] == '+' || expression[i] == '*' || expression[i] == '%' || expression[i] == '/' || expression[i] == '(' || expression[i] == ',') &&
			(expression[i+1] == ')' || expression[i + 1] == '*' || expression[i + 1] == '/' || expression[i + 1] == ')' || expression[i + 1] == ',')) {
			return false;
		}
		if (expression[i] == ')' && expression[i + 1] == '(') {
			return false;
		}
	}
}

class Printer {
private:
	void charHight(int* arr, vector<char> expression) {
		for (int i = 0; i < expression.size(); i++) {
			if (expression[i] == 'C') {
				int depth = 1;
				int j = i + 2;
				while (depth != 0) {
					if (expression[j] == '(') {
						depth++;
					}
					else {
						if (expression[j] == ')') {
							depth--;
						}
					}
					j++;
				}
				for (int k = i + 1; k < j; k++) {
					arr[k]--;
				}
			}
		}
		int min = 0;
		for (int i = 0; i < expression.size(); i++) {
			if (arr[i] < min) {
				min = arr[i];
			}
		}
		for (int i = 0; i < expression.size(); i++) {
			arr[i] += (abs(min) + 1);
		}
	}

	vector<char> arr;

	int spaceSize = 0;

	void zero(char** p, int k, int keyW, int keyH) {
		for (int j = keyH; j < keyH + 5 * k; j += k) {
			for (int i = keyW; i < keyW + 3 * k; i += 2 * k) {
				for (int w = j; w < j + k; w++) {
					for (int q = i; q < i + k; q++) {
						p[w][q] = '#';
					}
				}
			}
		}
		for (int j = keyH; j < keyH + 5 * k; j += 4 * k) {
			for (int w = j; w < j + k; w++) {
				for (int q = keyW + k; q < keyW + 2 * k; q++) {
					p[w][q] = '#';
				}
			}
		}
	}

	void one(char** p, int k, int keyW, int keyH) {
		for (int j = keyH; j < keyH + 5 * k; j += k) {
			for (int w = j; w < j + k; w++) {
				for (int q = keyW; q < keyW + k; q++) {
					p[w][q] = '#';
				}
			}
		}
	}

	void two(char** p, int k, int keyW, int keyH) {
		for (int j = keyH; j < keyH + 5 * k; j += 2 * k) {
			for (int i = keyW; i < keyW + 3 * k; i += k) {
				for (int w = j; w < j + k; w++) {
					for (int q = i; q < i + k; q++) {
						p[w][q] = '#';
					}
				}
			}
		}
		for (int w = keyH + k; w < keyH + k * 2; w++) {
			for (int q = keyW + 2 * k; q < keyW + 3 * k; q++) {
				p[w][q] = '#';
			}
		}
		for (int w = keyH + 3 * k; w < keyH + 4 * k; w++) {
			for (int q = keyW; q < keyW + k; q++) {
				p[w][q] = '#';
			}
		}
	}

	void three(char** p, int k, int keyW, int keyH) {
		for (int j = keyH; j < keyH + 5 * k; j += 2 * k) {
			for (int i = keyW; i < keyW + 3 * k; i += k) {
				for (int w = j; w < j + k; w++) {
					for (int q = i; q < i + k; q++) {
						p[w][q] = '#';
					}
				}
			}
		}
		for (int j = keyH + k; j < keyH + 5 * k; j += 2 * k) {
			for (int w = j; w < j + k; w++) {
				for (int q = keyW + 2 * k; q < keyW + 3 * k; q++) {
					p[w][q] = '#';
				}
			}
		}
	}

	void four(char** p, int k, int keyW, int keyH) {
		for (int j = keyH; j < keyH + 5 * k; j += k) {
			for (int w = j; w < j + k; w++) {
				for (int q = keyW + 2 * k; q < keyW + 3 * k; q++) {
					p[w][q] = '#';
				}
			}
		}
		for (int j = keyH; j < keyH + 3 * k; j += k) {
			for (int w = j; w < j + k; w++) {
				for (int q = keyW; q < keyW + k; q++) {
					p[w][q] = '#';
				}
			}
		}
		for (int w = keyH + 2 * k; w < keyH + 3 * k; w++) {
			for (int q = keyW + k; q < keyW + 2 * k; q++) {
				p[w][q] = '#';
			}
		}
	}

	void five(char** p, int k, int keyW, int keyH) {
		for (int i = keyW; i < keyW + 3 * k; i += k) {
			for (int j = keyH; j < keyH + 5 * k; j += 2 * k) {
				for (int q = i; q < i + k; q++) {
					for (int w = j; w < j + k; w++) {
						p[w][q] = '#';
					}
				}
			}
		}
		for (int q = keyW; q < keyW + k; q++) {
			for (int w = keyH + k; w < keyH + k * 2; w++) {
				p[w][q] = '#';
			}
		}
		for (int q = keyW + 2 * k; q < keyW + 3 * k; q++) {
			for (int w = keyH + 3 * k; w < keyH + 4 * k; w++) {
				p[w][q] = '#';
			}
		}
	}

	void six(char** p, int k, int keyW, int keyH) {
		for (int j = keyH; j < keyH + 5 * k; j += 2 * k) {
			for (int i = keyW; i < keyW + 3 * k; i += k) {
				for (int w = j; w < j + k; w++) {
					for (int q = i; q < i + k; q++) {
						p[w][q] = '#';
					}
				}
			}
		}
		for (int j = keyH; j < keyH + 5 * k; j += k) {
			for (int w = j; w < j + k; w++) {
				for (int q = keyW ; q < keyW + k; q++) {
					p[w][q] = '#';
				}
			}
		}
		for (int w = keyH + 3 * k; w < keyH + 4 * k; w++) {
			for (int q = keyW + k * 2; q < keyW + k * 3; q++) {
				p[w][q] = '#';
			}
		}
	}

	void seven(char** p, int k, int keyW, int keyH) {
		for (int j = keyH; j < keyH + 5 * k; j += k) {
			for (int q = keyW + 2 * k; q < keyW + 3 * k; q++) {
				for (int w = j; w < j + k; w++) {
					p[w][q] = '#';
				}
			}
		}
		for (int i = keyW; i < keyW + 2 * k; i += k) {
			for (int q = i; q < i + k; q++) {
				for (int w = keyH; w < keyH + k; w++) {
					p[w][q] = '#';
				}
			}
		}
	}

	void eight(char** p, int k, int keyW, int keyH) {
		for (int i = keyW; i < keyW + 3 * k; i += 2 * k) {
			for (int j = keyH; j < keyH + 5 * k; j += k) {
				for (int q = i; q < i + k; q++) {
					for (int w = j; w < j + k; w++) {
						p[w][q] = '#';
					}
				}
			}
		}
		for (int j = keyH; j < keyH + 5 * k; j += 2 * k) {
			for (int q = keyW + k; q < keyW + 2 * k; q++) {
				for (int w = j; w < j + k; w++) {
					p[w][q] = '#';
				}
			}
		}
	}

	void nine(char** p, int k, int keyW, int keyH) {
		for (int i = keyW; i < keyW + 3 * k; i += k) {
			for (int j = keyH; j < keyH + 5 * k; j += 2 * k) {
				for (int q = i; q < i + k; q++) {
					for (int w = j; w < j + k; w++) {
						p[w][q] = '#';
					}
				}
			}
		}
		for (int j = keyH; j < keyH + 5 * k; j += k) {
			for (int q = keyW + 2 * k; q < keyW + 3 * k; q++) {
				for (int w = j; w < j + k; w++) {
					p[w][q] = '#';
				}
			}
		}
		for (int q = keyW; q < keyW + k; q++) {
			for (int w = keyH + k; w < keyH + k * 2; w++) {
				p[w][q] = '#';
			}
		}
	}

	void dots(char** p, int k, int keyW, int keyH) {
		for (int j = keyH + k; j < keyH + 4 * k; j += 2 * k) {
			for (int q = keyW; q < keyW + k; q++) {
				for (int w = j; w < j + k; w++) {
					p[w][q] = '#';
				}
			}
		}
	}

	void mul(char** p, int k, int keyW, int keyH) {
		for (int j = keyH + k; j < keyH + 4 * k; j += 2 * k) {
			for (int q = keyW; q < keyW + k; q++) {
				for (int w = j; w < j + k; w++) {
					p[w][q] = '#';
				}
			}
		}
		for (int j = keyH + k; j < keyH + 4 * k; j += 2 * k) {
			for (int q = keyW + 2 * k; q < keyW + 3 * k; q++) {
				for (int w = j; w < j + k; w++) {
					p[w][q] = '#';
				}
			}
		}
		for (int q = keyW + k; q < keyW + 2 * k; q++) {
			for (int w = keyH + 2 * k; w < keyH + 3 * k; w++) {
				p[w][q] = '#';
			}
		}
	}

	void plus(char** p, int k, int keyW, int keyH) {
		for (int i = keyW; i < keyW + 3 * k; i += k) {
			for (int w = keyH + 2 * k; w < keyH + 3 * k; w++) {
				for (int q = i; q < i + k; q++) {
					p[w][q] = '#';
				}
			}
		}
		for (int i = keyH + k; i < keyH + 4 * k; i += 2 * k) {
			for (int w = keyW + k; w < keyW + 2 * k; w++) {
				for (int q = i; q < i + k; q++) {
					p[q][w] = '#';
				}
			}
		}

	}

	void minus(char** p, int k, int keyW, int keyH) {
		for (int i = keyW; i < keyW + 3 * k; i += k) {
			for (int w = keyH + 2 * k; w < keyH + 3 * k; w++) {
				for (int q = i; q < i + k; q++) {
					p[w][q] = '#';
				}
			}
		}
	}

	void left(char** p, int k, int keyW, int keyH) {
		for (int q = keyW; q < keyW + k; q++) {
			for (int w = keyH + 2 * k; w < keyH + 3 * k; w++) {
				p[w][q] = '#';
			}
		}
		for (int j = keyH + k; j < keyH + 4 * k; j += 2 * k) {
			for (int q = keyW + k; q < keyW + 2 * k; q++) {
				for (int w = j; w < j + k; w++) {
					p[w][q] = '#';
				}
			}
		}
		for (int j = keyH; j < keyH + 5 * k; j += 4 * k) {
			for (int q = keyW + 2 * k; q < keyW + 3 * k; q++) {
				for (int w = j; w < j + k; w++) {
					p[w][q] = '#';
				}
			}
		}
	}

	void right(char** p, int k, int keyW, int keyH) {
		for (int q = keyW + 2 * k; q < keyW + 3 * k; q++) {
			for (int w = keyH + 2 * k; w < keyH + 3 * k; w++) {
				p[w][q] = '#';
			}
		}

		for (int j = keyH + k; j < keyH + 4 * k; j += 2 * k) {
			for (int q = keyW + k; q < keyW + 2 * k; q++) {
				for (int w = j; w < j + k; w++) {
					p[w][q] = '#';
				}
			}
		}
		for (int j = keyH; j < keyH + 5 * k; j += 4 * k) {
			for (int q = keyW; q < keyW + k; q++) {
				for (int w = j; w < j + k; w++) {
					p[w][q] = '#';
				}
			}
		}
	}

	void equal(char** p, int k, int keyW, int keyH) {
		for (int i = keyW; i < keyW + 3 * k; i += k) {
			for (int w = keyH + k; w < keyH + 2 * k; w++) {
				for (int q = i; q < i + k; q++) {
					p[w][q] = '#';
				}
			}
		}
		for (int i = keyW; i < keyW + 3 * k; i += k) {
			for (int w = keyH + 3 * k; w < keyH + 4 * k; w++) {
				for (int q = i; q < i + k; q++) {
					p[w][q] = '#';
				}
			}
		}
	}

	void cnk(char** p, int k, int keyW, int keyH) {
		for (int j = keyH + 2 * k; j < keyH + 8 * k; j += k) {
			for (int w = j; w < j + k; w++) {
				for (int q = keyW; q < keyW + k; q++) {
					p[w][q] = '#';
				}
			}
		}
		for (int j = keyH + 2 * k; j < keyH + 8 * k; j += 5 * k) {
			for (int i = keyW; i < keyW + 6 * k; i += 5 * k) {
				for (int w = j; w < j + k; w++) {
					for (int q = i; q < i + k; q++) {
						p[w][q] = '#';
					}
				}
			}
		}
		for (int j = keyH + k; j < keyH + 9 * k; j += 7 * k) {
			for (int i = keyW + k; i < keyW + 5 * k; i += 3 * k) {
				for (int w = j; w < j + k; w++) {
					for (int q = i; q < i + k; q++) {
						p[w][q] = '#';
					}
				}
			}
		}
		for (int j = keyH; j < keyH + 10 * k; j += 9 * k) {
			for (int i = keyW + 2 * k; i < keyW + 4 * k; i += k) {
				for (int w = j; w < j + k; w++) {
					for (int q = i; q < i + k; q++) {
						p[w][q] = '#';
					}
				}
			}
		}
	}

	void makeAns(char** ans, int* ptr, vector<char> arr, int position, int keyH, int keyW, int height) {
		for (int i = 0; i < arr.size(); i++) {
			if (arr[i] == '.') {
				i++;
				position++;
				while (arr[i] != '.') {
					switch (arr[i]) {
					case '0':
						zero(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
						keyW += 3 * pow(2, ptr[position++] - 1);
						break;
					case '1':
						one(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
						keyW += pow(2, ptr[position++] - 1);
						break;
					case '2':
						two(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
						keyW += 3 * pow(2, ptr[position++] - 1);
						break;
					case '3':
						three(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
						keyW += 3 * pow(2, ptr[position++] - 1);
						break;
					case '4':
						four(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
						keyW += 3 * pow(2, ptr[position++] - 1);
						break;
					case '5':
						five(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
						keyW += 3 * pow(2, ptr[position++] - 1);
						break;
					case '6':
						six(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
						keyW += 3 * pow(2, ptr[position++] - 1);
						break;
					case '7':
						seven(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
						keyW += 3 * pow(2, ptr[position++] - 1);
						break;
					case '8':
						eight(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
						keyW += 3 * pow(2, ptr[position++] - 1);
						break;
					case '9':
						nine(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
						keyW += 3 * pow(2, ptr[position++] - 1);
						break;
					case '-':
						minus(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
						keyW += 3 * pow(2, ptr[position++] - 1);
						break;
					default:
						cout << "Error!";
						break;
					}
					i++;
					keyW++;
				}
				position++;
			}
			else {
				switch (arr[i]) {
				case '0':
					zero(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '1':
					one(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += pow(2, ptr[position++] - 1) + 1;
					break;
				case '2':
					two(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '3':
					three(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '4':
					four(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '5':
					five(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '6':
					six(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '7':
					seven(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '8':
					eight(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '9':
					nine(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '-':
					minus(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '+':
					plus(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '*':
					mul(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '/':
					dots(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += pow(2, ptr[position++] - 1) + 1;
					break;
				case '=':
					equal(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case ')':
					right(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case '(':
					left(ans, pow(2, ptr[position] - 1), keyW, keyH + height / 2 - pow(2, ptr[position] - 1) * 5 / 2);
					keyW += 3 * pow(2, ptr[position++] - 1) + 1;
					break;
				case 'C': {
					cnk(ans, pow(2, ptr[position] - 2), keyW, keyH + height / 2 - pow(2, ptr[position] - 2) * 5);
					keyW += 6 * (pow(2, ptr[position] - 2)) + 1;
					position += 2;
					int keyLeft = i + 2;
					int deep = 0;
					int keyI = i + 2;
					while (arr[keyI] != ',' || deep != 0) {
						if (arr[keyI] == '(') {
							deep++;
						}
						else {
							if (arr[keyI] == ')') {
								deep--;
							}
						}
						keyI++;
					}
					vector<char> copyLeft;
					for (int j = i + 2; j < keyI; j++) {
						copyLeft.push_back(arr[j]);
					}
					makeAns(ans, ptr, copyLeft, position, keyH, keyW, height / 2);
					i = keyI;
					keyI = i + 1;
					while (arr[keyI] != ')' || deep != 0) {
						if (arr[keyI] == '(') {
							deep++;
						}
						else {
							if (arr[keyI] == ')') {
								deep--;
							}
						}
						keyI++;
					}
					position += (copyLeft.size() + 1);
					vector<char> copyRight;
					for (int j = i + 1; j < keyI; j++) {
						copyRight.push_back(arr[j]);
					}
					makeAns(ans, ptr, copyRight, position, keyH + height / 2 + 1, keyW, height / 2);

					for (int j = 0; j < 210 - 2; j++) {
						int key = 0;
						for (int z = 0; z < height; z++) {
							if (ans[z][j] == ' ' && ans[z][j + 1] == ' ' && ans[z][j + 2] == ' ') {
								key++;
							}
							else {
								continue;
							}
						}
						if (key == height) {
							keyW = j + 1;
							break;
						}
					}

					i = keyI;
					position += (copyRight.size() + 1);

					//��������� ��������� �� 2
					//��������� ������
					//������� �������
					break;
				}
				default:
					cout << "Error!";
					break;
				}
			}
			keyW++;
		}
	}

public:
	void putChar(char c) {
		arr.push_back(c);
		spaceSize++;
	}

	void putInt(int k) {
		spaceSize++;
		int min = 0;
		if (k < 0) {
			k *= -1;
			min = 1;
		}
		if (k / 10 == 0 && min == 0) {
			arr.push_back(k + '0');
			return;
		}
		vector<char> key;
		while (k / 10 != 0) {
			key.push_back(k % 10 + '0');
			k /= 10;
		}
		key.push_back(k % 10 + '0');
		arr.push_back('.');
		if (min == 1) {
			arr.push_back('-');
		}
		for (int i = key.size() - 1; i >= 0; i--) {
			arr.push_back(key[i]);
		}
		arr.push_back('.');
	}


	void print() {
		ofstream cout("output.txt");
		//������ �������� ������� �������
		int *ptr = new int[arr.size()];
		for (int i = 0; i < arr.size(); i++) {
			ptr[i] = 0;
		}
		charHight(ptr, arr);

		/*for (int i = 0; i < arr.size(); i++) {
			cout << arr[i] << " ";
		}
		cout << "\n";
		for (int i = 0; i < arr.size(); i++) {
			cout << ptr[i] << " ";
		}
		cout << "\n";*/

		//������������ ������ �������
		int maxH = 0;
		for (int i = 0; i < arr.size(); i++){
			if (ptr[i]>maxH) {
				maxH = ptr[i];
			}
		}

		//������ ������� ��� ������
		int ansH = 5;
		if (maxH == 1) {
			ansH = 5;
		}
		else {
			for (int i = 1; i < maxH; i++) {
				ansH = ansH * 2 + 2;
			}
		}

		int size = 0;
		int k = 0;
		for (int i = 0; i < arr.size(); i++) {
			if (arr[i] == '.') {
				continue;
			}
			else {
				if (arr[i] == 'C') {
					size += (ptr[k++] - 1) * 6 + 1;
				}
				else {
					size += ptr[k++] * 3 + 2;
				}
			}
		}

		//������ ������ ��� ������ � ��������� ���������
		char **ans = new char*[ansH + 1];
		for (int i = 0; i < ansH + 1; i++) {
			ans[i] = new char[size];
		}
		for (int i = 0; i < ansH + 1; i++) {
			for (int j = 0; j < size; j++) {
				ans[i][j] = ' ';
			}
		}

		makeAns(ans, ptr, arr, 0, 0, 0, ansH);

		for (int i = 0; i < ansH + 1; i++) {
			for (int j = 0; j < size; j++) {
				cout << ans[i][j];
			}
			cout << "\n";
		}
	}

	void clear() {
		arr.clear();
	}
};

void makeCache(Printer* a, string expression) {
	for (int i = 0; i < expression.length(); i++) {
		switch (expression[i]) {
		case '+':
		case '*':
		case '/':
		case ')':
		case '(':
		case ',':
			a->putChar(expression[i]);
			break;
		case 'C':
			a->putChar(expression[i]);
			break;
		case '-':
			if (i == 0 || expression[i - 1] == '-' || expression[i - 1] == ',' || expression[i - 1] == '+' || expression[i - 1] == '*' || expression[i - 1] == '/' || expression[i - 1] == '(') {
				int length = 2;
				i += 2;
				while (expression[i] != '-' && expression[i] != '+' && expression[i] != ',' && expression[i] != '*' && expression[i] != '/' && expression[i] != ')' && i < expression.length()) {
					i++;
					length++;
				}
				string cop = expression.substr(i - length, length);
				int intK = atoi(cop.c_str());
				a->putInt(intK);
				i--;
			}
			else {
				a->putChar(expression[i]);
			}
			break;
		default:
			int length = 1;
			i++;
			while (expression[i] != '-' && expression[i] != ',' && expression[i] != '+' && expression[i] != '*' && expression[i] != '/' && expression[i] != ')' && i<expression.length()) {
				i++;
				length++;
			}
			string cop = expression.substr(i - length, length);
			int intK = atoi(cop.c_str());
			a->putInt(intK);
			i--;
			break;
		}
	}
}

int main() {
	ofstream cout("output.txt");
	Parser parser;
	Printer printer;
	//for (;;) {
		string f;
		getline(cin, f);
		if (!check(f)) {
			cout << "ERROR!" << "\n";
		}
		else {
			int answer = parser.parse(f);
			if (answer == INT_MIN) {
				cout << "ERROR!" << "\n";
			}
			else {
				makeCache(&printer, f);
				printer.putChar('=');
				printer.putInt(answer);
				printer.print();
				printer.clear();
				//cout << f << "=" << parser.parse(f) << "\n";
			}
		}
	//}
	return 0;
}