/**
 * @file java_instr.h
 * @note Java virtual machine instruction set
 *
 * @author cjeong
 */
#ifndef JAVA_INSTR_H
#define JAVA_INSTR_H

#include "java_base.h"

#define JOP_NOP              0x00  
#define JOP_ACONST_NULL      0x01
#define JOP_ICONST_M1        0x02
#define JOP_ICONST_0         0x03
#define JOP_ICONST_1         0x04
#define JOP_ICONST_2         0x05
#define JOP_ICONST_3         0x06
#define JOP_ICONST_4         0x07
#define JOP_ICONST_5         0x08
#define JOP_LCONST_1         0x09
#define JOP_LCONST_2         0x0A
#define JOP_FCONST_0         0x0B
#define JOP_FCONST_1         0x0C
#define JOP_FCONST_2         0x0D
#define JOP_DCONST_0         0x0E
#define JOP_DCONST_1         0x0F
#define JOP_BIPUSH           0x10
#define JOP_SIPUSH           0x11
#define JOP_LDC              0x12
#define JOP_LDC_W            0x13
#define JOP_LDC2_W           0x14
#define JOP_ILOAD            0x15
#define JOP_LLOAD            0x16
#define JOP_FLOAD            0x17
#define JOP_DLOAD            0x18
#define JOP_ALOAD            0x19
#define JOP_ILOAD_0          0x1A
#define JOP_ILOAD_1          0x1B
#define JOP_ILOAD_2          0x1C
#define JOP_ILOAD_3          0x1D
#define JOP_LLOAD_0          0x1E
#define JOP_LLOAD_1          0x1F
#define JOP_LLOAD_2          0x20
#define JOP_LLOAD_3          0x21
#define JOP_FLOAD_0          0x22
#define JOP_FLOAD_1          0x23
#define JOP_FLOAD_2          0x24
#define JOP_FLOAD_3          0x25
#define JOP_DLOAD_0          0x26
#define JOP_DLOAD_1          0x27
#define JOP_DLOAD_2          0x28
#define JOP_DLOAD_3          0x29
#define JOP_ALOAD_0          0x2A
#define JOP_ALOAD_1          0x2B
#define JOP_ALOAD_2          0x2C
#define JOP_ALOAD_3          0x2D
#define JOP_IALOAD           0x2E
#define JOP_LALOAD           0x2F
#define JOP_FALOAD           0x30
#define JOP_DALOAD           0x31
#define JOP_AALOAD           0x32
#define JOP_BALOAD           0x33
#define JOP_CALOAD           0x34
#define JOP_SALOAD           0x35
#define JOP_ISTORE           0x36
#define JOP_LSTORE           0x37
#define JOP_FSTORE           0x38
#define JOP_DSTORE           0x39
#define JOP_ASTORE           0x3A
#define JOP_ISTORE_0         0x3B
#define JOP_ISTORE_1         0x3C
#define JOP_ISTORE_2         0x3D
#define JOP_ISTORE_3         0x3E
#define JOP_LSTORE_0         0x3F
#define JOP_LSTORE_1         0x40
#define JOP_LSTORE_2         0x41
#define JOP_LSTORE_3         0x42
#define JOP_FSTORE_0         0x43
#define JOP_FSTORE_1         0x44
#define JOP_FSTORE_2         0x45
#define JOP_FSTORE_3         0x46
#define JOP_DSTORE_0         0x47
#define JOP_DSTORE_1         0x48
#define JOP_DSTORE_2         0x49
#define JOP_DSTORE_3         0x4A
#define JOP_ASTORE_0         0x4B
#define JOP_ASTORE_1         0x4C
#define JOP_ASTORE_2         0x4D
#define JOP_ASTORE_3         0x4E
#define JOP_IASTORE          0x4F
#define JOP_LASTORE          0x50
#define JOP_FASTORE          0x51
#define JOP_DASTORE          0x52
#define JOP_AASTORE          0x53
#define JOP_BASTORE          0x54
#define JOP_CASTORE          0x55
#define JOP_SASTORE          0x56
#define JOP_POP              0x57
#define JOP_POP2             0x58
#define JOP_DUP              0x59
#define JOP_DUP_X1           0x5A
#define JOP_DUP_X2           0x5B
#define JOP_DUP2             0x5C
#define JOP_DUP2_X1          0x5D
#define JOP_DUP2_X2          0x5E
#define JOP_SWAP             0x5F
#define JOP_IADD             0x60
#define JOP_LADD             0x61
#define JOP_FADD             0x62
#define JOP_DADD             0x63
#define JOP_ISUB             0x64
#define JOP_LSUB             0x65
#define JOP_FSUB             0x66
#define JOP_DSUB             0x67
#define JOP_IMUL             0x68
#define JOP_LMUL             0x69
#define JOP_FMUL             0x6A
#define JOP_DMUL             0x6B
#define JOP_IDIV             0x6C
#define JOP_LDIV             0x6D
#define JOP_FDIV             0x6E
#define JOP_DDIV             0x6F
#define JOP_IREM             0x70
#define JOP_LREM             0x71
#define JOP_FREM             0x72
#define JOP_DREM             0x73
#define JOP_INEG             0x74
#define JOP_LNEG             0x75
#define JOP_FNEG             0x76
#define JOP_DNEG             0x77
#define JOP_ISHL             0x78
#define JOP_LSHL             0x79
#define JOP_ISHR             0x7A
#define JOP_LSHR             0x7B
#define JOP_IUSHR            0x7C
#define JOP_LUSHR            0x7D
#define JOP_IAND             0x7E
#define JOP_LAND             0x7F
#define JOP_IOR              0x80
#define JOP_LOR              0x81
#define JOP_IXOR             0x82
#define JOP_LXOR             0x83
#define JOP_IINC             0x84
#define JOP_I2L              0x85
#define JOP_I2F              0x86
#define JOP_I2D              0x87
#define JOP_L2I              0x88
#define JOP_L2F              0x89
#define JOP_L2D              0x8A
#define JOP_F2I              0x8B
#define JOP_F2L              0x8C
#define JOP_F2D              0x8D
#define JOP_D2I              0x8E
#define JOP_D2L              0x8F
#define JOP_D2F              0x90
#define JOP_I2B              0x91
#define JOP_I2C              0x92
#define JOP_I2S              0x93
#define JOP_LCMP             0x94
#define JOP_FCMPL            0x95
#define JOP_FCMPG            0x96
#define JOP_DCMPL            0x97
#define JOP_DCMPG            0x98
#define JOP_IFEQ             0x99
#define JOP_IFNE             0x9A
#define JOP_IFLT             0x9B
#define JOP_IFGE             0x9C
#define JOP_IFGT             0x9D 
#define JOP_IFLE             0x9E
#define JOP_IF_ICMPEQ        0x9F
#define JOP_IF_ICMPNE        0xA0
#define JOP_IF_ICMPLT        0xA1
#define JOP_IF_ICMPGE        0xA2
#define JOP_IF_ICMPGT        0xA3
#define JOP_IF_ICMPLE        0xA4
#define JOP_IF_ACMPEQ        0xA5
#define JOP_IF_ACMPNE        0xA6
#define JOP_GOTO             0xA7
#define JOP_JSR              0xA8
#define JOP_RET              0xA9
#define JOP_TABLESWITCH      0xAA
#define JOP_LOOKUPSWITCH     0xAB
#define JOP_IRETURN          0xAC
#define JOP_LRETURN          0xAD
#define JOP_FRETURN          0xAE
#define JOP_DRETURN          0xAF
#define JOP_ARETURN          0xB0
#define JOP_RETURN           0xB1
#define JOP_GETSTATIC        0xB2
#define JOP_PUTSTATIC        0xB3
#define JOP_GETFIELD         0xB4
#define JOP_PUTFIELD         0xB5
#define JOP_INVOKEVIRTUAL    0xB6
#define JOP_INVOKESPECIAL    0xB7
#define JOP_INVOKESTATIC     0xB8
#define JOP_INVOKEINTERFACE  0xB9
#define JOP_UNUSED           0xBA
#define JOP_NEW              0xBB
#define JOP_NEWARRAY         0xBC
#define JOP_ANEWARRAY        0xBD
#define JOP_ARRAYLENGTH      0xBE
#define JOP_ATHROW           0xBF
#define JOP_CHECKCAST        0xC0
#define JOP_INSTANCEOF       0xC1
#define JOP_MONITORENTER     0xC2
#define JOP_MONITOREXIT      0xC3
#define JOP_WIDE             0xC4
#define JOP_MULTIANEWARRAY   0xC5
#define JOP_IFNULL           0xC6
#define JOP_IFNONNULL        0xC7
#define JOP_GOTO_W           0xC8
#define JOP_JSR_W            0xC9

/* reserved opcodes */
#define JOP_BREAKPOINT       0xCA
#define JOP_IMPDEP1          0xFE
#define JOP_IMPDEP2          0xFF

/* extended opcodes */
#define JOP_EXT1             0xCB


typedef struct {
  const char *str;    /* opcode string */
  int size;           /* size of the opcode in bytes */
} jop_info_t, *jop_p;

extern const jop_info_t jop_info[];


/* Java virtual machine instruction */
class JavaInstr {
public:
  enum JavaInstrTagE {
    JavaInstrConst,
    JavaInstrUnop,
    JavaInstrInc,
    JavaInstrBinop,
    JavaInstrPush,
    JavaInstrLoad,
    JavaInstrStore,
    JavaInstrConv,
    JavaInstrGet,
    JavaInstrReturn,
    JavaInstrInvoke,
    JavaInstrNew
  };

protected:
  JavaInstrTagE _tag;
  unsigned _opcode      : 16;
  unsigned _numOperands : 16;

public:
  JavaInstr(JavaInstrTagE t, u2 o) : _tag(t), _opcode(o) { }
  ~JavaInstr() { }

  GET_SET(JavaInstrTagE, _tag, tag);
  GET_SET(u2, _opcode, opcode);
  GET_SET(u2, _numOperands, numOperands);

  //  virtual char *toString() = 0;
};

/* ACONST_NUL, ICONST_M1, ICONST_0, ICONST_1, ICONST_2, ICONST_3, ICONST_4,
   ICONST_5, LCONST_1, LCONST_2, FCONST_0, FCONST_1, FCONST_2, DCONST_0,
   DCONST_1 */
class JavaConstInstr : public JavaInstr {
public:
  JavaConstInstr(u1 op);
  ~JavaConstInstr() { }
};

/* ILOAD, LLOAD, FLOAD, DLOAD, ALOAD, ILOAD_0, ILOAD_1, ILOAD_2, ILOAD_3,
   LLOAD_0, LLOAD_1, LLOAD_2, LLOAD_3, FLOAD_0, FLOAD_1, FLOAD_2, FLOAD_3,
   DLOAD_0, DLOAD_1, DLOAD_2, DLOAD_3, IALOAD, LALOAD, FALOAD, DALOAD,
   AALOAD, BALOAD, CALOAD, SALOAD, LDC, LDC_W, LDC2_W */
class JavaLoadInstr : public JavaInstr {
private:
  unsigned _index : 16;  /* index to local variables or constant pool */
  unsigned _pad   : 16;
  
public:
  JavaLoadInstr(u1 op, u2 idx);
  ~JavaLoadInstr() { }
};

/* ISTORE, LSTORE, FSTORE, DSTORE, ASTORE, ISTORE_0, ISTORE_1, ISTORE_2,
   ISTORE_3, LSTORE_0, LSTORE_1, LSTORE_2, LSTORE_3, FSTORE_0, FSTORE_1,
   FSTORE_2, FSTORE_3, DSTORE_0, DSTORE_1, DSTORE_2, DSTORE_3, ASTORE_0,
   ASTORE_1, ASTORE_2, ASTORE_3, IASTORE, LASTORE, FASTORE, DASTORE,
   AASTORE, BASTORE, CASTORE, SASTORE */
class JavaStoreInstr : public JavaInstr {
private:
  unsigned _index : 16;  /* index to local variables or constant pool */
  unsigned _pad   : 16;
  
public:
  JavaStoreInstr(u1 op, u2 idx);
  ~JavaStoreInstr() { }
};

/* SIPUSH, BIPUSH */
class JavaPushInstr : public JavaInstr {
private:
  unsigned _byte1 :  8;
  unsigned _byte2 :  8;
  unsigned _pad   : 16;

public:
  JavaPushInstr(u1 op, u1 byte1, u1 byte2);
  ~JavaPushInstr() { }

  u1 biValue();
  u2 siValue();
};

/* INEG, LNEG, FNEG, DNEG */
class JavaUnopInstr : public JavaInstr {
public:
  JavaUnopInstr(u1 op) : JavaInstr(JavaInstrUnop, op) { }
  ~JavaUnopInstr() { }
};

/* IINC */
class JavaIncInstr : public JavaInstr {
private:
  unsigned _lvarIndex : 8;
  unsigned _constVal  : 8;
  unsigned _pad       : 16;
public:
  JavaIncInstr(u1 op, u1 l, u1 c) : 
    JavaInstr(JavaInstrInc, op), _lvarIndex(l), _constVal(c) { }
  ~JavaIncInstr() { }

  GET_SET(u1, _lvarIndex, lvarIndex);
  GET_SET(u1, _constVal, constVal);
};

/* IADD, LADD, FADD, DADD, ISUB, LSUB, FSUB, DSUB, IMUL, LMUL, FMUL,
   DMUL, IDIV, LDIV, FDIV, DDIV, IREM, LREM, FREM, DREM, ISHL, LSHL,
   ISHR, LSHR, IUSHR, LUSHR, IAND, LAND, IOR, LOR, IXOR, LXOR */
class JavaBinopInstr : public JavaInstr {
public:
  JavaBinopInstr(u1 op) : JavaInstr(JavaInstrBinop, op) { }
  ~JavaBinopInstr() { }
};

/* I2L, I2F, I2D, L2I, L2F, L2D, F2I, F2L, F2D, D2I, D2L, D2F, I2B, 
   I2C, I2S */
class JavaConvInstr : public JavaInstr {

public:
  JavaConvInstr(u1 op) : JavaInstr(JavaInstrConv, op) { }
  ~JavaConvInstr() { }
};


class JavaCmpInstr : public JavaInstr {

};

class JavaCmplInstr : public JavaInstr {
};

class JavaCmpgInstr : public JavaInstr {
};

class JavaIfCompInstr : public JavaInstr {
};

/* GETSTATIC, GETFIELD */
class JavaGetInstr : public JavaInstr {
private:
  unsigned _cpoolIndex : 16;
  unsigned _pad        : 16;

public:
  JavaGetInstr(u1 op, u2 idx) : 
    JavaInstr(JavaInstrGet, op), _cpoolIndex(idx) { }
  ~JavaGetInstr() { }

  GET_SET(u2, _cpoolIndex, cpoolIndex);
};

/* INVOKESPECIAL, INVOKEVIRTUAL, INVOKEINTERFACE, INVOKESTATIC */
class JavaInvokeInstr : public JavaInstr {
private:
  unsigned _cpoolIndex : 16;
  unsigned _count      :  8;
  unsigned _pad        :  8;

public:
  JavaInvokeInstr(u1 op, u2 idx);
  ~JavaInvokeInstr() { }

  GET_SET(u2, _cpoolIndex, cpoolIndex);
  GET_SET(u1, _count, count);
};

/* IRETURN, LRETURN, FRETURN, DRETURN, ARETURN, RETURN */
class JavaReturnInstr : public JavaInstr {
public:
  JavaReturnInstr(u1 op);
  ~JavaReturnInstr() { }
};

/* NEW, NEWARRAY, ANEWARRAY, MULTIANEWARRAY */
class JavaNewInstr : public JavaInstr {
public:
  enum JavaArrayTypeE {
    JavaarrayBoolean =  4,
    JavaArrayChar    =  5,
    JavaArrayFloat   =  6,
    JavaArrayDouble  =  7,
    JavaArrayByte    =  8,
    JavaArrayShort   =  9,
    JavaArrayInt     = 10,
    JavaArrayLong    = 11
  };

private:
  unsigned _cpoolIndex : 16;
  unsigned _arrayType  :  8;
  unsigned _dimension  :  8;

public:
  JavaNewInstr(u1 op, u2 idx, u1 dim);
  ~JavaNewInstr() { }
};

#endif /* JAVA_INSTR_H */
