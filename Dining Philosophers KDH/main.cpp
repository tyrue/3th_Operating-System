#include <stdio.h>			
#include "pthread.h"    // 스레드 헤더파일
#include <Windows.h>	// Sleep용
#define NUM_THREADS 5	// 철학자 5명

pthread_mutex_t mutexes[NUM_THREADS];		// 뮤텍스 생성, 공유 자원 공간에 대한 접근 시간 제어로 동기화를 달성한다.
pthread_cond_t conditionVars[NUM_THREADS];	// 각 스레드의 상태 값, 정적으로 초기화한다.

int permits[NUM_THREADS];					// 포크 사용 여부, TRUE면 사용 가능
pthread_t tids[NUM_THREADS];				// 스레드ID를 생성하여 각 스레드를 식별한다, tids배열에 저장

void pickup_forks(int philosopher_number)	// 포크를 집는 함수, 임계구역(포크를 집는 동작)에서 다른 스레드가 반납 할 때까지 대기함
{
	Sleep(10);
	pthread_mutex_lock(&mutexes[philosopher_number%NUM_THREADS]);	// 해당 철학자 번호(왼쪽 포크)의 뮤텍스를 잠근다. 즉, 임계구역 설정. 
																	// 만약 잠그려는 뮤텍스가 다른 쓰레드에 의해서 이미 잠겨있다면, 
																	// 이미 잠근 다른 쓰레드가 뮤텍스의 잠금을 해제할때까지 봉쇄(블럭)되게 된다. 
	while (permits[philosopher_number%NUM_THREADS] == FALSE)		// 왼쪽 포크가 이미 사용되고 있으면 대기한다.
	{
		// 동작중인 쓰레드를 잠시 중단시킨다. 다른 스레드로부터 signal이나 broadcast를 받았을 경우 깨어나서 mutex_lock을 한다.
		pthread_cond_wait(&conditionVars[philosopher_number%NUM_THREADS], &mutexes[philosopher_number%NUM_THREADS]);
	} 
	permits[philosopher_number%NUM_THREADS] = FALSE;				// 해당 왼쪽 포크는 사용 불가로 만든다.
	pthread_mutex_unlock(&mutexes[philosopher_number%NUM_THREADS]);	// 쓰레드를 실행을 끝내고 종료하기 전에 자신이 가지고 있는 mutex를 풀어준다.
																	// 만약 이를 실행하지 않으면 다른 쓰레드는 mutex가 풀릴 때 까지 대기해야함
}

void return_forks(int philosopher_number)							// 포크를 내려놓는함수, 임계구역에서 대기하는 다른 스레드를 풀어줌
{
	Sleep(10);
	pthread_mutex_lock(&mutexes[philosopher_number%NUM_THREADS]);	// 해당 번호 뮤텍스를 잠근다.

	permits[philosopher_number%NUM_THREADS] = TRUE;					// 해당 왼쪽 포크는 사용 가능하게 한다.
	pthread_cond_signal(&conditionVars[philosopher_number%NUM_THREADS]);// 해당 상태 번호(conditionVasrs)를 가지고 대기중(pthread_cond_wait)인 
																// 하나의 스레드를 깨운다. 만약 thread가 다수일 경우 단 하나의 thread만 깨어난다.
	pthread_mutex_unlock(&mutexes[philosopher_number%NUM_THREADS]);	// 뮤텍스를 풀어줌
}

void* Philosopher(void * arg)		// 짝수 철학자의 함수, 왼쪽 먼저 들고, 오른쪽을 든다.
{
	int philosopher_number;			// 철학자 식별자
	philosopher_number = (int)arg;	// 식별자 할당
	for (int i = 0; i < NUM_THREADS; i++)	// 철학자 수 만큼 먹도록 한다.
	{
		// 왼쪽 포크를 집음
		pickup_forks(philosopher_number);
		printf("philosopher(%d) picks up the left fork(%d).\n", philosopher_number, philosopher_number);

		// 오른쪽 포크를 집음
		pickup_forks(philosopher_number + 1);
		printf("philosopher(%d) picks up the right fork(%d).\n", philosopher_number, (philosopher_number + 1) % NUM_THREADS);

		printf("philosopher(%d) starts eating(%d) \n\n", philosopher_number, i+1); // 몇 번 먹었는지 확인
		Sleep(40);	// 식사중..
		printf("philosopher(%d) finishes eating \n", philosopher_number);

		// 오른쪽 포크 내려놓음
		return_forks(philosopher_number + 1);
		printf("philosopher(%d) put down the right fork(%d).\n", philosopher_number, (philosopher_number + 1) % NUM_THREADS);

		// 왼쪽 포크 내려놓음
		return_forks(philosopher_number);
		printf("philosopher(%d) put down the left fork(%d).\n", philosopher_number, philosopher_number);
	}
	return NULL;	// 스레드 종료
}

void * OddPhilosopher(void * arg) // 홀수 철학자의 함수, 오른쪽 포크를 먼저 집고, 왼쪽 포크를 집는다.
{
	int philosopher_number;
	philosopher_number = (int)arg;
	for (int i = 0; i < NUM_THREADS; i++) // 철학자 수 만큼 먹도록 한다.
	{
		// 오른쪽 포크를 집음
		pickup_forks(philosopher_number + 1);
		printf("philosopher(%d) picks up the right fork(%d).\n", philosopher_number, (philosopher_number + 1) % NUM_THREADS);

		// 왼쪽 포크를 집음
		pickup_forks(philosopher_number);
		printf("philosopher(%d) picks up the left fork(%d).\n", philosopher_number, philosopher_number);

		printf("philosopher(%d) starts eating(%d) \n\n", philosopher_number, i+1); // 몇 번 먹었는지 확인.
		Sleep(40);	// 식사중....
		printf("philosopher(%d) finishes eating \n", philosopher_number);

		// 왼쪽 포크를 내려놓음
		return_forks(philosopher_number);
		printf("philosopher(%d) puts down the left fork(%d).\n", philosopher_number, philosopher_number);

		// 오른쪽 포크를 내려놓음
		return_forks(philosopher_number + 1);
		printf("philosopher(%d) puts down the right fork(%d).\n", philosopher_number, (philosopher_number + 1) % NUM_THREADS);
	}
	return NULL;	// 스레드 종료
}

int main() 
{
	int i;

	for (i = 0; i < NUM_THREADS; i++) 
	{
		pthread_mutex_init(&mutexes[i], NULL);		// 뮤텍스 생성
		pthread_cond_init(&conditionVars[i], NULL); // 상태를 NULL로 초기화한다.
		permits[i] = TRUE;							// 모든 포크는 식탁 위에 있다.
	}

	for (i = 0; i < NUM_THREADS; i++) 
	{
		if (i % 2) 
			pthread_create(&tids[i], NULL, OddPhilosopher, (void*)(i)); // 홀수 스레드(철학자) 생성, 홀수철학자 함수를 계속 실행하고 있다.
		
		else 
			pthread_create(&tids[i], NULL, Philosopher, (void*)(i));	// 짝수 스레드(철학자) 생성, 짝수철학자 함수를 계속 실행하고 있다.
	}

	for (i = 0; i < NUM_THREADS; i++) 
		pthread_join(tids[i], NULL); // 보조 스레드(철학자 스레드)가 종료 될 때까지 메인스레드(main)가 대기 하도록 하는 함수.
									 // 1 : 어떤 스레드를 기다릴 지 정하는 식별자, 2 : 스레드의 return 값, 포인트로 값을 받아오는 점을 주의.

	for (i = 0; i < NUM_THREADS; i++)
	{
		pthread_mutex_destroy(&mutexes[i]);			// 쓰레드를 파괴. 
		pthread_cond_destroy(&conditionVars[i]);	// 상태값 파괴.
	}
	return 0;
}