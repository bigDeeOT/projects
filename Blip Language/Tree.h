#pragma once
#include <stdio.h>
#include "Parse.h"
#include "hash.h"
#include "Vector.h"

struct node;
Vector <node*> returnNode;
Vector <Hash*> localVariableData;
Vector <variable*> returnFunction;
struct expressionTree;
int localStacks = 0;

struct tokenTree {
public:
	node* root;
	void creator(node* currentNode);
	void evaluate(node* currentNode);
	tokenTree();
	tokenTree(node* currentNode);
	~tokenTree();
	void deleteNodes(node* currentNode);
};


void addVariable(const char* name, int value) {
	if (localStacks) {											//if local
		localVariableData[localStacks - 1]->var(name, value);
	}
	else {
		variableData.var(name, value);
	}
}

void setVariable(const char* name, int value) {
	if (localStacks) {																//if within a function
		if (localVariableData[localStacks - 1]->doesVariableExist(name)) {			//if variable exists locally
			localVariableData[localStacks - 1]->set(name, value);
		}
		else if (variableData.doesVariableExist(name)) {							//doesn't exit locally, but does globally
			variableData.set(name, value);
			return;
		}
		else if (!variableData.doesVariableExist(name)) {							//doesn't exit locally or globally
			localVariableData[localStacks - 1]->set(name, value);
		}
	}
	else {
		variableData.set(name, value);
	}
}

int returnVar(const char* name) {
	if (localStacks) {																//if within a function
		if (localVariableData[localStacks - 1]->doesVariableExist(name)) {			//if variable exists locally
			return localVariableData[localStacks - 1]->returnVar(name);
		}
		else if (variableData.doesVariableExist(name)) {							//doesn't exit locally, but does globally
			return variableData.returnVar(name);
		}
		else if (!variableData.doesVariableExist(name)) {							//doesn't exit locally or globally
			printf("Something is wrong with local returnVar. Perhaps %s does not exist\n", name);
			return 0;
		}
	}
	else {
		return variableData.returnVar(name);
	}
}

char* copyToken() {
	const char* varName = next_token();
	int len = 0;
	int i = 0;
	char* copy;
	while (varName[len] != 0) {
		len++;
	}
	copy = new char[len + 1];
	for (i = 0; i < len; i++) {
		copy[i] = varName[i];
	}
	copy[i] = 0;
	return copy;
}

struct node {
	int value;
	char* operation;
	bool isValue;
	bool isVariable;
	bool isUnary;
	expressionTree* expression;
	node* left;
	node* right;
	node* loopBack;
	variable* function;
	node(int Value);
	node(const char* Operation);
	~node();
};
node::node(int Value)
{
	value = Value;
	isValue = true;
	isUnary = false;
	isVariable = false;
	left = 0;
	right = 0;
	loopBack = 0;
	operation = 0;
	expression = 0;
	function = 0;
}

node::node(const char* Operation)
{
	value = 0;
	isValue = false;
	isUnary = false;
	isVariable = false;
	left = 0;
	right = 0;
	loopBack = 0;
	expression = 0;
	int len = 0;
	function = 0;
	while (Operation[len] != 0) {
		len++;
	}
	operation = new char[len + 1];
	int i;
	for (i = 0; i < len; i++) {
		operation[i] = Operation[i];
	}
	operation[i] = 0;
	if (isStringEqualTo("~", operation) | isStringEqualTo("!", operation)) {
		isUnary = true;
	}
}
node::~node() {
	if (operation != 0) {
		delete operation;
	}
	left = 0;
	right = 0;
}

struct expressionTree {
public:
	node* root;
	void creator(node* currentNode);
	int evaluate(node* currentNode);
	void createFunction(node* currentNode);
	expressionTree();
};

expressionTree::expressionTree() {
	if (isStringEqualTo(next_token(), "call")) {
		root = new node(next_token());
		createFunction(root);
		return;							//not needed?
	}
	else if (next_token_type == NUMBER) {
		root = new node(token_number_value);
	}
	else if (next_token_type == SYMBOL) {
		root = new node(next_token());
		creator(root);
	}
	else if (next_token_type == NAME) {
		root = new node(next_token());
		root->isVariable = true;
	}
	
}

void expressionTree::createFunction(node * currentNode)
{
	read_next_token();										//function name
	currentNode->left = new node(next_token());
	read_next_token();										//"args"
	currentNode->left->left = new node(next_token());
	node* ptr = currentNode->left->left;
	read_next_token();										//arg1 expression, if available
	while (!isStringEqualTo(next_token(), "sgra")) {
		ptr->left = new node("expression");
		ptr->left->expression = new expressionTree;
		read_next_token();							
		ptr = ptr->left;
	}
	ptr->left = new node(next_token());						//"sgra"
}

void expressionTree::creator(node * currentNode)
{
	read_next_token();
	if (isStringEqualTo(next_token(), "call")) {
		currentNode->left = new node(next_token());
		createFunction(currentNode->left);
	}
	if ((next_token_type == NUMBER) ) {
		currentNode->left = new node(token_number_value);
	}
	if ((next_token_type == NAME) && !isStringEqualTo(next_token(), "sgra")) {
		currentNode->left = new node(next_token());
		currentNode->left->isVariable = true;
	}
	if ((next_token_type == SYMBOL)) {
		currentNode->left = new node(next_token());
		creator(currentNode->left);
	}
	if (currentNode->isUnary) { return; }
	read_next_token();
	if (isStringEqualTo(next_token(), "call")) {
		currentNode->right = new node(next_token());
		createFunction(currentNode->right);
	}
	if ((next_token_type == NUMBER)) {
		currentNode->right = new node(token_number_value);
	}
	if ((next_token_type == NAME) && !isStringEqualTo(next_token(), "sgra")) {
		currentNode->right = new node(next_token());
		currentNode->right->isVariable = true;
	}
	if ((next_token_type == SYMBOL)) {
		currentNode->right = new node(next_token());
		creator(currentNode->right);
	}
}

int expressionTree::evaluate(node * currentNode)
{
	if (currentNode == 0) { return 0; }					//completely empty tree
	if (currentNode->operation != 0) {
		if (isStringEqualTo(currentNode->operation, "call")) {
			int value = 0;
			int i = 0;
			variable* funcPtr = functionData.returnFunction(currentNode->left->operation);
			node* argPtr = currentNode->left->left->left;
			localVariableData.push_back(new Hash(10));
			while (!isStringEqualTo(argPtr->operation, "sgra")) {
				value = argPtr->expression->evaluate(argPtr->expression->root);
				localVariableData[localVariableData.size() - 1]->var(funcPtr->arguments[i], value);
				argPtr = argPtr->left;
				i++;
			}
			localStacks++;
			tokenTree* runTheFunction = new tokenTree(funcPtr->functionBody);
			delete runTheFunction;
			localStacks--;
			delete localVariableData[localVariableData.size() - 1];		//remember to setup destructor
			localVariableData.pop_back();
			int returnValue = funcPtr->value;
			funcPtr->value = 0;
			return returnValue;
		}
	}

	if (currentNode->isValue) {
		return currentNode->value;
	}
	if (currentNode->isVariable) {
		return returnVar(currentNode->operation);
	}
	if (!currentNode->isValue && !currentNode->isVariable) {
		if (isStringEqualTo(currentNode->operation, "+")) {
			return evaluate(currentNode->left) + evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, "-")) {
			return evaluate(currentNode->left) - evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, "/")) {
			return evaluate(currentNode->left) / evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, "*")) {
			return evaluate(currentNode->left) * evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, "%")) {
			return evaluate(currentNode->left) % evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, "<")) {
			return evaluate(currentNode->left) < evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, ">")) {
			return evaluate(currentNode->left) > evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, "<=")) {
			return evaluate(currentNode->left) <= evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, ">=")) {
			return evaluate(currentNode->left) >= evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, "==")) {
			return evaluate(currentNode->left) == evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, "!=")) {
			return evaluate(currentNode->left) != evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, "&&")) {
			return evaluate(currentNode->left) && evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, "||")) {
			return evaluate(currentNode->left) || evaluate(currentNode->right);
		}
		if (isStringEqualTo(currentNode->operation, "~")) {
			return -evaluate(currentNode->left);
		}
		if (isStringEqualTo(currentNode->operation, "!")) {
			return !evaluate(currentNode->left);
		}
	}
}



void tokenTree::deleteNodes(node * currentNode)
{
	if ((currentNode->left == 0) && (currentNode->right == 0)) {
		delete currentNode;
		return;
	}
	if (currentNode->left != 0) {
		deleteNodes(currentNode->left);
	}
	if (currentNode->right != 0) {
		deleteNodes(currentNode->right);
	}
	delete currentNode;
}
tokenTree::~tokenTree()
{
	if (localStacks == 0) {
		deleteNodes(root);
	}
}

tokenTree::tokenTree()
{
	while (isStringEqualTo(next_token(), "//")) { skip_line(); read_next_token();}
	root = new node(next_token());
	creator(root);
	evaluate(root);
}
tokenTree::tokenTree(node* currentNode) {
	root = currentNode;
	evaluate(root);
}

void tokenTree::creator(node * currentNode)
{
	if (isStringEqualTo(currentNode->operation, "defun")) {
		read_next_token();					//function name
		functionData.var(next_token(), 0);	//save function name in database
		variable* funcPtr = functionData.returnFunction(next_token());
		returnFunction.push_back(funcPtr);	//push pointer to function info on stack
		char* functionName = copyToken();
		read_next_token();					//"params"
		read_next_token();					// name of first parameter if available
		while (!isStringEqualTo(next_token(), "smarap")) {
			functionData.addParameters(functionName, next_token());
			read_next_token();
		}
		read_next_token();
		currentNode->right = new node(next_token());		//start of body
		funcPtr->functionBody = currentNode->right;
		creator(currentNode->right);
		read_next_token();
		while (isStringEqualTo(next_token(), "//")) { skip_line(); read_next_token(); }
		if (next_token_type == END) { return; }
		currentNode->left = new node(next_token());
		creator(currentNode->left);
	}
	if (isStringEqualTo(currentNode->operation, "return")) {
		if (returnFunction.size()) {
			currentNode->function = returnFunction[returnFunction.size() - 1];
		}
		read_next_token();
		currentNode->expression = new expressionTree;
		read_next_token();
		currentNode->left = new node(next_token());		
		creator(currentNode->left);
	}
	if (isStringEqualTo(currentNode->operation, "nufed")) {
		currentNode->function = returnFunction[returnFunction.size() - 1];
		returnFunction.pop_back();
	}
	if (isStringEqualTo(currentNode->operation, "text")) {
		read_next_token();
		currentNode->left = new node(next_token());
		read_next_token();
		while (isStringEqualTo(next_token(), "//")) { skip_line(); read_next_token(); }
		if (next_token_type == END) { return; }
		currentNode->left->left = new node(next_token());
		creator(currentNode->left->left);
	}
	if (isStringEqualTo(currentNode->operation, "output")) {
		read_next_token();
		currentNode->expression = new expressionTree;
		read_next_token();
		currentNode->left = new node(next_token());
		creator(currentNode->left);
	}
	if (isStringEqualTo(currentNode->operation, "var") || isStringEqualTo(currentNode->operation, "set")) {
		read_next_token();
		currentNode->left = new node(next_token());
		read_next_token();
		currentNode->left->expression = new expressionTree;
		read_next_token();
		while (isStringEqualTo(next_token(), "//")) { skip_line(); read_next_token(); }
		if (next_token_type == END) { return; }
		currentNode->left->left = new node(next_token());
		creator(currentNode->left->left);
	}
	if (isStringEqualTo(currentNode->operation, "if") || isStringEqualTo(currentNode->operation, "do")) {
		read_next_token();
		currentNode->expression = new expressionTree;
		read_next_token();
		currentNode->right = new node(next_token());
		if (isStringEqualTo(currentNode->operation, "if")) {
			returnNode.push_back(currentNode);
		}
		creator(currentNode->right);
		read_next_token();
		while (isStringEqualTo(next_token(), "//")) { skip_line(); read_next_token(); }
		if (next_token_type == END) { return; }
		currentNode->left = new node(next_token());
		creator(currentNode->left);
	}

	if (isStringEqualTo(currentNode->operation, "fi")) {
		currentNode->loopBack = returnNode[returnNode.size() - 1];
		returnNode.pop_back();
		return;
	}
	if (isStringEqualTo(currentNode->operation, "else")) {
		read_next_token();
		currentNode->loopBack = returnNode[returnNode.size() - 1];
		currentNode->left = new node(next_token());
		currentNode->loopBack->loopBack = currentNode->left;
		creator(currentNode->left);
	}
}

void tokenTree::evaluate(node * currentNode)
{
	int value;
	if (currentNode == 0) {
		return;
	}
	if (isStringEqualTo(currentNode->operation, "return")) {
		value = currentNode->expression->evaluate(currentNode->expression->root);
		if (currentNode->function != NULL) {
			currentNode->function->value = value;
		}
		return;
	}
	if (isStringEqualTo(currentNode->operation, "defun")) {
		evaluate(currentNode->left);
	}
	if (isStringEqualTo(currentNode->operation, "nufed")) {
		//currentNode->function->value = 0;
		return;
	}
	if (isStringEqualTo(currentNode->operation, "text")) {
		printf("%s", currentNode->left->operation);
		if (currentNode->left->left == 0) { return; }
		evaluate(currentNode->left->left);
	}
	if (isStringEqualTo(currentNode->operation, "output")) {
		value = currentNode->expression->evaluate(currentNode->expression->root);
		printf("%d", value);
		if (currentNode->left == 0) { return; }
		evaluate(currentNode->left);
	}
	if (isStringEqualTo(currentNode->operation, "var")) {
		value = currentNode->left->expression->evaluate(currentNode->left->expression->root);
		addVariable(currentNode->left->operation, value);
		if (currentNode->left->left == 0) { return; }
		evaluate(currentNode->left->left);
	}
	if (isStringEqualTo(currentNode->operation, "set")) {
		value = currentNode->left->expression->evaluate(currentNode->left->expression->root);
		setVariable(currentNode->left->operation, value);
		if (currentNode->left->left == 0) { return; }
		evaluate(currentNode->left->left);
	}
	if (isStringEqualTo(currentNode->operation, "do")) {
		while (currentNode->expression->evaluate(currentNode->expression->root)) {
			evaluate(currentNode->right);
		}											
		if (currentNode->left == 0) { return; }
		evaluate(currentNode->left);
		
	}
	if (isStringEqualTo(currentNode->operation, "if")) {
		value = currentNode->expression->evaluate(currentNode->expression->root);
		if (value && currentNode->loopBack == 0) {			//true for if 
			evaluate(currentNode->right);
		}
		else if (!value && currentNode->loopBack != 0){		//false for if-else
			evaluate(currentNode->loopBack);
		}
		else if (value && currentNode->loopBack != 0) {		//true for if-else
			evaluate(currentNode->right);
		}
		else {												//false for if
			if (currentNode->left == 0) { return; }
			evaluate(currentNode->left);
		}
	}
	if (isStringEqualTo(currentNode->operation, "else")) {
		evaluate(currentNode->loopBack->left);
	}
	if (isStringEqualTo(currentNode->operation, "fi")) {
		if (currentNode->loopBack->left == 0) { return; }
		evaluate(currentNode->loopBack->left);
	}
}

