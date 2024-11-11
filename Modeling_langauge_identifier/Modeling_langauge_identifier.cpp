#include<iostream>
#include<vector>
#include<string>
#include<regex>
#include<queue>

using namespace std;

const string example1 = " dim ab , b integer \n dim c , d boolean \n a := 532.1 \n c := true \n if ( a = = 0101b ) begin b := 6 ; a := 3 end \n else b := 4 \n while ( c != false ) begin a := a - 1 ; c := false end \n (* test example #1 *) \n end";
const string example2 = " dim a , c real \n c := 1 \n while ( c < 2 && c > 0 ) begin \n a := 0102321H ; c := c + 1 end \n end";
const string example3 = " dim a , i integer \n readln a (* ввод значения переменной a *) \n for i := 1 to i <= a step i + 2 begin \n writeln i , 2 , 3 ; \n readln a \n end next \n end";
const string example4 = " dim a , b , x integer \n while ( x > 3 ) begin writeln x ; x := x + 1 end \n end";
const string example5 = " dim a , b integer \n if ( a = = 2 ) writeln a \n while ( a < 3 ) a := a + 3 \n end";

const string count_factorial = " dim a , i integer \n readln a (* ввод числа, для которого вычисляется факториал *) \n if ( a = = 0 ) writeln 1 \n else begin \n for i := 1 to i <= a \n a := a * i \n next ; \n writeln a \n end \n end";
const string count_average = " dim a , sum real (* объявление переменных для ввода значений и накопления суммы *) \n dim i , count (* объявление переменных для иетариции цикла и количества вводимых значений *) integer \n readln count \n for i := 1 to count \n begin \n readln a ; \n sum := sum + a \n end \n next \n writeln sum / count \n end";
const string count_triangle_perimeter = " dim a , b , c integer \n readln a , b , c \n if ( a >= b + c || b >= a + c || c >= a + b ) \n writeln 0 \n  else \n writeln a + b + c \n end";

class Lexer {
	friend class Syntaxer; // выделение синтаксеру доступа к закрытым полям
	friend class SyntaxError; // выделение классам обработки исключений доступа к полям
	friend class LexicalError;
	friend class SemanticError;
	vector <string> key_words; // 1
	vector <string> delimiters; // 2
	vector <string> numbers; // 3
	vector <string> identificators; // 4
	vector <int> tokens_per_string; // вектор для хранения количества токенов в каждой строке
public: Lexer() {
	key_words = vector <string>{ "end", "dim", "integer", "real",
				"boolean", "begin", "if", "else",
				"for", "to", "step", "next", "while",
				"readln", "writeln", "true", "false" };
	delimiters = vector <string>{ "!=", "= =", "<", "<=", ">", ">=",
				  "+", "-", "||", "*", "/", "&&", ",", ";",
				  "!", "(", ")", "(*", "*)", "{", "}", ":=" };
	numbers = {};
	identificators = {};
	tokens_per_string = {};
}
public: void print_table() {
	cout << "Key_words: ";
	for (string s : key_words) cout << s << " ";
	cout << "\n";
	cout << "Delimiters: ";
	for (string s : delimiters) cout << s << " ";
	cout << "\n";
	cout << "Numbers: ";
	for (string s : numbers) cout << s << " ";
	cout << "\n";
	cout << "Identificators: ";
	for (string s : identificators) cout << s << " ";
	cout << "\n";
}
private: bool is_num(string s) {
	regex pattern_decimal_real("([0-9]*[.]?[0-9]+)([eE][+-]?[0-9]+)?"); // проверка на научную запись десятичного числа
	regex pattern_decimal("[0-9]+[dD]?"); // проверка на целое десятичное число
	regex pattern_binary("[01]+[bB]"); // проверка на целое двоичное число
	regex pattern_octonary("[0-7]+[oO]"); // проверка на целое восьмиричное число
	regex pattern_hexical("[0-9]*[0-9A-Fa-f]+[hH]"); // проверка на целое шестандцатеричное число
	return regex_match(s, pattern_decimal) || regex_match(s, pattern_binary) || regex_match(s, pattern_octonary) || regex_match(s, pattern_hexical) || regex_match(s, pattern_decimal_real);
}
private: bool is_keyword(string s) {
	return find(key_words.begin(), key_words.end(), s) != key_words.end(); // поиск строки в векторе идентификаторов
}
public: vector <pair<int, int> > lex_analyz(string s) {
	vector <pair <int, int> > result; // результирующий вектор пар (a, b); a - номер группы токенов, b - номер токена в группе
	int count = 0; // счётчик токенов на одной строке
	char state = 'H'; // состояние конечного автомата, по умолчанию H - чтение входного символа
	string temp = "";
	for (int i = 0; i < s.size(); i++) {
		switch (state)
		{
		case 'H':
			if (s[i] == ':') {
				temp = s[i];
				state = 'A'; // состояние формирования знака присваивания
			}
			else if (s[i] == '=') {
				temp = s[i];
				state = 'Q'; // состояние формирования знака проверки на равенство
			}
			else if (int(s[i]) == 95 || int(s[i]) >= 97 && int(s[i]) <= 122 || int(s[i]) >= 65 && int(s[i]) <= 90) {
				temp = s[i];
				state = 'I'; // состояние формирования идентификатора
			}
			else if (s[i] == '+' || s[i] == '-' || s[i] == '.' || int(s[i]) >= 48 && int(s[i]) <= 57) {
				temp = s[i];
				state = 'N'; // состояние формирования числа
			}
			else if (s[i] == ' ' || s[i] == '\n' || s[i] == '\t') {
				if (s[i] == '\n') {
					tokens_per_string.push_back(count); // запись количества токенов в текущей строке
					count = 0;
				}
				continue; // переход к следующему входному символу
			}
			else {
				temp = s[i];
				state = 'D'; // состояние формирования разделителя
			}
			break;
		case 'A':
			if (temp + s[i] == ":=") { // при формировании знака присваивания - добавление токена
				result.push_back(pair <int, int>(2, find(delimiters.begin(), delimiters.end(), s) - delimiters.begin() + 1));
				count++;
				temp = "";
				state = 'H';
			}
			else state = 'E'; // при любом другом входном символе - состояние ошибки
			break;
		case 'Q':
			if (s[i] == ' ') temp += s[i]; // накопление разделителя до встречи символа, отличного от пробела
			else {
				temp += s[i];
				if (temp == "= =") { // если накоплен разделитель - знак проверки на равенство
					result.push_back(pair <int, int>(2, find(delimiters.begin(), delimiters.end(), temp) - delimiters.begin() + 1));
					count++;
					temp = "";
					state = 'H';
				}
				else state = 'E'; // иначе - сосотояние ошибки
			}
			break;
		case 'I':
			if (s[i] != ' ' && (int(s[i]) >= 48 && int(s[i]) <= 57 || int(s[i]) >= 97 && int(s[i]) <= 122 || int(s[i]) >= 65 && int(s[i]) <= 90)) temp += s[i];
			else if (s[i] == ' ') { // если считан пробел - определение подходящей группы токенов
				if (is_keyword(temp)) { // проверка - является ли последовательность ключевым словом
					result.push_back(pair <int, int>(1, find(key_words.begin(), key_words.end(), temp) - key_words.begin() + 1));
					count++;
					temp = "";
					state = 'H';
				}
				else {
					// проверка, существует ли текущий идентификатор в таблице идентификаторов
					if (find(identificators.begin(), identificators.end(), temp) == identificators.end()) { 
						identificators.push_back(temp);
						result.push_back(pair <int, int>(4, identificators.size()));
						count++;
					}
					else {
						result.push_back(pair <int, int>(4, find(identificators.begin(), identificators.end(), temp) - identificators.begin() + 1));
						count++;
					}
					temp = "";
					state = 'H';
				}
			}
			else state = 'E'; // при любом другом входном символе - состояние ошибки
			break;
		case 'N':
			if (s[i] == ' ') { // при чтении пробела - проверка на принадлежность сформированной последовательности к разделителям или числам
				if (temp == "+" || temp == "-") {
					result.push_back(pair <int, int>(2, find(delimiters.begin(), delimiters.end(), temp) - delimiters.begin() + 1));
					count++;
					temp = "";
					state = 'H';
				}
				else {
					if (is_num(temp)) { // если последовательность - число, то проверка на существование данного токена в таблице чисел
						if (find(numbers.begin(), numbers.end(), temp) == numbers.end()) {
							numbers.push_back(temp);
							result.push_back(pair <int, int>(3, numbers.size()));
							count++;
						}
						else {
							result.push_back(pair <int, int>(3, find(numbers.begin(), numbers.end(), temp) - numbers.begin() + 1));
							count++;
						}
						temp = "";
						state = 'H';
					}
					else {
						state = 'E'; // в случае, если последовательность - не число, переход в состояние ошибки
						continue;
					}
				}
				temp = "";
				state = 'H';
			} else if (s[i] >= '0' && s[i] <= '9' || s[i] == '+' || s[i] == '-' || s[i] == '.' || s[i] >= 'A' && s[i] <= 'F' || s[i] >= 'a' && s[i] <= 'f' || s[i] == 'h' || s[i] == 'H' || s[i] == 'o' || s[i] == 'O') temp += s[i];
			else state = 'E'; // при любом другом входном символе - состояние ошибки
			break;
		case 'D':
			if (temp == "(" && s[i] == '*') { // если накоплено сочетание начала комментария
				state = 'C'; // переход в состояние обработки комментария
			}
			else if (s[i] != ' ') {
				temp += s[i]; // накапливание разделителя
			}
			else {
				// поиск разделителя в таблице разделителей
				if (find(delimiters.begin(), delimiters.end(), temp) != delimiters.end()) {
					result.push_back(pair <int, int> (2, find(delimiters.begin(), delimiters.end(), temp) - delimiters.begin() + 1));
					count++;
					temp = "";
					state = 'H';
				}
				else state = 'E';
			}
			break;
		case 'C':
			// проверка на сочетание завершения комментария
			if (temp[temp.size() - 1] == '*' && s[i] == ')') {
				temp = ""; // очистка буфера
				state = 'H';
			}
			else temp += s[i]; // накопление комментария 
			break;
		case 'E':
			result = { pair <int, int>(-1, -1) }; // возврат диагностического значения, сообщающего об ошибке во входных данных
			break;
		}
	}
	if (state == 'E' && result != vector <pair<int, int> >{ pair <int, int>(-1, -1) }) result = { pair <int, int>(-1, -1) };
	return result;
}
};

struct Node {
	string value;
	vector <Node*> children;
	Node(string value) {
		this->value = value;
	}
};

class LexicalError : public runtime_error {
public:
	LexicalError(const string& message) : runtime_error(message) {}
	string getMessage() const {
		return what();
	}
};

// класс для обработки исключений, выбрасываемых при нахождении синтаксической ошибки
class SyntaxError : public runtime_error { 
	int index;
	Lexer lx;
public:
	SyntaxError(const string& message, int index, Lexer& lx) : runtime_error(message), index(index), lx(lx) {}
	string getMessage() const {
		int i = 1;
		int sum = lx.tokens_per_string[0];
		while (i < lx.tokens_per_string.size() && sum <= index) {
			sum += lx.tokens_per_string[i];
			i++;
		}
		return "Строка: " + to_string(i) + ". " + what();
	}
};

// класс для обработки исключений, выбрасываемых при нахождении семантической ошибки
class SemanticError : public runtime_error {
	int index;
	Lexer lx;
public:
	SemanticError(const string& message, int index, Lexer& lx) : runtime_error(message), index(index), lx(lx) {}
	string getMessage() const {
		int i = 1;
		int sum = lx.tokens_per_string[0];
		while (sum <= index) {
			sum += lx.tokens_per_string[i];
			i++;
		}
		return "Строка: " + to_string(i) + ". " + what();
	}
};

class Syntaxer {
public:
	Syntaxer(Lexer& lx) {
		this->lx = lx;
		described = {};
	};
	Node* synt_analyz(vector<pair<int, int>> lex_res) {
		this->lex_res = lex_res;
		index = 0; // инициализация индекса
		return PROGRAM(); // разбор программы
	}
	void printTreeByLevels(Node* root) {
		if (root->children.size() == 0) return;
		else {
			if (root == nullptr) return;
			cout << "\n" << root->value<<"\t";
			for (int i = 0; i < root->children.size(); i++) {
				cout << root->children[i]->value<<" ";
			}
			for (int i = 0; i < root->children.size(); i++)	printTreeByLevels(root->children[i]);
		}
	}
private:
	Lexer lx; // лексер
	vector<pair<int, int>> lex_res; // результат работы лексера
	size_t index; // индекс текущей лексемы
	vector <string> described; // вектор для хранения описанных переменных

	Node* PROGRAM() {
		Node* programNode = new Node("program"); // создание корня дерева разбора
		try {
			if (lex_res[0].first == -1) throw LexicalError("Ошибка. Код: 14. Комментарий: лексическая ошибка\n"); // если на выходе лексера получился вывод, сообщающий о наличии ошибки
			while (index < lex_res.size() && !(lex_res[index].first == 1 && lex_res[index].second == 1)) {
				if (lex_res[index].first == 1 && lex_res[index].second == 2) { // "dim"
					programNode->children.push_back(DESC());
					continue;
				}
				else {
					programNode->children.push_back(OPERATOR());
				}
				if (index < lex_res.size() && lex_res[index].first == 2 && lex_res[index].second == 1) { // ":"
					index++; // пропуск двоеточия
				}
				if (index >= lex_res.size()) break;
				index++;
			}
			if (index == lex_res.size() && !(lex_res[index - 1].first == 1 && lex_res[index - 1].second == 1)) {
				throw SyntaxError("Ошибка. Код: 1. Комментарий: требуется наличие ключевого слова end.\n", index, lx);
			}
		}
		catch (const SyntaxError& e) {
			cerr << e.getMessage();
			exit(1);
		}
		catch (const LexicalError& e) {
			cerr << e.getMessage();
			exit(1);
		}
		return programNode; // возврат корня дерева
	}

	Node* DESC() {
		Node* descNode = new Node("dim"); // создание корня ветки описания
		try {
			index++; // пропуск "dim"
			while (index < lex_res.size() && lex_res[index].first == 4) { // пока встречается идентификатор
				descNode->children.push_back(new Node(lexResToString(lex_res[index])));
				if (find(described.begin(), described.end(), lexResToString(lex_res[index])) != described.end()) { // при повторном объявлении переменной 
					throw SemanticError("Ошибка. Код: 16. Комментарий: повторное объявление переменной.\n", index, lx);
				}
				described.push_back(lexResToString(lex_res[index])); // добавление переменной в список объявленных
				index++;
				if (index < lex_res.size() && lex_res[index].first == 2 && lex_res[index].second == 13) { // при встрече запятой
					index++; // пропуск
				}
				else if (lex_res[index].first == 4) {
					throw SyntaxError("Ошибка. Код: 2. Комментарий: требуется наличие разделителя ',' между переменными.\n", index, lx);
				}
			}
			if (index < lex_res.size() && lex_res[index].first == 1 && (lex_res[index].second == 3 || lex_res[index].second == 4 || lex_res[index].second == 5)) { // тип переменной
				for (Node* p : descNode->children) {
					p->children.push_back(new Node(lexResToString(lex_res[index]))); // добавление типа для каждой переменной
				}
				if (index < lex_res.size() - 1 && lex_res[index + 1].first == 1 && (lex_res[index + 1].second == 3 || lex_res[index + 1].second == 4 || lex_res[index + 1].second == 5)) { // при указании нескольких типов
					throw SyntaxError("Ошибка. Код: 15. Комментарий: многократное объявление типа данных.\n", index, lx);
				}
				index++;
			}
			else {
				throw SyntaxError("Ошибка. Код: 3. Комментарий: требуется указать тип переменной.\n", index, lx);
			}
		}
		catch (const SyntaxError& e) {
			cerr << e.getMessage();
			exit(1);
		}
		catch (const SemanticError& e) {
			cerr << e.getMessage();
			exit(1);
		}
		return descNode;
	}

	Node* OPERATOR() {
		Node* operatorNode = new Node("operator"); // создание ветки для оператора
		if (index < lex_res.size() - 1 && lex_res[index + 1].first == 2 && lex_res[index + 1].second == 23) { // Присваивание ":="
			operatorNode->children.push_back(ASSIGN());
			index--;
		}
		else if (lex_res[index].first == 1 && lex_res[index].second == 7) { // Условный оператор "if"
			operatorNode->children.push_back(COND());
		}
		else if (lex_res[index].first == 1 && lex_res[index].second == 13) { // Цикл "while"
			operatorNode->children.push_back(WHILE());
		}
		else if (lex_res[index].first == 1 && lex_res[index].second == 9) { // цикл for
			operatorNode->children.push_back(FOR());
		}
		else if (lex_res[index].first == 1 && lex_res[index].second == 14) { // Ввод "readln"
			operatorNode->children.push_back(READ());
		}
		else if (lex_res[index].first == 1 && lex_res[index].second == 15) { // Вывод "writeln"
			operatorNode->children.push_back(WRITE());
			index--;
		}
		else if (lex_res[index].first == 1 && lex_res[index].second == 6) { // Составной оператор "begin ... end"
			operatorNode->children.push_back(COMP());
		}
		return operatorNode;
	}

	Node* ASSIGN() { // обработка присваивания
		Node* assignNode = new Node(":="); // создание векти для присваивания
		assignNode->children.push_back(new Node(lexResToString(lex_res[index])));
		index += 2; // пропуска ":="
		assignNode->children.push_back(EXPR()); // выражение
		return assignNode;
	}

	Node* COND() {
		Node* condNode = new Node("if"); // создание ветки для разбора условного оператора
		try {
			index++; // пропуск "if"
			if (lex_res[index].first == 2 && lex_res[index].second == 16) { // "("
				index++;
				condNode->children.push_back(EXPR()); // разбор условия
				if (index < lex_res.size() && !(lex_res[index].first == 2 && lex_res[index].second == 17)) {
					throw SyntaxError("Ошибка. Код: 4. Комментарий: требуется наличие закрывающей скобки ')'.\n", index, lx);
				}
				index++; // пропуск ")"
				condNode->children.push_back(OPERATOR()); // разбор оператора после условия
				if (condNode->children[condNode->children.size() - 1]->children[condNode->children[condNode->children.size() - 1]->children.size() - 1]->value != "writeln") index--;
				if (index < lex_res.size() - 1 && lex_res[index + 1].first == 1 && lex_res[index + 1].second == 8) { // "else"
					index += 2;
					Node* parseElse = new Node("else");
					condNode->children.push_back(parseElse);
					parseElse->children.push_back(OPERATOR()); // разбор выражения после else
				}
				//else if (index < lex_res.size() && lex_res[index].first == 1 && lex_res[index].second == 8) {
				//	index++;
				//	Node* parseElse = new Node("else");
				//	condNode->children.push_back(parseElse);
				//	parseElse->children.push_back(OPERATOR()); // разбор выражения после else
				//}
			}
			else {
				throw SyntaxError("Ошибка. Код: 5. Комментарий: требуется наличие открывающей скобки '('.\n", index, lx);
			}
		} catch (const SyntaxError& e) {
			cerr << e.getMessage();
			exit(1);
		}
		return condNode;
	}

	Node* WHILE() {
		Node* whileNode = new Node("while"); // создание ветки для разбора условного цикла
		try {
			index++; // пропуск "while"
			if (lex_res[index].first == 2 && lex_res[index].second == 16) { // "("
				index++;
				whileNode->children.push_back(EXPR()); // разбор условия цикла
				if (index < lex_res.size() && !(lex_res[index].first == 2 && lex_res[index].second == 17)) {
					throw SyntaxError("Ошибка. Код: 4. Комментарий: требуется наличие закрывающей скобки ')'.\n", index, lx);
				}
				if ((index < lex_res.size() - 1 && ((lex_res[index + 1].first == 1 && lex_res[index + 1].second == 7) || (lex_res[index + 1].first == 1 && lex_res[index + 1].second == 13) || (lex_res[index + 1].first == 1 && lex_res[index + 1].second == 9) || (lex_res[index + 1].first == 1 && lex_res[index + 1].second == 14) || (lex_res[index + 1].first == 1 && lex_res[index + 1].second == 15) || (lex_res[index + 1].first == 1 && lex_res[index + 1].second == 6)))) {
					whileNode->children.push_back(OPERATOR()); // разбор оператора в цикле
				}
				else if (index < lex_res.size() - 2 && lex_res[index + 2].first == 2 && lex_res[index + 2].second == 23) {
					index++;
					whileNode->children.push_back(OPERATOR()); // разбор оператора в цикле
				}
				else {
					throw SyntaxError("Ошибка. Код: 6. Комментарий: пустое тело цикла.\n", index, lx);
				}
			}
			else {
				throw SyntaxError("Ошибка. Код: 5. Комментарий: требуется наличие открывающей скобки '('.\n", index, lx);
			}
		}
		catch (const SyntaxError& e) {
			cerr << e.getMessage();
			exit(1);
		}
		return whileNode;
	}

	Node* FOR() {
		Node* forNode = new Node("for"); // создание ветки для разбора фиксированного цикла
		try {
			index++;
			if (index < lex_res.size() - 1 && !(lex_res[index + 1].first == 2 && lex_res[index + 1].second == 23)) {
				throw SyntaxError("Ошибка. Код: 7. Комментарий: требуется наличие оператора присвоения.\n", index, lx);
			}
			forNode->children.push_back(ASSIGN());
			if (lex_res[index].first == 1 && lex_res[index].second == 10) {
				index++;
				forNode->children.push_back(EXPR());
				if (lex_res[index].first == 1 && lex_res[index].second == 11) {
					index++;
					Node* stepNode = new Node("step");
					forNode->children.push_back(stepNode);
					stepNode->children.push_back(EXPR());
					forNode->children.push_back(OPERATOR());
					if (lex_res[index].first == 1 && lex_res[index].second == 12) index++;
					else {
						throw SyntaxError("Ошибка. Код: 8. Комментарий: требуется наличие ключевого слова next.\n", index, lx);
					}
				}
				else {
					if (lex_res[index].first == 1 && lex_res[index].second == 12) {
						throw SyntaxError("Ошибка. Код: 9. Комментарий: пустое тело цикла.\n", index, lx);
					}
					forNode->children.push_back(OPERATOR());
					for (Node* p : forNode->children) {
						if (p->value == "operator") {
							if (p->children[p->children.size() - 1]->value == "compound") {
								index--;
								break;
							}
						}
					}
					if (index < lex_res.size() - 1 && lex_res[index + 1].first == 1 && lex_res[index + 1].second == 12) index++;
					else {
						throw SyntaxError("Ошибка. Код: 8. Комментарий: требуется наличие ключевого слова next.\n", index, lx);
					}
				}
			}
			else {
				throw SyntaxError("Ошибка. Код: 10. Комментарий: требуется наличие ключевого слова to\n", index, lx);
			}
		}
		catch (const SyntaxError& e) {
			cerr << e.getMessage();
			exit(1);
		}
		return forNode;
	}

	Node* READ() {
		Node* inputNode = new Node("readln"); // создание ветки для разбора ввода
		try {
			index++; // пропускаем "readln"
			while (index < lex_res.size() && lex_res[index].first == 4) { // пока читается идентификатор
				Node* identifier = new Node(lexResToString(lex_res[index]));
				inputNode->children.push_back(identifier);
				if (index < lex_res.size() - 1 && lex_res[index + 1].first == 2 && lex_res[index + 1].second == 13) {
					index += 2; // пропуск запятой
					if (lex_res[index].first != 4) {
						throw SyntaxError("Ошибка. Код: 11. Комментарий: требуется наличие операнда.\n", index, lx);
					}
				}
				else if (index < lex_res.size() - 2 && lex_res[index + 1].first == 4 && !(lex_res[index + 2].first == 2 && lex_res[index + 2].second == 23)) {
					throw SyntaxError("Ошибка. Код: 2. Комментарий: требуется наличие разделителя ',' между переменными.\n", index, lx);
				}
				else break;
			}
			if (inputNode->children.size() == 0) {
				throw SyntaxError("Ошибка. Код: 11. Комментарий: требуется наличие операнда.\n", index, lx);
			}
		}
		catch (const SyntaxError& e) {
			cerr << e.getMessage();
			exit(1);
		}
		return inputNode;
	}

	Node* WRITE() {
		Node* outputNode = new Node("writeln");
		try {
			index++; // пропуск "writeln"
			while (index < lex_res.size()) {
				outputNode->children.push_back(EXPR());
				if (index < lex_res.size() && lex_res[index].first == 2 && lex_res[index].second == 13) index++; // пропуск запятой
				else {
					if (index < lex_res.size() && (lex_res[index].first == 4 || lex_res[index].first == 3)) {
						throw SyntaxError("Ошибка. Код: 2. Комментарий: требуется наличие разделителя ',' между переменными.\n", index, lx);
					}
					else break;
				}
				if (lex_res[index].first != 4 && lex_res[index].first != 3) {
					throw SyntaxError("Ошибка. Код: 12. Комментарий: требуется наличие выражения.\n", index, lx);
				}
			}
		}
		catch (const SyntaxError& e) {
			cerr << e.getMessage();
			exit(1);
		}
		return outputNode;
	}

	Node* COMP() {
		Node* compoundNode = new Node("compound");
		try {
			compoundNode->children.push_back(new Node("begin"));
			index++; // Пропускаем "begin"
			while (index < lex_res.size() && !(lex_res[index].first == 1 && lex_res[index].second == 1)) {
				compoundNode->children.push_back(OPERATOR());
				index++;
				if (lex_res[index].first == 2 && lex_res[index].second == 14) { // ";"
					index++;
				}
				else if (lex_res[index].first == 1 && lex_res[index].second == 1) break;
				else {
					throw SyntaxError("Ошибка. Код: 1. Комментарий: требуется наличие ключевого слова end.\n", index, lx);
				}
			}
			if (index == lex_res.size() - 1) {
				throw SyntaxError("Ошибка. Код: 1. Комментарий: требуется наличие ключевого слова end.\n", index, lx);
			}
			if (index < lex_res.size()) {
				index++;
			}
			compoundNode->children.push_back(new Node("end"));
		}
		catch (const SyntaxError& e) {
			cerr << e.getMessage();
			exit(1);
		}
		return compoundNode;
	}

	Node* EXPR() {
		Node* exprNode = new Node("expression"); // создание ветки для выражения
		exprNode->children.push_back(OPERAND());
		while (index < lex_res.size() && isRelationOp(lex_res[index]) && !(lex_res[index].first == 2 && lex_res[index].second == 17)) {
			exprNode->children.push_back(new Node(lexResToString(lex_res[index])));
			index++;
			exprNode->children.push_back(OPERAND());
		}
		return exprNode;
	}

	bool isRelationOp(pair<int, int> token) {
		return token.first == 2 && (token.second >= 1 && token.second <= 12);
	}

	Node* OPERAND() {
		Node* operandNode = new Node("operand");
		try {
			if (index < lex_res.size()) {
				// Проверяем на идентификатор
				if (lex_res[index].first == 4) { // Идентификатор
					if (find(described.begin(), described.end(), lexResToString(lex_res[index])) != described.end()) {
						operandNode->children.push_back(new Node(lexResToString(lex_res[index])));
						index++; // Пропускаем идентификатор
					}
					else { // при встрече необъявленной переменной
						throw SemanticError("Ошибка. Код: 13. Комментарий: необъявленная переменная\n", index, lx);
					}
				}
				// Проверяем на число
				else if (lex_res[index].first == 3) { // Число
					operandNode->children.push_back(new Node(lexResToString(lex_res[index])));
					index++; // Пропускаем число
				}
				// Проверяем на логическую константу
				else if (lex_res[index].first == 1 && (lex_res[index].second == 16 || lex_res[index].second == 17)) { // true или false
					operandNode->children.push_back(new Node(lexResToString(lex_res[index])));
					index++; // Пропускаем логическую константу
				}
				// Проверяем на унарную операцию
				else if (lex_res[index].first == 2 && lex_res[index].second == 7) { // Унарная операция "!"
					Node* unaryNode = new Node("!");
					index++; // Пропускаем "!"
					unaryNode->children.push_back(OPERAND()); // Рекурсивно разбираем следующий операнд
					operandNode->children.push_back(unaryNode);
				}
				// Проверяем на выражение в скобках
				else if (lex_res[index].first == 2 && lex_res[index].second == 11) { // "("
					index++; // Пропускаем "("
					operandNode->children.push_back(EXPR()); // Разбираем выражение внутри скобок
					if (index < lex_res.size() && lex_res[index].first == 2 && lex_res[index].second == 12) { // ")"
						index++; // Пропускаем ")"
					}
					else {
						throw SyntaxError("Ошибка. Код: 4. Комментарий: требуется наличие закрывающей скобки ')'.\n", index, lx);
					}
				}
				else {
					throw SyntaxError("Ошибка. Код: 11. Комментарий: требуется наличие операнда.\n", index, lx);
				}
			}
		}
		catch (const SemanticError& e) {
			cerr << e.getMessage();
			exit(1);
		}
		catch (const SyntaxError& e) {
			cerr << e.getMessage();
			exit(1);
		}
		return operandNode;
	}

	string lexResToString(pair<int, int> token) {
		string result = "";
		switch (token.first) {
		case 1:
			result = lx.key_words[token.second - 1];
			break;
		case 2:
			result = lx.delimiters[token.second - 1];
			break;
		case 3:
			result = lx.numbers[token.second - 1];
			break;
		case 4:
			result = lx.identificators[token.second - 1];
			break;
		}
		return result;
	}
};

int main() {
	setlocale(LC_ALL, "");
	Lexer lx;
	cout << count_average << '\n';
	vector <pair <int, int> > answ = lx.lex_analyz(count_average + " ");
	cout << "\n";
	Syntaxer sx(lx);
	Node* r = sx.synt_analyz(answ);
	sx.printTreeByLevels(r);
	return 0;
}