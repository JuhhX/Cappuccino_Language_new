use std::collections::LinkedList;
use std::ffi::c_char;
use std::fs::File;
use std::io::Read;

//O rust não suporta String ou str como paramêtros
#[no_mangle]
pub extern "C" fn analyze(filename: *const c_char) -> bool{

    //Transforma em string
    let c_str = unsafe {
        assert!(!filename.is_null());
        std::ffi::CStr::from_ptr(filename)
    };
    let str_from_c = c_str.to_str().unwrap();

    let mut content : String = String::new();

    // Abre o arquivo
    let mut file = match File::open(&str_from_c) {
        Ok(file) => file,
        Err(e) => {
            panic!("Não foi possível abrir o arquivo: {}", e);
        }
    };

    // Leia o conteúdo do arquivo para o buffer
    match file.read_to_string(&mut content) {
        Ok(_) => {}
        Err(e) => {
            panic!("Erro ao ler o arquivo: {}", e);
        }
    }

    let lines: Vec<&str> = content.split("\n").collect();

    let syntax_ : bool = analyse_syntax(&lines, &str_from_c);
    let variables_ : bool = analyse_variables(&lines, &str_from_c);

    syntax_ && variables_
}

fn analyse_syntax(content: &Vec<&str>, filename: &str) -> bool{
    
    let mut brackets_count: i32 = 0;
    let mut parentheses_count: i32 = 0;
    let mut curly_brackets_count: i32 = 0;

    let mut brackets_last_line: i32 = 0;
    let mut parentheses_last_line: i32 = 0;
    let mut curly_brackets_last_line: i32 = 0;
    let mut quotation_marks_last_line: i32 = 0;
    let mut apostrophe_last_line: i32 = 0;

    let mut is_ok : bool = true;
    let mut is_apostrophe_ok : bool = true;
    let mut is_quotation_marks_ok : bool = true;

    let mut errors_stack : LinkedList<String> = Default::default();
    let mut errors_count: i32 = 0;
    
    let lines: &Vec<&str> = content;

    //Lê cada linha
    for line in 0..lines.len(){
        
        //Ignora linhas em branco
        if lines[line].trim().is_empty() {
            continue;
        }

        //Verifica se a linha foi finalizada corretamente
        if !correctly_finalized(&lines[line].trim()) && !native_structure(&lines[line].trim()){
            is_ok = false;
            errors_count += 1;
            let line_error = (line as i32) + 1;
            
            let current_line = lines[line];
            let error_message = format!("\n\x1b[31m{filename}:{line_error} : Error: \";\" expected.\x1b[0m\n\t{current_line}");
            errors_stack.push_back(error_message);
        }

        //Lê cada caractere
        for x in 0 .. lines[line].len() {
            if lines[line].chars().nth(x) == Some('{') {
                curly_brackets_count += 1;
            }
            else if lines[line].chars().nth(x) == Some('}'){
                curly_brackets_count -= 1;
                curly_brackets_last_line = (line + 1) as i32;
            }
            else if lines[line].chars().nth(x) == Some('('){
                parentheses_count += 1;
            }
            else if lines[line].chars().nth(x) == Some(')'){
                parentheses_count -= 1;
                parentheses_last_line = (line + 1) as i32;
            }
            else if lines[line].chars().nth(x) == Some('['){
                brackets_count += 1;
            }
            else if lines[line].chars().nth(x) == Some(']'){
                brackets_count -= 1;
                brackets_last_line = (line + 1) as i32;
            }
            else if lines[line].chars().nth(x) == Some('"'){
                is_quotation_marks_ok = !is_quotation_marks_ok;
                quotation_marks_last_line = (line + 1) as i32;
            }
            else if lines[line].chars().nth(x) == Some('\''){
                is_apostrophe_ok = !is_apostrophe_ok;
                apostrophe_last_line = (line + 1) as i32;
            }
        }
    }

    if !is_ok{
        //Mostra os erros encontrados
        println!("\n\x1b[31mFounded {errors_count} errors\x1b[0m");
        for e in errors_stack{
            println!("{}", e);
        } 
        return false;
    }
    else{
        if brackets_count == 0 && curly_brackets_count == 0 && parentheses_count == 0 && is_apostrophe_ok && is_quotation_marks_ok{
            //println!("Syntax: OK");
            return true;
        }
        else{
            println!("\n\x1b[31mSyntaxe errors\x1b[0m");

            if brackets_count != 0 {
                println!("\n\x1b[31mError: Unclosed bracket in {brackets_last_line} line\x1b[0m");
            }
            if curly_brackets_count != 0 {
                println!("\n\x1b[31mError: Unclosed curly bracket in {curly_brackets_last_line} line\x1b[0m");
            }
            if parentheses_count != 0 {
                println!("\n\x1b[31mError: Unclosed curly bracket in {parentheses_last_line} line\x1b[0m");
            }
            if !is_quotation_marks_ok{
                println!("\n\x1b[31mError: Unclosed string in {quotation_marks_last_line} line\x1b[0m");
            }
            if !is_apostrophe_ok{
                println!("\n\x1b[31mError: Unclosed char in {apostrophe_last_line} line\x1b[0m");
            }

            return false;
        }
    }
}

fn analyse_variables(content: &Vec<&str>, filename: &str) -> bool{
    let lines: &Vec<&str> = content;
    let mut line_error = 0;

    for line in lines{

        //Ignora linhas em branco
        if line.trim().is_empty() {
            continue;
        }

        let analysed_line: &str = line.trim();
        if is_variable_declaration(analysed_line){
            let declaration_parts = analysed_line.split_once(" ").unwrap();

            if declaration_parts.1.contains("="){
                let var_parts : Vec<&str> = declaration_parts.1.split("=").collect();

                if verify_name(var_parts[0].trim()){
                    let error_message = format!("\n\x1b[31m{filename}:{line_error} : Error: this name is not allowed.\x1b[0m\n\t{line}");
                    println!("{}", error_message);
                    return false;
                }
            }
            else{
                //Verifica se o nome não é permitido
                if verify_name(declaration_parts.1){
                    
                    let error_message = format!("\n\x1b[31m{filename}:{line_error} : Error: this name is not allowed.\x1b[0m\n\t{line}");
                    println!("{}", error_message);
                    return false;
                }
            }
        }
        line_error = line_error+1;
    }

    //println!("Variables names: OK");
    return true;
}

fn native_structure(line: &str) -> bool{
    line.starts_with("if") || line.starts_with("else if") || line.starts_with("while") || line.starts_with("for")
}

fn correctly_finalized(line: &str) -> bool{
    line.ends_with(";") || line.ends_with("}") || line.ends_with(");") || line.ends_with("}") || line.ends_with("(") || line.ends_with("{") || line.ends_with("[")
}

fn is_variable_declaration(line: &str) -> bool{
    line.chars().nth(0).unwrap().is_uppercase()
}

fn verify_name(name: &str) -> bool{
    
    let not_correct_start = name.chars().nth(0).unwrap().is_uppercase() || name.chars().nth(0).unwrap().is_numeric();

    if not_correct_start{
        return true;
    }

    for c in 0..name.len(){
        if name.chars().nth(c) == Some(',') || name.chars().nth(c) == Some('-') || name.chars().nth(c) == Some('.'){
            return true;
        }
    }
    
    false
}