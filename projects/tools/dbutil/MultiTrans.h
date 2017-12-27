#ifndef _MULTI_TRANS_H_
#define _MULTI_TRANS_H_

#ifdef	__cplusplus
extern "C" {
#endif

void MultiTransCount(int *count);
int MultiTransSetUp(int count);
int MultiTransBeginWork();
int MultiTransCommit(int enforce_flag);
int MultiTransRollback();

#ifdef	__cplusplus
}
#endif

#endif /* _MULTI_TRANS_H_ */
