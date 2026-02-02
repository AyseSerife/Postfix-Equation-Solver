    /*05*/
    /*150123061 Ayşe Şerife SEVER*/
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <math.h>

    #define SEARCH_LIMIT 500

    struct stack /* Postfix stack structure */
    {
        char *element;
        int type; /* 0 operator 1 operand */
        struct stack *next;
    };
    typedef struct stack stack_struct;

    struct option /* Different options structure */
    {
        stack_struct *stack;
        int numOfOperand;
        int numOfOperator;
        int validity;
    };
    typedef struct option options;

    struct solutions /* Solutions linked list structure */
    {
        char *element[100];
        struct solutions *next;
    };
    typedef struct solutions *solutionPtr;

    char* deep_copy_string(char* source);
    int calculate_and_push_op(stack_struct **stack, char operator);
    int evaluate_postfix(char *array[],int numOfElements);
    int isNumber(char* data);
    int isOperator(char c);
    int is_valid_postfix(stack_struct *stack);    
    int push(stack_struct **stackPtr, char *input);
    int stack_to_array(stack_struct *stack, int type[], char *elements[]);
    options *create_new_option(options *opt);
    stack_struct* deep_copy_stack(stack_struct* stack_head);
    stack_struct* pop(stack_struct **stackPtr);
    void append_solution(solutionPtr *solution_head, char *var[], int element_num);
    void solver (char *array[],int types[] ,int numOfElements,int index ,solutionPtr *sol_H, int var_index[], int num_i, int result_value);

    int main(void)
    {
        
        char line[256]; /*max line length */
        char *token; 
        char variable[8];
        FILE *file;
        int var_count = 0;
        int i, a, f ,k;
        int option_counter = 1;
        int result = 0, resC = 0;
        int varNum;
        options *all_options[256]; /* 2^8 8 variable */
        options *option1 = NULL;
        solutionPtr solution_head = NULL;
        stack_struct *stack_head;
        stack_struct *cur;
        solutionPtr curSol;

        file = fopen("input.txt", "r"); /*File opened*/ 
        if(file == NULL)
        {
            printf("File openning eror!");
            exit(1);
        }
        stack_head = NULL; 
        
        option1 = (options*)malloc(sizeof(options));
        if(option1 == NULL){
            printf("memory error");
            exit(2);
        }

        option1->stack = stack_head;
        option1->validity = 1;
        option1->numOfOperand = 0;
        option1->numOfOperator = 0;

        all_options[0] = option1; /*Option 1 created and added to all_options*/ 
        

        fgets(line,sizeof(line),file); /* Read a line in the file*/
         
        token = strtok(line," \n");
        /* Tokenize the line and process each token creating options as needed for variables */
        while(token != NULL){
            if(isNumber(token)){/* Token is a number */
                if(resC){ /* '=' read and next token is result */
                    result = atoi(token);
                }else{ /* '=' not read yet */
                    /* number : push to stack */
                    for (i = 0 ; i < option_counter; i++){
                        if(all_options[i]->validity){ /* if it is valid push else don't care */
                            push(&(all_options[i]->stack),token);
                            all_options[i]->numOfOperand++;
                            all_options[i]->stack->type = 1; /* operand type */
                        }    
                    }
                }   
            }else if(strlen(token) == 1 && isalpha(*token)){ /* Token is an variable */
                int option_num;
                variable[var_count++] = *token; /* store variable */
                /* create new options for new variable half operand half operator */
                /* ----------------------------------------------------------- */
                /* creating new options equal to the number of current options */
                option_num = option_counter;
                for(i = 0; i < option_num; i++){
                    options *new_option = create_new_option(all_options[i]);
                    all_options[option_num+(i)] = new_option; /* adding new option to all_options */
                    option_counter++;
                }
                /* ------------------------------------------------------------- */
                /* Process all options with the newly created options */
                /* For operands: from 0 to option_num - 1 */
                for(i = 0; i < option_num; i++){
                    if(all_options[i]->validity){ /* if valid push else don't care */
                        push(&(all_options[i]->stack),token);
                        all_options[i]->numOfOperand++;
                        all_options[i]->stack->type = 1;
                    } 
                }
                /* For operators: from option_num to 2*option_num - 1 */
                for(i = option_num; i < 2*option_num; i++){
                    if(all_options[i]->validity){ /* if valid push else don't care */
                    push(&(all_options[i]->stack),token);
                        all_options[i]->numOfOperator++;
                        all_options[i]->stack->type = 0;
                    } 
                }
            }else if(strlen(token) == 1 && isOperator(*token)){ /* Token is an operator */
                for(i = 0; i < option_counter; i++){
                    if(all_options[i]->validity){ /* if valid push else don't care */
                        all_options[i]->numOfOperator++;
                        if(!calculate_and_push_op(&(all_options[i]->stack),*token)){
                            all_options[i]->validity = 0;
                        }    
                    }
                }
            }else if(strlen(token) == 1 && (*token) == '='){ /* Token is '=' */
                resC = 1; /* Next token will be result */
            }
            token = strtok(NULL, " \n"); /* Get next token */
        }
        /* All tokens processed */
        /* ---------------------------------------------------------- */
        /* Iterate through options and find results */
        for(i = 0; i < option_counter; i++){
            char *element_array[100];
            int types[100];
            int num_of_el;
            int var_index[100];
            int inner_var_count = 0;
            if(!all_options[i]->validity || !is_valid_postfix(all_options[i]->stack)){/* invalid option */
                continue;;
            }
            num_of_el = stack_to_array(all_options[i]->stack, types, element_array); /* stack to array */
            for (f = 0; f < num_of_el; f++) { /* find variable indices */
                char *el = element_array[f];
                if (!(strlen(el) == 1 && isOperator(el[0])) && !isNumber(el)) {
                    var_index[inner_var_count++] = f;
                }
            }
            varNum = inner_var_count;
            solver(element_array, types, num_of_el, 0, &solution_head, var_index, inner_var_count, result); /*solve recursively*/ 
        }

        printf("------------------------------------------\n");
        printf("Possible value combinations for (");
        for(a = 0; a < var_count-1; a++){
            printf("%c, ", variable[a]);
        }
        printf("%c)\n", variable[var_count -1]);
        /* print solutions */
        curSol = solution_head;
        while (curSol != NULL)
        {
            printf("(");
            for(a = 0; a < varNum-1; a++){
                printf("%s, ", curSol->element[a]);
            }
            printf("%s)\n", curSol->element[varNum -1]);
            curSol = curSol->next;
        }
        
        /* memory cleaning */
        while (stack_head != NULL) {
            stack_struct *tmp = stack_head;
            stack_head = stack_head->next;
            free(tmp->element);
            free(tmp);
        }

        for (i = 0; i < option_counter; i++) {
            cur = all_options[i]->stack;
            while (cur) {
                stack_struct *tmp = cur;
                cur = cur->next;
                free(tmp->element);
                free(tmp);
            }
        free(all_options[i]);
        }
        /* Clean up solution list */
        while (solution_head != NULL) {
            solutionPtr tmpSol = solution_head;
            solution_head = solution_head->next;
            
            /* Also clean the strings inside */
            for(k = 0; k < varNum; k++) {
                free(tmpSol->element[k]);
            }
            free(tmpSol);
        }
        
        fclose(file); /* File closed */
        return 0;
        
    }/* end of main */

    int stack_to_array(stack_struct *stack, int type[], char *elements[]){ /*converts stack to array returns number of elements*/ 
        stack_struct *curPtr = stack;
        int i = 0;
        int j, counter;
        int tempType[100];
        char *tempElement[100];
        if(curPtr == NULL){
            return 0;
        }
        while(curPtr != NULL){
            tempElement[i] = curPtr->element;
            tempType[i++] = curPtr->type; 
            curPtr = curPtr->next;
        }
        counter = i; /* number of elements */
        for (j = 0; j < counter; j++){
            elements[j] = tempElement[--i];
            type[j] = tempType[i];
        }
        return counter;
    }

    int evaluate_postfix(char *array[],int numOfElements){ /* evaluates postfix expression and returns result or -1 if invalid */
        stack_struct *postfix_stack = NULL;
        int i;
        int return_value;
        for(i = 0; i < numOfElements; i++){
            if(isNumber(array[i])){ /* operand */
                push(&postfix_stack, array[i]);
            }else if(strlen(array[i]) == 1 && isOperator(array[i][0])){ /* operator */
                if(!calculate_and_push_op(&postfix_stack, array[i][0])){
                    /* free stack and return -1 */
                    while (postfix_stack != NULL) {
                        stack_struct *tmp = postfix_stack;
                        postfix_stack = postfix_stack->next;
                        free(tmp->element);
                        free(tmp);
                    }
                    return -1; /* invalid option */
                } 
            }else{ /* variable in postfix expression */
                /* free stack and return -1 */
                while (postfix_stack != NULL) {
                    stack_struct *tmp = postfix_stack;
                    postfix_stack = postfix_stack->next;
                    free(tmp->element);
                    free(tmp);
                }
                return -1;
            }
        }
        if(postfix_stack == NULL){ /* empty stack : invalid */
            return_value = -1; 
        }else if(postfix_stack->next == NULL){/* there is only one element : valid */
            return_value = atoi(postfix_stack->element);
        }else{ /* more than one element left : invalid */
            return_value = -1;
        }
        /* free stack */
        while (postfix_stack != NULL) {
            stack_struct *tmp = postfix_stack;
            postfix_stack = postfix_stack->next;
            free(tmp->element);
            free(tmp);
        }
        return return_value;
    }

    void append_solution(solutionPtr *solution_head, char *var[], int element_num){ /* adds a new solution to the solution linked list */
        solutionPtr new_solution= (solutionPtr)malloc(sizeof(struct solutions));
        int i;
        if(new_solution == NULL) exit(3);

        new_solution->next = NULL;
        for(i = 0; i < element_num; i++){ /* deep copy for each element */
            new_solution->element[i] = deep_copy_string(var[i]);
        }
        if(*solution_head == NULL){ /* empty list */
           *solution_head = new_solution;
        }else{
            solutionPtr curPtr= *solution_head;
            while (curPtr->next != NULL){ /* traverse to the end of the list */
                curPtr = curPtr->next;
            }
            curPtr->next = new_solution;
        }
    }
    
    /*Takes an array and recursively tries all possibilities for the index. When it reaches the base case,
    it calls the evaluate function to get the result. If the result is correct, 
    it takes this as an option and adds it to the solution part.*/
    void solver (char *array[],int types[] ,int numOfElements,int index ,solutionPtr *sol_H, int var_index[], int num_i, int result_value){
        int k, i;
        char *element;
        char *original_val;
        int result;
        char *var[100]; /*char* var[num_i]; I didn't use beacuse of the c89*/
        if(index == numOfElements){/* base case all list elements are processed */
            result = evaluate_postfix(array, numOfElements);
            if (result == result_value ) { /* checking result if it is eaqual to desired result */
                /* option found add to solution list */
                for (k = 0; k < num_i; k++){ /* get variable values */
                    var[k] = array[var_index[k]];
                }
                append_solution(sol_H, var, num_i);
            }
            return;
        }
        element = array[index];
        if(!(strlen(element) == 1 && isOperator(element[0])) && !isNumber(element)){ /* it is a variable */
            original_val = element; /* for backtracking in recursion */
            if(types[index] == 1){/* operand */
                for (i= 1; i <= SEARCH_LIMIT; i++){ /* Testing positive integers between 1 and SEARCH_LIMIT recursively */
                    char value[16];
                    char *tmp;
                    sprintf(value, "%d", i);
                    tmp = deep_copy_string(value);
                    array[index] = tmp;
                    solver(array, types, numOfElements, index + 1, sol_H, var_index, num_i, result_value);

                    free(tmp);
                    array[index] = original_val; /* backtracking */
                }
            }else{ /* operator */
                char ops[] = "+-*/^";
                for (i = 0; i < 5; i++){ /* Testing all operators recursively */
                    char op[2];
                    char *tmp;
                    op[0] = ops[i];
                    op[1] = '\0';
                    tmp = deep_copy_string(op);
                    array[index] = tmp;
                    solver(array, types, numOfElements, index + 1, sol_H, var_index, num_i, result_value);

                    free(tmp);
                    array[index] = original_val; /* backtracking */
                }
            }
        }else{ /* not a variable move to next index */
            solver(array, types, numOfElements, index + 1, sol_H, var_index, num_i, result_value);
        }
    }   

    stack_struct* pop(stack_struct **stackPtr) /* pops the top element from the stack and returns it */
    {
        stack_struct *node;
        if(*stackPtr == NULL)
        {
            printf("Stack is empty");
            return NULL;
        }
        node = *stackPtr;
        *stackPtr = (*stackPtr)->next;
        return node;
    }

    int push(stack_struct **stackPtr, char *input) /* pushes an element onto the stack */
    {
        stack_struct *node = (stack_struct*)malloc(sizeof(stack_struct));
        if(node == NULL)
        {
            printf("Memory error!");
            return 0;
        }
        node->element = (char*)malloc(strlen(input) + 1); /* +1 for \0 */
        if (node->element == NULL) {
            printf("Memory error!\n");
            free(node);
            return 0;
        }
        
        strcpy(node->element,input);
        node->next = *stackPtr;
        *stackPtr = node; 
        return 1; /* success */
    }

    int isNumber(char* data){ /* checks if the given string is a valid integer number */
        int i = 0;
        if (data == NULL || *data == '\0'){
            return 0;
        }
        if(data[0] == '-'){
            i = 1;
            /* If only "-" is present (length is 1), it is not a number */
            if (data[1] == '\0') return 0;
        } 
        for(; i < strlen(data) ; i++){
            if(!isdigit(data[i])){
                return 0;
            }        
        }
        return 1;
    }

    int isOperator(char c){ /* checks if the given character is an operator */
        if(c == '+' || c == '-' || c == '/' || c == '*' || c == '^'){
            return 1;
        }else{
            return 0;
        }
    }

    int is_valid_postfix(stack_struct *stack) { /* checks if the given stack represents a valid postfix expression */
        int operand_count = 0;
        int i;
        /* Convert stack to array */
        char *elements[100];
        int types[100];
        int count = stack_to_array(stack, types, elements);
        
        /* Check postfix rule */
        for(i = 0; i < count; i++){
            if(types[i] == 1){  /* operand */
                operand_count++;
            }
            else if(types[i] == 0){  /* operator */
                if(operand_count < 2) return 0;
                operand_count--;
            }
        }
        
        return (operand_count == 1);
    }
    /*pops 2 elements if the elements are numbers evaluates the operation and pushes the result
     if they are not numbers pushes them back and the operator*/
    int calculate_and_push_op(stack_struct **stack, char operator) 
    {
        int i;
        char* value[2];
        int s_types[2];
        int result;
        char res[100];
        for(i = 0; i < 2; i++ ){ /* popping first and second element */
            stack_struct *popping_element = pop(stack);
            if(popping_element == NULL)
            {
                printf("You cannot pop from the empty stack!");
                return 0;
            }     
            value[i] = popping_element->element;
            s_types[i] = popping_element->type; /* remember type for later */
            free(popping_element);   
        }
                    
        if(isNumber(value[0]) && isNumber(value[1])){ /* incoming operands are numbers */
            int var0 = atoi(value[0]);
            int var1 = atoi(value[1]);
            /* perform operation */
            if(operator == '+'){
                result = var1 + var0;
            }else if(operator == '-'){
                result = var1 - var0;
            }else if(operator == '*'){
                result = var1 * var0;
            }else if(operator == '/'){
                if (var0 == 0) {  /* cannot divide by 0 */
                    return 0;
                }
                if(var1 % var0 != 0){ /* if not divisible exactly, invalid */
                    return 0;
                }
                result = var1 / var0;
            }else if(operator == '^'){
                if(var0 < 0) return 0; /* negative exponent check */
                result = (int)pow(var1,var0);
            }
            /* if(result <= 0) return 0;*/  /*only positive results are allowed */
            sprintf(res, "%d", result);
            push(stack, res); 
            (*stack)->type = 1;
            return 1;
        }else{/* operands are not numbers, they are variables */
            char opStr[2];
            push(stack,value[1]);
            (*stack)->type = s_types[1]; /* restore type */
            push(stack,value[0]);
            (*stack)->type = s_types[0]; /* restore type */
            opStr[0] = operator;
            opStr[1] = '\0';
            push(stack, opStr);
            (*stack)->type = 0;
            
            return 1;
        }
    }

    options *create_new_option(options *opt){ /* creates a deep copy of the given option and returns it */
        options *newOption = (options*)malloc(sizeof(options));
        if(newOption == NULL){
            printf("Memory error");
            exit(4);
        }

        newOption->stack = deep_copy_stack(opt->stack);
        newOption->numOfOperand = opt->numOfOperand;
        newOption->numOfOperator = opt->numOfOperator;
        newOption->validity = opt->validity;

        return newOption;
    }

    char* deep_copy_string(char* source) { /* creates a deep copy of the given string and returns it */
        char* new_string;
        if (source == NULL) {
            return NULL;
        }
        /* Allocate memory for the new string (length + 1 for '\0' character) */
        new_string = (char*)malloc(strlen(source) + 1);
        if (new_string == NULL) {
            return NULL; /* Memory error */
        }
        /* Copy the string to the new memory area */
        strcpy(new_string, source);
        return new_string;
    }

    stack_struct* deep_copy_stack(stack_struct* stack_head) { /*creates a deep copy of the given stack and returns it*/ 
        stack_struct* new_head;
        stack_struct* current_source; 
        stack_struct* current_new;
        if (stack_head == NULL) {
            return NULL; /* Nothing to copy */
        }
        /* Create the head of the NEW list */
        new_head = (stack_struct*)malloc(sizeof(stack_struct));
        if (new_head == NULL) {
            return NULL; /* Memory error */
        }
        /* Deep copy the content of the first node */
        new_head->element = deep_copy_string(stack_head->element);
        new_head->type = stack_head->type;
        new_head->next = NULL;

        /* Helper pointers to traverse the list */
        current_source = stack_head->next; /* Traversing the original list */
        current_new = new_head; /* Traversing the copied list */

        /* Traverse the rest of the original list in a loop */
        while (current_source != NULL) {    
            /* Allocate memory for a new node */
            current_new->next = (stack_struct*)malloc(sizeof(stack_struct));
            if (current_new->next == NULL) {
                /* Memory error (no space for the entire list) */
                break; 
            } 
            /* Move the new list to the next node */
            current_new = current_new->next;

            /* Deep copy the content of the new node */
            current_new->element = deep_copy_string(current_source->element);
            current_new->type = current_source->type;
            current_new->next = NULL;

            /* Move the original list to the next node */
            current_source = current_source->next;
        }
        return new_head;
    }

    /*End of the project*/ 