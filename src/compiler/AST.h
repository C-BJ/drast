//
// Created by Ashwin Paudel on 2022-03-20.
//

#ifndef DRAST_AST_H
#define DRAST_AST_H

#include <iostream>
#include <vector>
#include <sstream>
#include <memory>
#include <optional>
#include "Token.h"

enum class ASTType {
	COMPOUND_STATEMENT, // { ... }

	IMPORT, // import io

	FUNCTION_DECLARATION, // int :: test(int a, int b) { ... }
	FUNCTION_ARGUMENT, // int a, int b
	FUNCTION_CALL, // test(1, 2)

	TYPE, // int, string, float, bool, etc.

	STRUCT_DECLARATION, // struct Test { ... }
	STRUCT_INITIALIZER_CALL, // .init(1, 2)

	ENUM_DECLARATION, // enum Test { ... }
	ENUM_CASE, // case A = 50, B = 100 etc.

	VARIABLE_DECLARATION, // int a = 1

	WHILE, // while (a == 1) { ... }
	FOR, // for (int i = 0; i < 10; i++) { ... }

	SWITCH, // switch (a) { case 1: ... }
	SWITCH_CASE, // case 1: ...

	DO, // do { ... }
	CATCH, // catch (...) { ... }
	TRY, // try myVariable = myFunction()


	RETURN, // return 1
	IF, // if (a == 1) { ... } else { ... }
	ASM, // asm("mov rax, 1")
	ALIAS, // alias Test = int

	BINARY_EXPRESSION, // 5 + 6;
	UNARY_EXPRESSION, // -5;
	GROUPING_EXPRESSION, // (5 + 6)
	LITERAL, // 5;
	CAST, // cast(5.50, int);
};

class AST {
protected:
	ASTType ast_type;
	size_t line;

public:
	AST(ASTType ast_type, size_t line) : ast_type(ast_type), line(line) {}

	virtual std::string toString() const = 0;


	friend std::ostream &operator<<(std::ostream &out, AST const &ast) {
		out << "AST: " << ast.toString();
		return out;
	}
};

class CompoundStatement : public AST {
private:
	std::vector<std::unique_ptr<AST>> statements;

public:
	explicit CompoundStatement(size_t line) : AST(ASTType::COMPOUND_STATEMENT, line) {}

	void insertStatement(std::unique_ptr<AST> &statement) {
		statements.push_back(std::move(statement));
	}

	std::string toString() const override {
		std::stringstream ss;
		ss << "CompoundStatement: {";
		for (auto &statement: statements) {
			ss << statement->toString() << "; ";
		}
		ss << "}";
		return ss.str();
	}
};

class Import : public AST {
private:
	std::string &import_path;

public:
	Import(std::string &import_path, size_t line) : AST(ASTType::IMPORT, line), import_path(import_path) {}

	std::string &getImportPath() const {
		return this->import_path;
	}

	void setImportPath(std::string &path) {
		this->import_path = path;
	}

	std::string toString() const override {
		return "import '" + import_path + "'";
	}
};

class FunctionDeclaration : public AST {
private:
	std::optional<std::vector<TokenType>> modifiers;
	std::string &name;
	std::vector<std::unique_ptr<AST>> &arguments;
	std::unique_ptr<AST> &body; // AST CompoundStatement

public:
	FunctionDeclaration(std::string &name, std::vector<std::unique_ptr<AST>> &arguments, std::unique_ptr<AST> &body,
	                    size_t line) : AST(
		ASTType::FUNCTION_DECLARATION, line), name(name), arguments(arguments), body(body) {}

	std::string &getName() const {
		return this->name;
	}

	std::vector<std::unique_ptr<AST>> &getArguments() {
		return this->arguments;
	}

	std::unique_ptr<AST> &getBody() {
		return this->body;
	}

	std::string toString() const override {
		std::stringstream ss;
		ss << "FunctionDeclaration: " << name << "(";
		for (auto &argument: arguments) {
			ss << argument->toString() << ", ";
		}
		ss << ") {\n";
		ss << body->toString() << "}";
		return ss.str();
	}
};

class FunctionArgument : public AST {
private:
	std::string &name;
	std::unique_ptr<AST> &type;

public:
	FunctionArgument(std::string &name, std::unique_ptr<AST> &type, size_t line) : AST(ASTType::FUNCTION_ARGUMENT,
	                                                                                   line), name(name),
	                                                                               type(type) {}

	std::string &getName() const {
		return this->name;
	}

	std::unique_ptr<AST> &getType() {
		return this->type;
	}

	std::string toString() const override {
		return name + ": " + type->toString();
	}
};

class FunctionCall : public AST {
private:
	std::string &name;
	std::vector<std::unique_ptr<AST>> &arguments;

public:
	FunctionCall(std::string &name, std::vector<std::unique_ptr<AST>> &arguments, size_t line) : AST(
		ASTType::FUNCTION_CALL, line),
	                                                                                             name(name),
	                                                                                             arguments(
		                                                                                             arguments) {}

	std::string &getName() const {
		return this->name;
	}

	std::vector<std::unique_ptr<AST>> &getArguments() {
		return this->arguments;
	}

	std::string toString() const override {
		std::stringstream ss;
		ss << name << "(";
		for (auto &argument: arguments) {
			ss << argument->toString() << ", ";
		}
		ss << ")";
		return ss.str();
	}
};

class Type : public AST {
private:
	Token &token;
	bool is_pointer;
	bool is_array;
	bool is_optional;

public:
	Type(Token token, bool is_pointer, bool is_array, bool is_optional, size_t line) : AST(ASTType::TYPE, line),
	                                                                                   token(token),
	                                                                                   is_pointer(is_pointer),
	                                                                                   is_array(is_array),
	                                                                                   is_optional(is_optional) {};

	void setIsPointer(bool isPointer) {
		this->is_pointer = isPointer;
	}

	void setIsArray(bool isArray) {
		this->is_array = isArray;
	}

	void setIsOptional(bool isOptional) {
		this->is_optional = isOptional;
	}

	void setType(Token token) {
		this->token = token;
	}

	Token &getType() const {
		return this->token;
	}

	std::string toString() const override {
		return this->token.value + (is_array ? "[]" : "") + (is_pointer ? "*" : "") + (is_optional ? "?" : "");
	}
};

class StructDeclaration : public AST {
private:
	std::string &name;
	std::vector<std::unique_ptr<AST>> &fields; // Variable Or Function Declarations

public:
	StructDeclaration(std::string &name, std::vector<std::unique_ptr<AST>> &fields, size_t line) :
		AST(ASTType::STRUCT_DECLARATION, line), name(name), fields(fields) {}

	std::string &getName() const {
		return this->name;
	}

	std::vector<std::unique_ptr<AST>> &getFields() {
		return this->fields;
	}

	std::string toString() const override {
		std::stringstream ss;
		ss << "StructDeclaration: " << name << " {\n";
		for (auto &field: fields) {
			ss << field->toString() << "\n";
		}
		ss << "}";
		return ss.str();
	}
};

class StructInitializerCall : public AST {
private:
	std::string &name;
	std::vector<std::unique_ptr<AST>> &arguments;

public:
	StructInitializerCall(std::string &name, std::vector<std::unique_ptr<AST>> &arguments, size_t line) : AST(
		ASTType::STRUCT_INITIALIZER_CALL, line), name(name), arguments(arguments) {}

	std::string &getName() const {
		return this->name;
	}

	std::vector<std::unique_ptr<AST>> &getArguments() {
		return this->arguments;
	}

	std::string toString() const override {
		std::stringstream ss;
		ss << name << "(";
		for (auto &argument: arguments) {
			ss << argument->toString() << ", ";
		}
		ss << ")";
		return ss.str();
	}
};

class EnumDeclaration : public AST {
private:
	std::string &name;
	std::vector<std::unique_ptr<AST>> &cases;

public:
	EnumDeclaration(std::string &name, std::vector<std::unique_ptr<AST>> &cases, size_t line) : AST(
		ASTType::ENUM_DECLARATION, line),
	                                                                                            name(name),
	                                                                                            cases(cases) {}

	std::string &getName() const {
		return this->name;
	}

	std::vector<std::unique_ptr<AST>> &getCases() {
		return this->cases;
	}

	std::string toString() const override {
		std::stringstream ss;
		ss << "EnumDeclaration: " << name << " {\n";
		for (auto &case_: cases) {
			ss << case_->toString() << "\n";
		}
		ss << "}";
		return ss.str();
	}
};

class EnumCase : public AST {
private:
	std::string &name;
	std::unique_ptr<AST> &value;

public:
	EnumCase(std::string &name, std::unique_ptr<AST> &value, size_t line) : AST(ASTType::ENUM_CASE, line),
	                                                                        name(name),
	                                                                        value(value) {}

	std::string &getName() const {
		return this->name;
	}

	std::unique_ptr<AST> &getValue() {
		return this->value;
	}

	std::string toString() const override {
		return "case " + name + " = " + value->toString();
	}
};

class VariableDeclaration : public AST {
private:
	std::string &name;
	std::unique_ptr<AST> &type;
	std::unique_ptr<AST> &value;
	bool has_value;

public:
	VariableDeclaration(std::string &name, std::unique_ptr<AST> &type, std::unique_ptr<AST> &value, size_t line,
	                    bool has_value) : AST(ASTType::VARIABLE_DECLARATION,
	                                          line), name(name), type(type),
	                                      value(value), has_value(has_value) {}

	std::string &getName() const {
		return this->name;
	}

	std::unique_ptr<AST> &getType() {
		return this->type;
	}

	std::unique_ptr<AST> &getValue() {
		return this->value;
	}

	std::string toString() const override {
		if (has_value) {
			return type->toString() + " " + name + " = " + value->toString();
		} else {
			return type->toString() + " " + name;
		}
	}
};

#endif //DRAST_AST_H