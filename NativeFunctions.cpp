#include "NativeFunctions.h"

//ADICIONA NO RETORNO ADEQUADO
void setInReturnType(ReturnType* returnType, ReturnCategory category, string value) {
    if (category == INTEGER) {
        returnType->returnValue.integer = stoi(value);
    }
    else if (category == DOUBLE) {
        returnType->returnValue.double_ = stod(value);
    }
    else if (category == CHAR) {
        returnType->returnValue.character = extractStringValue(value)[0];
    }
    else if (category == BOOLEAN) {
        returnType->returnValue.boolean = stoi(value);
    }
    else if (category == STRING) {
        returnType->string_ = value;
    }
    else if (category >= 6 && category <= 11) {
        returnType->string_ = value;
    }
}

//VERIFICA SE OS PARAMETROS ESTÁ OK (CUSTOM FUNCTIONS)
bool isParametersOk(string assigned, string used) {
    vector<string> assigned_vector = split(assigned, ',');
    vector<string> used_vector = split(used, ',');

    bool isOk = true;

    if (assigned_vector.size() != used_vector.size()) {
        return false;
    }

    //Verifica os tipos
    for (int x = 0; x < assigned_vector.size(); x++) {
        if (isVariable(trim(used_vector[x]))) {
            if (stringToType(assigned_vector[x]) != getVariableType(trim(used_vector[x]))) {
                isOk = false;
                break;
            }
        }
        else {
            
            //Para verificar métodos
            if (used_vector[x].find("(") != string::npos && used_vector[x].find(")") != string::npos) {
                if (functions_available.count(splitFirst(used_vector[x], '(')[0])) {
                    if (stringToType(splitFirst(trim(assigned_vector[x]), ' ')[0]) != functions_available[splitFirst(used_vector[x], '(')[0]].returns) {
                        isOk = false;
                        break;
                    }
                }
            }
            else {

                if (stringToType(splitFirst(trim(assigned_vector[x]), ' ')[0]) == STRING) {
                    if (!isString(used_vector[x])) {
                        isOk = false;
                        break;
                    }
                }
                else if (stringToType(splitFirst(trim(assigned_vector[x]), ' ')[0]) != getDataType(resolveEvaluation(trim(used_vector[x])))) {
                    isOk = false;
                    break;
                }
            }

        }
    }

    return isOk;
}

//DECLARA AS VARIAVEIS DO ESCOPO DA FUNÇÃO
void declareFunctionVariables(string assigned, string used) {
    vector<string> assigned_vector = split(assigned, ',');
    vector<string> used_vector;

    int bracket_count = 0;
    bool is_reading_bracket = false;

    string read_param = "";

    //SEPARADOR DE PARAMETROS
    for (char c : used) {
        if (c == '[') {
            is_reading_bracket = true;
            bracket_count++;
        }
        else if (c == ']') {
            bracket_count--;

            if (bracket_count == 0) {
                is_reading_bracket = false;
            }
        }

        if (c == ',' && !is_reading_bracket) {
            used_vector.push_back(read_param);
            read_param = "";
        }
        else {
            read_param += c;
        }
    }

    if ((used_vector.empty() && !read_param.empty()) || !read_param.empty()) {
        used_vector.push_back(read_param);
    }

    //Declara as variáveis
    for (int x = 0; x < assigned_vector.size(); x++) {
        //Ele não avalia parametros como expressões metodo(2 * 3);
        if (isVector(used_vector[x])) {
            declareVariable(trim(assigned_vector[x] + " = " + used_vector[x]));
        }
        else if (isVariableInReferences(trim(used_vector[x]))) {
            declareVariable(trim(assigned_vector[x] + " = " + resolveEvaluation(used_vector[x])));
        }
        else {
            declareVariable(trim(assigned_vector[x] + " = " + used_vector[x]));
        }
    }

}

//FORMATA O VALOR DAS VARIAVEIS
string formatParams(string params) {
    string response = "";
    vector<string> used_vector = split(params, ',');
    vector<string> new_vector;

    for (string param : used_vector){
        if (!isVector(trim(param)) && isVariable(trim(param))) {
            new_vector.push_back(getVariableValue(trim(param)));
        }
        else {

            interpretLine(param);

            if (current_scope.current_return.category != VOID) {
                if (current_scope.current_return.category == INTEGER) {
                    new_vector.push_back(trim(to_string(current_scope.current_return.returnValue.integer)));
                }
                else if (current_scope.current_return.category == DOUBLE) {
                    new_vector.push_back(trim(to_string(current_scope.current_return.returnValue.double_)));
                }
                else if (current_scope.current_return.category == BOOLEAN) {
                    new_vector.push_back(trim(to_string(current_scope.current_return.returnValue.boolean)));
                }
                else if (current_scope.current_return.category == CHAR) {
                    new_vector.push_back(trim(to_string(current_scope.current_return.returnValue.character)));
                }
                else if (current_scope.current_return.category == STRING) {
                    new_vector.push_back(trim(current_scope.current_return.string_));
                }
            }
            else {
                if (isVector(trim(param))) {
                    new_vector.push_back(trim(param));
                }
                else {
                    if (isString(param)) {
                        new_vector.push_back(param);
                    }
                    else {
                        new_vector.push_back(resolveEvaluation(param));
                    }
                }
            }

        }
    }

    for (int x = 0; x < new_vector.size(); x++) {
        response += new_vector[x] + ((x == new_vector.size() - 1) ? "" : ",");
    }

    return response;
}

//OBTEM O TAMANHO DE ALGO
ReturnType length(string params) {
    ReturnType response = { "", INTEGER, 0 };

    if (isString(params)) {
        params = extractStringValue(params);
        response.returnValue.integer = (int) params.length();
    }
    else if (isVector(params)) {
        params = getVariableValue(params);
        response.returnValue.integer = (int) split(params, ',').size();
    }
    else if (isVariable(params)) {
        if (getVariableType(params) == STRING) {
            params = extractStringValue(getVariableValue(params));
            response.returnValue.integer = (int)params.length();
        }
        else {
            //ERRO
        }
    }
    else {
        //ERRO
    }

    return response;
}

//LIMPA O CONSOLE
ReturnType clear(string params) {
    ReturnType response = { "", VOID };

    system("cls");

    return response;
}

//IMPRIME COM QUEBRA DE LINHA
ReturnType println(string params) {
    ReturnType response = { "", VOID };

    if (isString(params)) {
        resolveLineBreak(&params);
        resolveTab(&params);
        resolveRet(&params);
        resolveAlert(&params);
        cout << extractStringValue(params) << endl;
    }
    else if (isVariable(params)) {
        string response = getVariableValue(params);

        cout << response << endl;
    }

    return response;
}

//IMPRIME
ReturnType print(string params) {
    ReturnType response = { "", VOID };

    if (isString(params)) {
        resolveLineBreak(&params);
        resolveTab(&params);
        resolveRet(&params);
        resolveAlert(&params);
        cout << extractStringValue(params);
    }
    else if (isVariable(params)) {
        string response = getVariableValue(params);
        cout << response;
    }

    return response;
}

//IMPRIME
ReturnType printf_(string params) {
    ReturnType response = { "", VOID };

    if (isString(params)) {
        string response = formatInString(extractStringValue(params));

        resolveLineBreak(&response);
        resolveTab(&response);
        resolveRet(&response);
        resolveAlert(&response);

        if (current_scope.current_return.category == CHAR)
            response = char(stoi(response));

        cout << response << endl;
    }
    else if (isVariable(params)) {
        string response = getVariableValue(params);

        if (getVariableType(params) == CHAR)
            response = char(stoi(response));

        cout << response << endl;
    }

    return response;
}

//LÊ ENTRADA DO USUÁRIO
ReturnType readLine(string params) {
    ReturnType response = { "", STRING, 0};

    getline(cin, response.string_);
    response.string_ = "\"" + response.string_ + "\"";
    return response;
}

//LÊ ENTRADA DO USUÁRIO
ReturnType readInt(string params) {
    ReturnType response = { "", INTEGER, 0 };
    string read = "";

    getline(cin, read);
    response.returnValue.integer = stoi(read);
    return response;
}

//LÊ ENTRADA DO USUÁRIO
ReturnType readDouble(string params) {
    ReturnType response = { "", DOUBLE, 0 };
    string read = "";

    getline(cin, read);
    response.returnValue.double_ = stod(read);
    return response;
}

//LÊ ENTRADA DO USUÁRIO
ReturnType readBoolean(string params) {
    ReturnType response = { "", BOOLEAN, 0 };
    string read = "";

    getline(cin, read);

    response.returnValue.boolean = !(read == "false" || read == "0" || read == "");
    return response;
}

//LÊ ENTRADA DO USUÁRIO
ReturnType readChar(string params) {
    ReturnType response = { "", CHAR, 0 };
    string read = "";

    getline(cin, read);
    response.returnValue.character = read[0];
    return response;
}

//CRIA UM DELAY ANTES DE EXECUTAR A PRÓXIMA LINHA
ReturnType sleep(string params) {
    ReturnType response = { "", BOOLEAN, 1 };

    this_thread::sleep_for(chrono::seconds(stoi(params)));

    return response;
}

//CONVERTE UM TIPO PARA INTEIRO
ReturnType toInt(string params) {
    ReturnType response = { "", INTEGER, 0 };

    ReturnCategory type = getVariableType(params);

    if (isString(params) || type == STRING) {
        response.string_ = "NaN";
        response.category = STRING;
    }
    else {
        if (isVariable(params)) {
            response.returnValue.integer = stoi(getVariableValue(params));
        }
        else {
            interpretLine(params);

            if (current_scope.current_return.category != VOID) {
                if (current_scope.current_return.category == INTEGER) {
                    response.returnValue.integer = current_scope.current_return.returnValue.integer;
                }
                else if (current_scope.current_return.category == DOUBLE) {
                    response.returnValue.integer = (int) current_scope.current_return.returnValue.double_;
                }
                else if (current_scope.current_return.category == BOOLEAN) {
                    response.returnValue.integer = (int) current_scope.current_return.returnValue.boolean;
                }
                else if (current_scope.current_return.category == CHAR) {
                    response.returnValue.integer = (int) current_scope.current_return.returnValue.character;
                }
                else if (current_scope.current_return.category == STRING) {
                    response.string_ = "NaN";
                    response.category = STRING;
                }
                else {
                    //ERRO
                }
            }
            else {
                response.returnValue.integer = stoi(params);
            }
        }
    }

    return response;
}

//CONVERTE UM TIPO PARA DOUBLE
ReturnType toDouble(string params) {
    ReturnType response = { "", DOUBLE, 0 };

    ReturnCategory type = getVariableType(params);

    if (isString(params) || type == STRING) {
        response.string_ = "NaN";
        response.category = STRING;
    }
    else {
        if (isVariable(params)) {
            response.returnValue.double_ = stod(getVariableValue(params));
        }
        else {
            interpretLine(params);

            if (current_scope.current_return.category != VOID) {
                if (current_scope.current_return.category == INTEGER) {
                    response.returnValue.double_ = (double) current_scope.current_return.returnValue.integer;
                }
                else if (current_scope.current_return.category == DOUBLE) {
                    response.returnValue.double_ = current_scope.current_return.returnValue.double_;
                }
                else if (current_scope.current_return.category == BOOLEAN) {
                    response.returnValue.double_ = (double) current_scope.current_return.returnValue.boolean;
                }
                else if (current_scope.current_return.category == CHAR) {
                    response.returnValue.double_ = (double) current_scope.current_return.returnValue.character;
                }
                else if (current_scope.current_return.category == STRING) {
                    response.string_ = "NaN";
                    response.category = STRING;
                }
                else {
                    //ERRO
                }
            }
            else {
                response.returnValue.double_ = stod(params);
            }
        }
    }

    return response;
}

//CONVERTE UM TIPO PARA BOOLEANO
ReturnType toBoolean(string params) {
    ReturnType response = { "", BOOLEAN, 0 };

    ReturnCategory type = getVariableType(params);

    if (isString(params) || type == STRING) {
        response.returnValue.boolean = (!params.empty());
    }
    else {
        if (isVariable(params)) {
            string value = getVariableValue(params);

            response.returnValue.boolean = !(value == "0" || value == "false");
        }
        else {
            interpretLine(params);

            if (current_scope.current_return.category != VOID) {
                if (current_scope.current_return.category == INTEGER) {
                    response.returnValue.double_ = (bool) current_scope.current_return.returnValue.integer;
                }
                else if (current_scope.current_return.category == DOUBLE) {
                    response.returnValue.double_ = (bool) current_scope.current_return.returnValue.double_;
                }
                else if (current_scope.current_return.category == BOOLEAN) {
                    response.returnValue.double_ = current_scope.current_return.returnValue.boolean;
                }
                else if (current_scope.current_return.category == CHAR) {
                    response.returnValue.double_ = (bool) current_scope.current_return.returnValue.character;
                }
                else if (current_scope.current_return.category == STRING) {
                    string value = current_scope.current_return.string_;
                    response.returnValue.boolean = !(value == "0" || value == "false");
                }
                else {
                    //ERRO
                }
            }
            else {
                response.returnValue.boolean = !(params == "0" || params == "false");
            }
        }
    }

    return response;
}

//CONVERTE UM TIPO PARA STRING
ReturnType toString(string params) {
    ReturnType response = { "", STRING, 0 };

    ReturnCategory type = getVariableType(params);

    if (isString(params) || type == STRING) {
        response.string_ = params;
    }
    else {
        if (isVariable(params)) {
            response.string_ = getVariableValue(params);
        }
        else {
            interpretLine(params);

            if (current_scope.current_return.category != VOID) {
                if (current_scope.current_return.category == INTEGER) {
                    response.string_ = to_string(current_scope.current_return.returnValue.integer);
                }
                else if (current_scope.current_return.category == DOUBLE) {
                    response.string_ = to_string(current_scope.current_return.returnValue.double_);
                }
                else if (current_scope.current_return.category == BOOLEAN) {
                    response.string_ = to_string(current_scope.current_return.returnValue.boolean);
                }
                else if (current_scope.current_return.category == CHAR) {
                    response.string_ = to_string(current_scope.current_return.returnValue.character);
                }
                else if (current_scope.current_return.category == STRING) {
                    response.string_ = current_scope.current_return.string_;
                }
                else {
                    //ERRO
                }
            }
            else {
                if (isString(params) || type == STRING) {
                    response.string_ = params;
                }
                else {
                    //ERRO
                }
            }
        }
    }

    return response;
}

//ESTRUTURA DE LOOP
ReturnType loop(string params) {
    ReturnType response = { "", BOOLEAN, 0 };

    int times = stoi(resolveEvaluation(params));
    int* current_line = current_scope.current_method_line;

    //Variavel de controle do Loop
    string controller_variable = "_#loop#_" + to_string(scopes.size());
    string controller_expression = controller_variable + "<" + to_string(times);

    //Controle de linha, métodos são lido por vetores e não por arquivo como de costume.
    if (!universal_integer_variables.count(controller_variable)) {
        universal_integer_variables[controller_variable] = 1;
    }

    if (current_scope.is_reading_function) {
        current_scope.stack_method_line.push(*current_scope.current_method_line);
    }
    
    //Abre o escopo
    scopes.pop();
    scopes.push(current_scope);

    Scope new_scope;

    new_scope.is_reading_function = current_scope.is_reading_function;
    new_scope.curly_braces_count++;
    new_scope.basic_structures = {LOOP, (size_t) file_reference->tellg(), controller_variable, controller_expression};

    //ATUALIZA PONTEIROS DE VETORES
    updateVectorsReferencesPointers();

    scopes.push(new_scope);
    pushVariablesReferences();
    current_scope = scopes.top();

    if (current_scope.is_reading_function) {
        current_scope.current_method_line = current_line;
    }

    return response;
}

//ESTRUTURA DE WHILE
ReturnType while_(string params) {
    ReturnType response = { "", VOID, 0 };

    int* current_line = current_scope.current_method_line;

    //Controle de linha, métodos são lido por vetores e não por arquivo como de costume.
    if (current_scope.is_reading_function) {
        current_scope.stack_method_line.push(*current_scope.current_method_line);
    }

    //Abre o escopo
    scopes.pop();
    scopes.push(current_scope);

    Scope new_scope;

    new_scope.is_reading_function = current_scope.is_reading_function;
    new_scope.ignore_scope = !(resolveEvaluation(params) == "1");

    if (!new_scope.ignore_scope) {
        new_scope.curly_braces_count++;
    }

    new_scope.basic_structures = { WHILE, (size_t)file_reference->tellg(), "", params };

    //ATUALIZA PONTEIROS DE VETORES
    updateVectorsReferencesPointers();

    scopes.push(new_scope);
    pushVariablesReferences();
    current_scope = scopes.top();

    if (current_scope.is_reading_function) {
        current_scope.current_method_line = current_line;
    }

    return response;
}

//CONDIÇÃO IF
ReturnType if_condition(string params) {
    ReturnType response = { "", VOID, 0 };

    bool satisfied = !(resolveEvaluation(params) == "1");
    if (!satisfied) {
        current_scope.basic_structures.satisfied = IF_SATISFIED;
    }

    //Abre o escopo
    scopes.pop();
    scopes.push(current_scope);

    Scope new_scope;
    new_scope.ignore_scope = satisfied;

    //Se true inicia a contagem e bloqueia as outras execuções
    //Senão, deixa o código contar normalmente
    if (!new_scope.ignore_scope) {
        new_scope.curly_braces_count++;
    }

    new_scope.basic_structures = { IF, (size_t)0, "", params };

    //ATUALIZA PONTEIROS DE VETORES
    updateVectorsReferencesPointers();

    scopes.push(new_scope);
    pushVariablesReferences();
    current_scope = scopes.top();

    return response;
}

//CONDIÇÃO ELSE IF
ReturnType else_if_condition(string params) {
    ReturnType response = { "", VOID, 0 };

    bool satisfied = !(resolveEvaluation(params) == "1");
    if (!satisfied) {
        current_scope.basic_structures.satisfied = ELSE_IF_SATISFIED;
    }

    //Abre o escopo
    scopes.pop();
    scopes.push(current_scope);


    Scope new_scope;
    if (current_scope.basic_structures.satisfied == IF_SATISFIED) {
        new_scope.ignore_scope = true;
    }
    else {
        new_scope.ignore_scope = satisfied;
    }

    //Se true inicia a contagem e bloqueia as outras execuções
    //Senão, deixa o código contar normalmente
    if (!new_scope.ignore_scope) {
        new_scope.curly_braces_count++;
    }

    new_scope.basic_structures = { ELSE_IF, (size_t)0, "", params };

    //ATUALIZA PONTEIROS DE VETORES
    updateVectorsReferencesPointers();

    scopes.push(new_scope);
    pushVariablesReferences();
    current_scope = scopes.top();

    return response;
}

//CONDIÇÃO ELSE
ReturnType else_condition(string params) {
    ReturnType response = { "", VOID, 0 };

    //Abre o escopo
    scopes.pop();
    scopes.push(current_scope);

    Scope new_scope;
    new_scope.ignore_scope = (current_scope.basic_structures.satisfied != NOT_SATISFIED);

    //Se true inicia a contagem e bloqueia as outras execuções
    //Senão, deixa o código contar normalmente
    if (!new_scope.ignore_scope) {
        new_scope.curly_braces_count++;
    }

    new_scope.basic_structures = { ELSE, (size_t)0, "", params };

    //ATUALIZA PONTEIROS DE VETORES
    updateVectorsReferencesPointers();

    scopes.push(new_scope);
    pushVariablesReferences();
    current_scope = scopes.top();

    return response;
}

//EXECUTA METODO CRIADO PELO USUÁRIO
ReturnType execute_custom(Functions func, string params) {
    ReturnType response = { "", func.returns, 0 };

    if ((func.created_params.empty() && params.empty()) || isParametersOk(func.created_params, params)) {

        if (!(params.empty() && func.created_params.empty()))
            params = formatParams(params);

        Class class_;

        //Cria um escopo para a execução do método
        Scope new_scope;
        scopes.pop();
        scopes.push(current_scope);

        new_scope.ignore_scope = false;
        new_scope.is_reading_function = true;
        new_scope.curly_braces_count++;

        if (func.isClassMethod) {
            loadClassVariables(func.class_owner, &new_scope);
        }

        //ATUALIZA PONTEIROS DE VETORES
        updateVectorsReferencesPointers();

        scopes.push(new_scope);
        pushVariablesReferences();
        current_scope = scopes.top();

        if (!(func.created_params.empty() && params.empty())) {
            declareFunctionVariables(func.created_params, params);
        }

        //current_scope.basic_structures.type = EXECUTING_FUNCTION;
        //scopes.top().basic_structures.type = EXECUTING_FUNCTION;


        int current_line = 0;
        current_scope.current_method_line = &current_line;
        //for (string line : func.created_content) {
        for (; current_line < func.created_content.size(); ) {
            if (func.created_content[current_line] == "{")
                continue;

            interpretLine(func.created_content[current_line]);
            if (func.created_content[current_line].find("return") != string::npos && func.returns != VOID && !current_scope.ignore_scope) {
                string returned = trim(splitFirst(func.created_content[current_line], ' ')[1]);

                if (isVariable(trim(returned))) {
                    if (func.returns == getVariableType(trim(returned))) {
                        setInReturnType(&response, func.returns, getVariableValue(returned));
                    }
                    else {
                        //ERRO
                    }
                }
                else {
                    string value = resolveEvaluation(trim(returned));

                    if (func.returns == getDataType(trim(value))) {
                        setInReturnType(&response, func.returns, value);
                    }
                    else {
                        //ERRO
                    }
                }

                while (current_scope.basic_structures.type == IF || current_scope.basic_structures.type == ELSE_IF || current_scope.basic_structures.type == ELSE) {
                    scopes.pop();
                    popVariablesReferences();
                    current_scope = scopes.top();
                }

                break;
            }

            if (!current_scope.block_current_line) {
                current_scope.block_current_line = false;
                current_line++;
            }
        }

        scopes.pop();

        if (func.isClassMethod) {
            unloadClassVariables(func.class_owner);
        }

        popVariablesReferences();
        current_scope = scopes.top();

    }

    return response;
}

//CARREGA AS FUNÇÕES NATIVAS DO CAPPUCCINO
void loadNativeFunctions() {
    functions_available["print"] = { NATIVE, PUBLIC, VOID, print };
    functions_available["println"] = { NATIVE, PUBLIC, VOID, println };
    functions_available["printf"] = { NATIVE, PUBLIC, VOID, printf_ };
    functions_available["clear"] = { NATIVE, PUBLIC, VOID, clear };
    functions_available["readLine"] = { NATIVE, PUBLIC, STRING, readLine };
    functions_available["readInt"] = { NATIVE, PUBLIC, INTEGER, readInt };
    functions_available["readDouble"] = { NATIVE, PUBLIC, DOUBLE, readDouble };
    functions_available["readBoolean"] = { NATIVE, PUBLIC, BOOLEAN, readBoolean };
    functions_available["readChar"] = { NATIVE, PUBLIC, CHAR, readChar };
    functions_available["sleep"] = { NATIVE, PUBLIC, BOOLEAN, sleep };
    functions_available["toInt"] = { NATIVE, PUBLIC, INTEGER, toInt };
    functions_available["toDouble"] = { NATIVE, PUBLIC, DOUBLE, toDouble };
    functions_available["toBoolean"] = { NATIVE, PUBLIC, BOOLEAN, toBoolean };
    functions_available["toString"] = { NATIVE, PUBLIC, STRING, toString };
    functions_available["loop"] = { NATIVE, PUBLIC, BOOLEAN, loop };
    functions_available["if"] = { NATIVE, PUBLIC, VOID, if_condition };
    functions_available["else if"] = { NATIVE, PUBLIC, VOID, else_if_condition };
    functions_available["else"] = { NATIVE, PUBLIC, VOID, else_condition };
    functions_available["while"] = { NATIVE, PUBLIC, VOID, while_ };
    functions_available["length"] = { NATIVE, PUBLIC, INTEGER, length };
}

//FUTURO Imports.h

//VERIFICA SE É UMA DECLARAÇÃO DE MÉTODO E REGISTRA SE FOR
bool isTargetFunction(string params, string to_import, bool founded) {
    string assignature = params;
    if ((params.find("fn") != string::npos || params.find("function") != string::npos) && ((params.find("(") != string::npos && params.find(")") != string::npos) && (params.find(to_import) != string::npos || to_import == "*") || current_scope.basic_structures.type == USER_CLASS)) {
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

            if (splited.size() == 0) return false;

            //Isola os parâmetros
            function.created_params = splitFirst(splited[1], ')')[0];

            if (current_scope.basic_structures.type == USER_CLASS) {
                creating_class_model.functions_available[splitFirst(splited[0], ' ')[1]] = function;
                creating_class_model.functions_available[splitFirst(splited[0], ' ')[1]].assignature = assignature;
                creating_class_model.functions_available[splitFirst(splited[0], ' ')[1]].isClassMethod = true;
            }
            else {
                functions_available[splitFirst(splited[0], ' ')[1]] = function;
            }

            new_scope.basic_structures.creating_function = splitFirst(splited[0], ' ')[1];

        }
        else {
            function.accessibility = PRIVATE;

            //Separa o retorno do resto
            splited = splitFirst(params, ' ');

            if (splited.size() == 0) return false;

            function.returns = stringToType(splited[0]);

            //Serpara fn ou function do nome
            params = splitFirst(splited[1], ' ')[1];

            //Separa nome dos parâmetros
            splited = splitFirst(params, '(');

            //Isola os parâmetros
            function.created_params = splitFirst(splited[1], ')')[0];

            if (current_scope.basic_structures.type == USER_CLASS) {
                creating_class_model.functions_available[splited[0]] = function;
                creating_class_model.functions_available[splited[0]].isClassMethod = true;
            }
            else {
                functions_available[splited[0]] = function;
            }
            new_scope.basic_structures.creating_function = splited[0];
        }


        //Cria escopo para armazenar as instruções
        scopes.pop();
        scopes.push(current_scope);

        new_scope.ignore_scope = true;

        new_scope.curly_braces_count++;

        new_scope.basic_structures.type = USER_FUNCTION;
        scopes.push(new_scope);
        pushVariablesReferences();
        current_scope = scopes.top();

        return true;
    }
    else if (params.find(to_import) != string::npos && !founded) {
        //ERRO
    }
    return false;
}

//EXTRAI A IMPORTAÇÃO SOLICITADA
void extractImport(string line, string import_name, bool* founded) {

    line = trim(line);

    string before_parenthesis = "";
    string read_in_line = "";

    bool is_reading_parenthesis = false;
    bool is_reading_scope = false;

    int parenthesis_count = 0;

    //Remover os ; dos finais aqui de forma global
    if (endsWith(line, ";"))
        line = line.substr(0, line.length() - 1);

    if(isImport(line)){}
    else if (startsWith(line, "export ") && !(*founded)) {
        vector<string> exporting = split(line, ",");

        if (import_name == "*") {
            *founded = true;
        }
        else if (exporting.size() == 1) {
            if (trim(exporting[0]) == import_name) {
                *founded = true;
            }
        }
        else {
            for (string name : exporting) {
                if (trim(name) == import_name) {
                    *founded = true;
                    break;
                }
            }
        }

    }
    else if( isTargetFunction(line, import_name, founded) ) {}
    else if(isTargetClass(line)){}
    else if(current_scope.basic_structures.type != USER_FUNCTION && isClassVariable(line)){}
    else {

        for (char c : line) {

            if (!is_reading_parenthesis && c == '{') {

                if (current_scope.basic_structures.type == VOID_STRUCUTRE) {
                    scopes.pop();
                    scopes.push(current_scope);

                    Scope new_scope;
                    scopes.push(new_scope);
                    current_scope = scopes.top();
                    current_scope.curly_braces_count++;
                }


                if (functions_available.count(before_parenthesis) && current_scope.ignore_scope) {
                    current_scope.curly_braces_count++;
                }
                else if ((current_scope.basic_structures.type == IF || current_scope.basic_structures.type == ELSE_IF || current_scope.basic_structures.type == ELSE) && current_scope.ignore_scope) {
                    current_scope.curly_braces_count++;
                }
                else if (current_scope.basic_structures.type == USER_FUNCTION) {
                    current_scope.curly_braces_count++;
                }

            }
            else if (c == '}' && !is_reading_parenthesis) {
                current_scope.curly_braces_count--;

                if (current_scope.curly_braces_count == 0) {

                    BasicScopesStructures basic_structure = current_scope.basic_structures;

                    scopes.pop();
                    popVariablesReferences();

                    if (current_scope.basic_structures.type != VOID_STRUCUTRE) {
                        if (current_scope.basic_structures.type == LOOP || current_scope.basic_structures.type == WHILE) {

                            if (resolveEvaluation(basic_structure.evaluate_expression) == "1") {
                                //Recria o escopo
                                Scope new_scope;
                                new_scope.curly_braces_count++;
                                new_scope.basic_structures = basic_structure;
                                scopes.push(new_scope);
                                pushVariablesReferences();

                                if (current_scope.basic_structures.type == LOOP) {
                                    universal_integer_variables[basic_structure.controller_variable] += 1;
                                }
                                file_reference->seekg(basic_structure.back_in);
                            }
                            else {
                                universal_integer_variables.erase(basic_structure.controller_variable);
                            }
                        }
                        else if (current_scope.basic_structures.type == USER_CLASS) {
                            classes_models[creating_class_model.typename_] = creating_class_model;
                            creating_class_model = {};
                        }
                    }

                    current_scope = scopes.top();
                }
            }

            //Encontra parenteses
            if (c == '(' && !is_reading_parenthesis) {
                before_parenthesis = read_in_line;
                read_in_line = "";
                is_reading_parenthesis = true;
                parenthesis_count++;
            }
            else if (c == ')') {
                parenthesis_count--;

                if (is_reading_parenthesis && parenthesis_count != 0) {
                    read_in_line += ")";
                }

                if (parenthesis_count == 0) {
                    is_reading_parenthesis = false;
                }

            }
            else {
                if (c == '(') {
                    parenthesis_count++;
                }
                read_in_line += c;
            }

        }
        if (current_scope.basic_structures.type == USER_FUNCTION) {
            if (creating_class_model.functions_available[current_scope.basic_structures.creating_function].isClassMethod) {
                creating_class_model.functions_available[current_scope.basic_structures.creating_function].created_content.push_back(line);
            }
            else {
                functions_available[current_scope.basic_structures.creating_function].created_content.push_back(line);
            }
        }
    }

}

//LER ARQUIVO DE IMPORTAÇÃO
void readImportFile(string code_name, string to_import) {
    ifstream file;
    string content;

    file.open(code_name+".cappuccino");

    bool founded = false;

    if (file.good()) {

        if (file.is_open()) {

            while (getline(file, content)) {

                if (startsWith(trim(content), "//") || content.empty()) {
                    continue;
                }

                extractImport(content, to_import, &founded);
            }
        }
    }
    else
        cout << colors.RED << "File \"" << code_name << "\" not found or not exists" << colors.RESET << endl;

    file.close();
}

//REALIZA IMPORTAÇÕES
bool isImport(string params) {
    if (startsWith(params, "import ")) {
        //Separa keyword da importação;
        params = trim(splitFirst(params, ' ')[1]);

        //Bibliotecas padrão do Cappuccino
        if (startsWith(params, "Cappuccino.Math.")) {
            #include "Math.h"
            params = trim(split(params, ".")[2]);

            if (params == "*") {
                registerAll(&functions_available);
            }
            else {
                if (inList(params)) {
                    registerMethod(&functions_available, params);
                }
                else {
                    //ERRO
                }
            }
        }
        else if (startsWith(params, "Cappuccino.String.")) {
            #include "String.h"
            params = trim(split(params, ".")[2]);

            if (params == "*") {
                registerAll_String(&functions_available);
            }
            else {
                if (inList_String(params)) {
                    registerMethod_String(&functions_available, params);
                }
                else {
                    //ERRO
                }
            }
        }
        else {
            //IMPORT DE BIBLIOTECA CRIADA;
            vector<string> names = splitFirst(params, '.');
            names[0] = trim(names[0]);
            names[1] = trim(names[1]);

            vector<string> import_names = split(names[1], ',');

            if (import_names.size() > 1) {
                for (string name : import_names) {
                    readImportFile(names[0], name);
                }
            }
            else {
                if (analyze((names[0] + ".cappuccino").c_str())) {
                    readImportFile(names[0], names[1]);
                }
            }

        }

        return true;
    }

    return false;
}