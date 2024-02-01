#include <fstream>
#include <iostream>
#include <string>

#include "General.h"
#include "Variables.h"
#include "Functions.h"
#include "NativeFunctions.h"
#include "Classes.h"

using namespace std;

ifstream* file_reference;

map<string, Functions> functions_available;

map<string, int> universal_integer_variables;
map<string, double> universal_double_variables;
map<string, char> universal_char_variables;
map<string, bool> universal_boolean_variables;
map<string, string> universal_string_variables;
map<string, Class> universal_class_variables;

vector<map<string, int>*> references_integer_variables;
vector<map<string, double>*> references_double_variables;
vector<map<string, char>*> references_char_variables;
vector<map<string, bool>*> references_boolean_variables;
vector<map<string, string>*> references_string_variables;
vector<map<string, Class>*> references_class_variables;

vector< map<string, VectorReference>*> references_vector_references_variables;

map<string, ClassModel> classes_models;

ClassModel creating_class_model;

Scope current_scope;
stack<Scope> scopes;

void readFile(string code_name);

//INICIA ESCOPO PRINCIPAL
void loadMainScope() {
    Scope main_scope;

    scopes.push(main_scope);
    pushVariablesReferences();
    current_scope = scopes.top();
}

int main() {
    loadNativeFunctions();
    loadMainScope();
    readFile("Main.cappuccino");
}

//LER ARQUIVO
void readFile(string code_name) {
    ifstream file;
    string content;

    file.open(code_name);

    file_reference = &file;

    if (file.good()) {

        if (file.is_open()) {

            while (getline(file, content)) {

                if (startsWith(trim(content), "//") || content.empty()) {
                    continue;
                }

                interpretLine(content);
                current_scope.current_return.category = VOID;
            }
        }
    }
    else
        cout << colors.RED << "File \"" << code_name << "\" not found or not exists" << colors.RESET << endl;

    file.close();
}