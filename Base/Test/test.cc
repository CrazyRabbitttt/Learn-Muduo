#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lockA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockB = PTHREAD_MUTEX_INITIALIZER;

void* task1(void* arg)                                    
{
  pthread_mutex_lock(&lockA);
  sleep(2); //防止一个线程执行过快 把锁都拿到了
  pthread_mutex_lock(&lockB);
    
  return NULL;
}
void* task2(void* arg)
{
  pthread_mutex_lock(&lockB);
  sleep(2);
  pthread_mutex_lock(&lockA);
    
  return NULL;
}

int main()
{
  pthread_t thread1,thread2;

  int ret_th1 = pthread_create(&thread1,NULL,task1,NULL);
  int ret_th2 = pthread_create(&thread2,NULL,task2,NULL);
  if(ret_th1<0 && ret_th2<0)
  {
    perror("pthread_create");
    return -1;
  }

  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);
  pthread_mutex_destroy(&lockA);
  pthread_mutex_destroy(&lockB);

  return 0;
}
