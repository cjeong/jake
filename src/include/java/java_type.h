/**
 * @file java_type.h
 * @note Java datatypes
 *
 * @author cjeong
 */
#ifndef RZ_JAVA_TYPE_H
#define RZ_JAVA_TYPE_H

#include "common.h"


/* JavaType enum; when this type is changed, be sure to change the 
   type predicate macros (e.g. PRIMITIVE_TYPE_P) below */
enum JavaTypeE {
  JavaTypeInvalid    =  0, 
  JavaTypeUnknown    =  1,
  JavaTypeVoid       =  2,
  JavaTypeBoolean    =  3,  /* boolean type */
  JavaTypeByte       =  4,  /* integral types */
  JavaTypeChar       =  5,
  JavaTypeShort      =  6,
  JavaTypeInt        =  7,
  JavaTypeLong       =  8,
  JavaTypeFloat      =  9,  /* floating-point types */
  JavaTypeDouble     = 10,
  JavaTypeClass      = 11,  /* class or interface type */
  JavaTypeInterface  = 12,
  JavaTypeArray      = 13,  /* array type */
  JavaTypeTypeVar    = 14,  /* type variable */
};

#define PRIMITIVE_TYPE_P(p) \
  (  ( p ) >= JavaTypeBoolean && ( p ) <= JavaTypeChar )
#define INTEGRAL_TYPE_P(p) \
  (  ( p ) >= JavaTypeByte && ( p ) <= JavaTypeDouble )
#define FLOATINGPOINT_TYPE_P(p) \
  (  ( p ) == JavaTypeFloat || ( p ) <= JavaTypeDouble )
#define NUMERIC_TYPE_P(p) \
  (  ( p ) >= JavaTypeByte && ( p ) <= JavaTypeDouble )
#define REFERENCE_TYPE_P(p) \
  (  ( p ) >= JavaTypeClass && ( p ) <= JavaTypeArray )
#define VOID_TYPE_P(p) (  ( p ) == JavaTypeVoid )


class JavaType {
protected:
  JavaTypeE _tag;

public:
  JavaType(JavaTypeE t) : _tag(t) { }
  ~JavaType() { }

  GET_SET(JavaTypeE, _tag, tag);

  virtual int width() const = 0;
};

class JavaBooleanType : public JavaType {

public:
  JavaBooleanType() : JavaType(JavaTypeBoolean) { }
  ~JavaBooleanType() { }

  inline int width() const { return 1; }
};

class JavaByteType : public JavaType {
public:
  JavaByteType() : JavaType(JavaTypeByte) { }
  ~JavaByteType() { }

  inline int width() const { return 8; }
};

class JavaShortType : public JavaType {
public:
  JavaShortType() : JavaType(JavaTypeShort) { }
  ~JavaShortType() { }

  inline int width() const { return 16; }
};

class JavaIntType : public JavaType {
public:
  JavaIntType() : JavaType(JavaTypeInt) { }
  ~JavaIntType() { }

  inline int width() const { return 32; }
};

class JavaLongType : public JavaType {
public:
  inline int width() const { return 64; }
};

class JavaFloatType : public JavaType {
public:
  JavaFloatType() : JavaType(JavaTypeFloat) { }
  ~JavaFloatType() { }

  inline int width() const { return 32; }
};

class JavaDoubleType : public JavaType {
public:
  JavaDoubleType() : JavaType(JavaTypeDouble) { }
  ~JavaDoubleType() { }

  inline int width() const { return 64; }
};

class JavaCharType : public JavaType {
public:
  JavaCharType() : JavaType(JavaTypeChar) { }
  ~JavaCharType() { }

  inline int width() const { return 8; }
};

class JavaClassType : public JavaType {
public:
  JavaClassType() : JavaType(JavaTypeClass) { }
  ~JavaClassType() { }

  inline int width() const { return 0; }
};

class JavaInterfaceType : public JavaType {
public:
  JavaInterfaceType() : JavaType(JavaTypeInterface) { }
  ~JavaInterfaceType() { }

  inline int width() const { return 0; }
};

class JavaTypeVarType : public JavaType {

public:
  JavaTypeVarType() : JavaType(JavaTypeTypeVar) { }
  ~JavaTypeVarType() { }
  
  inline int width() const { return 0; }
};

class JavaArrayType : public JavaType {
private:
  std::vector<int> _dims;
  JavaType *_baseType;

public:
  JavaArrayType(JavaType *b) : JavaType(JavaTypeArray), _baseType(b) { }
  ~JavaArrayType() { }

  GET_SET(std::vector<int>&, _dims, dims);
  GET_SET(JavaType *, _baseType, baseType);

  int width() const;
};

class JavaVoidType : public JavaType {
public:
  JavaVoidType() : JavaType(JavaTypeVoid) { }
  ~JavaVoidType() { }

  inline int width() const { return -1; }
};

#endif /* RZ_JAVA_TYPE_H */
