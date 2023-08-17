#include <criterion/criterion.h>

int reasonable_values(float value){
    if(value > 8.0 && value < 35.0){
        return 1;
    }else{
        return 0;
    }  
}

Test(suite_name, test_name){
    cr_assert(reasonable_values(28.0) == 1);
}