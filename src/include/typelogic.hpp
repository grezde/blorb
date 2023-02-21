#pragma once
#include "types.hpp"

namespace typelogic {

    bool canCoerce(TypeInfo* fromType, TypeInfo* toType);
    Variable coerce(Variable v, TypeInfo* toType);
    
    TypeInfo* canUnaryOp(Token::Type opType, TypeInfo* type);
    Variable unaryOp(Token::Type opType, Variable v);

    TypeInfo* canUnaryOpInt(Token::Type opType, TypeInfo* type);
    Variable unaryOpInt(Token::Type opType, Variable v);

    TypeInfo* canBinaryOp(Token::Type opType, TypeInfo* atype, TypeInfo* btype);
    Variable binaryOp(Token::Type opType, Variable a, Variable b);

    TypeInfo* canBinaryOpInt(Token::Type opType, TypeInfo* atype, TypeInfo* btype);
    Variable binaryOpInt(Token::Type opType, Variable a, Variable b);

    bool canScan(TypeInfo* type);
    void* scan(std::istream& is, TypeInfo* type);
    
    bool canPrint(TypeInfo* type);
    string print(Variable v);

}