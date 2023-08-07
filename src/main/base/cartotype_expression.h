/*
cartotype_expression.h
Copyright (C) 2009-2023 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_base.h>
#include <cartotype_errors.h>
#include <cartotype_string.h>
#include <cartotype_feature_info.h>
#include <set>
#include <float.h>

namespace CartoTypeCore
{

#ifndef NAN
static const double infinity { CartoTypeCore::CT_DBL_MAX * CartoTypeCore::CT_DBL_MAX };
static const double NAN { infinity * 0.0 };
#endif

using DimensionConverterFunction = std::function<double(const MString& aText,size_t& aLength)>;

/**
A class for expression values or the values of variables used in expressions.
If iString is non-null it is a string.
If iString is null it is a number unless iNumber is NAN (not a number),
in which case it is undefined.
*/
class ExpressionValue
    {
    public:
    ExpressionValue():
        iNumber(NAN)
        {
        }

    ExpressionValue(double aNumber):
        iNumber(aNumber)
        {
        }

    ExpressionValue(const MString& aString,bool aAllowFeatureTypeNames = false,DimensionConverterFunction aDimensionConverter = nullptr):
        iNumber(NAN)
        {
        if (!aString.IsEmpty())
            {
            iString = std::make_shared<String>(aString);
            iNumber = ValueOfLiteral(aString,true,aAllowFeatureTypeNames,aDimensionConverter);
            }
        }

    ExpressionValue(double aNumber,const MString& aString):
        iNumber(aNumber)
        {
        if (!aString.IsEmpty())
            iString = std::make_shared<String>(aString);
        }

    operator double() const { return iNumber; }
    int ToInt() const
        {
        return IsNan() ? 0 : int(iNumber);
        }

    int operator<(const ExpressionValue& aOther) const
        {
        if (iString && aOther.iString)
            return iString->Compare(*aOther.iString) < 0;
        return iNumber < aOther.iNumber;
        }
    int operator<=(const ExpressionValue& aOther) const
        {
        if (iString && aOther.iString)
            return iString->Compare(*aOther.iString) <= 0;
        return iNumber <= aOther.iNumber;
        }
    int operator==(const ExpressionValue& aOther) const
        {
        if (iString && aOther.iString)
            return *iString == *aOther.iString;
        return iNumber == aOther.iNumber ||
               (iNumber != iNumber && aOther.iNumber != aOther.iNumber); // if both numbers are NAN, they compare equal: both are undefined
        }
    int operator!=(const ExpressionValue& aOther) const
        {
        return int(!(*this == aOther));
        }
    int operator>=(const ExpressionValue& aOther) const
        {
        if (iString && aOther.iString) return
            iString->Compare(*aOther.iString) >= 0;
        return iNumber >= aOther.iNumber;
        }
    int operator>(const ExpressionValue& aOther) const
        {
        if (iString && aOther.iString) return
            iString->Compare(*aOther.iString) > 0;
        return iNumber > aOther.iNumber;
        }

    /** Case-folded, accent-folded or fuzzy comparison; reverts to equality for numbers. */
    int Compare(const ExpressionValue& aOther,StringMatchMethod aMethod) const
        {
        if (iString && aOther.iString)
            return iString->Compare(*aOther.iString,aMethod) == 0;
        return iNumber == aOther.iNumber ||
            (iNumber != iNumber && aOther.iNumber != aOther.iNumber); // if both numbers are NAN, they compare equal: both are undefined
        }

    /** Wild-card match: aOther can contain wild cards; reverts to equality for numbers. */
    int WildMatch(const ExpressionValue& aOther) const
        {
        if (iString && aOther.iString)
            return iString->WildMatch(*aOther.iString);
        return iNumber == aOther.iNumber ||
            (iNumber != iNumber && aOther.iNumber != aOther.iNumber); // if both numbers are NAN, they compare equal: both are undefined
        }

    bool IsTrue() const
        {
        return (iString && iString->Length() > 0) || (iNumber != 0 && iNumber == iNumber);
        }

    bool IsNan() const
        {
        return iNumber != iNumber;
        }

    const MString* StringValue() const { return iString ? &(*iString) : nullptr; }
    static double ValueOfLiteral(const MString& aText,bool aTolerateSyntaxError,bool aAllowFeatureTypeNames,DimensionConverterFunction aDimensionConverter);

    private:
    double iNumber;
    std::shared_ptr<String> iString;
    };

enum class ExpressionOpType
    {
    Value,
    Variable,
    
    UnaryMinus,
    BitwiseNot,
    LogicalNot,

    Multiply,
    Divide,
    Mod,
    Round,

    Plus,
    Minus,

    LeftShift,
    RightShift,

    LessThan,
    LessThanOrEqual,
    Equal,
    NotEqual,
    GreaterThanOrEqual,
    GreaterThan,

    BitwiseAnd,
    BitwiseXor,
    BitwiseOr,

    LogicalAnd,
    LogicalOr,

    InSet,
    NotInSet,
    InRange,
    NotInRange,

    // A range set is a set of ranges, each a pair of values: lower and upper limit.
    InRangeSet,
    NotInRangeSet,

    EqualIgnoreCase,
    EqualIgnoreAccents,
    EqualFuzzy,
    EqualWild,

    Concat,
    Subscript
    };

/**
An expression operator is part of an expression encoded in reverse polish notation (RPN).
Numbers and strings cause values to be pushed to a stack. Other operators are evaluated
using one or more stack values, and the result is pushed to the stack.
*/
class ExpressionOp
    {
    public:
    ExpressionOp()
        {
        }

    explicit ExpressionOp(ExpressionOpType aOpType):
        Type(aOpType)
        {
        }

    explicit ExpressionOp(ExpressionOpType aOpType,const MString& aString,double aNumber = 0):
        Type(aOpType),
        Number(aNumber),
        String(aString)
        {
        }

    explicit ExpressionOp(double aNumber):
        Number(aNumber)
        {
        }

    explicit ExpressionOp(InputStream& aInput);

    void Write(OutputStream& aOutput) const;

    /** The operator type. */
    ExpressionOpType Type = ExpressionOpType::Value;

    /**
    For ExpressionOpType::Value, the number;
    for ExpressionOpType::Variable, a variable index if >= 0;
    otherwise ignored.
    */
    double Number = 0;

    /**
    For ExpressionOpType::Value, the string;
    for ExpressionOpType::Variable, the variable name.
    */
    CartoTypeCore::String String;
    };

class RpnExpression
    {
    public:
    void Append(ExpressionOpType aOpType)
        {
        Exp.emplace_back(aOpType);
        }
    void Append(ExpressionOpType aOpType,const MString& aString,double aNumber = 0)
        {
        Exp.emplace_back(aOpType,aString,aNumber);
        }
    void Append(const ExpressionValue& aValue)
        {
        Exp.emplace_back(aValue);
        if (aValue.StringValue())
            Exp.back().String = *aValue.StringValue();
        }
    void Read(InputStream& aInput);
    void Write(OutputStream& aOutput) const;

    std::vector<ExpressionOp> Exp;
    };

class MVariableDictionary
    {
    public:
    virtual ~MVariableDictionary() { }

    /** Get the value of a variable from its name. */
    virtual bool Find(const MString& aName,ExpressionValue& aValue,const size_t* aSubscriptArray,size_t aSubscriptCount) const = 0;
    /** Get the value of a variable from its index. */
    virtual bool Find(int32_t aIndex,ExpressionValue& aValue,const size_t* aSubscriptArray,size_t aSubscriptCount) const = 0;
    };

/** A simple implementation of a variable dictionary. */
class VariableDictionary: public MVariableDictionary
    {
    public:
    bool Find(const MString& aKey,ExpressionValue& aValue,const size_t* /*aSubscriptArray*/,size_t /*aSubscriptCount*/) const override
        {
        auto p = iDictionary.find(aKey);
        if (p != iDictionary.end())
            {
            aValue = p->second;
            return true;
            }
        else
            return false;
        }
    bool Find(int32_t /*aIndex*/,ExpressionValue& /*aValue*/,const size_t* /*aSubscriptArray*/,size_t /*aSubscriptCount*/) const override
        {
        return false;
        }
    void Set(const String& aVariableName,const String& aValue)
        {
        if (aValue.Length())
            iDictionary[aVariableName] = aValue;
        else
            iDictionary.erase(aVariableName);
        }
    template<typename Functor> void Apply(Functor& aFunctor) { for (auto& p : iDictionary) { aFunctor(p.first,p.second); } }

    private:
    StringDictionary iDictionary;
    };

/**
An evaluator for simple expressions.
String expressions can be compiled into reverse-polish form for efficient storage.
*/
class ExpressionEvaluator
    {
    public:
    ExpressionEvaluator(const MVariableDictionary* aVariableDictionary = nullptr,DimensionConverterFunction aDimensionConverter = nullptr);
    void Evaluate(const MString& aExpression,double* aNumericResult,MString* aStringResult,bool* aLogicalResult);
    void EvaluateBracketedExpression(const MString& aExpression,double* aNumericResult,MString* aStringResult,bool* aLogicalResult,size_t* aLengthUsed,
                                     std::function<void(const String&)> aVariablesUsedHandler);
    void Evaluate(const RpnExpression& aExpression,double* aNumericResult,MString* aStringResult,bool* aLogicalResult);
    bool EvaluateLogical(const MString& aExpression);
    bool EvaluateLogical(const RpnExpression& aExpression);
    void Compile(const MString& aExpression,RpnExpression& aDestExpression);

    private:
    const MVariableDictionary* iVariableDictionary;
    DimensionConverterFunction iDimensionConverter;
    };

}

