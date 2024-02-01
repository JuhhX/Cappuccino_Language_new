#include "Functions.h"
#include "StringUtilities.h"

//CONVERTE STRING PARA TIPO
ReturnCategory stringToType(string type) {
	
	vector<string> pieces = splitFirst(trim(type), ' ');

	if(pieces.size() > 0)
		type = pieces[0];

	if (type == "Int[]") {
		return INTEGER_VECTOR;
	}
	else if (type == "Double[]") {
		return DOUBLE_VECTOR;
	}
	else if (type == "Char[]") {
		return CHAR_VECTOR;
	}
	else if (type == "Boolean[]") {
		return BOOLEAN_VECTOR;
	}
	else if (type == "String[]") {
		return STRING_VECTOR;
	}
	
	if (type == "Int") {
		if (pieces.size() > 0) {
			if (pieces[1].find("[") != string::npos && pieces[1].find("]") != string::npos) {
				return INTEGER_VECTOR;
			}
		}
			
		return INTEGER;
	}
	else if (type == "Double") {
		if (pieces.size() > 0) {
			if (pieces[1].find("[") != string::npos && pieces[1].find("]") != string::npos) {
				return DOUBLE_VECTOR;
			}
		}
		return DOUBLE;
	}
	else if (type == "Char") {
		if (pieces.size() > 0) {
			if (pieces[1].find("[") != string::npos && pieces[1].find("]") != string::npos) {
				return CHAR_VECTOR;
			}
		}
		return CHAR;
	}
	else if (type == "Boolean") {
		if (pieces.size() > 0) {
			if (pieces[1].find("[") != string::npos && pieces[1].find("]") != string::npos) {
				return BOOLEAN_VECTOR;
			}
		}
		return BOOLEAN;
	}
	else if (type == "String") {
		if (pieces.size() > 0) {
			if (pieces[1].find("[") != string::npos && pieces[1].find("]") != string::npos) {
				return STRING_VECTOR;
			}
		}
		return STRING;
	}
	return VOID;
}

//VERIFICA SE É UMA DECLARAÇÃO DE MÉTODO E REGISTRA SE FOR
bool isFunction(string params) {
	if ((params.find("fn") != string::npos || params.find("function") != string::npos) && params.find("(") != string::npos && params.find(")") != string::npos) {
		Scope new_scope;
		Functions function;
		vector<string> splited;

		function.category = USER_CREATED;

		//Definição da função
		if (startsWith(params, "pub ")) {

			function.accessibility = PUBLIC;

			//Separa o retorno do resto
			params = splitFirst(params, ' ')[1];
			splited = splitFirst(params, ' ');

			function.returns = stringToType(splited[0]);

			//Separa o nome dos parâmetros
			params = splitFirst(params, ' ')[1];
			splited = splitFirst(params, '(');

			//Isola os parâmetros
			function.created_params = splitFirst(splited[1], ')')[0];
			functions_available[splitFirst(splited[0], ' ')[1]] = function;
			new_scope.basic_structures.creating_function = splitFirst(splited[0], ' ')[1];
			
		}
		else {
			function.accessibility = PRIVATE;

			//Separa o retorno do resto
			splited = splitFirst(params, ' ');

			function.returns = stringToType(splited[0]);

			//Serpara fn ou function do nome
			params = splitFirst(splited[1], ' ')[1];

			//Separa nome dos parâmetros
			splited = splitFirst(params, '(');

			//Isola os parâmetros
			function.created_params = splitFirst(splited[1], ')')[0];
			functions_available[splited[0]] = function;
			new_scope.basic_structures.creating_function = splited[0];
		}


		//Cria escopo para armazenar as instruções
		//Abre o escopo
		scopes.pop();
		scopes.push(current_scope);

		
		new_scope.ignore_scope = true;

		//Se true inicia a contagem e bloqueia as outras execuções
		//Senão, deixa o código contar normalmente

		new_scope.curly_braces_count++;

		new_scope.basic_structures.type = USER_FUNCTION;
		scopes.push(new_scope);
		pushVariablesReferences();
		current_scope = scopes.top();

		return true;
	}
	return false;
}

//VERIFICA SE É UMA DECLARAÇÃO DE MÉTODO E REGISTRA SE FOR (PARA CLASSES)
bool isFunction(Functions fn, string name) {

	fn.class_owner = name;

	string params = fn.assignature;
	if ((params.find("fn") != string::npos || params.find("function") != string::npos) && params.find("(") != string::npos && params.find(")") != string::npos) {
		vector<string> splited;

		//Definição da função
		if (startsWith(params, "pub ")) {

			//Separa o retorno do resto
			params = splitFirst(params, ' ')[1];
			splited = splitFirst(params, ' ');

			//Separa o nome dos parâmetros
			params = splitFirst(params, ' ')[1];
			splited = splitFirst(params, '(');

			functions_available[name + "." + splitFirst(splited[0], ' ')[1]] = fn;

		}
		else {

			//Separa o retorno do resto
			splited = splitFirst(params, ' ');

			//Serpara fn ou function do nome
			params = splitFirst(splited[1], ' ')[1];

			//Separa nome dos parâmetros
			splited = splitFirst(params, '(');

			//Isola os parâmetros
			functions_available[name + "." + splited[0]] = fn;
		}

		return true;
	}
	return false;
}