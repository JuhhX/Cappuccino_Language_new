#include "Classes.h"

//RETORNA O TIPO DA VARIÁVEL QUE ESTA SENDO DECLARADA
ReturnCategory getDeclarationTypeInClass(string params) {
	ReturnCategory response = VOID;
	vector<string> types = { "Int", "Double", "Char", "Boolean", "String" };
	int index = 0;

	if (startsWith(params, "pub ")) {
		params = splitFirst(params, ' ')[1];
	}

	for (string type : types) {
		if (startsWith(params, type)) {
			response = static_cast<ReturnCategory>(index);
			break;
		}
		index++;
	}
	return response;
}

//DECLARA UM VETOR
template <typename T>
bool isVectorClassDeclare(map<string, T>* variables_vector, map<string, T>* universal, string params, ReturnCategory category) {
	vector<string> variable = splitFirst(trim(splitFirst(params, ' ')[1]), '=');
	variable[0] = trim(variable[0]);
	variable[1] = trim(variable[1]);

	if (startsWith(params, "pub ")) {
		params = splitFirst(params, ' ')[1];
		variable[0] = splitFirst(variable[0], ' ')[1];
		variables_vector = universal;
	}

	if (variable[0].find("[") != string::npos && variable[0].find("]") != string::npos) {
		//Adiciona os vetores
		vector<string> infos = splitFirst(variable[0], '[');
		infos[0] = trim(infos[0]);
		infos[1] = trim(splitFirst(infos[1], ']')[0]);

		string values_string = trim(splitFirst(splitFirst(variable[1], '[')[1], ']')[0]);
		vector<string> values = split(values_string, ',');

		int size = stoi(infos[1]);

		while (values.size() < size) {
			values.push_back("0");
		}

		for (int count = 0; count < size; count++) {
			string name = infos[0] + "[" + to_string(count) + "]";
			values[count] = trim(values[count]);

			if (isVariable(variable[0])) {
				//ERRO
			}
			else {
				variables_vector->insert({ name , 0 });
			}

			if (isVariable(values[count])) {
				if (getVariableType(name) == getVariableType(values[count])) {
					updateVariable(name, getVariableValue(values[count]), getVariableType(name));
				}
				else {
					//ERRO
				}
			}
			else {
				localInterpreter(values[count]);

				if (current_scope.current_return.category != VOID) {
					if (getVariableType(name) == current_scope.current_return.category) {

						if (current_scope.current_return.category == INTEGER) {
							updateVariable<int>(name, current_scope.current_return.returnValue.integer, current_scope.current_return.category);
						}
						else if (current_scope.current_return.category == DOUBLE) {
							updateVariable<double>(name, current_scope.current_return.returnValue.double_, current_scope.current_return.category);
						}
						else if (current_scope.current_return.category == BOOLEAN) {
							updateVariable<bool>(name, current_scope.current_return.returnValue.boolean, current_scope.current_return.category);
						}
						else if (current_scope.current_return.category == CHAR) {
							updateVariable<char>(name, current_scope.current_return.returnValue.character, current_scope.current_return.category);
						}
					}
					else {
						//ERRO
					}
				}
				else {
					string resolved = resolveEvaluation(values[count]);
					if (getVariableType(name) == getDataType(resolved) || category != STRING) {
						updateVariable(name, resolved, getVariableType(name));
					}
					else {
						//ERRO
					}
				}
			}

		}

		return true;
	}

	return false;
}

//DECLARA UM VETOR DE STRING
bool isStringClassVectorDeclare(map<string, string>* variables_vector, map<string, string>* universal, string params, ReturnCategory category) {
	vector<string> variable = splitFirst(trim(splitFirst(params, ' ')[1]), '=');
	variable[0] = trim(variable[0]);
	variable[1] = trim(variable[1]);

	if (startsWith(params, "pub ")) {
		params = splitFirst(params, ' ')[1];
		variable[0] = splitFirst(variable[0], ' ')[1];
		variables_vector = universal;
	}

	if (variable[0].find("[") != string::npos && variable[0].find("]") != string::npos) {
		//Adiciona os vetores
		vector<string> infos = splitFirst(variable[0], '[');
		infos[0] = trim(infos[0]);
		infos[1] = trim(splitFirst(infos[1], ']')[0]);

		string values_string = trim(splitFirst(splitFirst(variable[1], '[')[1], ']')[0]);
		vector<string> values = split(values_string, ',');

		int size = stoi(infos[1]);

		while (values.size() < size) {
			values.push_back("0");
		}

		for (int count = 0; count < size; count++) {
			string name = infos[0] + "[" + to_string(count) + "]";
			values[count] = trim(values[count]);

			if (isVariable(variable[0])) {
				//ERRO
			}
			else {
				variables_vector->insert({ name , "" });
			}

			if (isVariable(values[count])) {
				if (getVariableType(name) == getVariableType(values[count])) {
					updateVariable(name, getVariableValue(values[count]), getVariableType(name));
				}
				else {
					//ERRO
				}
			}
			else {
				localInterpreter(values[count]);

				if (current_scope.current_return.category != VOID) {
					if (getVariableType(name) == current_scope.current_return.category) {

						if (current_scope.current_return.category == STRING) {
							updateVariable(name, current_scope.current_return.string_, current_scope.current_return.category);
						}
					}
					else {
						//ERRO
					}
				}
				else {
					string resolved = resolveAssignmentString(values[count]);
					if (getVariableType(name) == getDataType(resolved)) {
						updateVariable(name, resolved, getVariableType(name));
					}
					else {
						//ERRO
					}
				}
			}

		}

		return true;
	}

	return false;
}

//DECLARA UMA VARIÁVEL
template <typename T>
void declareInClass(map<string, T>* variables_vector, map<string, T>* universal, string params, ReturnCategory category) {
	vector<string> variable = splitFirst(trim(splitFirst(params, ' ')[1]), '=');
	variable[0] = trim(variable[0]);
	variable[1] = trim(variable[1]);

	if (startsWith(params, "pub ")) {
		params = splitFirst(params, ' ')[1];
		variable[0] = splitFirst(variable[0], ' ')[1];
		variables_vector = universal;
	}

	if (isVariable(variable[0])) {
		//ERRO
	}
	else {
		variables_vector->insert({ variable[0] , 0 });
	}

	if (isVariable(variable[1])) {
		if (getVariableType(variable[0]) == getVariableType(variable[1])) {
			variables_vector->insert_or_assign(variable[0], stoi(getVariableValue(variable[1])));
		}
		else {
			//ERRO
		}
	}
	else {
		localInterpreter(variable[1]);

		if (current_scope.current_return.category != VOID) {
			if (getVariableType(variable[0]) == current_scope.current_return.category) {

				if (current_scope.current_return.category == INTEGER) {
					variables_vector->insert_or_assign(variable[0], current_scope.current_return.returnValue.integer);
				}
				else if (current_scope.current_return.category == DOUBLE) {
					variables_vector->insert_or_assign(variable[0], (int)current_scope.current_return.returnValue.double_);
				}
				else if (current_scope.current_return.category == BOOLEAN) {
					variables_vector->insert_or_assign(variable[0], current_scope.current_return.returnValue.boolean);
				}
				else if (current_scope.current_return.category == CHAR) {
					variables_vector->insert_or_assign(variable[0], current_scope.current_return.returnValue.character);
				}
			}
			else {
				//ERRO
			}
		}
		else {
			string resolved = resolveEvaluation(variable[1]);
			if (getVariableType(variable[0]) == getDataType(resolved) || category != STRING) {
				variables_vector->insert_or_assign(variable[0], stoi(resolved));
			}
			else {
				//ERRO
			}
		}
	}
}

//OBTEM O TIPO DA VARIAVEL NA CLASSE
ReturnCategory getVariableTypeInClass(string name) {

	resolveVector(&name);

	ReturnCategory returned = VOID;

	if (creating_class_model.integer_variables.count(name) || creating_class_model.pub_integer_variables.count(name))
		returned = INTEGER;
	else if (creating_class_model.double_variables.count(name) || creating_class_model.pub_double_variables.count(name))
		returned = DOUBLE;
	else if (creating_class_model.string_variables.count(name) || creating_class_model.pub_string_variables.count(name))
		returned = STRING;
	else if (creating_class_model.boolean_variables.count(name) || creating_class_model.pub_boolean_variables.count(name))
		returned = BOOLEAN;
	else if (creating_class_model.char_variables.count(name) || creating_class_model.pub_char_variables.count(name))
		returned = CHAR;

	return returned;
}

//DECLARA UMA VARIÁVEL TIPO STRING
void declareStringInClass(map<string, string>* variables_vector, map<string, string>* universal, string params) {
	vector<string> variable = splitFirst(trim(splitFirst(params, ' ')[1]), '=');
	variable[0] = trim(variable[0]);
	variable[1] = trim(variable[1]);

	if (startsWith(params, "pub ")) {
		params = splitFirst(params, ' ')[1];
		variable[0] = splitFirst(variable[0], ' ')[1];
		variables_vector = universal;
	}

	if (isVariable(variable[0])) {
		//ERRO
	}
	else {
		variables_vector->insert({ variable[0] , ""});
	}

	if (isVariable(variable[1])) {
		if (getVariableTypeInClass(variable[0]) == getVariableTypeInClass(variable[1])) {
			variables_vector->insert_or_assign(variable[0], getVariableValue(variable[1]));
		}
		else {
			//ERRO
		}
	}
	else {
		localInterpreter(variable[1]);

		if (current_scope.current_return.category != VOID) {
			if (getVariableTypeInClass(variable[0]) == current_scope.current_return.category) {

				if (current_scope.current_return.category == STRING) {
					variables_vector->insert_or_assign(variable[0], current_scope.current_return.string_);
				}

			}
			else {
				//ERRO
			}
		}
		else {
			if (getVariableTypeInClass(variable[0]) == getDataType(variable[1])) {
				variables_vector->insert_or_assign(variable[0], extractStringValue(variable[1]));
			}
			else {
				//ERRO
			}
		}
	}
}

//VERIFICA SE É UMA DECLARAÇÃO DE CLASSE E REGISTRA SE FOR
bool isTargetClass(string params) {
    if (startsWith(params, "class ")) {

        ClassModel class_model;
        creating_class_model = class_model;
		creating_class_model.typename_ = trim(splitFirst(splitFirst(params, ' ')[1], '{')[0]);

        Scope new_scope;

        //Abre escopo para interpretar a classe
        scopes.pop();
        scopes.push(current_scope);

        new_scope.ignore_scope = true;

        new_scope.curly_braces_count++;

        new_scope.basic_structures.type = USER_CLASS;
        scopes.push(new_scope);
        pushVariablesReferences();
        current_scope = scopes.top();

        return true;
    }

    return false;
}

//VERIFICA SE É UMA VARIAVEL DA CLASSE
bool isClassVariable(string line) {
	ReturnCategory type = getDeclarationTypeInClass(line);
	string variable_name = "";
	bool declareDefault = !(line.find("=") != string::npos);

	if (line.find("fn") != string::npos || line.find("function") != string::npos)
		return false;

	//Adicionar erros
	if (type == INTEGER) {
		if (declareDefault) {
			if (startsWith(line, "pub ")) {
				line = splitFirst(line, ' ')[1];
				variable_name = trim(splitFirst(line, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &creating_class_model.pub_integer_variables, INTEGER_VECTOR)) {}
				else {
					creating_class_model.pub_integer_variables[variable_name] = 0;
				}
			}
			else {
				variable_name = trim(splitFirst(line, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &creating_class_model.integer_variables, INTEGER_VECTOR)) {}
				else {
					creating_class_model.integer_variables[variable_name] = 0;
				}
			}
		}
		else {
			if (isVectorClassDeclare(&creating_class_model.integer_variables, &creating_class_model.pub_integer_variables, line, INTEGER)) {}
			else
				declareInClass<int>(&creating_class_model.integer_variables, &creating_class_model.pub_integer_variables, line, INTEGER);
		}
		return true;
	}
	else if (type == DOUBLE) {
		if (declareDefault) {
			if (startsWith(line, "pub ")) {
				line = splitFirst(line, ' ')[1];
				variable_name = trim(splitFirst(line, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &creating_class_model.pub_double_variables, DOUBLE_VECTOR)) {}
				else {
					creating_class_model.pub_double_variables[variable_name] = 0;
				}
			}
			else {
				variable_name = trim(splitFirst(line, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &creating_class_model.double_variables, DOUBLE_VECTOR)) {}
				else {
					creating_class_model.double_variables[variable_name] = 0;
				}
			}
		}
		else {
			if (isVectorClassDeclare(&creating_class_model.double_variables, &creating_class_model.pub_double_variables, line, INTEGER)) {}
			else
				declareInClass<double>(&creating_class_model.double_variables, &creating_class_model.pub_double_variables, line, INTEGER);
		}
		return true;
	}
	else if (type == CHAR) {
		if (declareDefault) {
			if (startsWith(line, "pub ")) {
				line = splitFirst(line, ' ')[1];
				variable_name = trim(splitFirst(line, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &creating_class_model.pub_char_variables, CHAR_VECTOR)) {}
				else {
					creating_class_model.pub_char_variables[variable_name] = 0;
				}
			}
			else {
				variable_name = trim(splitFirst(line, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &creating_class_model.char_variables, CHAR_VECTOR)) {}
				else {
					creating_class_model.char_variables[variable_name] = 0;
				}
			}
		}
		else {
			if (isVectorClassDeclare(&creating_class_model.char_variables, &creating_class_model.pub_char_variables, line, INTEGER)) {}
			else
				declareInClass<char>(&creating_class_model.char_variables, &creating_class_model.pub_char_variables, line, INTEGER);
		}
		return true;
	}
	else if (type == BOOLEAN) {
		if (declareDefault) {
			if (startsWith(line, "pub ")) {
				line = splitFirst(line, ' ')[1];
				variable_name = trim(splitFirst(line, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &creating_class_model.boolean_variables, BOOLEAN_VECTOR)) {}
				else {
					creating_class_model.boolean_variables[variable_name] = 0;
				}
			}
			else {
				variable_name = trim(splitFirst(line, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &creating_class_model.boolean_variables, BOOLEAN_VECTOR)) {}
				else {
					creating_class_model.boolean_variables[variable_name] = 0;
				}
			}
		}
		else {
			if (isVectorClassDeclare(&creating_class_model.boolean_variables, &creating_class_model.boolean_variables, line, INTEGER)) {}
			else
				declareInClass<bool>(&creating_class_model.boolean_variables, &creating_class_model.boolean_variables, line, INTEGER);
		}
		return true;
	}
	else if (type == STRING) {
		if (declareDefault) {
			if (startsWith(line, "pub ")) {
				line = splitFirst(line, ' ')[1];
				variable_name = trim(splitFirst(line, ' ')[1]);

				if (isDefaultVectorStringDeclare(variable_name, &creating_class_model.pub_string_variables)) {}
				else {
					creating_class_model.pub_string_variables[variable_name] = "";
				}
			}
			else {
				variable_name = trim(splitFirst(line, ' ')[1]);

				if (isDefaultVectorStringDeclare(variable_name, &creating_class_model.string_variables)) {}
				else {
					creating_class_model.string_variables[variable_name] = "";
				}
			}
		}
		else {
			if (isStringClassVectorDeclare(&creating_class_model.string_variables, &creating_class_model.pub_string_variables, line, STRING)) {}
			else
				declareStringInClass(&creating_class_model.string_variables, &creating_class_model.pub_string_variables, line);
		}
		return true;
	}

    return false;
}

//CARREGA AS VARIAVEIS PARA O ESCOPO ATUAL
void loadClassVariables(string name, Scope* scope) {
	Class class_ = current_scope.class_variables[name];

	//Declara as váriaveis privadas
	for (const auto& key : class_.integer_variables) {
		scope->integer_variables[key.first] = key.second;
	}

	for (const auto& key : class_.double_variables) {
		scope->double_variables[key.first] = key.second;
	}

	for (const auto& key : class_.char_variables) {
		scope->char_variables[key.first] = key.second;
	}

	for (const auto& key : class_.boolean_variables) {
		scope->boolean_variables[key.first] = key.second;
	}

	for (const auto& key : class_.string_variables) {
		scope->string_variables[key.first] = key.second;
	}

	//Declara as váriaveis publicas
	for (const auto& key : *class_.pub_integer_variables_references) {
		string key_name = key.first;
		if (key_name.find(".") != string::npos) {
			//O ideal é o splitlast (ALTERAR NO FUTURO)
			vector<string> pieces = split(key_name, '.');
			if (pieces[0] == name) {
				scope->integer_variables[pieces[1]] = key.second;
			}
		}
	}

	for (const auto& key : *class_.pub_double_variables_references) {
		string key_name = key.first;
		if (key_name.find(".") != string::npos) {
			//O ideal é o splitlast (ALTERAR NO FUTURO)
			vector<string> pieces = split(key_name, '.');
			if (pieces[0] == name) {
				scope->double_variables[pieces[1]] = key.second;
			}
		}
	}

	for (const auto& key : *class_.pub_boolean_variables_references) {
		string key_name = key.first;
		if (key_name.find(".") != string::npos) {
			//O ideal é o splitlast (ALTERAR NO FUTURO)
			vector<string> pieces = split(key_name, '.');
			if (pieces[0] == name) {
				scope->boolean_variables[pieces[1]] = key.second;
			}
		}
	}

	for (const auto& key : *class_.pub_char_variables_references) {
		string key_name = key.first;
		if (key_name.find(".") != string::npos) {
			//O ideal é o splitlast (ALTERAR NO FUTURO)
			vector<string> pieces = split(key_name, '.');
			if (pieces[0] == name) {
				scope->char_variables[pieces[1]] = key.second;
			}
		}
	}

	for (const auto& key : *class_.pub_string_variables_references) {
		string key_name = key.first;
		if (key_name.find(".") != string::npos) {
			//O ideal é o splitlast (ALTERAR NO FUTURO)
			vector<string> pieces = split(key_name, '.');
			if (pieces[0] == name) {
				scope->string_variables[pieces[1]] = key.second;
			}
		}
	}
}

//ATUALIZA AS VARIAVEIS DE UMA CLASSE
void unloadClassVariables(string name) {

	Class* class_ = &scopes.top().class_variables[name];

	//Lê as variáveis do escopo atual
	for (const auto& key : current_scope.integer_variables) {
		if (class_->integer_variables.count(key.first)) {
			//Atualiza variáveis privadas
			class_->integer_variables.insert_or_assign(key.first, key.second);
		}
		else {
			//Atualiza variáveis publicas
			class_->pub_integer_variables_references->insert_or_assign(name + "." + key.first, key.second);
		}
	}

	for (const auto& key : current_scope.double_variables) {
		if (class_->double_variables.count(key.first)) {
			class_->double_variables.insert_or_assign(key.first, key.second);
		}
		else {
			class_->pub_double_variables_references->insert_or_assign(name + "." + key.first, key.second);
		}
	}

	for (const auto& key : current_scope.boolean_variables) {
		if (class_->boolean_variables.count(key.first)) {
			class_->boolean_variables.insert_or_assign(key.first, key.second);
		}
		else {
			class_->pub_boolean_variables_references->insert_or_assign(name + "." + key.first, key.second);
		}
	}

	for (const auto& key : current_scope.char_variables) {
		if (class_->char_variables.count(key.first)) {
			class_->char_variables.insert_or_assign(key.first, key.second);
		}
		else {
			class_->pub_char_variables_references->insert_or_assign(name + "." + key.first, key.second);
		}
	}

	for (const auto& key : current_scope.string_variables) {
		if (class_->string_variables.count(key.first)) {
			class_->string_variables.insert_or_assign(key.first, key.second);
		}
		else {
			if (class_->pub_string_variables_references->count(name + "." + key.first)) {
				class_->pub_string_variables_references->insert_or_assign(name + "." + key.first, key.second);
			}
		}
	}

}