/**
 * @file java_class_stream.h
 * @note Java classfile stream
 *
 * @author cjeong
 */
#ifndef JAVA_CLASS_STREAM_H
#define JAVA_CLASS_STREAM_H

#include <ios>

class JavaClassStream : public std::istream {
private:
  unsigned _cur_pos; 

public:
  JavaClassStream& open(char *filename);
  u1 read_u1();
  u2 read_u2();
  u4 read_u4();
};


#endif /* JAVA_CLASS_STREAM_H */
