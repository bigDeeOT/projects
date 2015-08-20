#pragma once
struct node;

bool isStringEqualTo(const char* s1, const char* s2) {
	int i = 0;
	while ((s1[i] != 0) | (s2[i] != 0)) {
		if (s1[i] != s2[i]) {
			return false;
		}
		i++;
	}
	if (s1[i] != s2[i]) {
		return false;
	}
	else {
		return true;
	}
}

struct variable {
	char* name;
	char* arguments[10];
	int value;
	node* functionBody;
	variable* next;
	variable();
	variable(const char* str, int Value);
	~variable() { delete[] this->name; }
};

variable::variable(const char* str, int Value) {
	int len = 0;
	int i = 0;
	while (str[len] != 0) {
		len++;
	}
	this->name = new char[len + 1];
	for (i = 0; i < len; i++) {
		name[i] = str[i];
	}
	name[i] = 0;
	value = Value;
	next = NULL;

	for (int i = 0; i < 10; i++) {
		arguments[i] = 0;
	}
	functionBody = NULL;
}
variable::variable(void) {
	name = NULL;
	next = NULL;
	value = 0;
}

class Hash {
public:
	variable** hashtable;
	int size = 1;
	int hashKey(const char* key);
	void var(const char* name, int value);
	void func(const char* name, int value);
	variable* doesVariableExist(const char* name);
	void addParameters(const char* funcName, const char* paramName);
	void set(const char* name, int value);
	int returnVar(const char* name);
	variable* returnFunction(const char* name);
	Hash() { hashtable = NULL; }
	Hash(int tableSize);
	~Hash();
	void deleteChain(variable* ptr);
};
void Hash::deleteChain(variable* ptr) {
	if (ptr->next == NULL) {
		delete ptr;
		return;
	}
	deleteChain(ptr->next);
	delete ptr;
}
Hash::~Hash() {
	variable* ptr;
	for (int i = 0; i < size; i++) {
		ptr = hashtable[i];
		if (ptr) {
			deleteChain(ptr);
		}
	}
}

Hash::Hash (int tableSize) {
	hashtable = new variable*[tableSize]; 
	size = tableSize;
	for (int i = 0; i < tableSize; i++) {
		hashtable[i] = NULL;
	}
}
int Hash::hashKey(const char * key) {
	int keyNumber = 0;
	for (int i = 0; key[i] != 0; i++) {
		keyNumber += key[i];
	}
	return keyNumber % size;
}

inline variable* Hash::doesVariableExist(const char * name)
{
	int index = hashKey(name);
	variable* ptr = hashtable[index];
	if (ptr == NULL) {
		return false;
	}
	if (isStringEqualTo(ptr->name, name)) {
		return ptr;
	}
	while (ptr->next != NULL) {
		ptr = ptr->next;
		if (isStringEqualTo(ptr->name, name)) {
			return ptr;
		}
	}
	return false;
}

void Hash::addParameters(const char* funcName, const char* paramName) {
	variable* ptr;
	ptr = doesVariableExist(funcName);

	int len = 0;
	int i = 0;
	int j = 0;
	while (paramName[len] != 0) {		//get length of parameter
		len++;
	}
	while (ptr->arguments[i] != 0) {	//get index of first open argument slot
		i++;
	}
	ptr->arguments[i] = new char[len + 1];
	for (j = 0; j < len; j++) {
		ptr->arguments[i][j] = paramName[j];
	}
	ptr->arguments[i][j] = 0;
}

void Hash::var(const char* name, int value) {
	if (doesVariableExist(name)) {
		printf("variable %s incorrectly re-intialized\n", name);
		set(name, value);
		return;
	}
	int index = hashKey(name);
	if (hashtable[index] == NULL) {
		hashtable[index] = new variable(name, value);
	}
	else {
		variable* ptr = hashtable[index];
		hashtable[index] = new variable(name, value);
		hashtable[index]->next = ptr;
	}
}

inline void Hash::set(const char * name, int value)
{
	variable* ptr = doesVariableExist(name);
	if (!ptr) {
		printf("variable %s not declared\n", name);
		var(name, value);
		return;
	}
	ptr->value = value;
}

inline int Hash::returnVar(const char * name)
{
	variable* ptr = doesVariableExist(name);
	if (ptr) {
		return ptr->value;
	}
	else {
		printf("Something is wrong with returnVar. Perhaps %s does not exist\n", name);
		return 0;
	}	
}

variable* Hash::returnFunction(const char * name)
{
	variable* ptr = doesVariableExist(name);
	if (ptr) {
		return ptr;
	}
	else {
		printf("Something is wrong with returnFunction. Perhaps %s does not exist\n", name);
		return 0;
	}
}

Hash variableData = Hash(100);
Hash functionData = Hash(10);

