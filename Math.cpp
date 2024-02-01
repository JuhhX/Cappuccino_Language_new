#include "Math.h"

//RETORNA VALOR ABSOLUTO
ReturnType abs_(string params) {
	ReturnType response = { "", VOID, 0 };

	if (isVariable(params)) {
		if (getVariableType(params) == STRING) {
			//ERRO
		}
		else {
			if (getVariableType(params) == DOUBLE) {
				double value = stod(getVariableValue(params));

				response.category = DOUBLE;
				response.returnValue.double_ = (value >= 0) ? value : (value * -1);
			}
			else {
				int value = stoi(getVariableValue(params));

				response.category = INTEGER;
				response.returnValue.integer = (value >= 0) ? value : (value * -1);
			}
		}
	}
	else {
		if (getDataType(params) == INTEGER) {
			int value = stoi(params);

			response.category = INTEGER;
			response.returnValue.integer = (value >= 0) ? value : (value * -1);
		}
		else if (getDataType(params) == DOUBLE) {
			double value = stod(params);

			response.category = DOUBLE;
			response.returnValue.double_ = (value >= 0) ? value : (value * -1);
		}
		else {
			int value = stoi(resolveEvaluation(params));

			response.category = INTEGER;
			response.returnValue.integer = (value >= 0) ? value : (value * -1);
		}
	}

	return response;
}

//ARREDONDA PARA CIMA
ReturnType ceil_(string params) {
	ReturnType response = { "", VOID, 0 };

	if (isVariable(params)) {
		if (getVariableType(params) == STRING) {
			//ERRO
		}
		else {
			if (getVariableType(params) == DOUBLE) {
				double value = stod(getVariableValue(params));
				int integer = (int)value;

				response.category = DOUBLE;
				response.returnValue.double_ = (value - integer >= 0.1) ? (integer + 1) : value;
			}
			else {
				response.category = INTEGER;
				response.returnValue.integer = stoi(getVariableValue(params));
			}
		}
	}
	else {
		if (getDataType(params) == INTEGER) {
			response.category = INTEGER;
			response.returnValue.integer = stoi(params);
		}
		else if (getDataType(params) == DOUBLE) {
			double value = stod(params);
			int integer = (int)value;

			response.category = DOUBLE;
			response.returnValue.double_ = (value - integer >= 0.1) ? (integer + 1) : value;
		}
		else {
			response.category = INTEGER;
			response.returnValue.integer = stoi(resolveEvaluation(params));
		}
	}

	return response;
}

//ARREDONDA PARA BAIXO
ReturnType floor_(string params) {
	ReturnType response = { "", VOID, 0 };

	if (isVariable(params)) {
		if (getVariableType(params) == STRING) {
			//ERRO
		}
		else {
			if (getVariableType(params) == DOUBLE) {
				double value = stod(getVariableValue(params));
				int integer = (int)value;

				response.category = DOUBLE;
				response.returnValue.double_ = (value - integer >= 0.1) ? (integer - 1) : value;
			}
			else {
				response.category = INTEGER;
				response.returnValue.integer = stoi(getVariableValue(params));
			}
		}
	}
	else {
		if (getDataType(params) == INTEGER) {
			response.category = INTEGER;
			response.returnValue.integer = stoi(params);
		}
		else if (getDataType(params) == DOUBLE) {
			double value = stod(params);
			int integer = (int)value;

			response.category = DOUBLE;
			response.returnValue.double_ = (value - integer >= 0.1) ? (integer - 1) : value;
		}
		else {
			response.category = INTEGER;
			response.returnValue.integer = stoi(resolveEvaluation(params));
		}
	}

	return response;
}

//ARREDONDA PARA CIMA OU BAIXO
ReturnType round_(string params) {
	ReturnType response = { "", VOID, 0 };

	if (isVariable(params)) {
		if (getVariableType(params) == STRING) {
			//ERRO
		}
		else {
			if (getVariableType(params) == DOUBLE) {
				double value = stod(getVariableValue(params));
				int integer = (int)value;

				response.category = DOUBLE;
				response.returnValue.double_ = (value - integer >= 0.5) ? (integer + 1) : (value - integer < 0.5) ? (integer - 1) : value;
			}
			else {
				response.category = INTEGER;
				response.returnValue.integer = stoi(getVariableValue(params));
			}
		}
	}
	else {
		if (getDataType(params) == INTEGER) {
			response.category = INTEGER;
			response.returnValue.integer = stoi(params);
		}
		else if (getDataType(params) == DOUBLE) {
			double value = stod(params);
			int integer = (int)value;

			response.category = DOUBLE;
			response.returnValue.double_ = (value - integer >= 0.5) ? (integer + 1) : (value - integer < 0.5) ? (integer - 1) : value;
		}
		else {
			response.category = INTEGER;
			response.returnValue.integer = stoi(resolveEvaluation(params));
		}
	}

	return response;
}

//ELEVA UM NUMERO COM BASE EM OUTRO
ReturnType pow_(string params) {
	ReturnType response = { "", VOID, 0 };

	vector<string> paramethers = splitFirst(params, ',');
	if (paramethers.size() == 2) {
		paramethers[0] = trim(paramethers[0]);
		paramethers[1] = trim(paramethers[1]);

		if (isVariable(paramethers[0])) {
			paramethers[0] = getVariableValue(paramethers[0]);
		}
		if (isVariable(paramethers[1])) {
			paramethers[1] = getVariableValue(paramethers[1]);
		}

		paramethers[0] = resolveEvaluation(paramethers[0]);
		paramethers[1] = resolveEvaluation(paramethers[1]);

		response.category = DOUBLE;
		response.returnValue.double_ = pow(stod(paramethers[0]), stod(paramethers[1]));
	}
	else {
		//ERRO
	}

	return response;
}

//RETORNA A RAÍZ QUADRADA DE UM NUMERO
ReturnType sqrt_(string params) {
	ReturnType response = { "", DOUBLE, 0 };

	if (isVariable(params)) {
		if (getVariableType(params) == STRING) {
			//ERRO
		}
		else {
			double value = stod(getVariableValue(params));
			response.returnValue.double_ = sqrt(value);
		}
	}
	else {
		if (getDataType(params) == STRING) {
			//ERRO
		}
		else {
			double value = stod(resolveEvaluation(params));
			response.returnValue.double_ = sqrt(value);
		}
	}

	return response;
}

//GERA UM NÚMERO ALEATÓRIO
ReturnType random(string params) {
	ReturnType response = { "", INTEGER, 0 };
	int generated = 0;

	srand((int)time(nullptr));

	if (params.empty()) {
		generated = rand();
	}
	else {
		vector<string> paramethers = split(params, ',');
		paramethers[0] = trim(paramethers[0]);
		paramethers[1] = trim(paramethers[1]);

		if (paramethers.size() == 2) {

			if (isVariable(paramethers[0]))
				paramethers[0] = getVariableValue(paramethers[0]);
			if(isVariable(paramethers[1]))
				paramethers[1] = getVariableValue(paramethers[1]);

			int min = stoi(paramethers[0]), max = stoi(paramethers[1]);
			generated = rand() % (max - min) + min;
		}
		else {
			//ERRO
		}
	}

	response.returnValue.integer = generated;

	return response;
}

//RETORNA O LOGARÍTIMO DE UM NÚMERI]O
ReturnType log_(string params) {
	ReturnType response = { "", DOUBLE, 0 };

	if (isVariable(params)) {
		if (getVariableType(params) == STRING) {
			//ERRO
		}
		else {
			double value = stod(getVariableValue(params));
			response.returnValue.double_ = log(value);
		}
	}
	else {
		if (getDataType(params) == STRING) {
			//ERRO
		}
		else {
			double value = stod(resolveEvaluation(params));
			response.returnValue.double_ = log(value);
		}
	}

	return response;
}

//REGISTRA TODOS OS MÉTODOS NA LISTA
void registerAll(map<string, Functions>* functions_available) {
	functions_available->insert({ "abs", {NATIVE, PUBLIC, DOUBLE, abs_}});
	functions_available->insert({ "ceil", {NATIVE, PUBLIC, INTEGER, ceil_}});
	functions_available->insert({ "floor", {NATIVE, PUBLIC, INTEGER, floor_}});
	functions_available->insert({ "round", {NATIVE, PUBLIC, INTEGER, round_}});
	functions_available->insert({ "pow", {NATIVE, PUBLIC, DOUBLE, pow_}});
	functions_available->insert({ "sqrt", {NATIVE, PUBLIC, DOUBLE, sqrt_}});
	functions_available->insert({ "random", {NATIVE, PUBLIC, INTEGER, random}});
	functions_available->insert({ "log", {NATIVE, PUBLIC, DOUBLE, log_}});
}

//REGISTRA NA LISTA DE MÉTODOS
void registerMethod(map<string, Functions>* functions_available, string name) {
	if (name == "abs") functions_available->insert({name, { NATIVE, PUBLIC, DOUBLE, abs_ } });
	else if(name == "ceil") functions_available->insert({ "ceil", {NATIVE, PUBLIC, INTEGER, ceil_} });
	else if(name == "floor") functions_available->insert({ "floor", {NATIVE, PUBLIC, INTEGER, floor_} });
	else if(name == "round") functions_available->insert({ "round", {NATIVE, PUBLIC, INTEGER, round_} });
	else if(name == "pow") functions_available->insert({ "pow", {NATIVE, PUBLIC, DOUBLE, pow_} });
	else if(name == "sqrt") functions_available->insert({ "sqrt", {NATIVE, PUBLIC, DOUBLE, sqrt_} });
	else if(name == "random") functions_available->insert({ "random", {NATIVE, PUBLIC, INTEGER, random} });
	else if(name == "random") functions_available->insert({ "random", {NATIVE, PUBLIC, INTEGER, random} });
	else if(name == "log") functions_available->insert({ "log", {NATIVE, PUBLIC, DOUBLE, log_} });
}

//VERIFICA SE EXISTE NA LISTA DE METODOS
bool inList(string name) {

vector<string> list = { "abs", "ceil", "floor", "round", "pow", "sqrt", "random", "log"};

for (string method : list) {
	if (name == method) {
		return true;
	}
}

return false;
}