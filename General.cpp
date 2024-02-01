#include "General.h"

void updateVectorsReferencesPointers() {
    for (auto& key : scopes.top().vector_references_variables) {
        VectorReference ref = scopes.top().vector_references_variables[key.first];

        if (ref.type == INTEGER_VECTOR) {
            key.second.integers = &scopes.top().integer_variables;
        }
    }
}

void pushVariablesReferences() {

    references_integer_variables.push_back(&scopes.top().integer_variables);
    references_double_variables.push_back(&scopes.top().double_variables);
    references_char_variables.push_back(&scopes.top().char_variables);
    references_boolean_variables.push_back(&scopes.top().boolean_variables);
    references_string_variables.push_back(&scopes.top().string_variables);
    references_class_variables.push_back(&scopes.top().class_variables);

    //Atualiza ponteiros de classes
    for (auto& c : current_scope.class_variables) {
        c.second.pub_integer_variables_references = &scopes.top().integer_variables;
        c.second.pub_double_variables_references = &scopes.top().double_variables;
        c.second.pub_boolean_variables_references = &scopes.top().boolean_variables;
        c.second.pub_char_variables_references = &scopes.top().char_variables;
        c.second.pub_string_variables_references = &scopes.top().string_variables;
    }

    references_vector_references_variables.push_back(&scopes.top().vector_references_variables);
}

//REMOVE A REFERENCIA PARA AS VARIAVEIS QUANDO O ESCOPO É DESTRUIDO
void popVariablesReferences() {
    if (!references_integer_variables.empty()) {
        references_integer_variables.pop_back();
    }
    if (!references_double_variables.empty()) {
        references_double_variables.pop_back();
    }
    if (!references_char_variables.empty()) {
        references_char_variables.pop_back();
    }
    if (!references_boolean_variables.empty()) {
        references_boolean_variables.pop_back();
    }
    if (!references_string_variables.empty()) {
        references_string_variables.pop_back();
    }
    if (!references_class_variables.empty()) {
        references_class_variables.pop_back();
    }
    if (!references_vector_references_variables.empty()) {
        references_vector_references_variables.pop_back();
    }
}

//INTERPRETA LINHA E EXECUTA
void interpretLine(string line) {

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
    else if (isFunction(line)) {}
    else if (isVariableDeclaration(line) && !current_scope.ignore_scope) {
        declareVariable(line);
    }
    else if (!current_scope.ignore_scope && isVariableUpdate(line)) {}

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
                    //current_scope.current_return = functions_available[before_parenthesis].function(read_in_line);
                }
                else if ((current_scope.basic_structures.type == IF || current_scope.basic_structures.type == ELSE_IF || current_scope.basic_structures.type == ELSE) && current_scope.ignore_scope) {
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

                            if (resolveEvaluation(basic_structure.evaluate_expression) == "1" && scopes.top().is_reading_function) {
                                *scopes.top().current_method_line = scopes.top().stack_method_line.top();
                                scopes.top().stack_method_line.pop();
                                scopes.top().block_current_line = true;

                                if (current_scope.basic_structures.type == LOOP) {
                                    universal_integer_variables[basic_structure.controller_variable] += 1;
                                }

                            }
                            else if (resolveEvaluation(basic_structure.evaluate_expression) == "1") {
                                //Recria o escopo
                                Scope new_scope;
                                new_scope.curly_braces_count++;
                                new_scope.basic_structures = basic_structure;

                                if (current_scope.basic_structures.type == LOOP) {
                                    universal_integer_variables[basic_structure.controller_variable] += 1;
                                }

                                file_reference->seekg(basic_structure.back_in);

                                scopes.push(new_scope);
                                pushVariablesReferences();
                            }
                            else {
                                universal_integer_variables.erase(basic_structure.controller_variable);
                                scopes.top().block_current_line = false;
                            }
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
                    if (functions_available.count(before_parenthesis) && !current_scope.ignore_scope) {
                        if(functions_available[before_parenthesis].category == NATIVE)
                            current_scope.current_return = functions_available[before_parenthesis].function(read_in_line);
                        else
                            current_scope.current_return = execute_custom(functions_available[before_parenthesis], read_in_line);

                    }
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
            functions_available[current_scope.basic_structures.creating_function].created_content.push_back(line);
        }
    }

    while (!current_scope.commonRequestUpdateVariable.empty()) {
        CommonRequestUpdateVariable top = current_scope.commonRequestUpdateVariable.top();
        string update_expression = (top.mode == INCREASE) ? top.variable_name + " + 1" : top.variable_name + " - 1";

        updateVariable(top.variable_name, update_expression, getVariableType(top.variable_name));
        current_scope.commonRequestUpdateVariable.pop();
    }
}