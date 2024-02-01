#include "String.h"

void parametherInterpreter(string * paramether) {
	interpretLine(*paramether);

	if (current_scope.current_return.category != VOID) {
		if (current_scope.current_return.category == INTEGER) {
			*paramether = to_string(current_scope.current_return.returnValue.integer);
		}
		else if (current_scope.current_return.category == DOUBLE) {
			*paramether = to_string(current_scope.current_return.returnValue.double_);
		}
		else if (current_scope.current_return.category == BOOLEAN) {
			*paramether = to_string(current_scope.current_return.returnValue.boolean);
		}
		else if (current_scope.current_return.category == CHAR) {
			*paramether = to_string(current_scope.current_return.returnValue.character);
		}
		else if (current_scope.current_return.category == STRING) {
			*paramether = extractStringValue(current_scope.current_return.string_);
		}
		else {
			//ERRO
		}
	}
}

//RETORNA O CARACTERE NA POSIÇÃO
ReturnType charAt(string params) {
	ReturnType response = { "", CHAR, 0 };

	if (params.empty()) {
		//ERRO
	}
	else {
		vector<string> paramethers = split(params, ',');
		paramethers[0] = trim(paramethers[0]);
		paramethers[1] = trim(paramethers[1]);

		if (paramethers.size() == 2) {

			if (isVariable(paramethers[0])) {
				if (getVariableType(paramethers[0]) == STRING) {
					paramethers[0] = getVariableValue(paramethers[0]);
				}
				else {
					//ERRO
				}
			}
			if (isVariable(paramethers[1])) {
				if (getVariableType(paramethers[0]) != STRING) {
					paramethers[1] = getVariableValue(paramethers[1]);
				}
				else {
					//ERRO
				}
			}

			int index = stoi(paramethers[1]);
			int current_index = 0;

			for (char c : paramethers[0]) {

				if (current_index == index) {
					response.returnValue.character = c;
					break;
				}

				current_index++;
			}

		}
		else {
			//ERRO
		}
	}

	return response;
}

//VERIFICA SE UMA STRING CONTEM A STRING
ReturnType contains(string params) {
	ReturnType response = { "", BOOLEAN, 0 };

	if (params.empty()) {
		//ERRO
	}
	else {
		vector<string> paramethers = split(params, ',');
		paramethers[0] = trim(paramethers[0]);
		paramethers[1] = trim(paramethers[1]);

		if (paramethers.size() == 2) {

			if (isVariable(paramethers[0])) {
				if (getVariableType(paramethers[0]) == STRING) {
					paramethers[0] = getVariableValue(paramethers[0]);
				}
				else {
					//ERRO
				}
			}
			else {
				parametherInterpreter(&paramethers[0]);
			}

			if (isVariable(paramethers[1])) {
				if (getVariableType(paramethers[1]) == STRING) {
					paramethers[1] = getVariableValue(paramethers[1]);
				}
				else {
					//ERRO
				}
			}
			else {
				parametherInterpreter(&paramethers[1]);
			}

			response.returnValue.boolean = paramethers[0].find(paramethers[1]) != string::npos;

		}
		else {
			//ERRO
		}
	}

	return response;
}

//CONCATENA STRINGS
ReturnType concat(string params) {
	ReturnType response = { "", STRING, 0 };

	if (params.empty()) {
		//ERRO
	}
	else {
		vector<string> paramethers = split(params, ',');

		if (paramethers.size() >= 2) {

			for (int x = 0; x < paramethers.size(); x++) {
				paramethers[x] = trim(paramethers[x]);
				if (isVariable(paramethers[x])) {
					paramethers[x] = getVariableValue(paramethers[x]);
				}
				else if (getDataType(paramethers[x]) == STRING) {
					paramethers[x] = extractStringValue(paramethers[x]);
				}
				else {
					parametherInterpreter(&paramethers[x]);
				}

				response.string_ += paramethers[x];
			}

		}
		else {
			//ERRO
		}
	}

	return response;
}

//VERIFICA SE UMA STRING TERMINA COM
ReturnType endsWith_(string params) {
	ReturnType response = { "", BOOLEAN, 0 };

	if (params.empty()) {
		//ERRO
	}
	else {
		vector<string> paramethers = split(params, ',');
		paramethers[0] = trim(paramethers[0]);
		paramethers[1] = trim(paramethers[1]);

		if (paramethers.size() == 2) {

			if (isVariable(paramethers[0])) {
				if (getVariableType(paramethers[0]) == STRING) {
					paramethers[0] = getVariableValue(paramethers[0]);
				}
				else {
					//ERRO
				}
			}
			else {
				parametherInterpreter(&paramethers[0]);
			}

			if (isVariable(paramethers[1])) {
				if (getVariableType(paramethers[1]) == STRING) {
					paramethers[1] = getVariableValue(paramethers[1]);
				}
				else {
					//ERRO
				}
			}
			else {
				parametherInterpreter(&paramethers[1]);
			}

			response.returnValue.boolean = endsWith(paramethers[0], paramethers[1]);

		}
		else {
			//ERRO
		}
	}

	return response;
}

//VERIFICA SE UMA STRING COMEÇA COM
ReturnType startsWith_(string params) {
	ReturnType response = { "", BOOLEAN, 0 };

	if (params.empty()) {
		//ERRO
	}
	else {
		vector<string> paramethers = split(params, ',');
		paramethers[0] = trim(paramethers[0]);
		paramethers[1] = trim(paramethers[1]);

		if (paramethers.size() == 2) {

			if (isVariable(paramethers[0])) {
				if (getVariableType(paramethers[0]) == STRING) {
					paramethers[0] = getVariableValue(paramethers[0]);
				}
				else {
					//ERRO
				}
			}
			else {
				parametherInterpreter(&paramethers[0]);
			}

			if (isVariable(paramethers[1])) {
				if (getVariableType(paramethers[1]) == STRING) {
					paramethers[1] = getVariableValue(paramethers[1]);
				}
				else {
					//ERRO
				}
			}
			else {
				parametherInterpreter(&paramethers[1]);
			}

			response.returnValue.boolean = startsWith(paramethers[0], paramethers[1]);

		}
		else {
			//ERRO
		}
	}

	return response;
}

//CONVERTE STRING PARA LETRAS MINUSCULAS
ReturnType toUpper(string params) {
	ReturnType response = { "", BOOLEAN, 0 };

	if (params.empty()) {
		//ERRO
	}
	else {
		if (getDataType(params) == STRING) {
			params = extractStringValue(params);
		}

		if (isVariable(params)) {
			if (getVariableType(params) == STRING) {
				params = getVariableValue(params);
			}
			else {
				//ERRO
			}
		}
		else {
			parametherInterpreter(&params);
		}

		string result = "";
		for (char c : params) {
			result += toupper(c);
		}
		response.string_ = result;
	}

	return response;
}

//CONVERTE STRING PARA LETRAS MINUSCULAS
ReturnType toLower(string params) {
	ReturnType response = { "", BOOLEAN, 0 };

	if (params.empty()) {
		//ERRO
	}
	else {
		if (getDataType(params) == STRING) {
			params = extractStringValue(params);
		}

		if (isVariable(params)) {
			if (getVariableType(params) == STRING) {
				params = getVariableValue(params);
			}
			else {
				//ERRO
			}
		}
		else {
			parametherInterpreter(&params);
		}

		string result = "";
		for (char c : params) {
			result += tolower(c);
		}
		response.string_ = result;
	}

	return response;
}

//REMOVE ESPAÇOS EM BRANCO NO INICIO E FIM DA STRING
ReturnType trim_(string params) {
	ReturnType response = { "", STRING, 0 };

	if (params.empty()) {
		//ERRO
	}
	else {
		if (isVariable(params)) {
			if (getVariableType(params) == STRING) {
				params = getVariableValue(params);
			}
			else {
				//ERRO
			}
		}
		else {
			parametherInterpreter(&params);
		}

		response.string_ = trim(params);
	}

	return response;
}

//VERIFICA IGUALDADE
ReturnType equals(string params) {

	ReturnType response = { "", BOOLEAN, 0 };

	if (params.empty()) {
		//ERRO
	}
	else {
		vector<string> paramethers = split(params, ',');
		paramethers[0] = trim(paramethers[0]);
		paramethers[1] = trim(paramethers[1]);

		if (paramethers.size() == 2) {

			if (isVariable(paramethers[0])) {
				if (getVariableType(paramethers[0]) == STRING) {
					paramethers[0] = getVariableValue(paramethers[0]);
				}
				else {
					//ERRO
				}
			}
			else {
				parametherInterpreter(&paramethers[0]);
			}

			if (isVariable(paramethers[1])) {
				if (getVariableType(paramethers[1]) == STRING) {
					paramethers[1] = getVariableValue(paramethers[1]);
				}
				else {
					//ERRO
				}
			}
			else {
				parametherInterpreter(&paramethers[1]);
			}

			response.returnValue.boolean = paramethers[0] == paramethers[1];

		}
		else {
			//ERRO
		}
	}

	return response;

}

//REGISTRA TODOS OS MÉTODOS NA LISTA
void registerAll_String(map<string, Functions>* functions_available) {
	functions_available->insert({ "charAt", {NATIVE, PUBLIC, CHAR, charAt} });
	functions_available->insert({ "concat", {NATIVE, PUBLIC, STRING, concat} });
	functions_available->insert({ "contains", {NATIVE, PUBLIC, STRING, contains} });
	functions_available->insert({ "startsWith", {NATIVE, PUBLIC, STRING, startsWith_} });
	functions_available->insert({ "endsWith", {NATIVE, PUBLIC, STRING, endsWith_} });
	functions_available->insert({ "toUpper", {NATIVE, PUBLIC, STRING, toUpper} });
	functions_available->insert({ "toLower", {NATIVE, PUBLIC, STRING, toLower} });
	functions_available->insert({ "trim", {NATIVE, PUBLIC, STRING, trim_} });
	functions_available->insert({ "equals", {NATIVE, PUBLIC, STRING, equals} });
}

//REGISTRA NA LISTA DE MÉTODOS
void registerMethod_String(map<string, Functions>* functions_available, string name) {
	if (name == "charAt") functions_available->insert({ "charAt", {NATIVE, PUBLIC, CHAR, charAt} });
	else if (name == "concat") functions_available->insert({ "concat", {NATIVE, PUBLIC, STRING, concat} });
	else if (name == "contains") functions_available->insert({ "contains", {NATIVE, PUBLIC, STRING, contains} });
	else if (name == "startsWith") functions_available->insert({ "startsWith", {NATIVE, PUBLIC, STRING, startsWith_} });
	else if (name == "endsWith") functions_available->insert({ "endsWith", {NATIVE, PUBLIC, STRING, endsWith_} });
	else if (name == "toUpper") functions_available->insert({ "toUpper", {NATIVE, PUBLIC, STRING, toUpper} });
	else if (name == "toLower") functions_available->insert({ "toLower", {NATIVE, PUBLIC, STRING, toLower} });
	else if (name == "trim") functions_available->insert({ "trim", {NATIVE, PUBLIC, STRING, trim_} });
	else if (name == "equals") functions_available->insert({ "equals", {NATIVE, PUBLIC, STRING, equals} });
}

//VERIFICA SE EXISTE NA LISTA DE METODOS
bool inList_String(string name) {

	vector<string> list = { "charAt", "concat", "contains", "startsWith", "endsWith", "toUpper", "toLower", "trim", "equals"};

	for (string method : list) {
		if (name == method) {
			return true;
		}
	}

	return false;
}