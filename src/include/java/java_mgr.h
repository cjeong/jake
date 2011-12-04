/**
 * @file java_mgr.h
 * @brief manages global class info
 *
 * @author cjeong
 */
#ifndef RZ_JAVA_MGR_H
#define RZ_JAVA_MGR_H

#include "common.h"
#include "java_classfile.h"

class JavaMgr {
private:
  JavaMgr *_instance;
  
public:
  JavaMgr();
  ~JavaMgr();

  static JavaMgr *instance();

  /* given a class name, e.g. java.lang.Object, returns the Java class file */
  JavaClassFile *lookupClassFile(char *s);
};

#endif /* RZ_JAVA_MGR_H */
