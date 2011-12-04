/**
 * @file java_classfile.h
 * @desc interface to Java class files
 *
 * @author cjeong
 */
#ifndef JAVA_CLASSFILE_H
#define JAVA_CLASSFILE_H

#include <stdio.h>
#include <vector>
#include "java/java_base.h"
#include "java/java_instr.h"
#include "java/java_type.h"

#define JAVA_CLASSFILE_MAGIC            0xCAFEBABE

/* class access and property flags */
#define JAVA_CLASS_ACC_PUBLIC           0x0001
#define JAVA_CLASS_ACC_FINAL            0x0010
#define JAVA_CLASS_ACC_SUPER            0x0020
#define JAVA_CLASS_ACC_INTERFACE        0x0200
#define JAVA_CLASS_ACC_ABSTRACT         0x0400
#define JAVA_CLASS_ACC_SYNTETIC         0x1000
#define JAVA_CLASS_ACC_ANNOTATION       0x2000
#define JAVA_CLASS_ACC_ENUM             0x4000

/* nested class access and property flags */
#define JAVA_INNER_CLASS_ACC_PUBLIC     0x0001
#define JAVA_INNER_CLASS_ACC_PRIVATE    0x0002
#define JAVA_INNER_CLASS_ACC_PROTECTED  0x0004
#define JAVA_INNER_CLASS_ACC_STATIC     0x0008
#define JAVA_INNER_CLASS_ACC_FINAL      0x0010
#define JAVA_INNER_CLASS_ACC_INTERFACE  0x0200
#define JAVA_INNER_CLASS_ACC_ABSTRACT   0x0400

/* field access and property flags */
#define JAVA_FIELD_ACC_PUBLIC           0x0001
#define JAVA_FIELD_ACC_PRIVATE          0x0002 
#define JAVA_FIELD_ACC_PROTECTED        0x0004
#define JAVA_FIELD_ACC_STATIC           0x0008
#define JAVA_FIELD_ACC_FINAL            0x0010
#define JAVA_FIELD_ACC_VOLATILE         0x0040
#define JAVA_FIELD_ACC_TRANSIENT        0x0080

/* method access and property flags */
#define JAVA_METHOD_ACC_PUBLIC          0x0001
#define JAVA_METHOD_ACC_PRIVATE         0x0002 
#define JAVA_METHOD_ACC_PROTECTED       0x0004
#define JAVA_METHOD_ACC_STATIC          0x0008
#define JAVA_METHOD_ACC_FINAL           0x0010
#define JAVA_METHOD_ACC_SYNCHRONIZED    0x0020
#define JAVA_METHOD_ACC_NATIVE          0x0100
#define JAVA_METHOD_ACC_ABSTRACT        0x0400
#define JAVA_METHOD_ACC_STRICT          0x0800

/* Java major version */
#define JAVA_VERSION_J2SE_7                 51
#define JAVA_VERSION_J2SE_6                 50
#define JAVA_VERSION_J2SE_5                 49
#define JAVA_VERIONS_JDK_1_4                48
#define JAVA_VERSION_JDK_1_3                47
#define JAVA_VERSION_JDK_1_2                46
#define JAVA_VERSION_JDK_1_1                45

class JavaClassFile;

/* info items that constitute a single Java classfile */
class JavaClassFileInfo {
public:
  enum JavaClassFileInfoE {
    ConstInfo,
    FieldInfo,
    MethodInfo,
    AttrInfo
  };

private:
  JavaClassFileInfoE _infoCode;

public:
  JavaClassFileInfo(JavaClassFileInfoE e) : _infoCode(e) { }
  ~JavaClassFileInfo() { }
};

/* constant pool entries */
class JavaConstInfo : public JavaClassFileInfo  {
public:
  enum JavaConstE {
    ConstClass              =  7,
    ConstFieldref           =  9,
    ConstMethodref          = 10,
    ConstInterfaceMethodref = 11,
    ConstString             =  8,
    ConstInteger            =  3,
    ConstFloat              =  4,
    ConstLong               =  5,
    ConstDouble             =  6,
    ConstNameAndType        = 12,
    ConstUtf8               =  1
  };

private:
  JavaConstE _tag;

public:
  JavaConstInfo(JavaConstE e) : 
    JavaClassFileInfo(ConstInfo), _tag(e) { }
  ~JavaConstInfo() { }

  JavaConstE tag() { return _tag; }
};

class JavaClassInfo : public JavaConstInfo {
public:
  u2 nameIndex;

public:
  JavaClassInfo(u2 n) : 
    JavaConstInfo(ConstClass), nameIndex(n) { }
  ~JavaClassInfo() { }
};

class JavaFieldrefInfo : public JavaConstInfo {
public:
  u2 classIndex;
  u2 nameAndTypeIndex;

public:
  JavaFieldrefInfo(u2 c, u2 n) : 
    JavaConstInfo(ConstFieldref) , classIndex(c), nameAndTypeIndex(n) { }
  ~JavaFieldrefInfo() { }
};

class JavaMethodrefInfo : public JavaConstInfo {
public:
  u2 classIndex;
  u2 nameAndTypeIndex;

public:
  JavaMethodrefInfo(u2 c, u2 n) : 
    JavaConstInfo(ConstMethodref), classIndex(c), nameAndTypeIndex(n) { }
  ~JavaMethodrefInfo() { }
};

class JavaInterfaceMethodrefInfo : public JavaConstInfo {
public:
  u2 classIndex;
  u2 nameAndTypeIndex;

public:
  JavaInterfaceMethodrefInfo(u2 c, u2 n) :
    JavaConstInfo(ConstInterfaceMethodref), classIndex(c), 
    nameAndTypeIndex(n) { }
  ~JavaInterfaceMethodrefInfo() { }
};

class JavaStringInfo : public JavaConstInfo {
public:
  u2 stringIndex;

public:
  JavaStringInfo(u2 s) : 
    JavaConstInfo(ConstString), stringIndex(s) { }
  ~JavaStringInfo() { }
};

class JavaIntegerInfo : public JavaConstInfo {
public:
  u4 bytes;

public:
  JavaIntegerInfo(u4 b) : 
    JavaConstInfo(ConstInteger), bytes(b) { }
  ~JavaIntegerInfo() { }
};

class JavaFloatInfo : public JavaConstInfo {
public:
  u4 bytes;

public:
  JavaFloatInfo(u4 b) : 
    JavaConstInfo(ConstFloat), bytes(b) { }
  ~JavaFloatInfo() { }
};

class JavaLongInfo : public JavaConstInfo {
public:
  u4 highBytes;
  u4 lowBytes;
  
public:
  JavaLongInfo(u4 h, u4 l) : 
    JavaConstInfo(ConstLong), highBytes(h), lowBytes(l) { }
  ~JavaLongInfo() { }
};

class JavaDoubleInfo : public JavaConstInfo {
public:
  u4 highBytes;
  u4 lowBytes;

public:
  JavaDoubleInfo(u4 h, u4 l) : 
    JavaConstInfo(ConstDouble), highBytes(h), lowBytes(l) { }
  ~JavaDoubleInfo() { }
};

class JavaNameAndTypeInfo : public JavaConstInfo {
public:
  u2 nameIndex;
  u2 descIndex;

public:
  JavaNameAndTypeInfo(u2 n, u2 d) : 
    JavaConstInfo(ConstNameAndType), nameIndex(n), descIndex(d) { }
  ~JavaNameAndTypeInfo();
};

class JavaUtf8Info : public JavaConstInfo {
public:
  u2 length;
  char *bytes;

public:
  JavaUtf8Info(u2 l, char *s);
  ~JavaUtf8Info();
};

/* field info */
class JavaAttr;
class JavaFieldInfo : public JavaClassFileInfo {
private:
  unsigned _accessFlags : 16;
  unsigned _nameIndex   : 16;
  unsigned _descIndex   : 16;
  unsigned _pad         : 16;

  std::vector<JavaAttr *> _attributes;
  
public:
  JavaFieldInfo(u2 flags, u2 n, u2 d);
  ~JavaFieldInfo() { }

  GET_SET(u2, _accessFlags, accessFlags);
  
  std::vector<JavaAttr *>& attributes() { return _attributes; }
};

/* method info */
class JavaCodeAttr;
class JavaMethodInfo : JavaClassFileInfo {
private:
  unsigned _accessFlags : 16;
  unsigned _nameIndex   : 16;
  unsigned _descIndex   : 16;
  unsigned _pad         : 16;

  std::vector<JavaAttr *> _attributes;
  JavaCodeAttr *_codeAttr;
  
public:
  JavaMethodInfo(u2 flags, u2 n, u2 d);
  ~JavaMethodInfo() { }

  GET_SET(u2, _accessFlags, accessFlags);
  GET_SET(JavaCodeAttr *, _codeAttr, codeAttr);

  std::vector<JavaAttr *>& attributes() { return _attributes; }
};

/* attributes */
class JavaAttr : public JavaClassFileInfo {
public:
  enum JavaAttrE {
    AttrConstantValue,
    AttrCode,
    AttrStackMapTable,
    AttrExceptions,
    AttrInnerClasses,
    AttrEnclosingMethod,
    AttrSynthetic,
    AttrSourceFile,
    AttrSourceDebugExtension,
    AttrLineNumberTable,
    AttrLocalVariableTable,
    AttrLocalVariableTypeTable,
    AttrDeprecated,
    AttrRuntimeVisibleAnnotations,
    AttrRuntimeInvisibleAnnotations,
    AttrRuntimeVisibleParameterAnnotations,
    AttrRuntimeInvisibleParameterAnnotations,
    AttrAnnotationDefault,
    AttrUserDefined
  };

private:
  JavaAttrE _attrCode;

public:
  JavaAttr(JavaAttrE c) : JavaClassFileInfo(AttrInfo), _attrCode(c) { }
  ~JavaAttr() { }

  GET_SET(JavaAttrE, _attrCode, attrCode);
};

class JavaConstantValueAttr : public JavaAttr {
private:
  u2 _valueIndex;

public:
  JavaConstantValueAttr(u2 v) : JavaAttr(AttrConstantValue), _valueIndex(v) { }
  ~JavaConstantValueAttr() { }

  GET_SET(u2, _valueIndex, valueIndex);
};

typedef struct JavaException {
  u2 start_pc;
  u2 end_pc;
  u2 handler_pc;
  u2 catch_type;
} JavaException_t, *JavaException_p;

class JavaCodeAttr : public JavaAttr {
private:
  u2 _nameIndex;
  u4 _length;
  u2 _maxStack;
  u2 _maxLocals;
  u4 _codeLength;
  std::vector<u1> _code;
  std::vector<JavaException_p> _exceptions;
  std::vector<JavaAttr *> _attributes;
  
public:
  JavaCodeAttr() : JavaAttr(AttrCode) { }
  ~JavaCodeAttr();

  GET_SET(u2, _maxStack, maxStack);
  GET_SET(u2, _maxLocals, maxLocals);

  std::vector<u1>& code() { return _code; }
  std::vector<JavaException_p>& exceptions() { return _exceptions; }
  std::vector<JavaAttr *>& attributes() { return _attributes; }
};

class JavaVTypeInfo {
public:
  enum JavaVarInfoE {
    VarInfoTop,
    VarInfoInteger,
    VarInfoFloat,
    VarInfoLong,
    VarInfoDouble,
    VarInfoNull,
    VarInfoUninitializedThis,
    VarInfoObject,
    VarInfoUninitialized
  };

private:
  unsigned _tag        :  8;
  unsigned _cpoolIndex : 16;
  unsigned _pad        :  8;
  
public: 
  JavaVTypeInfo(JavaVarInfoE t) : _tag(t) { }
  ~JavaVTypeInfo() { }
  
  GET_SET(u1, _tag, tag);
  GET_SET(u2, _cpoolIndex, cpoolIndex);
  GET_SET(u2, _cpoolIndex, offset);
};

class JavaStackMapFrame {
public:
  enum JavaStackFrameE {
    FrameSame                = 0,
    FrameSameLocals          = 1,
    FrameSameLocalsExtended  = 2,
    FrameChop                = 3,
    FrameSameExtended        = 4,
    FrameAppend              = 5,
    FrameFull                = 6
  };

private:
  unsigned _tag         : 8;
  unsigned _frameType   : 8;
  unsigned _offsetDelta : 16;
  
  std::vector<JavaVTypeInfo *> _stack;
  std::vector<JavaVTypeInfo *> _locals;

public:
  JavaStackMapFrame(u1 t);
  ~JavaStackMapFrame() { }

  GET_SET(JavaStackFrameE, _tag, tag);
  GET_SET(u1, _frameType, frameType);
  GET_SET(u2, _offsetDelta, offsetDelta);

  std::vector<JavaVTypeInfo *>& locals() { return _locals; }
  std::vector<JavaVTypeInfo *>& stack() { return _stack; }
};

class JavaStackMapTableAttr : public JavaAttr {
private:
  std::vector<JavaStackMapFrame *> _frames;

public:
  JavaStackMapTableAttr() : JavaAttr(AttrStackMapTable) { }
  ~JavaStackMapTableAttr();

  std::vector<JavaStackMapFrame *>& frames() { return _frames; }
};

class JavaExceptionsAttr : public JavaAttr {
private:
  std::vector<JavaClassInfo *> _exceptions;

public:
  JavaExceptionsAttr() : JavaAttr(AttrExceptions) { }
  ~JavaExceptionsAttr() { }

  std::vector<JavaClassInfo *>& exceptions() { return _exceptions; }
};

typedef struct JavaInnerClass {
  u2 inner_class_info_idx;
  u2 outer_class_info_idx;
  u2 inner_name_idx;
  u2 inner_class_access_flags;
} JavaInnerClass_t, *JavaInnerClass_p;
class JavaInnerClassesAttr : public JavaAttr {
private:
  std::vector<JavaInnerClass_p> _classes;
  
public:
  JavaInnerClassesAttr() : JavaAttr(AttrInnerClasses) { }
  ~JavaInnerClassesAttr() { }

  std::vector<JavaInnerClass_p>& classes() { return _classes; }
};

class JavaSyntheticAttr : public JavaAttr {
public:
  JavaSyntheticAttr() : JavaAttr(AttrSynthetic) { }
  ~JavaSyntheticAttr() { }
};

class JavaSourceFileAttr : public JavaAttr {
private:
  u2 _sourceFileIndex;

public:
  JavaSourceFileAttr(u2 s) : JavaAttr(AttrSourceFile), _sourceFileIndex(s) { }
  
  GET_SET(u2, _sourceFileIndex, sourceFileIndex);
};

typedef struct JavaLineNumber {
  u2 start_pc;
  u2 line_number;
} JavaLineNumber_t, *JavaLineNumber_p;

class JavaLineNumberTableAttr : public JavaAttr {
private:
  std::vector<JavaLineNumber_p> _lineNumbers;

public:
  JavaLineNumberTableAttr() : JavaAttr(AttrLineNumberTable) { }
  ~JavaLineNumberTableAttr();

  std::vector<JavaLineNumber_p>& lineNumbers() { return _lineNumbers; }
};

typedef struct JavaLocalVariable {
  u2 start_pc;
  u2 length;
  u2 name_idx;
  u2 desc_idx;
  u2 idx;
} JavaLocalVariable_t, *JavaLocalVariable_p;

class JavaLocalVariableTableAttr : public JavaAttr {
private:
  std::vector<JavaLocalVariable_p> _localVars;

public:
  JavaLocalVariableTableAttr() : JavaAttr(AttrLocalVariableTable) { }
  ~JavaLocalVariableTableAttr();

  std::vector<JavaLocalVariable_p>& localVars() { return _localVars; }
};

class JavaDeprecatedAttr : public JavaAttr {
public:
  JavaDeprecatedAttr() : JavaAttr(AttrDeprecated) { }
  ~JavaDeprecatedAttr() { }
};

/* Java class file */
class JavaClassFile : public JavaBase {
private:
  unsigned _minorVersion  : 16;
  unsigned _majorVersion  : 16;
  unsigned _thisClass     : 16;
  unsigned _superClass    : 16;
  unsigned _accessFlags   : 16;
  unsigned _pad           : 16;

  std::vector<u2> _interfaces;
  std::vector<JavaConstInfo *> _consts;
  std::vector<JavaFieldInfo *> _fields;
  std::vector<JavaMethodInfo *> _methods;
  std::vector<JavaAttr *> _attributes;

public:
  JavaClassFile(char *file);
  ~JavaClassFile();

  GET_SET(u2, _minorVersion, minorVersion);
  GET_SET(u2, _majorVersion, majorVersion);
  GET_SET(u2, _accessFlags, accessFlags);
  GET_SET(u2, _thisClass, thisClass);
  GET_SET(u2, _superClass, superClass);

  int numInterfaces() { return _interfaces.size(); }
  int numConsts() { return _consts.size(); }
  int numFields() { return _fields.size(); }
  int numMethods() { return _methods.size(); }
  int numAttrs() { return _attributes.size(); }

  std::vector<u2>& interfaces() { return _interfaces; }
  std::vector<JavaConstInfo *>& consts() { return _consts; }
  std::vector<JavaFieldInfo *>& fields() { return _fields; }
  std::vector<JavaMethodInfo *>& methods() { return _methods; }
  std::vector<JavaAttr *>& attributes() { return _attributes; }

  /* methods and fields for Java classfile reader */
private:
  FILE *_fp;

private:
  JavaConstInfo *readConstInfo();
  JavaMethodInfo *readMethodInfo();
  JavaFieldInfo *readFieldInfo();
  JavaException_p readException();
  JavaAttr *readAttr();
  JavaCodeAttr *readCodeAttr();
  JavaExceptionsAttr *readExceptionsAttr();
  JavaInnerClassesAttr *readInnerClassesAttr();
  JavaLineNumberTableAttr *readLineNumberTableAttr();
  JavaLocalVariableTableAttr *readLocalVariableTableAttr();
  JavaStackMapTableAttr *readStackMapTableAttr();
  JavaVTypeInfo *readVTypeInfo();
  JavaType *parseFieldDescriptor(char *s);
  JavaType *parseMethodDescriptor(char *s);
  JavaInstr *readInstr(u1 opcode, u2& skip_bytes);

  void dump();
};

#endif /* JAVA_CLASSFILE_H */
