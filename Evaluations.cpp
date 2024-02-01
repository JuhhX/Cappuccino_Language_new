#include "Evaluations.h"
#include "General.h"

//Verifica se o caracter � uma express�o matem�tica
bool isOperation(string params) {
    if (params == to_string('+') || params == to_string('-') || params == to_string('*') || params == to_string('/') || params == to_string('%'))
        return true;
    return false;
}

//Express�o de multiplica��o
void multiply_expressions(stack<double>* numbers) {
    double n1 = numbers->top();
    numbers->pop();
    double n2 = numbers->top();
    numbers->pop();
    numbers->push(n1 * n2);
}

//Express�o de divis�o
void divide_expressions(stack<double>* numbers) {
    double n2 = numbers->top();
    numbers->pop();
    double n1 = numbers->top();
    numbers->pop();
    numbers->push(n1 / n2);
}

//Express�o de divis�o
void module_expressions(stack<double>* numbers) {
    int n2 = (int)numbers->top();
    numbers->pop();
    int n1 = (int)numbers->top();
    numbers->pop();
    numbers->push(n1 % n2);
}

//Express�o de soma
void sum_expressions(stack<double>* numbers) {
    double n1 = numbers->top();
    numbers->pop();
    double n2 = numbers->top();
    numbers->pop();
    numbers->push(n1 + n2);
}

//Express�o de subtra��o
void subtraction_expressions(stack<double>* numbers) {
    double n1 = numbers->top();
    numbers->pop();
    double n2 = numbers->top();
    numbers->pop();
    numbers->push(n1 - n2);
}

//Verifica se � um operador booleano
bool isBooleanOperation(char params) {

    return params == '>' || params == '<' || params == '=' || params == '!' || params == '&' || params == '|';

}

//Resolve uma atribui��o de Inteiros e Doubles
string resolveAssignment(string params) {
    stack<double> numbers, aux_numbers;
    stack<char> operations, aux_operations;
    string temp;
    bool is_reading_parenthesis = false;
    int parenthesis_count = 0;

    string method_name = "";

    resolveVector(&params);

    //Separa os operadores dos numeros
    for (char c : params) {
        if (c == ' ' || c == ';' || isBooleanOperation(c)) continue;

        if (!is_reading_parenthesis) {
            if (isOperation(to_string(c))) {
                if (isVariable(temp))
                    numbers.push(stod(getVariableValue(temp)));
                else {
                    numbers.push(stod(temp.empty() ? "0" : temp));
                }
                operations.push(c);
                temp = "";
            }
            else {
                if (c == '(') {
                    parenthesis_count++;
                    is_reading_parenthesis = true;
                }
                else
                    temp += c;
            }
        }
        else {
            if (c == ')') {
                parenthesis_count--;
                if (parenthesis_count == 0) {
                        temp = resolveAssignment(temp);
                    is_reading_parenthesis = false;
                }
            }
            else if (c == '(')
                parenthesis_count++;
            else
                temp += c;
        }
    }

    if (!temp.empty()) {
        if (isVariable(temp))
            numbers.push(stod(getVariableValue(temp)));
        else {
            if (getDataType(temp) != VOID) {
                numbers.push(stod(temp));
            }
            else {
                numbers.push(0);
            }
        }
    }

    if (numbers.size() == 1 && operations.empty())
        return to_string(numbers.top());

    //Resolve multiplica��o e divis�o
    while (!operations.empty()) {
        char op = operations.top();
        operations.pop();

        if (op == '*')
            multiply_expressions(&numbers);
        else if (op == '/')
            divide_expressions(&numbers);
        else if (op == '%')
            module_expressions(&numbers);
        else {
            if (!numbers.empty()) {
                double n1 = numbers.top();
                numbers.pop();
                aux_numbers.push(n1);

                if (!numbers.empty()) {
                    double n2 = numbers.top();
                    numbers.pop();

                    if (!operations.empty() && (operations.top() == '*' || operations.top() == '/'))
                        numbers.push(n2);
                    else
                        aux_numbers.push(n2);
                }
            }
            aux_operations.push(op);
        }

    }

    if (numbers.size() == 1 && aux_operations.size() == 0)
        return to_string(numbers.top());
    else if (numbers.size() == 1)
        aux_numbers.push(numbers.top());

    numbers = aux_numbers;
    operations = aux_operations;

    //Resolve adi��o e subtra��o
    while (!operations.empty()) {
        char op = operations.top();
        operations.pop();

        if (op == '+')
            sum_expressions(&numbers);
        else if (op == '-')
            subtraction_expressions(&numbers);
    }

    return to_string(numbers.top());
}

//Resolve uma atribui��o de String
string resolveAssignmentString(string params) {

    if (isVariable(params))
        return getVariableValue(params);
    else if (params.find("+") != string::npos) {
        string result = "";
        vector<string> operands = split(params, '+');

        for (string s : operands) {
            s = trim(s);

            if (startsWith(s, "\"") && endsWith(s, "\""))
                result += s.substr(1, s.length() - 2);
            else if (isVariable(s))
                result += getVariableValue(s);
            else {
                //ERRO
                /*throwError(errors.UNCLOSED_STRING, params);
                throw - 1;*/
            }
        }

        return result;

    }
    return formatInString(params);
}

//Resolve a express�o NOT
void not_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr) {
    while (!ops->empty()) {
        string op = ops->top();
        ops->pop();

        Operands n1 = opr->top();
        opr->pop();

        if (op == to_string('!')) {
            Operands op_struct;
            op_struct.double_value = !n1.double_value;
            opr->push(op_struct);
        }
        else {
            aux_ops->push(op);
            aux_opr->push(n1);
        }
    }

    if (opr->size() == 1)
        aux_opr->push(opr->top());

    *opr = *aux_opr;
    *ops = *aux_ops;
    *aux_opr = stack<Operands>();
    *aux_ops = stack<string>();
}

//Resolve express�es de MAIOR, MAIOR IGUAL, MENOR e MENOR IGUAL
void more_or_less_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr) {
    while (!ops->empty()) {
        string op = ops->top();
        ops->pop();

        Operands n1 = opr->top();
        opr->pop();
        Operands n2 = opr->top();
        opr->pop();

        Operands op_struct;
        if (op == to_string('<')) {
            op_struct.double_value = n1.double_value < n2.double_value;
            opr->push(op_struct);
        }
        else if (op == "<=") {
            op_struct.double_value = n1.double_value <= n2.double_value;
            opr->push(op_struct);
        }
        else if (op == to_string('>')) {
            op_struct.double_value = n1.double_value > n2.double_value;
            opr->push(op_struct);
        }
        else if (op == ">=") {
            op_struct.double_value = n1.double_value >= n2.double_value;
            opr->push(op_struct);
        }
        else {
            aux_ops->push(op);
            aux_opr->push(n1);
            opr->push(n2);
        }
    }

    if (opr->size() == 1)
        aux_opr->push(opr->top());

    *opr = *aux_opr;
    *ops = *aux_ops;
    *aux_opr = stack<Operands>();
    *aux_ops = stack<string>();
}

//Resolve express�es de IGUAL ou DIFERENTE
void equals_or_different_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr) {
    while (!ops->empty()) {
        string op = ops->top();
        ops->pop();

        Operands n1 = opr->top();
        opr->pop();
        Operands n2 = opr->top();
        opr->pop();

        Operands op_struct;
        string n1_value = (n1.is_string) ? n1.string_value : to_string(n1.double_value);
        string n2_value = (n2.is_string) ? n2.string_value : to_string(n2.double_value);
        if (op == "==") {
            op_struct.double_value = n1_value == n2_value;
            opr->push(op_struct);
        }
        else if (op == "!=") {
            op_struct.double_value = n1_value != n2_value;
            opr->push(op_struct);
        }
        else {
            aux_ops->push(op);
            aux_opr->push(n1);
            opr->push(n2);
        }
    }

    if (opr->size() == 1)
        aux_opr->push(opr->top());

    *opr = *aux_opr;
    *ops = *aux_ops;
    *aux_opr = stack<Operands>();
    *aux_ops = stack<string>();
}

//Resolve a express�es AND
void and_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr) {
    while (!ops->empty()) {
        string op = ops->top();
        ops->pop();

        Operands n1 = opr->top();
        opr->pop();
        Operands n2 = opr->top();
        opr->pop();

        Operands op_struct;
        if (op == "&&") {
            op_struct.double_value = n1.double_value && n2.double_value;
            opr->push(op_struct);
        }
        else {
            aux_ops->push(op);
            aux_opr->push(n1);
            opr->push(n2);
        }
    }

    if (opr->size() == 1)
        aux_opr->push(opr->top());

    *opr = *aux_opr;
    *ops = *aux_ops;
    *aux_opr = stack<Operands>();
    *aux_ops = stack<string>();
}

//Resolve a express�es OR
void or_expression(stack<string>* ops, stack<string>* aux_ops, stack<Operands>* opr, stack<Operands>* aux_opr) {
    while (!ops->empty()) {
        string op = ops->top();
        ops->pop();

        Operands n1 = opr->top();
        opr->pop();
        Operands n2 = opr->top();
        opr->pop();

        Operands op_struct;
        if (op == "||") {
            op_struct.double_value = n1.double_value || n2.double_value;
            opr->push(op_struct);
        }
        else {
            aux_ops->push(op);
            aux_opr->push(n1);
            opr->push(n2);
        }
    }

    if (opr->size() == 1)
        aux_opr->push(opr->top());

    *opr = *aux_opr;
    *ops = *aux_ops;
    *aux_opr = stack<Operands>();
    *aux_ops = stack<string>();
}

//Resolve uma express�o booleana
string resolveEvaluation(string params) {

    stack<Operands> operands, aux_operands;
    stack<string> operations, aux_operations;
    string temp;
    bool is_reading_parenthesis = false;
    int parenthesis_count = 0;

    for (int x = 0; x < params.length(); x++) {

        if (params[x] == ' ' || params[x] == ';')
            continue;

        if (!is_reading_parenthesis) {
            if (isBooleanOperation(params[x])) {
                if (isBooleanOperation(params[x + 1])) {
                    operations.push(params.substr(x, 2));
                    x++;
                }
                else
                    operations.push(to_string(params[x]));

                if (temp != "") {
                    Operands op_struct;
                    if (temp == "true") temp = "1";
                    else if (temp == "false") temp = "0";
                    else if (isVariable(temp)) {
                        if (getVariableType(temp) != STRING) {
                            op_struct.double_value = stod(getVariableValue(temp));
                        }
                        else {
                            op_struct.is_string = true;
                            op_struct.string_value = "\"" + getVariableValue(temp) + "\"";
                        }
                        operands.push(op_struct);
                    }
                    else {
                        if (startsWith(temp, "\"") && endsWith(temp, "\"")) {
                            op_struct.is_string = true;
                            op_struct.string_value = resolveAssignmentString(temp);
                        }
                        else
                            op_struct.double_value = stod(resolveAssignment(temp));
                        operands.push(op_struct);
                    }
                }
                temp = "";
            }
            else {
                if (params[x] == '(') {
                    parenthesis_count++;
                    is_reading_parenthesis = true;
                }
                else
                    temp += params[x];
            }
        }
        else {
            if (params[x] == ')') {
                parenthesis_count--;
                if (parenthesis_count == 0) {
                    temp = resolveEvaluation(temp);
                    is_reading_parenthesis = false;
                }
            }
            else if (params[x] == '(')
                parenthesis_count++;
            else
                temp += params[x];
        }

    }

    if (temp != "") {
        Operands op_struct;
        if (temp == "true") temp = "1";
        else if (temp == "false") temp = "0";
        else if (isVariable(temp)) {
            if (getVariableType(temp) != STRING) {
                op_struct.double_value = stod(getVariableValue(temp));
            }
            else {
                op_struct.is_string = true;
                op_struct.string_value = "\"" + getVariableValue(temp) + "\"";
            }
            operands.push(op_struct);
        }
        else {
            //Arrumar o resolveAssignment
            if (startsWith(temp, "\"") && endsWith(temp, "\"")) {
                op_struct.is_string = true;
                op_struct.string_value = resolveAssignmentString(temp);
            }
            else
                op_struct.double_value = stod(resolveAssignment(temp));
            operands.push(op_struct);
        }
    }

    not_expression(&operations, &aux_operations, &operands, &aux_operands);

    more_or_less_expression(&operations, &aux_operations, &operands, &aux_operands);

    equals_or_different_expression(&operations, &aux_operations, &operands, &aux_operands);

    and_expression(&operations, &aux_operations, &operands, &aux_operands);

    or_expression(&operations, &aux_operations, &operands, &aux_operands);

    if (operands.top().is_string) {
        string formated = operands.top().string_value;
        operands.top().string_value = formated.substr(1, formated.length() - 2);
    }

    if (operands.top().is_string) {
        return operands.top().string_value;
    }

    double value = operands.top().double_value;
    int integer_value = (int)value;

    if (value - integer_value >= 0.1) {
        return to_string(operands.top().double_value);
    }
    return to_string((int)operands.top().double_value);
}

