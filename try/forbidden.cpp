#include <string>
#include <iostream>

int main() {
    const char FORBIDDEN_CHAR[] = { '\\', '?', '/', '"', ':','<','>','*','|','\0' };
    std::string forbiddenString(FORBIDDEN_CHAR);

    // Print the forbidden characters string
    std::cout << "Forbidden characters string: " << forbiddenString << std::endl;
    //\?/m"a:<n>*o|j
    return 0;
}