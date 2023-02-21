#include "typelogic.hpp"

namespace typelogic {

    bool canPrint(TypeInfo* type) {
        if(type->type == TypeInfo::M_REF)
            return canPrint(type->asRef()->atype);
        if(type->type != TypeInfo::PRIMITIVE)
            return false;
        auto ptype = type->asPrimitive()->ptype;
        if(ptype >= PrimitiveTypeInfo::INT8 && ptype <= PrimitiveTypeInfo::INT64)
            return true;
        switch(ptype) {
            case PrimitiveTypeInfo::CHAR: 
            case PrimitiveTypeInfo::STRING: 
            case PrimitiveTypeInfo::BOOL:
                return true;
            default:
                return false;
        }
    }

    string print(Variable v) {
        if(v.typeinfo->type == TypeInfo::M_REF)
            return print(v.unref());
        auto ptype = v.typeinfo->asPrimitive()->ptype;
        osstream ss;
        if(ptype == PrimitiveTypeInfo::INT8)
            ss << (short int)(*(char*)v.value);
        else if(ptype == PrimitiveTypeInfo::INT16)
            ss << *(short int*)v.value;
        else if(ptype == PrimitiveTypeInfo::INT32)
            ss << *(int*)v.value;
        else if(ptype == PrimitiveTypeInfo::INT64)
            ss << *(long long*)v.value;
        else if(ptype == PrimitiveTypeInfo::CHAR)
            ss << *(char*)v.value;
        else if(ptype == PrimitiveTypeInfo::STRING)
            ss << *(string*)v.value;
        else if(ptype == PrimitiveTypeInfo::BOOL)
            if(*(bool*)v.value)
                return "true";
            else 
                return "false";
        return ss.str();
    }

    bool canScan(TypeInfo* ti) {
        if(ti->type != TypeInfo::PRIMITIVE)
            return false;
        auto ptype = ti->asPrimitive()->ptype;
        if(ptype >= PrimitiveTypeInfo::INT8 && ptype <= PrimitiveTypeInfo::INT64)
            return true;
        return ptype == PrimitiveTypeInfo::CHAR || ptype == PrimitiveTypeInfo::STRING;
    }

    void* scan(std::istream& is, TypeInfo* ti) {
        auto ptype = ti->asPrimitive()->ptype;
        if(ptype == PrimitiveTypeInfo::INT8) {
            auto cv = new char;
            short int c;
            is >> c;
            *cv = c;
            return cv;
        }
        else if(ptype == PrimitiveTypeInfo::INT16) {
            auto c = new short int;
            is >> *c;
            return c;
        }
        else if(ptype == PrimitiveTypeInfo::INT32) {
            auto c = new int;
            is >> *c;
            return c;
        }
        else if(ptype == PrimitiveTypeInfo::INT64) {
            auto c = new long long;
            is >> *c;
            return c;
        } 
        else if(ptype == PrimitiveTypeInfo::CHAR) {
            auto c = new char;
            is >> *c;
            return c;
        }
        else if(ptype == PrimitiveTypeInfo::STRING) {
            auto s = new string;
            is >> *s;
            return s;
        }
    }

    bool canCoerce(TypeInfo* fromType, TypeInfo* toType) {
        if(toType == fromType)
            return true;
        if(fromType->type == TypeInfo::M_REF)
            return canCoerce(toType, fromType->asRef()->atype);
        if(toType->type != TypeInfo::PRIMITIVE || fromType->type != TypeInfo::PRIMITIVE)
            return false;
        if(toType->asPrimitive()->ptype <= PrimitiveTypeInfo::INT64 && fromType->asPrimitive()->ptype <= PrimitiveTypeInfo::INT64)
            return true;
        if(fromType->asPrimitive()->ptype == PrimitiveTypeInfo::CHAR && toType->asPrimitive()->ptype == PrimitiveTypeInfo::STRING)
            return true;
        return false;
    }

    Variable coerce(Variable v, TypeInfo* toType) {
        if(v.typeinfo == toType)
            return v.copy();
        if(v.typeinfo->type == TypeInfo::M_REF)
            return coerce(v.unref(), toType);
        if(v.typeinfo->asPrimitive()->ptype < 4 && toType->asPrimitive()->ptype < 4) {
            Variable nv(toType);
            long long value = v.typeinfo->asPrimitive()->toLL(v.value);
            toType->asPrimitive()->fromLL(value, nv.value);
            return nv;
        }
        if(v.typeinfo->asPrimitive()->ptype == PrimitiveTypeInfo::CHAR && toType->asPrimitive()->ptype == PrimitiveTypeInfo::STRING)
            return Variable(toType, new string(1, *(char*)v.value));
        return Variable();
    }

    TypeInfo* canUnaryOp(Token::Type opType, TypeInfo* ti) {
        if(ti->type == TypeInfo::M_REF)
            return canUnaryOp(opType, ti->asRef()->atype);
        if(ti->type != TypeInfo::PRIMITIVE)
            return nullptr;
        auto ptype = ti->asPrimitive()->ptype;
        if(ptype == PrimitiveTypeInfo::BOOL && opType == Token::NEGATE)
            return PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::BOOL];
        if(ptype <= PrimitiveTypeInfo::INT64) {
            if(opType == Token::PLUS || opType == Token::MINUS)
                return ti;
        }
        return nullptr;
    }

    Variable unaryOp(Token::Type opType, Variable v) {
        if(v.typeinfo->type == TypeInfo::M_REF)
            return unaryOp(opType, v.unref());
        auto pt = v.typeinfo->asPrimitive()->ptype;
        if(pt == PrimitiveTypeInfo::BOOL && opType == Token::NEGATE) {
            Variable vres = v.copy();
            bool b = *(bool*)v.value;
            b = !b;
            vres.copyFrom(&b);
            return vres;
        }
        if(opType == Token::PLUS)
            return v.copy();
        if(opType == Token::MINUS) {
            Variable vres = v.copy();
            if(pt == PrimitiveTypeInfo::INT8) {
                auto c = *(char*)v.value;
                c = -c;
                vres.copyFrom(&c);
            }
            else if(pt == PrimitiveTypeInfo::INT16) {
                auto c = *(short int*)v.value;
                c = -c;
                vres.copyFrom(&c);
            }
            else if(pt == PrimitiveTypeInfo::INT32) {
                auto c = *(int*)v.value;
                c = -c;
                vres.copyFrom(&c);
            }
            else if(pt == PrimitiveTypeInfo::INT64) {
                auto c = *(long long*)v.value;
                c = -c;
                vres.copyFrom(&c);
            }
            return vres;  
        }
    }

    TypeInfo* canUnaryOpInt(Token::Type opType, TypeInfo* ti) {
        if(ti->type != TypeInfo::M_REF)
            return nullptr;
        if(ti->asRef()->atype->type != TypeInfo::PRIMITIVE)
            return nullptr;
        PrimitiveTypeInfo* tt = ti->asRef()->atype->asPrimitive();
        if(tt->ptype > PrimitiveTypeInfo::INT64)
            return nullptr;
        return tt;
    }

    Variable unaryOpInt(Token::Type opType, Variable v) {
        Variable tv = v.unref();
        PrimitiveTypeInfo* tt = tv.typeinfo->asPrimitive();
        long long ca = tt->toLL(tv.value);
        switch(opType) {
            case Token::MINUSMINUS_PREF: {
                tt->fromLL(ca-1, tv.value);
                return tv.copy();
            }
            case Token::MINUSMINUS_POST: {
                Variable rv = tv.copy();
                tt->fromLL(ca-1, tv.value);
                return rv;
            }
            case Token::PLUSPLUS_PREF: {
                tt->fromLL(ca+1, tv.value);
                return tv.copy();
            }
            case Token::PLUSPLUS_POST: {
                Variable rv = tv.copy();
                tt->fromLL(ca+1, tv.value);
                return rv;
            }
        }
    }

    TypeInfo* canBinaryOp(Token::Type opType, TypeInfo* first, TypeInfo* second) {
        if(first->type == TypeInfo::M_REF)
            return canBinaryOp(opType, first->asRef()->atype, second);
        if(second->type == TypeInfo::M_REF)
            return canBinaryOp(opType, first, second->asRef()->atype);
        if(first->type != TypeInfo::PRIMITIVE || second->type != TypeInfo::PRIMITIVE)
            return nullptr;
        PrimitiveTypeInfo* atype = first->asPrimitive();
        PrimitiveTypeInfo* btype = second->asPrimitive();
        if(opType == Token::ANDAND || opType == Token::OROR) {
            if(atype->ptype == PrimitiveTypeInfo::BOOL && btype->ptype == PrimitiveTypeInfo::BOOL)
                return PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::BOOL];
        }
        else if(opType == Token::EQEQ || opType == Token::Token::NEQ) {
            if(atype->ptype == btype->ptype && (atype->ptype == PrimitiveTypeInfo::STRING || atype->ptype == PrimitiveTypeInfo::BOOL || atype->ptype == PrimitiveTypeInfo::CHAR))
                return PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::BOOL];
        }
        else if(opType == Token::PLUS) {
            if(atype->ptype == PrimitiveTypeInfo::STRING && btype->ptype == PrimitiveTypeInfo::STRING)
                return PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::STRING];
            if(atype->ptype == PrimitiveTypeInfo::STRING && btype->ptype == PrimitiveTypeInfo::CHAR)
                return PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::STRING];
            if(atype->ptype == PrimitiveTypeInfo::CHAR && btype->ptype == PrimitiveTypeInfo::STRING)
                return PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::STRING];
        }
        if(atype->ptype <= PrimitiveTypeInfo::INT64 && btype->ptype <= PrimitiveTypeInfo::INT64) {
            auto pt = atype->ptype > btype->ptype ? atype->ptype : btype->ptype;
            switch(opType) {
                case Token::GT:
                case Token::LT:
                case Token::GEQ:
                case Token::LEQ:
                case Token::EQEQ:
                case Token::NEQ:
                    return PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::BOOL];
                case Token::PLUS:
                case Token::MINUS:
                case Token::DIV:
                case Token::STAR:
                    return PrimitiveTypeInfo::primitivePtrs[pt];
            }
        }
        return nullptr;
    }

    Variable binaryOp(Token::Type opType, Variable a, Variable b) {
        if(a.typeinfo->type == TypeInfo::M_REF)
            return binaryOp(opType, a.unref(), b);
        if(b.typeinfo->type == TypeInfo::M_REF)
            return binaryOp(opType, a, b.unref());
        PrimitiveTypeInfo* ta = a.typeinfo->asPrimitive();
        PrimitiveTypeInfo* tb = b.typeinfo->asPrimitive();
        TypeInfo* boolType = PrimitiveTypeInfo::primitivePtrs[PrimitiveTypeInfo::BOOL];
        if(opType == Token::PLUS) {
            if(ta->ptype == PrimitiveTypeInfo::CHAR && tb->ptype == PrimitiveTypeInfo::STRING) {
                string v = string(*(string*)b.value);
                char c = *(char*)a.value;
                return Variable(tb, new string(c+v));
            }
            if(ta->ptype == PrimitiveTypeInfo::STRING && tb->ptype == PrimitiveTypeInfo::CHAR) {
                string v = string(*(string*)a.value);
                char c = *(char*)b.value;
                return Variable(ta, new string(v+c));
            }
            if(ta->ptype == PrimitiveTypeInfo::STRING && tb->ptype == PrimitiveTypeInfo::STRING) {
                string v = string(*(string*)a.value);
                string w = string(*(string*)b.value);
                return Variable(ta, new string(v+w));
            }
        }
        if(opType == Token::EQEQ || opType == Token::NEQ) {
            if(ta->ptype == PrimitiveTypeInfo::STRING && tb->ptype == PrimitiveTypeInfo::STRING) {
                const string v = *(string*)a.value;
                const string w = *(string*)b.value;
                if(opType == Token::EQEQ)
                    return Variable(boolType, new bool(v == w));
                else
                    return Variable(boolType, new bool(v != w));
            }
            if(ta->ptype == PrimitiveTypeInfo::BOOL && tb->ptype == PrimitiveTypeInfo::BOOL) {
                auto v = *(bool*)a.value;
                auto w = *(bool*)b.value;
                if(opType == Token::EQEQ)
                    return Variable(boolType, new bool(v == w));
                else
                    return Variable(boolType, new bool(v != w));
            }
            if(ta->ptype == PrimitiveTypeInfo::CHAR && tb->ptype == PrimitiveTypeInfo::CHAR) {
                auto v = *(char*)a.value;
                auto w = *(char*)b.value;
                if(opType == Token::EQEQ)
                    return Variable(boolType, new bool(v == w));
                else
                    return Variable(boolType, new bool(v != w));
            }
        }
        if(ta->ptype == PrimitiveTypeInfo::BOOL && tb->ptype == PrimitiveTypeInfo::BOOL) {
            auto v = *(bool*)a.value;
            auto w = *(bool*)b.value;
            if(opType == Token::ANDAND)
                return Variable(boolType, new bool(v && w));
            if(opType == Token::OROR)
                return Variable(boolType, new bool(v || w));
        }
        if(ta->ptype < 4 && tb->ptype < 4) {
            long long ca = ta->toLL(a.value), cb = tb->toLL(b.value), cc;
            if(BinaryOpSN::isBooleanOp(opType)) {
                Variable v(boolType);
                bool res;
                switch(opType) {
                    case Token::EQEQ: res = ca == cb; break;
                    case Token::NEQ:  res = ca != cb; break;
                    case Token::GT:   res = ca >  cb; break;
                    case Token::LT:   res = ca <  cb; break;
                    case Token::GEQ:  res = ca >= cb; break;
                    case Token::LEQ:  res = ca <= cb; break;
                }
                v.copyFrom(&res);
                return v;
            }
            PrimitiveTypeInfo* tc = ta->ptype > tb->ptype ? ta : tb;
            Variable v(tc);
            if(opType == Token::PLUS)
                cc = ca+cb;
            else if(opType == Token::MINUS)
                cc = ca-cb;
            else if(opType == Token::STAR)
                cc = ca*cb;
            else if(opType == Token::DIV)
                cc = ca/cb;
            tc->fromLL(cc, v.value);
            return v;        
        }
    }

    TypeInfo* canBinaryOpInt(Token::Type opType, TypeInfo* first, TypeInfo* second) {
        if(second->type == TypeInfo::M_REF)
            return canBinaryOpInt(opType, first, second->asRef()->atype);
        if(first->type != TypeInfo::M_REF)
            return nullptr;
        if(first->asRef()->atype->type != TypeInfo::PRIMITIVE || second->type != TypeInfo::PRIMITIVE)
            return nullptr;
        PrimitiveTypeInfo* au = first->asRef()->atype->asPrimitive();
        PrimitiveTypeInfo* bu = second->asPrimitive();
        if(opType == Token::EQ)
            return canCoerce(bu, au) ? au : nullptr;
        if(opType == Token::PLUS_EQ && au->ptype == PrimitiveTypeInfo::STRING)
            if(bu->ptype == PrimitiveTypeInfo::CHAR || bu->ptype == PrimitiveTypeInfo::STRING)
                return au;
        if(au->ptype <= PrimitiveTypeInfo::INT64 && bu->ptype <= PrimitiveTypeInfo::INT64)
            return au;
        return nullptr;
    }

    Variable binaryOpInt(Token::Type opType, Variable a, Variable b) {
        if(b.typeinfo->type == TypeInfo::M_REF)
            return binaryOpInt(opType, a, b.unref());
        if(opType == Token::EQ) {
            Variable c = coerce(b, a.unref().typeinfo);
            a.unref().copyFrom(c.value);
            return c;
        }
        PrimitiveTypeInfo* au = a.unref().typeinfo->asPrimitive();
        if(au->ptype == PrimitiveTypeInfo::STRING) {
            if(b.typeinfo->asPrimitive()->ptype == PrimitiveTypeInfo::CHAR) {
                char c = *(char*)b.value;
                string* s = (string*)a.unref().value;
                s->append(1, c);
            }
            if(b.typeinfo->asPrimitive()->ptype == PrimitiveTypeInfo::STRING) {
                const string s1 = *(const string*)b.value;
                string* s2 = (string*)a.unref().value;
                s2->append(s1);
            }
            return a.unref().copy();
        }
        if(au->ptype <= PrimitiveTypeInfo::INT64 && b.typeinfo->asPrimitive()->ptype <= PrimitiveTypeInfo::INT64) {
            switch(opType) {
                case Token::PLUS_EQ: {
                    Variable c = binaryOp(Token::PLUS, a.unref(), b);
                    a.unref().copyFrom(c.value);
                    return c;
                }
                case Token::MINUS_EQ: {
                    Variable c = binaryOp(Token::MINUS, a.unref(), b);
                    a.unref().copyFrom(c.value);
                    return c;
                }
                case Token::STAR_EQ: {
                    Variable c = binaryOp(Token::STAR, a.unref(), b);
                    a.unref().copyFrom(c.value);
                    return c;
                }
                case Token::DIV_EQ: {
                    Variable c = binaryOp(Token::DIV, a.unref(), b);
                    a.unref().copyFrom(c.value);
                    return c;
                }
            }
        }
    }

}