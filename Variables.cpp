#include "Variables.h"

template <typename T>
void updateVariable(string name, T value, ReturnCategory type);
void updateVariable(string name, string value, ReturnCategory type);
void localInterpreter(string line);
bool isVector(string name);
VectorReference getVectorReference(string name);
string getVectorName(string variable_name);

enum MapType
{
	UNIVERSAL,
	LOCAL
};

//RETORNA SE O VETOR REFERENCIA ELE MESMO
bool isSelfVectorReference(string name) {
	VectorReference reference = getVectorReference(name);

	return reference.name == name;
}

//RETORNA A REFERENCIA DE UM VETOR
VectorReference getVectorReference(string name) {
	VectorReference response;

	name = getVectorName(name);

	for (int count = (int)references_vector_references_variables.size() - 1; count > -1; count--) {
		if (references_vector_references_variables[count]->count(name)) {
			response = references_vector_references_variables[count]->find(name)->second;
			break;
		}
	}

	return response;
}

//RETORNA A REFERENCIA DE UM VETOR DE MODO REVERSO
VectorReference getVectorReferenceReverse(string name) {
	VectorReference response;

	name = getVectorName(name);

	for (int count = 0; count < (int)references_vector_references_variables.size(); count++) {
		if (references_vector_references_variables[count]->count(name)) {
			response = references_vector_references_variables[count]->find(name)->second;
			break;
		}
	}

	return response;
}

//ADICIONA REFERÊNCIA AO VETOR
void addVectorReference(string name, MapType type, int size, ReturnCategory category) {
	VectorReference reference;
	reference.type = category;
	reference.size = size;
	reference.name = name;

	if (type == UNIVERSAL) {
		if (category == INTEGER_VECTOR) {
			reference.integers = &universal_integer_variables;
		}
		else if (category == DOUBLE_VECTOR) {
			reference.doubles = &universal_double_variables;
		}
		else if (category == BOOLEAN_VECTOR) {
			reference.booleans = &universal_boolean_variables;
		}
		else if (category == CHAR_VECTOR) {
			reference.chars = &universal_char_variables;
		}
		else if (category == STRING_VECTOR) {
			reference.strings = &universal_string_variables;
		}
	}
	else {
		if (category == INTEGER_VECTOR) {
			reference.integers = &current_scope.integer_variables;
		}
		else if (category == DOUBLE_VECTOR) {
			reference.doubles = &current_scope.double_variables;
		}
		else if (category == BOOLEAN_VECTOR) {
			reference.booleans = &current_scope.boolean_variables;
		}
		else if (category == CHAR_VECTOR) {
			reference.chars = &current_scope.char_variables;
		}
		else if (category == STRING_VECTOR) {
			reference.strings = &current_scope.string_variables;
		}
	}

	current_scope.vector_references_variables[name] = reference;
}

//COPIA REFERENCIA DE VETOR
void addVectorReference(string name, VectorReference reference) {
	current_scope.vector_references_variables[name] = reference;
}

//RESOLVE NOME PARA VETORES
void resolveVector(string * name) {
	if (name->find("[") != string::npos && name->find("]") != string::npos) {
		vector<string> infos = splitFirst(*name, '[');
		infos[0] = trim(infos[0]);
		infos[1] = trim(splitFirst(infos[1], ']')[0]);

		if (isVariable(infos[1])) {
			*name = infos[0] + "[" + getVariableValue(infos[1]) + "]";
		}
		else {
			string resolved = resolveEvaluation(infos[1]);
			*name = infos[0] + "[" + resolved + "]";
		}
	}
}

//VERIFICA SE O NOME DA VARIAVEL É ADEQUADO
bool unsuitableVariableName(string name) {

	vector<string> blocked_symbols = {"(", ")", "{", "}", "+", "-", "*", "/", "%", "="};

	for (string symbol : blocked_symbols) {
		if (name.find(symbol) != string::npos)
			return true;
	}
	
	if (getDataType(name) == INTEGER)
		return true;
	return false;
}

//VERIFICA SE É UMA DECLARAÇÃO DE VARIÁVEL
bool isVariableDeclaration(string params) {

	vector<string> types = { "Int", "Double", "Char", "Boolean", "String"};

	if (classes_models.size() > 0) {
		for (const auto& key : classes_models) {
			types.push_back(key.first);
		}
	}

	bool isDeclaration = false;
	
	for (string type : types) {
		if (startsWith(params, (type + " ")) || startsWith(params, ("uni " + type + " "))) {
			isDeclaration = true;
			break;
		}
	}

	return isDeclaration;
}

//RETORNA O TIPO DA VARIÁVEL QUE ESTA SENDO DECLARADA
ReturnCategory getDeclarationType(string params) {
	ReturnCategory response = VOID;
	vector<string> types = { "Int", "Double", "Char", "Boolean", "String" };
	vector<string> classes;
	int index = 0;

	if (classes_models.size() > 0) {
		for (const auto& key : classes_models) {
			classes.push_back(key.first);
		}
	}

	if (startsWith(params, "uni ")) {
		params = splitFirst(params, ' ')[1];
	}

	for (string type : types) {
		if (startsWith(params, type)) {
			response = static_cast<ReturnCategory>(index);
			break;
		}
		index++;
	}

	index = 0;

	if (response == VOID) {
		for (string type : classes) {
			if (startsWith(params, type)) {
				response = OBJECT;
				break;
			}
			index++;
		}
	}

	return response;
}

//DIVIDE OS DADOS EM UM VETOR
vector<string> getDataInVector(string params) {
	params = params.substr(1, params.length() - 1);

	return split(params, ',');
}

//DECLARA UM VETOR
template <typename T>
bool isVectorDeclare(map<string, T>* variables_vector, map<string, T>* universal, string params, ReturnCategory category) {
	vector<string> variable = splitFirst(trim(splitFirst(params, ' ')[1]), '=');
	variable[0] = trim(variable[0]);
	variable[1] = trim(variable[1]);

	bool create_reference = true;
	MapType type = LOCAL;

	if (startsWith(params, "uni ")) {
		params = splitFirst(params, ' ')[1];
		variable[0] = splitFirst(variable[0], ' ')[1];
		variables_vector = universal;
		type = UNIVERSAL;
	}

	if (variable[0].find("[") != string::npos && variable[0].find("]") != string::npos) {
		//Adiciona os vetores
		vector<string> infos = splitFirst(variable[0], '[');
		infos[0] = trim(infos[0]);
		infos[1] = trim(splitFirst(infos[1], ']')[0]);

		if (isVector(variable[1])) {
			VectorReference new_reference = getVectorReference(variable[1]);
			addVectorReference(infos[0], new_reference);
			create_reference = false;
			variable[1] = getVariableValue(variable[1]);
		}

		interpretLine(variable[1]);

		if (current_scope.current_return.category >= 6 && current_scope.current_return.category <= 11) {
			variable[1] = current_scope.current_return.string_;
		}

		if (variable[1].find("[") != string::npos) {
			variable[1] = splitFirst(splitFirst(variable[1], '[')[1], ']')[0];
		}
		string values_string = trim(variable[1]);
		vector<string> values = split(values_string, ',');

		int size = (infos[1].empty()) ? (int)values.size() : stoi(infos[1]);

		while (values.size() < size) {
			values.push_back("0");
		}

		for (int count = 0; count < size; count++) {
			string name = infos[0] + "[" + to_string(count) + "]";
			values[count] = trim(values[count]);

			if (isVariable(name)) {
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

					bool is_vector = false;
					if (current_scope.current_return.category >= 6) {
						current_scope.current_return.category = static_cast<ReturnCategory>(current_scope.current_return.category - 6);
						is_vector = true;
					}

					if (getVariableType(name) == current_scope.current_return.category) {

						if (current_scope.current_return.category == INTEGER && !is_vector) {
							updateVariable<int>(name, current_scope.current_return.returnValue.integer, current_scope.current_return.category);
						}
						else if (current_scope.current_return.category == DOUBLE && !is_vector) {
							updateVariable<double>(name, current_scope.current_return.returnValue.double_, current_scope.current_return.category);
						}
						else if (current_scope.current_return.category == BOOLEAN && !is_vector) {
							updateVariable<bool>(name, current_scope.current_return.returnValue.boolean, current_scope.current_return.category);
						}
						else if (current_scope.current_return.category == CHAR && !is_vector) {
							updateVariable<char>(name, current_scope.current_return.returnValue.character, current_scope.current_return.category);
						}
						else if (current_scope.current_return.category == INTEGER && is_vector) {
							//Pega o item do vetor e adiciona individualmente na variável
							int position = stoi(trim(splitFirst(splitFirst(name, '[')[1], ']')[0]));
							vector<string> data = getDataInVector(current_scope.current_return.string_);
							updateVariable<int>(name, stoi(data[position]), current_scope.current_return.category);

							//Volta o valor para adicionar o proximo
							current_scope.current_return.category = static_cast<ReturnCategory>(current_scope.current_return.category + 6);
						}
						else if (current_scope.current_return.category == DOUBLE && is_vector) {
							//Pega o item do vetor e adiciona individualmente na variável
							int position = stoi(trim(splitFirst(splitFirst(name, '[')[1], ']')[0]));
							vector<string> data = getDataInVector(current_scope.current_return.string_);
							updateVariable<double>(name, stod(data[position]), current_scope.current_return.category);

							//Volta o valor para adicionar o proximo
							current_scope.current_return.category = static_cast<ReturnCategory>(current_scope.current_return.category + 6);
						}
						else if (current_scope.current_return.category == BOOLEAN && is_vector) {
							//Pega o item do vetor e adiciona individualmente na variável
							int position = stoi(trim(splitFirst(splitFirst(name, '[')[1], ']')[0]));
							vector<string> data = getDataInVector(current_scope.current_return.string_);
							updateVariable<int>(name, stoi(data[position]), current_scope.current_return.category);

							//Volta o valor para adicionar o proximo
							current_scope.current_return.category = static_cast<ReturnCategory>(current_scope.current_return.category + 6);
						}
						else if (current_scope.current_return.category == CHAR && is_vector) {
							//Pega o item do vetor e adiciona individualmente na variável
							int position = stoi(trim(splitFirst(splitFirst(name, '[')[1], ']')[0]));
							vector<string> data = getDataInVector(current_scope.current_return.string_);
							updateVariable<int>(name, stoi(data[position]), current_scope.current_return.category);

							//Volta o valor para adicionar o proximo
							current_scope.current_return.category = static_cast<ReturnCategory>(current_scope.current_return.category + 6);
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

			//Atualiza ponteiros para variáveis do escopo
			scopes.pop();
			popVariablesReferences();

			scopes.push(current_scope);
			pushVariablesReferences();
		}


		if(create_reference) addVectorReference(infos[0], type, size, category);

		return true;
	}

	return false;
}

//DECLARA UM VETOR DE STRING
bool isStringVectorDeclare(map<string, string>* variables_vector, map<string, string>* universal, string params, ReturnCategory category) {
	vector<string> variable = splitFirst(trim(splitFirst(params, ' ')[1]), '=');
	variable[0] = trim(variable[0]);
	variable[1] = trim(variable[1]);

	MapType type = LOCAL;

	bool create_reference = true;

	if (startsWith(params, "uni ")) {
		params = splitFirst(params, ' ')[1];
		variable[0] = splitFirst(variable[0], ' ')[1];
		variables_vector = universal;
		type = UNIVERSAL;
	}

	if (variable[0].find("[") != string::npos && variable[0].find("]") != string::npos) {
		//Adiciona os vetores
		vector<string> infos = splitFirst(variable[0], '[');
		infos[0] = trim(infos[0]);
		infos[1] = trim(splitFirst(infos[1], ']')[0]);

		if (isVector(variable[1])) {
			VectorReference new_reference = getVectorReference(variable[1]);
			addVectorReference(infos[0], new_reference);
			create_reference = false;
			variable[1] = getVariableValue(variable[1]);
		}

		interpretLine(variable[1]);

		if (current_scope.current_return.category >= 6 && current_scope.current_return.category <= 11) {
			variable[1] = current_scope.current_return.string_;
		}

		if (variable[1].find("[") != string::npos) {
			variable[1] = splitFirst(splitFirst(variable[1], '[')[1], ']')[0];
		}

		string values_string = trim(variable[1]);
		vector<string> values = split(values_string, ',');

		int size = (infos[1].empty()) ? (int)values.size() : stoi(infos[1]);

		while (values.size() < size) {
			values.push_back("0");
		}

		for (int count = 0; count < size; count++) {
			string name = infos[0] + "[" + to_string(count) + "]";
			values[count] = trim(values[count]);

			if (isVariable(name)) {
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

		if (create_reference) addVectorReference(infos[0], type, size, STRING_VECTOR);

		return true;
	}

	return false;
}

//VERIFICA SE É DECLARAÇÃO PADRÃO COM VETORES
template <typename T>
bool isDefaultVectorDeclare(string variable_name, map<string, T>* variables_vector, ReturnCategory category) {
	if (variable_name.find("[") != string::npos && variable_name.find("]") != string::npos) {
		//Adiciona os vetores
		vector<string> infos = splitFirst(variable_name, '[');
		infos[0] = trim(infos[0]);
		infos[1] = trim(splitFirst(infos[1], ']')[0]);

		int size = stoi(infos[1]);

		for (int count = 0; count < size; count++) {
			variables_vector->insert({ (infos[0] + "[" + to_string(count) + "]"), 0 });
		}

		//Adiciona uma referência ao vetor
		addVectorReference(infos[0], LOCAL, size, category);

		return true;
	}

	return false;
}

//RETORNA O NOME DO VETOR
string getVectorName(string variable_name) {
	if(variable_name.find("[") != string::npos)
		return trim(splitFirst(variable_name, '[')[0]);
	return variable_name;
}

//VERIFICA SE É DECLARAÇÃO PADRÃO DE CLASSES COM VETORES
bool isDefaultClassVectorDeclare(string variable_name, map<string, Class>* variables_vector, Class class_clone) {
	if (variable_name.find("[") != string::npos && variable_name.find("]") != string::npos) {
		//Adiciona os vetores
		vector<string> infos = splitFirst(variable_name, '[');
		infos[0] = trim(infos[0]);
		infos[1] = trim(splitFirst(infos[1], ']')[0]);

		int size = stoi(infos[1]);

		for (int count = 0; count < size; count++) {
			variables_vector->insert({ (infos[0] + "[" + to_string(count) + "]"), class_clone });
		}

		//VectorReference reference = { OBJECT_VECTOR, &variables_vector->find(infos[0] + "[0]"), size };
		//current_scope.vector_references_variables[infos[0]] = reference;

		return true;
	}

	return false;
}

//VERIFICA SE É DECLARAÇÃO PADRÃO DE STRING COM VETORES
bool isDefaultVectorStringDeclare(string variable_name, map<string, string>* variables_vector) {
	if (variable_name.find("[") != string::npos && variable_name.find("]") != string::npos) {
		//Adiciona os vetores
		vector<string> infos = splitFirst(variable_name, '[');
		infos[0] = trim(infos[0]);
		infos[1] = trim(splitFirst(infos[1], ']')[0]);

		int size = stoi(infos[1]);

		for (int count = 0; count < size; count++) {
			variables_vector->insert({ (infos[0] + "[" + to_string(count) + "]"), ""});
		}

		addVectorReference(infos[0], LOCAL, size, STRING_VECTOR);

		return true;
	}

	return false;
}

//DECLARA UMA VARIÁVEL
template <typename T>
void declare(map<string, T>* variables_vector, map<string, T>* universal, string params, ReturnCategory category) {
	vector<string> variable = splitFirst(trim(splitFirst(params, ' ')[1]), '=');
	variable[0] = trim(variable[0]);
	variable[1] = trim(variable[1]);

	if (startsWith(params, "uni ")) {
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
			updateVariable(variable[0], getVariableValue(variable[1]), getVariableType(variable[0]));
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
					updateVariable<int>(variable[0], current_scope.current_return.returnValue.integer, current_scope.current_return.category);
				}
				else if (current_scope.current_return.category == DOUBLE) {
					updateVariable<double>(variable[0], current_scope.current_return.returnValue.double_, current_scope.current_return.category);
				}
				else if (current_scope.current_return.category == BOOLEAN) {
					updateVariable<bool>(variable[0], current_scope.current_return.returnValue.boolean, current_scope.current_return.category);
				}
				else if (current_scope.current_return.category == CHAR) {
					updateVariable<char>(variable[0], current_scope.current_return.returnValue.character, current_scope.current_return.category);
				}
			}
			else {
				//ERRO
			}
			current_scope.current_return.category = VOID;
		}
		else {
			string resolved = resolveEvaluation(variable[1]);

			if (getVariableType(variable[0]) == getDataType(resolved) || category != STRING) {
				updateVariable(variable[0], resolved, getVariableType(variable[0]));
			}
			else {
				//ERRO
			}
		}
	}
}

//DECLARA UMA VARIÁVEL TIPO STRING
void declareString(map<string, string>* variables_vector, string params) {
	vector<string> variable = splitFirst(trim(splitFirst(params, ' ')[1]), '=');
	variable[0] = trim(variable[0]);
	variable[1] = trim(variable[1]);

	variables_vector->insert({ variable[0] , ""});

	if (isVariable(variable[1])) {
		if (getVariableType(variable[0]) == getVariableType(variable[1])) {
			updateVariable(variable[0], getVariableValue(variable[1]), getVariableType(variable[0]));
		}
		else {
			//ERRO
		}
	}
	else {
		localInterpreter(variable[1]);

		if (current_scope.current_return.category != VOID) {
			if (getVariableType(variable[0]) == current_scope.current_return.category) {

				if (current_scope.current_return.category == STRING) {
					updateVariable(variable[0], current_scope.current_return.string_, current_scope.current_return.category);
				}

			}
			else {
				//ERRO
			}
		}
		else {
			if (getVariableType(variable[0]) == getDataType(variable[1])) {
				updateVariable(variable[0], variable[1], getVariableType(variable[0]));
			}
			else {
				//ERRO
			}
		}
	}
}

//DECLARA OS METODOS PUBLICOS DA CLASS
void declareClassPublicMethods(ClassModel* model, string name) {
	for (const auto& key : model->functions_available) {
		isFunction(key.second, name);
	}
}

//DECLARA AS VARIAVEIS PUBLICAS DA CLASSE
void declarePublicClassAttributes(ClassModel * model, string name) {
	for (const auto& key : model->pub_integer_variables) {
		declareVariable("Int " + name + "." + key.first + " = " + to_string(key.second));
	}
	for (const auto& key : model->pub_double_variables) {
		declareVariable("Double " + name + "." + key.first + " = " + to_string(key.second));
	}
	for (const auto& key : model->pub_boolean_variables) {
		declareVariable("Boolean " + name + "." + key.first + " = " + to_string(key.second));
	}
	for (const auto& key : model->pub_char_variables) {
		declareVariable("Char " + name + "." + key.first + " = " + to_string(key.second));
	}
	for (const auto& key : model->pub_string_variables) {
		declareVariable("String " + name + "." + key.first + " = \"" + key.second + "\"");
	}
}

//FORMATA DECLARAÇÃO ANTES DE DECLARAR
void declareVariable(string params) {
	ReturnCategory type = getDeclarationType(params);
	string variable_name = "";
	bool declareDefault = !(params.find("=") != string::npos);

	//Adicionar erros
	if (type == INTEGER) {
		if (declareDefault) {
			//Declara variáveis universais
			if (startsWith(params, "uni ")) {
				params = splitFirst(params, ' ')[1];
				variable_name = trim(splitFirst(params, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &universal_integer_variables, INTEGER_VECTOR)) {}
				else {
					universal_integer_variables[variable_name] = 0;
				}
			}
			else {
				//Declara variáveis de escopo com valor padrão
				variable_name = trim(splitFirst(params, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &current_scope.integer_variables, INTEGER_VECTOR)) {}
				else {
					current_scope.integer_variables[variable_name] = 0;
				}
			}
		}
		else {
			//Declara vetor com valor inicializado
			if(isVectorDeclare(&current_scope.integer_variables, &universal_integer_variables, params, INTEGER_VECTOR)){}
			else
				//Declara variavel comum com valor inicializado
				declare<int>(&current_scope.integer_variables, &universal_integer_variables, params, INTEGER);
		}
	}
	else if (type == DOUBLE) {
		if (declareDefault) {
			if (startsWith(params, "uni ")) {
				params = splitFirst(params, ' ')[1];
				variable_name = trim(splitFirst(params, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &universal_double_variables, DOUBLE_VECTOR)) {}
				else {
					universal_double_variables[variable_name] = 0;
				}
			}
			else {
				variable_name = trim(splitFirst(params, ' ')[1]);
				if (isDefaultVectorDeclare(variable_name, &current_scope.double_variables, DOUBLE_VECTOR)) {}
				else {
					current_scope.double_variables[variable_name] = 0;
				}
			}
		}
		else {
			if (isVectorDeclare(&current_scope.double_variables, &universal_double_variables, params, DOUBLE_VECTOR)) {}
			else
				declare<double>(&current_scope.double_variables, &universal_double_variables, params, DOUBLE);
		}
	}
	else if (type == CHAR) {
		if (declareDefault) {
			if (startsWith(params, "uni ")) {
				params = splitFirst(params, ' ')[1];
				variable_name = trim(splitFirst(params, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &universal_char_variables, CHAR_VECTOR)) {}
				else
					universal_char_variables[variable_name] = 0;
			}
			else {
				variable_name = trim(splitFirst(params, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &current_scope.char_variables, CHAR_VECTOR)) {}
				else
					current_scope.char_variables[variable_name] = 0;
			}
		}
		else {
			if (isVectorDeclare(&current_scope.char_variables, &universal_char_variables, params, CHAR_VECTOR)) {}
			else
				declare<char>(&current_scope.char_variables, &universal_char_variables, params, CHAR);
		}
	}
	else if (type == BOOLEAN) {
		if (declareDefault) {
			if (startsWith(params, "uni ")) {
				params = splitFirst(params, ' ')[1];
				variable_name = trim(splitFirst(params, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &universal_boolean_variables, BOOLEAN_VECTOR)) {}
				else
					universal_boolean_variables[variable_name] = 0;
			}
			else {
				variable_name = trim(splitFirst(params, ' ')[1]);

				if (isDefaultVectorDeclare(variable_name, &current_scope.boolean_variables, BOOLEAN_VECTOR)) {}
				else
					current_scope.boolean_variables[variable_name] = 0;
			}
		}
		else {
			if (isVectorDeclare(&current_scope.boolean_variables, &universal_boolean_variables, params, BOOLEAN_VECTOR)) {}
			else
				declare<bool>(&current_scope.boolean_variables, &universal_boolean_variables, params, BOOLEAN);
		}
	}
	else if (type == STRING) {
		if (declareDefault) {
			if (startsWith(params, "uni ")) {
				params = splitFirst(params, ' ')[1];
				variable_name = trim(splitFirst(params, ' ')[1]);

				if (isDefaultVectorStringDeclare(variable_name, &universal_string_variables)) {}
				else
					universal_string_variables[variable_name] = "";
			}
			else {
				variable_name = trim(splitFirst(params, ' ')[1]);

				if (isDefaultVectorStringDeclare(variable_name, &current_scope.string_variables)) {}
				else
					current_scope.string_variables[variable_name] = "";
			}
		}
		else {
			if (isStringVectorDeclare(&current_scope.string_variables, &universal_string_variables, params, STRING)) {}
			else
				declareString(&current_scope.string_variables, params);
		}
	}
	else if (type == OBJECT) {
		if (declareDefault) {
			if (startsWith(params, "uni ")) {
				params = splitFirst(params, ' ')[1];
				vector<string> classes_pieces = splitFirst(params, ' ');
				ClassModel model = classes_models[classes_pieces[0]];
				Class class_;
				class_.variable_name = classes_pieces[1];
				class_.integer_variables = classes_models[classes_pieces[0]].integer_variables;
				class_.double_variables = classes_models[classes_pieces[0]].double_variables;
				class_.char_variables = classes_models[classes_pieces[0]].char_variables;
				class_.boolean_variables = classes_models[classes_pieces[0]].boolean_variables;
				class_.string_variables = classes_models[classes_pieces[0]].string_variables;
				class_.functions_available = classes_models[classes_pieces[0]].functions_available;

				if (isDefaultClassVectorDeclare(trim(classes_pieces[1]), &universal_class_variables, class_)) {}
				else {
					declarePublicClassAttributes(&model, classes_pieces[1]);
					class_.pub_integer_variables_references = &current_scope.integer_variables;
					class_.pub_double_variables_references = &current_scope.double_variables;
					class_.pub_char_variables_references = &current_scope.char_variables;
					class_.pub_boolean_variables_references = &current_scope.boolean_variables;
					class_.pub_string_variables_references = &current_scope.string_variables;

					universal_class_variables[trim(classes_pieces[1])] = class_;
				}
				declareClassPublicMethods(&model, classes_pieces[1]);
			}
			else {
				vector<string> classes_pieces = splitFirst(params, ' ');
				ClassModel model = classes_models[classes_pieces[0]];
				Class class_;
				class_.variable_name = classes_pieces[1];
				class_.integer_variables = classes_models[classes_pieces[0]].integer_variables;
				class_.double_variables = classes_models[classes_pieces[0]].double_variables;
				class_.char_variables = classes_models[classes_pieces[0]].char_variables;
				class_.boolean_variables = classes_models[classes_pieces[0]].boolean_variables;
				class_.string_variables = classes_models[classes_pieces[0]].string_variables;
				class_.functions_available = classes_models[classes_pieces[0]].functions_available;

				if (isDefaultClassVectorDeclare(trim(classes_pieces[1]), &current_scope.class_variables, class_)) {}
				else {
					declarePublicClassAttributes(&model, classes_pieces[1]);

					class_.pub_integer_variables_references = &current_scope.integer_variables;
					class_.pub_double_variables_references = &current_scope.double_variables;
					class_.pub_char_variables_references = &current_scope.char_variables;
					class_.pub_boolean_variables_references = &current_scope.boolean_variables;
					class_.pub_string_variables_references = &current_scope.string_variables;

					current_scope.class_variables[trim(classes_pieces[1])] = class_;
				}
				declareClassPublicMethods(&model, classes_pieces[1]);
			}
		}
	}

	//Atualiza ponteiros para variáveis do escopo
	scopes.pop();
	popVariablesReferences();

	scopes.push(current_scope);
	pushVariablesReferences();
}

//VERIFICA SE A VARIAVEL EXISTE
bool isVariable(string name) {

	resolveVector(&name);

	if (name.empty() || unsuitableVariableName(name)) {
		return false;
	}

	if (universal_integer_variables.count(name)) return true;
	else if (universal_double_variables.count(name)) return true;
	else if (universal_char_variables.count(name)) return true;
	else if (universal_boolean_variables.count(name)) return true;
	else if (universal_string_variables.count(name)) return true;

	if (current_scope.basic_structures.type != VOID_STRUCUTRE) {
		for (int count = (int)references_integer_variables.size() - 1; count > -1 ; count--) {

			map<string, int> integer_variables = *references_integer_variables[count];
			map<string, double> double_variables = *references_double_variables[count];
			map<string, bool> boolean_variables = *references_boolean_variables[count];
			map<string, char> char_variables = *references_char_variables[count];
			map<string, string> string_variables = *references_string_variables[count];

			if (integer_variables.count(name)) {
				return true;
			}
			else if (double_variables.count(name)) {
				return true;
			}
			else if (char_variables.count(name)) {
				return true;
			}
			else if (boolean_variables.count(name)) {
				return true;
			}
			else if (string_variables.count(name)) {
				return true;
			} 

		}
	}
	else {
		if (current_scope.integer_variables.count(name)) return true;
		else if (current_scope.double_variables.count(name)) return true;
		else if (current_scope.char_variables.count(name)) return true;
		else if (current_scope.boolean_variables.count(name)) return true;
		else if (current_scope.string_variables.count(name)) return true;
	}

	if (isVector(name)) return true;

	return false;
}

//VERIFICA SE A VARIAVEL EXISTE
bool isVariableInReferences(string name) {

	if (name.empty() || unsuitableVariableName(name)) {
		return false;
	}

	if (universal_integer_variables.count(name)) return true;
	else if (universal_double_variables.count(name)) return true;
	else if (universal_char_variables.count(name)) return true;
	else if (universal_boolean_variables.count(name)) return true;
	else if (universal_string_variables.count(name)) return true;

	for (int count = (int)references_integer_variables.size() - 1; count > -1; count--) {

		map<string, int> integer_variables = *references_integer_variables[count];
		map<string, double> double_variables = *references_double_variables[count];
		map<string, bool> boolean_variables = *references_boolean_variables[count];
		map<string, char> char_variables = *references_char_variables[count];
		map<string, string> string_variables = *references_string_variables[count];

		if (integer_variables.count(name)) {
			return true;
		}
		else if (double_variables.count(name)) {
			return true;
		}
		else if (char_variables.count(name)) {
			return true;
		}
		else if (boolean_variables.count(name)) {
			return true;
		}
		else if (string_variables.count(name)) {
			return true;
		}

	}
	return false;
}

//RETORNA SE UMA VARIÁVEL É UM VETOR
bool isVector(string name) {

	name = getVectorName(name);

	//Se não achar com nome especifico vetor[2]
	for (int count = (int)references_vector_references_variables.size() - 1; count > -1; count--) {
		if (references_vector_references_variables[count]->count(name)) {
			return true;
		}
	}

	return false;
}

//OBTEM O VETOR EM GERAL
string getVectorValue(string name) {
	string response = "[";
	int vector_position = 0;
	int specific_index = -1;

	if (!isSelfVectorReference(name)) {
		VectorReference reference = getVectorReference(name);
		if (name.find("[") != string::npos && name.find("]") != string::npos) {
			specific_index = stoi(splitFirst(splitFirst(name, '[')[1], ']')[0]);
		}
		name = reference.name;
	}

	bool found = false;

	for (int count = (int)references_vector_references_variables.size() - 1; count > -1; count--) {
		if (references_vector_references_variables[count]->count(name)) {

			VectorReference reference = references_vector_references_variables[count]->find(name)->second;

			for (int x = 0; x < reference.size; x++) {
				if (reference.type == INTEGER_VECTOR &&reference.integers->count(name + "[" + to_string(x) + "]")) {
					if (specific_index == x) {
						response = to_string(reference.integers->find(name + "[" + to_string(x) + "]")->second);
						break;
					}
					else {
						response += to_string(reference.integers->find(name + "[" + to_string(x) + "]")->second) + ",";
					}
					found = true;
				}
				else if (reference.type == DOUBLE_VECTOR && reference.doubles->count(name + "[" + to_string(x) + "]")) {
					if (specific_index == x) {
						response = to_string(reference.doubles->find(name + "[" + to_string(x) + "]")->second);
						break;
					}
					else {
						response += to_string(reference.doubles->find(name + "[" + to_string(x) + "]")->second) + ",";
					}
					found = true;
				}
				else if (reference.type == BOOLEAN_VECTOR && reference.booleans->count(name + "[" + to_string(x) + "]")) {
					if (specific_index == x) {
						response = to_string(reference.booleans->find(name + "[" + to_string(x) + "]")->second);
						break;
					}
					else {
						response += to_string(reference.booleans->find(name + "[" + to_string(x) + "]")->second) + ",";
					}
					found = true;
				}
				else if (reference.type == CHAR_VECTOR && reference.chars->count(name + "[" + to_string(x) + "]")) {
					if (specific_index == x) {
						response = to_string(reference.chars->find(name + "[" + to_string(x) + "]")->second);
						break;
					}
					else {
						response += to_string(reference.chars->find(name + "[" + to_string(x) + "]")->second) + ",";
					}
					found = true;
				}
				else if (reference.type == STRING_VECTOR && reference.strings->count(name + "[" + to_string(x) + "]")) {
					if (specific_index == x) {
						response = reference.strings->find(name + "[" + to_string(x) + "]")->second;
						break;
					}
					else {
						response += reference.strings->find(name + "[" + to_string(x) + "]")->second + ",";
					}
					found = true;
				}
			}

			if(found) break;
		}
	}

	if (endsWith(response, ",")) {
		response = response.substr(0, response.length() - 1) + "]";
	}


	return response;
}

//RETORNA O VALOR DA VARIAVEL COMO STRING
string getVariableValue(string name) {
	
	resolveVector(&name);

	string response = "";
	if (name.find('[') != string::npos && name.find(']') == string::npos)
		return "0";

 	if (isVector(name)) {
		response = getVectorValue(name);
	}

	if (response == "[") {
		response = "";
	}

	if (universal_integer_variables.count(name)) {
		response = to_string(universal_integer_variables[name]);
	}
	else if (universal_double_variables.count(name)) {
		response = to_string(universal_double_variables[name]);
	}
	else if (universal_char_variables.count(name)) {
		response = to_string(universal_char_variables[name]);
	}
	else if (universal_boolean_variables.count(name)) {
		response = to_string(universal_boolean_variables[name]);
	}
	else if (universal_string_variables.count(name)) {
		response = universal_string_variables[name];
	}

	if (current_scope.basic_structures.type != VOID_STRUCUTRE && response.empty()) {
		for (int count = (int) references_integer_variables.size() - 1; count > -1; count--) {
			if (references_integer_variables[count]->count(name)) {
				map<string, int> integer_variables = *references_integer_variables[count];
				response = to_string(integer_variables[name]);
				break;
			}
			else if (references_double_variables[count]->count(name)) {
				map<string, double> double_variables = *references_double_variables[count];
				response = to_string(double_variables[name]);
				break;
			}
			else if (references_char_variables[count]->count(name)) {
				map<string, char> char_variables = *references_char_variables[count];
				response = to_string(char_variables[name]);
				break;
			}
			else if (references_boolean_variables[count]->count(name)) {
				map<string, bool> boolean_variables = *references_boolean_variables[count];
				response = to_string(boolean_variables[name]);
				break;
			}
			else if (references_string_variables[count]->count(name)) {
				map<string, string> string_variables = *references_string_variables[count];
				response = string_variables[name];
				break;
			}
		}
	}
	else {
		if (current_scope.integer_variables.count(name)) {
			response = to_string(current_scope.integer_variables[name]);
		}
		else if (current_scope.double_variables.count(name)){
			response = to_string(current_scope.double_variables[name]);
		}
		else if (current_scope.char_variables.count(name)){
			response = to_string(current_scope.char_variables[name]);
		}
		else if (current_scope.boolean_variables.count(name)){
			response = to_string(current_scope.boolean_variables[name]);
		}
		else if (current_scope.string_variables.count(name)){
			response = current_scope.string_variables[name];
		}
	}

	return response;

}

//OBTEM O TIPO DA VARIAVEL
ReturnCategory getVariableType(string name) {

	if (isVector(name)) {
		VectorReference reference;
		if (name.find('[') != string::npos) {
			reference = getVectorReference(trim(splitFirst(name, '[')[0]));
			name = reference.name + "[" + (splitFirst(name, '[')[1]);
		}
		else {
			reference = getVectorReference(name);
		}
	}

	resolveVector(&name);

	ReturnCategory returned = VOID;

	if (current_scope.integer_variables.count(name) || universal_integer_variables.count(name))
		returned = INTEGER;
	else if (current_scope.double_variables.count(name) || universal_double_variables.count(name))
		returned = DOUBLE;
	else if (current_scope.string_variables.count(name) || universal_char_variables.count(name))
		returned = STRING;
	else if (current_scope.boolean_variables.count(name) || universal_boolean_variables.count(name))
		returned = BOOLEAN;
	else if (current_scope.char_variables.count(name) || universal_string_variables.count(name))
		returned = CHAR;

	if (current_scope.basic_structures.type != VOID_STRUCUTRE) {
		for (int count = (int)references_integer_variables.size() - 1; count > -1; count--) {

			map<string, int> integer_variables = *references_integer_variables[count];
			map<string, double> double_variables = *references_double_variables[count];
			map<string, bool> boolean_variables = *references_boolean_variables[count];
			map<string, char> char_variables = *references_char_variables[count];
			map<string, string> string_variables = *references_string_variables[count];

			if (integer_variables.count(name)) {
				returned = INTEGER;
			}
			else if (double_variables.count(name)) {
				returned = DOUBLE;
			}
			else if (char_variables.count(name)) {
				returned = CHAR;
			}
			else if (boolean_variables.count(name)) {
				returned = BOOLEAN;
			}
			else if (string_variables.count(name)) {
				returned = STRING;
			}

		}
	}

	if (returned == VOID) {
		string vector_name = getVectorName(name);
		for (int count = (int)references_vector_references_variables.size() - 1; count > -1; count--) {
			if (references_vector_references_variables[count]->count(vector_name)) {
				if (name.find("[") != string::npos) {
					returned = static_cast<ReturnCategory>(references_vector_references_variables[count]->find(vector_name)->second.type - 6);
				}
				else {
					returned = references_vector_references_variables[count]->find(vector_name)->second.type;
				}
				break;
			}
		}
	}

	return returned;
}

//OBTEM O TIPO DE DADO
ReturnCategory getDataType(string s) {
	ReturnCategory returned = VOID;
	s = split(s, ";")[0];

	if (startsWith(s, "\"") && endsWith(s, "\""))
		returned = STRING;
	else if (s == "true" || s == "false")
		returned = BOOLEAN;
	else if (startsWith(s, "\'") && endsWith(s, "\'"))
		returned = CHAR;
	else {
		bool digit = true;
		int point_count = 0;

		for (char c : s) {
			if (c == '.') {
				point_count++;
				continue;
			}

			if (c == '-') {
				continue;
			}

			if (!isdigit(c)) {
				digit = false;
				break;
			}
		}

		if (point_count == 1 && digit)
			returned = DOUBLE;
		else if (digit)
			returned = INTEGER;
	}

	return returned;
}

//INTERPRETADOR LOCAL PARA VARIAVEIS
void localInterpreter(string line) {

	string before_parenthesis = "";
	string read_in_line = "";

	bool is_reading = false;

	int parenthesis_count = 0;

	for (char c : line) {

		//Encontra parenteses
		if (c == '(' && !is_reading) {
			before_parenthesis = read_in_line;
			read_in_line = "";
			is_reading = true;
			parenthesis_count++;
		}
		else if (c == ')') {
			parenthesis_count--;

			if (is_reading && parenthesis_count != 0) {
				read_in_line += ")";
			}

			if (parenthesis_count == 0) {
				if (functions_available[before_parenthesis].category == NATIVE)
					current_scope.current_return = functions_available[before_parenthesis].function(read_in_line);
				else
					current_scope.current_return = execute_custom(functions_available[before_parenthesis], read_in_line);
			}

		}
		else {
			if (c == '(') {
				parenthesis_count++;
			}
			read_in_line += c;
		}
	}


}

//ATUALIZA VALOR DE VARIAVEL
void updateVariable(string name, string value, ReturnCategory type) {

	if (isVector(name)) {
		if (!isSelfVectorReference(trim(splitFirst(name, '[')[0]))) {
			VectorReference reference = getVectorReference(name);
			name = reference.name + "[" + (splitFirst(name, '[')[1]);
		}
	}

	resolveVector(&name);

	if (type == INTEGER) {
		if (universal_integer_variables.count(name)) {
			universal_integer_variables[name] = stoi(resolveEvaluation(value));
		}
		else {
			if (current_scope.basic_structures.type != VOID_STRUCUTRE) {
				for (int count = (int)references_integer_variables.size() - 1; count > -1; count--) {
					map<string, int> integer_variables = *references_integer_variables[count];
					if (integer_variables.count(name)) {
						references_integer_variables[count]->find(name)->second = stoi(resolveEvaluation(value));
						break;
					}
				}
			}
			else
				current_scope.integer_variables[name] = stoi(resolveEvaluation(value));
		}
	}
	else if (type == DOUBLE) {
		if (universal_double_variables.count(name)) {
			universal_double_variables[name] = stod(resolveEvaluation(value));
		}
		else {
			if (current_scope.basic_structures.type != VOID_STRUCUTRE) {
				for (int count = (int)references_double_variables.size() - 1; count > -1; count--) {
					map<string, double> double_variables = *references_double_variables[count];
					if (double_variables.count(name)) {
						references_double_variables[count]->find(name)->second = stod(resolveEvaluation(value));
						break;
					}
				}
			}
			else
				current_scope.double_variables[name] = stod(resolveEvaluation(value));
		}
	}
	else if (type == BOOLEAN) {
		value = resolveEvaluation(value);
		if (universal_boolean_variables.count(name)) {
			universal_boolean_variables[name] = !(value == "0" || value == "false" || value == "");
		}
		else {
			if (current_scope.basic_structures.type != VOID_STRUCUTRE) {
				for (int count = (int)references_boolean_variables.size() - 1; count > -1; count--) {
					map<string, bool> boolean_variables = *references_boolean_variables[count];
					if (boolean_variables.count(name)) {
						references_boolean_variables[count]->find(name)->second = !(value == "0" || value == "false" || value == "");
						break;
					}
				}
			}
			else
				current_scope.boolean_variables[name] = !(value == "0" || value == "false" || value == "");
		}
	}
	else if (type == CHAR) {
		if (universal_char_variables.count(name)) {
			universal_char_variables[name] = extractStringValue(value)[0];
		}
		else {
			if (current_scope.basic_structures.type != VOID_STRUCUTRE) {
				for (int count = (int)references_char_variables.size() - 1; count > -1; count--) {
					map<string, char> char_variables = *references_char_variables[count];
					if (char_variables.count(name)) {
						references_char_variables[count]->find(name)->second = extractStringValue(value)[0];
						break;
					}
				}
			}
			else
				current_scope.char_variables[name] = extractStringValue(value)[0];
		}
	}
	else if (type == STRING) {
		if (universal_string_variables.count(name)) {
			universal_string_variables[name] = resolveAssignmentString(extractStringValue(value));
		}
		else {
			if (current_scope.basic_structures.type != VOID_STRUCUTRE) {
				for (int count = (int)references_string_variables.size() - 1; count > -1; count--) {
					map<string, string> string_variables = *references_string_variables[count];
					if (string_variables.count(name)) {
						references_string_variables[count]->find(name)->second = resolveAssignmentString(extractStringValue(value));
						break;
					}
				}
			}
			else
				current_scope.string_variables[name] = resolveAssignmentString(extractStringValue(value));
		}
	}

	//Atualiza referencia de ponteiros
	scopes.pop();
	popVariablesReferences();

	scopes.push(current_scope);
	pushVariablesReferences();
}


//ATUALIZA VALOR DE VARIAVEL
void updateVectorReference(string name, string value, ReturnCategory type) {

	if (type >= 6 && type != VOID) {
		type = static_cast<ReturnCategory>(type - 6);
	}

	resolveVector(&name);

	if (!isSelfVectorReference(name)) {
		VectorReference reference = getVectorReference(name);
		if (name.find("[") != string::npos && name.find("]") != string::npos) {
			name = reference.name + "[" + splitFirst(name, '[')[1];
		}
		else {
			name = reference.name;
		}
	}
	VectorReference reference = getVectorReferenceReverse(name);

	if (type == INTEGER) {
		reference.integers->find(name)->second = stoi(resolveEvaluation(value));
	}
	else if (type == DOUBLE) {
		reference.doubles->find(name)->second = stod(resolveEvaluation(value));
	}
	else if (type == BOOLEAN) {
		reference.booleans->find(name)->second = !(value == "0" || value == "false" || value == "");
	}
	else if (type == CHAR) {
		reference.chars->find(name)->second = extractStringValue(value)[0];
	}
	else if (type == STRING) {
		reference.strings->find(name)->second = resolveAssignmentString(extractStringValue(value));
	}

	//Atualiza referencia de ponteiros
	scopes.pop();
	popVariablesReferences();

	scopes.push(current_scope);
	pushVariablesReferences();
}


//ATUALIZA VALOR DE VARIAVEL
template <typename T>
void updateVariable(string name, T value, ReturnCategory type) {
	if (type == INTEGER) {
		if (universal_integer_variables.count(name)) {
			universal_integer_variables[name] = (int)value;
		}
		else {
			if (current_scope.basic_structures.type != VOID_STRUCUTRE) {
				for (int count = (int)references_integer_variables.size() - 1; count > -1; count--) {
					map<string, int> integer_variables = *references_integer_variables[count];
					if (integer_variables.count(name)) {
						references_integer_variables[count]->find(name)->second = stoi( resolveEvaluation( to_string(value) ) ) ;
						break;
					}
				}
			}
			else
				current_scope.integer_variables[name] = (int)value;
		}
	}
	else if (type == DOUBLE) {
		if (universal_integer_variables.count(name)) {
			universal_double_variables[name] = (double)value;
		}
		else {
			if (current_scope.basic_structures.type != VOID_STRUCUTRE) {
				for (int count = (int)references_double_variables.size() - 1; count > -1; count--) {
					map<string, double> double_variables = *references_double_variables[count];
					if (double_variables.count(name)) {
						references_double_variables[count]->find(name)->second = stod(resolveEvaluation(to_string(value)));
						break;
					}
				}
			}
			else
				current_scope.double_variables[name] = (double)value;
		}
	}
	else if (type == BOOLEAN) {
		if (universal_integer_variables.count(name)) {
			universal_boolean_variables[name] = (bool)value;
		}
		else {
			if (current_scope.basic_structures.type != VOID_STRUCUTRE) {
				for (int count = (int)references_boolean_variables.size() - 1; count > -1; count--) {
					map<string, bool> boolean_variables = *references_boolean_variables[count];
					if (boolean_variables.count(name)) {
						references_boolean_variables[count]->find(name)->second = (bool)value;
						break;
					}
				}
			}
			else
				current_scope.boolean_variables[name] = (bool)value;
		}
	}
	else if (type == CHAR) {
		if (universal_integer_variables.count(name)) {
			universal_char_variables[name] = (char)value;
		}
		else {
			if (current_scope.basic_structures.type != VOID_STRUCUTRE) {
				for (int count = (int)references_char_variables.size() - 1; count > -1; count--) {
					map<string, char> char_variables = *references_char_variables[count];
					if (char_variables.count(name)) {
						references_char_variables[count]->find(name)->second = (char)value;
						break;
					}
				}
			}
			else
				current_scope.char_variables[name] = (char)value;
		}
	}
}

//VERIFICA SE É UMA ATUALIZAÇÃO DA VARIAVEL
bool isVariableUpdate(string params) {

	vector<string> variable = {"", ""};
	
	if (startsWith(params, "++")) {
		string variable_name = trim(params.substr(2, params.length() - 2));
		variable[0] = variable_name;
		variable[1] = variable_name + " + 1";
	}
	else if (startsWith(params, "--")) {
		string variable_name = trim(params.substr(2, params.length() - 2));
		variable[0] = variable_name;
		variable[1] = variable_name + " - 1";
	}

	if (endsWith(params, "++")) {
		string variable_name = trim(params.substr(0, params.length() - 2));
		CommonRequestUpdateVariable request = { variable_name, INCREASE };
		variable[0] = variable_name;
		variable[1] = variable_name + " + 0";

		current_scope.commonRequestUpdateVariable.push(request);
	}
	else if (endsWith(params + ";", "-- ")) {
		string variable_name = trim(params.substr(0, params.length() - 2));
		CommonRequestUpdateVariable request = { variable_name, DECREASE };

		variable[0] = variable_name;
		variable[1] = variable_name + " + 0";
		current_scope.commonRequestUpdateVariable.push(request);
	}

	if (params.find("=") != string::npos) {
		variable = split(params, '=');
		variable[0] = trim(variable[0]);
		variable[1] = trim(variable[1]);
	}
	
	if (isVariable(variable[0])) {
		if (isVariable(variable[1])) {
			if (isVector(variable[0]) && !isSelfVectorReference(variable[0])) {
				updateVectorReference(variable[0], getVariableValue(variable[1]), getVariableType(variable[0]));
			}
			else {
				if (getVariableType(variable[0]) == getVariableType(variable[1])) {
					updateVariable(variable[0], getVariableValue(variable[1]), getVariableType(variable[0]));
				}
				else {
					//ERRO
				}
			}
		}
		else {
			localInterpreter(variable[1]);

			if (current_scope.current_return.category != VOID) {
				if (getVariableType(variable[0]) == current_scope.current_return.category) {
					
					if (current_scope.current_return.category == STRING) {
						updateVariable(variable[0], current_scope.current_return.string_, current_scope.current_return.category);
					}
					else if (current_scope.current_return.category == INTEGER) {
						updateVariable<int>(variable[0], current_scope.current_return.returnValue.integer, current_scope.current_return.category);
					}
					else if(current_scope.current_return.category == DOUBLE) {
						updateVariable<double>(variable[0], current_scope.current_return.returnValue.double_, current_scope.current_return.category);
					}
					else if(current_scope.current_return.category == BOOLEAN) {
						updateVariable<bool>(variable[0], current_scope.current_return.returnValue.boolean, current_scope.current_return.category);
					}
					else if(current_scope.current_return.category == CHAR) {
						updateVariable<char>(variable[0], current_scope.current_return.returnValue.character, current_scope.current_return.category);
					}
					else if (current_scope.current_return.category == INTEGER_VECTOR) {
						if (isVector(variable[0])) {
							interpretLine(variable[1]);
							string formated_name = "Int " + variable[0] + "[] = " + current_scope.current_return.string_;
							isVectorDeclare(&current_scope.integer_variables, &universal_integer_variables, formated_name, INTEGER_VECTOR);
						}
					}
					else if (current_scope.current_return.category == DOUBLE_VECTOR) {
						if (isVector(variable[0])) {
							string formated_name = "Int " + variable[0] + "[] = " + current_scope.current_return.string_;
							isVectorDeclare(&current_scope.double_variables, &universal_double_variables, formated_name, DOUBLE_VECTOR);
						}
					}
					else if (current_scope.current_return.category == BOOLEAN_VECTOR) {
						if (isVector(variable[0])) {
							string formated_name = "Int " + variable[0] + "[] = " + current_scope.current_return.string_;
							isVectorDeclare(&current_scope.boolean_variables, &universal_boolean_variables, formated_name, BOOLEAN_VECTOR);
						}
					}
					else if (current_scope.current_return.category == CHAR_VECTOR) {
						if (isVector(variable[0])) {
							string formated_name = "Int " + variable[0] + "[] = " + current_scope.current_return.string_;
							isVectorDeclare(&current_scope.char_variables, &universal_char_variables, formated_name, CHAR_VECTOR);
						}
					}
					else if (current_scope.current_return.category == STRING_VECTOR) {
						if (isVector(variable[0])) {
							string formated_name = "Int " + variable[0] + "[] = " + current_scope.current_return.string_;
							isStringVectorDeclare(&current_scope.string_variables, &universal_string_variables, formated_name, STRING_VECTOR);
						}
					}
				}
				else {
					//ERRO
				}
			}
			else {

				string resolved = resolveEvaluation(variable[1]);
				if (isVector(variable[0]) && !isSelfVectorReference(variable[0])) {
					updateVectorReference(variable[0], resolved, getVariableType(variable[0]));
				}
				else {
					if (getVariableType(variable[0]) == getDataType(resolved) || current_scope.current_return.category != STRING) {
						updateVariable(variable[0], resolved, getVariableType(variable[0]));
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
