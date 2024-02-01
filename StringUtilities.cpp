#include "StringUtilities.h"

string trim(string s) {

    if (s.empty() || s == "\t") return s;

    while (s[0] == ' ' || s[0] == '\t') {
        if (s.empty()) break;
        s = s.substr(1, s.size());
    }

    while (!s.empty() && (s[s.length() - 1] == ' ' || s[s.length() - 1] == '\t')) {
        if (s.empty()) break;
        s = s.substr(0, s.length() - 1);
    }

    if (s.empty()) {
        s = "";
    }

    return s;
}

//string trimEnd(string s) {
//    while (s[s.length() - 1] == ' ' || s[s.length() - 1] == '\t')
//        s = s.substr(0, s.length() - 1);
//
//    return s;
//}
//
//string trimStart(string s) {
//    while (s[0] == ' ' || s[0] == '\t')
//        s = s.substr(1, s.size());
//    return s;
//}

vector<string> split(string s, char separator) {
    vector<string> retorno;
    int pos_inicial = 0;
    int pos_caract = (int)s.find(separator);

    while (pos_caract != string::npos) {
        retorno.push_back(s.substr(pos_inicial, pos_caract - pos_inicial));
        pos_inicial = pos_caract + 1;
        pos_caract = (int)s.find(separator, pos_inicial);
    }

    retorno.push_back(s.substr(pos_inicial));

    return retorno;
}

vector<string> split(string s, string separator) {
    vector<string> retorno;
    size_t pos_inicial = 0;
    int pos_caract = (int)s.find(separator);

    while (pos_caract != string::npos) {
        retorno.push_back(s.substr(pos_inicial, pos_caract - pos_inicial));
        pos_inicial = pos_caract + separator.length();
        pos_caract = (int)s.find(separator, pos_inicial);
    }

    retorno.push_back(s.substr(pos_inicial));

    return retorno;
}

vector<string> splitFirst(string s, char separator) {
    vector<string> retorno;
    int pos_caract = (int)s.find(separator);

    if (pos_caract != string::npos) {
        retorno.push_back(s.substr(0, pos_caract));
        retorno.push_back(trim(s.substr(pos_caract + 1)));
    }

    return retorno;
}

//vector<string> splitWhenParenthesesClosed(string s, char separator) {
//    vector<string> retorno;
//    int parentheses_count = 1;
//    int pos_caract = 0;
//
//    for (char c : s) {
//        if (c == '(')
//            parentheses_count++;
//        else if (c == ')')
//            parentheses_count--;
//
//        if (parentheses_count == 0) {
//            retorno.push_back(s.substr(0, pos_caract));
//            retorno.push_back(trim(s.substr(pos_caract + 1)));
//            break;
//        }
//
//        pos_caract++;
//    }
//
//    return retorno;
//}
//
//vector<string> splitLast(string s, char separator) {
//    vector<string> retorno;
//
//    int pos_caract = (int)s.rfind(separator);
//
//    if (pos_caract != string::npos) {
//        retorno.push_back(s.substr(0, pos_caract));
//        retorno.push_back(trim(s.substr(pos_caract + 1)));
//    }
//
//    return retorno;
//}

bool startsWith(string s, string t) {
    return s.find(t) == 0;
}

//bool endsWith(string s, string t) {
//    return s.rfind(t) == s.length() - 1;
//}

bool endsWith(string a, string b) {
    return a.size() >= b.size() && a.substr(a.size() - b.size()) == b;
}

bool isString(string s) {
    return s.find("\"") == 0 && s.rfind("\"") == s.length() - 1;
}

string extractStringValue(string s) {
    if (startsWith(s, "\"") && endsWith(s, "\""))
        return s.substr(1, s.length() - 2);
    return s;
}

//Resolve uma expressão dentro de uma string
string resolveFormatExpression(string params) {
    params = trim(params);
    /*if (isVector(params)) {
        int slots = getVectorSlots(trim(params.substr(0, params.length() - 1)));
        params = splitFirst(params, '[')[0];

        if (variableExists(params + "[" + to_string(slots) + "]")) {
            return (getVariableValue(params + "[" + to_string(slots) + "]", true));
        }
        return "";
    }
    else*/ 
    if (isVariable(params))
        return (getVariableValue(params));
    else {
        interpretLine(params);

        if (current_scope.current_return.category != VOID) {
            if (current_scope.current_return.category == INTEGER) {
                return to_string(current_scope.current_return.returnValue.integer);
            }
            else if (current_scope.current_return.category == DOUBLE) {
                return to_string(current_scope.current_return.returnValue.double_);
            }
            else if (current_scope.current_return.category == BOOLEAN) {
                return to_string(current_scope.current_return.returnValue.boolean);
            }
            else if (current_scope.current_return.category == CHAR) {
                return to_string(current_scope.current_return.returnValue.character);
            }
            else if (current_scope.current_return.category == STRING) {
                return current_scope.current_return.string_;
            }
            else if (current_scope.current_return.category >= 6 && current_scope.current_return.category <= 11) {
                return current_scope.current_return.string_;
            }
            else {
                //ERRO
            }
        }
        else {
            return resolveEvaluation(params);
        }
    }
    return "";

}

//FORMATA STRING COM VARIAVEIS DENTRO
string formatInString(string s) {

    string result = "";
    int current_pos = 0;
    int expression_init = 0;
    bool is_reading = false;

    while (current_pos < s.length()) {

        if (s[current_pos] == '{') {
            expression_init = current_pos + 1;
            is_reading = true;
        }
        else if (s[current_pos] == '}') {
            result += resolveFormatExpression(s.substr(expression_init, current_pos - expression_init));
            is_reading = false;
        }
        else if (!is_reading)
            result += s[current_pos];

        current_pos++;
    }


    return result;
}

//Faz a tabulação de linhas em strings
void resolveTab(string* s) {
    if (s->find("\\r") != string::npos) {
        vector<string> parts = split(*s, "\\r");
        *s = "";
        int x = 0;

        for (; x < parts.size() - 1; x++) {
            *s += parts[x] + "\r";
        }
        *s += parts[x];
    }
}

//Faz a quebra de linhas nas strings
void resolveLineBreak(string* s) {
    if (s->find("\\n") != string::npos) {
        vector<string> parts = split(*s, "\\n");
        *s = "";
        int x = 0;

        for (; x < parts.size() - 1; x++) {
            *s += parts[x] + "\n";
        }
        *s += parts[x];
    }
}

//Faz a quebra de linhas nas strings
void resolveRet(string* s) {
    if (s->find("\\r") != string::npos) {
        vector<string> parts = split(*s, "\\r");
        *s = "";
        int x = 0;

        for (; x < parts.size() - 1; x++) {
            *s += parts[x] + "\r";
        }
        *s += parts[x];
    }
}

//Faz a quebra de linhas nas strings
void resolveAlert(string* s) {
    if (s->find("\\a") != string::npos) {
        vector<string> parts = split(*s, "\\a");
        *s = "";
        int x = 0;

        for (; x < parts.size() - 1; x++) {
            *s += parts[x] + "\a";
        }
        *s += parts[x];
    }
}
