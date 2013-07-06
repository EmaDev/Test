// Test.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"


enum TOKEN_TYPE { 
	TOKEN_INLINE_COMMENT, 
	TOKEN_COMMENT, 
	TOKEN_WHITE_SPACE, 
	TOKEN_NEW_LINE, 
	TOKEN_FLOAT,
	TOKEN_INTEGER,
	TOKEN_STRING,
	TOKEN_IF,
	TOKEN_END,
	TOKEN_IDENTITY,
	TOKEN_EQUAL,
	TOKEN_NOT_EQUAL,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_MULTIPLY,
	TOKEN_DIVIDE,
	TOKEN_LESS_EQ,
	TOKEN_GREATER_EQ,
	TOKEN_LESS,
	TOKEN_GREATER,
	TOKEN_START_PAREN,
	TOKEN_END_PAREN,
	TOKEN_START_SCOPE,
	TOKEN_END_SCOPE,
	TOKEN_TERMINATOR,
};

map<TOKEN_TYPE, string> tokenRegs;

void tokenRegsInit()
{
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_INLINE_COMMENT, "[/]{2}.*[\\n]{0,1}") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_COMMENT, "/\\*([^*]|[\\r\\n]|(\\*+([^*/]|[\\r\\n])))*\\*+/") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_WHITE_SPACE, "[ ]+") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_NEW_LINE, "[\\r\\n]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_FLOAT, "[0-9]+\\.[0-9]+") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_INTEGER, "[0-9]+") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_STRING, "\\\".*?\\\"") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_IF, "[i][f]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_END, "[e][n][d]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_IDENTITY, "[a-zA-Z_][a-zA-Z0-9_]*") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_EQUAL, "[=]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_NOT_EQUAL, "[!][=]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_PLUS, "[+]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_MINUS, "[-]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_MULTIPLY, "[*]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_DIVIDE, "[/]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_LESS_EQ, "[<][=]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_GREATER_EQ, "[>][=]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_LESS, "[<]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_GREATER, "[>]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_START_PAREN, "[(]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_END_PAREN, "[)]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_START_SCOPE, "[{]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_END_SCOPE, "[}]") );
	tokenRegs.insert( pair<TOKEN_TYPE, string>(TOKEN_TERMINATOR, "[;]") );
}

class Token
{
public:
	Token() {}
	Token(TOKEN_TYPE type, string value, unsigned col, unsigned row)
	{
		this->_type = type;
		this->_value = value;
		this->_col = col;
		this->_row = row;
	}
	Token(Token& t)
	{
		this->_type = t._type;
		this->_value = t._value;
		this->_col = t._col;
		this->_row = t._row;
	}
	string value() { return this->_value; }
	TOKEN_TYPE type() { return this->_type; }
	unsigned col() { return this->_col; }
	unsigned row() { return this->_row; }
	bool operator == (const TOKEN_TYPE & t)
	{
		return this->_type == t;
	}
	bool operator != (const TOKEN_TYPE & t)
	{
		return !(*this==t);
	}
private:
	string _value;
	unsigned _col;
	unsigned _row;
	TOKEN_TYPE _type;
};

vector<Token> tokens;


enum PARSE_STATUS { 
	PARSE_OK, 
	PARSE_UNEXPECTED_CHARACTERS
};

class ParseStatus
{
public:
	ParseStatus() {}
	ParseStatus(PARSE_STATUS status, string message = "", unsigned col = 0, unsigned row = 0)
	{
		this->_status = status;
		this->_message = message;
		this->_col = col;
		this->_row = row;
	}
	ParseStatus(ParseStatus & clone)
	{
		this->_status = clone._status;
		this->_message = clone._message;
		this->_col = clone._col;
		this->_row = clone._row;
	}
	~ParseStatus() {}
	PARSE_STATUS status() { return this->_status; }
	string message() { return this->_message; }
	unsigned col() { return this->_col; }
	unsigned row() { return this->_row; }
private:
	PARSE_STATUS _status;
	string _message;
	unsigned _col;
	unsigned _row;
};

ParseStatus run(string instrunctions);

ParseStatus parse(string instrunctions);

void error(string message);
void getSym();
bool is(TOKEN_TYPE s);
bool accept(TOKEN_TYPE s);
bool expect(TOKEN_TYPE s);
void factor();
void term();
void expression();
void condition();
void statement();
void syntaxCheck();

class SyntaxError
{
public:
	SyntaxError() {}
	SyntaxError(string message, Token token)
	{
		this->_message = message;
		this->_token = token;
	}
	SyntaxError(SyntaxError & clone)
	{
		this->_message = clone._message;
		this->_token = clone._token;
	}
	~SyntaxError() {}
	string message() { return this->_message; }
	Token token() { return this->_token; }
private:
	string _message;
	Token _token;
};


vector<SyntaxError> syntaxErrors;

int _tmain(int argc, _TCHAR* argv[])
{
	string instructions = "";
	bool exit;
	ParseStatus status;
	do
	{
		exit = false;
		getline(cin, instructions);
		//instructions = "var prova = \"ciao bello\"; print(prova);";
		//instructions = "prova = 3;";
		switch ((status = run(instructions)).status())
		{
		case PARSE_OK:
			for(unsigned i = 0; i < tokens.size(); i++)
			{
				cout<<"Token n."<<tokens[i].type()<<" : "<<tokens[i].value()<<endl;
			}
			cout<<endl;
			syntaxErrors.clear();
			syntaxCheck();
			if(syntaxErrors.size() > 0)
			{
				for(unsigned i = 0; i < syntaxErrors.size(); i++)
				{
					cout<<"Syntax error : "<<syntaxErrors[i].message()<<" at col ("<<syntaxErrors[i].token().col()<<") row("<<syntaxErrors[i].token().row()<<") near : "<<syntaxErrors[i].token().value()<<endl;
				}
				cout<<endl;
			}
			else
			{
				cout<<"Syntax correct"<<endl;
			}
			break;
		case PARSE_UNEXPECTED_CHARACTERS:
			cout<<"Parse error n."<<status.status()<<" : "<<status.message()<<" at col ("<<status.col()<<") row("<<status.row()<<")"<<endl;
			exit = true;
			break;
		}
		system("PAUSE");
		system("CLS");
	}while(!exit);

	return 0;
}

ParseStatus run(string instrunctions)
{
	return parse(instrunctions);
}

ParseStatus parse(string instrunctions)
{
	regex r;
	
	map<TOKEN_TYPE, string>::iterator tokenReg;
	bool foundToken;
	string valueTmp;
	unsigned col, row;

	col = 0;
	row = 0;

	tokenRegsInit();

	while(instrunctions != "")
	{
		foundToken = false;
		for (tokenReg = tokenRegs.begin(); tokenReg!=tokenRegs.end(); ++tokenReg )
		{
			r = regex(tokenReg->second);
			smatch m;
			regex_search(instrunctions, m, r);
			if(!m.empty() && m.position() == 0)
			{
				col += m.length();
				if(tokenReg->first == TOKEN_NEW_LINE)
				{
					col = 0;
					row += 1;
				}
				tokens.push_back( Token((TOKEN_TYPE)tokenReg->first, m[0], col, row) );
				foundToken = true;
				instrunctions = m.suffix();
			}
		}
		if(!foundToken)
			return ParseStatus(PARSE_UNEXPECTED_CHARACTERS, "Unexpected characters", col, row);
	}

	return ParseStatus(PARSE_OK);
}


unsigned indexSym;
Token sym;

void error(string message)
{
	syntaxErrors.push_back( SyntaxError(message, sym) );
}

void getSym()
{
	if(indexSym < tokens.size())
		sym = tokens[indexSym++];
}

bool is(TOKEN_TYPE s)
{
	if(sym == TOKEN_WHITE_SPACE || sym == TOKEN_NEW_LINE)
	{
		getSym();
		return is(s);
	}
	else
		return sym == s;
}

bool accept(TOKEN_TYPE s)
{
	if(is(s))
	{
		getSym();
		return true;
	}
	return false;
}

bool expect(TOKEN_TYPE s)
{
	if(accept(s))
	{
		return true;
	}
	error("expect: unexpected symbol");
	return false;
}

void factor()
{
	if(accept(TOKEN_IDENTITY)) {}
	else if(accept(TOKEN_INTEGER) || accept(TOKEN_FLOAT)) {}
	else if(accept(TOKEN_START_PAREN))
	{
		expression();
		expect(TOKEN_END_PAREN);
	}
	else
	{
		error("factor: syntax error");
        getSym();
	}
}

void term()
{
	factor();
	while(is(TOKEN_MULTIPLY) || is(TOKEN_DIVIDE))
	{
		getSym();
		factor();
	}
}

void expression()
{
	if(is(TOKEN_PLUS) || is(TOKEN_MINUS))
        getSym();
	term();
	while(is(TOKEN_PLUS) || is(TOKEN_MINUS))
	{
		getSym();
		term();
	}
}

void condition()
{
	expression();
	if(is(TOKEN_EQUAL) || is(TOKEN_NOT_EQUAL) || is(TOKEN_LESS) || is(TOKEN_GREATER) || is(TOKEN_LESS_EQ) || is(TOKEN_GREATER_EQ) )
	{
		getSym();
		expression();
	}
	else
	{
		error("condition: invalid operator");
        getSym();
	}
}

void statement()
{
	if(accept(TOKEN_IDENTITY))
	{
		expect(TOKEN_EQUAL);
		expression();
	}
	else if(accept(TOKEN_START_SCOPE))
	{
		do
		{
			statement();
		}while (accept(TOKEN_TERMINATOR));
		expect(TOKEN_END_SCOPE);
	}
	else if(accept(TOKEN_IF))
	{
		condition();
		statement();
	}
	else
	{
        error("statement: syntax error");
        getSym();
    }
}

void syntaxCheck()
{
	indexSym = 0;
	getSym();
	statement();
}