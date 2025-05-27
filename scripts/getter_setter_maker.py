f = open('input.txt')
var_list = f.readlines()


for var in var_list:
    var_t = var.split()[0]
    var_n = var.split()[1]

    code = f'void set_{var_n}({var_t} new_{var_n}){{\n'+\
        f'  {var_n} = new_{var_n};\n'+\
        f'}}\n\n'+\
        f'{var_t} get_{var_n}(void){{\n'+\
        f'  return {var_n};\n'+\
        f'}}'

    print(code)

for var in var_list:
    var_t = var.split()[0]
    var_n = var.split()[1]

    code = f'void set_{var_n}({var_t} new_{var_n});\n'+\
        f'{var_t} get_{var_n}(void);\n'
        

    print(code)