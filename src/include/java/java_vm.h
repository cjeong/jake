/* @file java_vm.h
 * @note Java virtual machine classes
 *
 * @author cjeong
 */
#ifndef JAVA_VM_H
#define JAVA_VM_H

#include <string.h>
#include <vector>
#include <map>
#include "java/java_base.h"
#include "java/java_classfile.h"


class JavaVMFrame;
class JavaVMThread {
private:
  unsigned long _pc;
  JavaVMFrame *_frame_stack;

public:
  JavaVMThread();
  ~JavaVMThread();

public:
  void start();
  void pause();
  void resume();
  void stop();
};


/* for heap area */
class JavaVMHeap {
private:
public:
};


/* for stack area; one frame for each thread */
class JavaVMFrame {
private:
  JavaVMFrame *_prev;

public:
  JavaVMFrame(JavaVMFrame *p);
  ~JavaVMFrame();
};

struct ltstr {
  bool operator()(const char* s1, const char* s2) const {
    return strcmp(s1, s2) < 0;
  }
};

class JavaVMMethod;
class JavaVMMethodArea {
public:
  std::map<char *, JavaInstr *, ltstr> _method_table();
};


class JavaVMMethod {
private:
  std::vector<JavaInstr *> _instructions;

public:
  std::vector<JavaInstr *>& instructions();
};

#endif /* JAVA_VM_H */
