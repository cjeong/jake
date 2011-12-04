/**
 * @file java_runtime.h
 * @brief Java virtual machine runtime
 *
 * @author cjeong
 */
#ifndef RZ_JAVA_RUNTIME_H
#define RZ_JAVA_RUNTIME_H


enum JavaRuntimeLocE {
  JavaLocConstPool,
  JavaLocLocalVar,
  JavaLocStackFrame,
  JavaLocHeap,
  JavaEvalStack
};

#endif /* RZ_JAVA_RUNTIME_H */
